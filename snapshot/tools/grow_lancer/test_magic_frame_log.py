import unittest
from inspect_magic_frame_log import inspect

HEADER = 'pid=7 event=magic-frames count=1 dropped=0 phase=after-particles\n'
ROW = 'pid=7 event=magic-frame index=0 ms=211 owned=1 ran=1 pendingBefore=211 pendingAfter=11 completed=5 action=287 frame=5.5\n'

class CaptureTests(unittest.TestCase):
    def test_consistent_is_not_visual_pass(self):
        result = inspect(HEADER + ROW)
        self.assertEqual(result['status'], 'ACCOUNTING_CONSISTENT')
        self.assertFalse(result['visual_acceptance'])
    def test_no_capture(self):
        self.assertEqual(inspect('old controller logs')['status'], 'NO_CAPTURE')
    def test_truncated_or_dropped(self):
        self.assertEqual(inspect(HEADER)['status'], 'INCOMPLETE')
        self.assertEqual(inspect(HEADER.replace('dropped=0','dropped=1')+ROW)['status'], 'INCOMPLETE')
        self.assertEqual(inspect(HEADER)['truncated_groups'], 1)
        self.assertEqual(inspect(HEADER.replace('dropped=0','dropped=1')+ROW)['dropped_groups'], 1)
    def test_wrong_accounting(self):
        self.assertEqual(inspect(HEADER+ROW.replace('completed=5','completed=4'))['status'], 'INVALID')
    def test_orphan_or_duplicate(self):
        self.assertEqual(inspect(ROW)['status'], 'INVALID')
        self.assertEqual(inspect(HEADER+ROW+ROW)['status'], 'INVALID')
    def test_reset_and_off(self):
        self.assertEqual(inspect(HEADER+ROW.replace('owned=1','owned=0'))['status'], 'RESET_REVIEW_REQUIRED')
        off=ROW.replace('owned=1','owned=0').replace('ran=1','ran=0').replace('pendingBefore=211','pendingBefore=0').replace('pendingAfter=11','pendingAfter=0').replace('completed=5','completed=0')
        self.assertEqual(inspect(HEADER+off)['status'], 'MODE_OFF')
    def test_nonfinite(self):
        self.assertEqual(inspect(HEADER+ROW.replace('frame=5.5','frame=nan'))['status'], 'INVALID')
    def test_zero_tick_capture_is_not_execution(self):
        zero=ROW.replace('pendingBefore=211','pendingBefore=11').replace('completed=5','completed=0')
        self.assertEqual(inspect(HEADER+zero)['status'], 'NO_QUANTA')
    def test_pid_filter_does_not_inherit_other_process_failure(self):
        other=HEADER.replace('pid=7','pid=8').replace('count=1','count=2')
        self.assertEqual(inspect(other+HEADER+ROW, 7)['status'], 'ACCOUNTING_CONSISTENT')
        self.assertEqual(inspect(other+HEADER+ROW, 8)['status'], 'INCOMPLETE')
        self.assertEqual(inspect(other+HEADER+ROW, 9)['status'], 'NO_CAPTURE')

if __name__ == '__main__':
    unittest.main()
