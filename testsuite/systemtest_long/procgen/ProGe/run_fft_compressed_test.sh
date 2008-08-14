#!/bin/bash
#
# Runs the FFT test case with compressed instructions
./generate_decomp_and_prog_image.sh
./generate_compr_processor.sh
./simulate_compr_processor.sh
cat ./proge-output/bus.dump
