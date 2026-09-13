import struct
import unittest
from inspect_equipment_meshes import inspect


class EquipmentLayoutTest(unittest.TestCase):
    def payload(self):
        return b'test.SMD'.ljust(32, b'\0') + struct.pack('<3h', 0, 0, 0)

    def test_tail_is_not_a_zero_padding_contract(self):
        for length in (0, 1, 15, 16):
            report = inspect(self.payload() + b'\xa5' * length)
            self.assertEqual(report['parsedBytes'], 38)
            self.assertEqual(report['paddingBytes'], length)
            self.assertFalse(report['paddingContentsVerified'])

    def test_offline_bound_retained(self):
        with self.assertRaisesRegex(ValueError, 'trailing'):
            inspect(self.payload() + b'\xa5' * 17)

    def test_truncated_mesh_rejected(self):
        payload = b'test.SMD'.ljust(32, b'\0') + struct.pack('<3h', 1, 0, 0)
        with self.assertRaisesRegex(ValueError, 'Truncated'):
            inspect(payload)


if __name__ == '__main__':
    unittest.main()
