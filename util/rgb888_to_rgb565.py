#!/usr/bin/env python3

import sys

if(len(sys.argv) == 1):
    print("No arguments specified. Use -h or --help for usage.")
    sys.exit(1)

clargs = iter(sys.argv[1:])

file888 = None
file565 = None

for arg in clargs:
    if(arg in ["-h", "--help"]):
        print("\nUsage: ./rgb888_to_rgb565.py -i 888.bin -o 565.bin")
        print("\nUsage: ./rgb888_to_rgb565.py --input 888.bin --output 565.bin")
        print("Convert RGB888 data in 888.bin and save as RGB565 in 565.bin")
        sys.exit(0)
    elif(arg in ["-i", "--input"]):
        file888 = next(clargs, None)
    elif(arg in ["-o", "--output"]):
        file565 = next(clargs, None)
    else:
        print("Received malformed argument list. Use -h or --help for usage.")
        sys.exit(1)

if(not file888 or not file565):
    print("Error: Required files not specified.")
    sys.exit(1)

try:
    handle888 = open(file888, "rb")
    data888 = handle888.read()
    handle888.close()
except Exception as E:
    print(f"Exception occured when reading file {file888}")
    print(f" -> {E}")
    sys.exit(1)

data565 = []

# Make data iterable and zip the iterable 3 times to get 3 element RGB pair
for r, g, b in list(zip(*[iter(data888)]*3)):
    r5 = int((r / 255) * 31)
    g6 = int((g / 255) * 63)
    b5 = int((b / 255) * 31)
    rgb565 = (r5 << 11) | (g6 << 5) | b5
    hi, lo = (rgb565 & 0xFF00) >> 8, rgb565 & 0x00FF
    data565.append(lo) # lo, hi for little endian
    data565.append(hi)

try:
    handle565 = open(file565, "wb")
    handle565.write(bytes(data565))
    handle565.close()
except Exception as E:
    print(f"Exception occured when writing file {file565}")
    print(f" -> {E}")
    sys.exit(1)
