#!/bin/sh

ln -sf ../tests-pekka/data ./data
make -C data
make -C .. -f TCE/Makefile

export TTASIM_OUTPUT_FILE=output_full.raw
time ttasim <<EOF
puts "Loading and processing the program..."
prog ../tremor
puts "Running..."
run
puts "Number of moves executed:"
info proc cycles
quit
EOF
echo
if [ -z "`diff output_40k.raw ../tests-pekka/correct_output.raw`" ]; then
	echo RESULT OK if no error messages on screen
else
	echo INCORRECT
fi
echo
