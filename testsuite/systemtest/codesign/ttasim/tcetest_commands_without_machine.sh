#!/bin/bash
### TCE TESTCASE
### title: Tests execution of simulation commands when ADF is not loaded


TTASIM_COMMANDS="
bp
run
stepi 12
deletebp 1
disablebp 1
disassemble
enablebp 1
ignore 1 2
load_data foo bar
nexti 12
resume
run
until 1
symbol_address foo
tbp 2
x
info buses foo
info breakpoints
info funits
info immediates foo bar
info iunits
info ports funit foo
info proc cycles
info proc mapping
info proc stats
info program
info program is_instruction_reference 1 1
info regfiles
info registers baz 1
info segments foo
info stats executed_operations
info stats register_reads
info stats register_writes"

set -u
IFS=$'\n' # 
for cmd in $TTASIM_COMMANDS; do
    echo "ttasim --no-debugmode -e \"$cmd; quit;\""
    ttasim --no-debugmode -e "$cmd; quit;" || echo TROUBLE!!!
done
