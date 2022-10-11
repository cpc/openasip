#!/bin/sh
{
~/openasip/src/codesign/ttasim/ttasim <<EOF
puts "#Loading the program to search the syscalls from..."
prog $1
puts "#Done... scanning for sys..."
disassemble 0 10000000
EOF
} | grep -Ei "\#|sys"

