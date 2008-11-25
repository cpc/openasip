#!/bin/bash
#
# Runs the FFT test case with compressed instructions
./generate_compr_processor.sh
./generate_decomp_and_prog_image.sh
./simulate_compr_processor.sh
cat ./proge-output/bus.dump
