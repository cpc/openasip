#!/bin/sh

make -C .. -f Makefile.PC

echo running tremor..

time ../tremor_full > output_full.raw

echo
if [ -z "`diff output_full.raw ../tests-pekka/correct_output.raw`" ]; then
	echo RESULT OK if no error messages on screen
else
	echo INCORRECT
fi
echo
