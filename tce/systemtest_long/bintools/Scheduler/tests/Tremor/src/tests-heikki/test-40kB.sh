#!/bin/sh

ln -sf ../tests-pekka/data ./data 
make -C data
make -C .. -f TCE/Makefile 

export TTASIM_OUTPUT_FILE=output_40kB.raw
time ttasim <<EOF
puts "Loading and processing the program..."
prog ../tremor_40kB
puts "Running..."
run
puts "Number of moves executed:"
info proc cycles
quit
EOF
echo
if [ -z "`diff output_40kB.raw correct_output_40kB.raw`" ]; then
	echo RESULT OK if no error messages on screen
else
	echo INCORRECT
fi
echo
