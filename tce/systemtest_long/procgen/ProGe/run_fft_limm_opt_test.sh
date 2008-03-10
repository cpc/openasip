#!/bin/bash
#
# Runs the FFT test case with optimised long immediates
./generate_limm_opt_processor.sh
./simulate_limm_opt_processor.sh
cat ./proge-output/bus.dump
