#!/usr/bin/python

import sys

a = 0

img = open(sys.argv[1], "r")
rom = open(sys.argv[2], "w")

w = sys.argv[3]
fmt = "%0{}x\n".format(w)

for line in img:
  rom.write(fmt % int(line, 2) )
  a+=1
