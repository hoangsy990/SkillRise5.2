import copy
import unittest
from compare_equipment_bones import mismatches


class BoneComparisonTest(unittest.TestCase):
    def setUp(self):
        self.equipment = dict(usedBones=[1], bones=[
            dict(name='root', parent=-1, dummy=False),
            dict(name='hand', parent=0, dummy=False)])

    def test_equal_and_unused_extra_bones(self):
        player = copy.deepcopy(self.equipment)
        player['bones'].append(dict(dummy=True))
        self.assertEqual(mismatches(self.equipment, player), [])

    def test_ancestor_name_difference(self):
        player = copy.deepcopy(self.equipment)
        player['bones'][0]['name'] = 'other'
        self.assertEqual(mismatches(self.equipment, player), [0])

    def test_parent_difference(self):
        player = copy.deepcopy(self.equipment)
        player['bones'][1]['parent'] = -1
        self.assertEqual(mismatches(self.equipment, player), [1])

    def test_missing_player_bone(self):
        self.assertEqual(mismatches(self.equipment,
                         dict(bones=self.equipment['bones'][:1])), [1])

    def test_cycle_is_error(self):
        self.equipment['bones'][0]['parent'] = 1
        with self.assertRaisesRegex(ValueError, 'Cyclic'):
            mismatches(self.equipment, self.equipment)


if __name__ == '__main__':
    unittest.main()
