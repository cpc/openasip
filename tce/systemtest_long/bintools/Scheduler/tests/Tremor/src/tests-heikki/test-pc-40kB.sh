#!/bin/sh

make -C .. -f Makefile.PC

echo running tremor..

time ../tremor_40kB > output_40kB.raw

echo
if [ -z "`diff output_40kB.raw correct_output_40kB.raw`" ]; then
	echo RESULT OK if no error messages on screen
else
	echo INCORRECT
fi
echo
