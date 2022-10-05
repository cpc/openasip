#!/usr/bin/python

import sys
from io import StringIO


img = open(sys.argv[1], "rb")
rom = open(sys.argv[2], "wb")

def nextPowerOf2(x):
    return 0 if x == 0 else 2**(x-1).bit_length()

# Find the max length of an ascii line
length = 0
for line in img:
    line_int = int(line.strip(), 2)
    new_length = nextPowerOf2(line_int.bit_length())
    if new_length > length:
        length = new_length

img.close()


img = open(sys.argv[1], "rb")
for line in img:
    line_int = int(line.strip(), 2)

    # Pad the binarystring from the left
    line_bin = bin(line_int)[2:].zfill(length)

    sio = StringIO(line_bin)

    char_list = []

    # Read the binarystring byte at a time and convert to char
    while 1:
        b = sio.read(8)
        if not b :
            break
        if len(b) < 8 :
            b = '0' * (8 - len(b)) + b
        i = int(b,2)
        c = chr(i)
        char_list.append(c)

    # Reverse the byte endianess
    for i in reversed(char_list):
        rom.write(i.encode('charmap'))
