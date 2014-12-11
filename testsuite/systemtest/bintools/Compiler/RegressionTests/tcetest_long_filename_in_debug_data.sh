#!/bin/sh
### TCE TESTCASE 
### title: Long source code path in debug data corrupting the TPEF
### xstdout: OK

mach=data/minimal_with_stdout.adf
src=data/debug_data_test.c
long_dir=data/.ultralong/directory/leading/to/thesourcefile/which/should/be/cut/before/it/gets/to/the/tpef/and/also/cut/at/the/tpef/just/in/case/so/it/doesnt/corrupt/things/here/
long_path=$long_dir/debug_data_test.c
mkdir -p $long_dir
cp $src $long_path
program=$(mktemp tmpXXXXXXXXX)

tcecc $long_path -llwpr -g -O0 -a $mach -o $program 

(tcedisasm -s $mach $program | grep -cq debug_data_test.c) && echo "OK"

rm -fr data/.ultralong $program

