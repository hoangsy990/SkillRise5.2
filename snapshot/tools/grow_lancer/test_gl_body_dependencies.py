"""Bounded parser fixtures, not real BMD loader validation."""
import struct
import unittest
from inspect_gl_body_dependencies import inspect, inspect_skeleton


class ParserTest(unittest.TestCase):
    def test_mesh_texture_and_bounds(self):
        header = b'fixture'.ljust(32, b'\0') + struct.pack('<3h', 1, 0, 0)
        payload = header + struct.pack('<5h', 0, 0, 0, 0, 0) + b'skin.jpg'.ljust(32,b'\0')
        identity, textures, end = inspect(payload)
        self.assertEqual(textures, [(0, 0, 'skin.jpg')])
        self.assertEqual(end, len(payload))
        self.assertEqual(inspect_skeleton(payload, identity, end), ([], [], b''))
        for cut in range(38, len(payload)):
            with self.assertRaises(ValueError):
                inspect(payload[:cut])

    def test_skeleton_preserves_unknown_tail(self):
        payload = struct.pack('<hB', 1, 0) + b'\0' + b'bone'.ljust(32,b'\0')
        payload += struct.pack('<h', -1) + bytes(24) + b'\xa8\x00'
        keys, bones, tail = inspect_skeleton(payload, {'actionCount': 1, 'boneCount': 1}, 0)
        self.assertEqual((keys, bones, tail), ([1], [('bone',-1)], b'\xa8\x00'))
        for cut in range(len(payload)-2):
            with self.assertRaises(ValueError):
                inspect_skeleton(payload[:cut], {'actionCount': 1, 'boneCount': 1}, 0)

    def test_invalid_flags(self):
        with self.assertRaises(ValueError):
            inspect_skeleton(struct.pack('<hB', 1, 2), {'actionCount': 1, 'boneCount': 0}, 0)
        with self.assertRaises(ValueError):
            inspect_skeleton(b'\x02', {'actionCount': 0, 'boneCount': 1}, 0)

    def test_vertex_node_bounds(self):
        header = b'fixture'.ljust(32,b'\0') + struct.pack('<3h', 1, 1, 0)
        mesh = struct.pack('<5h', 1, 0, 0, 0, 0)
        for node in (-1, 1):
            data = header + mesh + struct.pack('<h',node) + bytes(14) + b'a.jpg'.ljust(32,b'\0')
            with self.assertRaises(ValueError):
                inspect(data)
        data = header + mesh + bytes(16) + b'a.jpg'.ljust(32,b'\0')
        self.assertEqual(inspect(data)[0]['usedNodes'], [0])


if __name__ == '__main__':
    unittest.main()
