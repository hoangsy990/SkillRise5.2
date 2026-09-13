import argparse
import pathlib
import struct

import pefile


parser = argparse.ArgumentParser()
parser.add_argument("exe", type=pathlib.Path)
parser.add_argument("addresses", nargs="+")
args = parser.parse_args()

image = pefile.PE(str(args.exe))
data = args.exe.read_bytes()
for text in args.addresses:
    address = int(text, 0)
    offset = image.get_offset_from_rva(address - image.OPTIONAL_HEADER.ImageBase)
    value = struct.unpack_from("<f", data, offset)[0]
    print(f"0x{address:08X} {value:.9g}")
