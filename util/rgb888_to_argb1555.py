#!/usr/bin/env python3

import sys

if(len(sys.argv) == 1):
    print("No arguments specified. Use -h or --help for usage.")
    sys.exit(1)

clargs = iter(sys.argv[1:])

file888  = None
file1555 = None

for arg in clargs:
    if(arg in ["-h", "--help"]):
        print("\nUsage: ./rgb888_to_argb1555.py -i 888.bin -o 1555.bin")
        print("\nUsage: ./rgb888_to_argb1555.py --input 888.bin --output 1555.bin")
        print("Convert RGB888 data in 888.bin and save as RGB1555 in 1555.bin")
        print("Black Color (RGB=0x000000) is color key for transparent pixel")
        sys.exit(0)
    elif(arg in ["-i", "--input"]):
        file888 = next(clargs, None)
    elif(arg in ["-o", "--output"]):
        file1555 = next(clargs, None)
    else:
        print("Received malformed argument list. Use -h or --help for usage.")
        sys.exit(1)

if(not file888 or not file1555):
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

data1555 = []

# Make data iterable and zip the iterable 3 times to get 3 element RGB pair
for r, g, b in list(zip(*[iter(data888)]*3)):
    a1 = 1
    if(r == 0 and g == 0 and b == 0):
        a1 = 0
    
    r5 = int((r / 255) * 31)
    g5 = int((g / 255) * 31)
    b5 = int((b / 255) * 31)

    rgb1555 = (a1 << 15) | (r5 << 10) | (g5 << 5) | b5
    
    hi, lo = (rgb1555 & 0xFF00) >> 8, rgb1555 & 0x00FF
    data1555.append(lo) # lo, hi for little endian
    data1555.append(hi)

try:
    handle1555 = open(file1555, "wb")
    handle1555.write(bytes(data1555))
    handle1555.close()
except Exception as E:
    print(f"Exception occured when writing file {file1555}")
    print(f" -> {E}")
    sys.exit(1)
