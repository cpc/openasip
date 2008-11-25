#!/bin/bash
#
# Runs the FFT test case with long immediates
./generate_limm_processor.sh
./generate_limm_prog_image.sh
./simulate_limm_processor.sh
cat ./proge-output/bus.dump
