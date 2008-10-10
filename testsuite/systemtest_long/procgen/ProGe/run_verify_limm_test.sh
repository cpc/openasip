#!/bin/bash
#
# Runs the FFT test case with long immediates
./generate_limm_verification_processor.sh
./simulate_limm_verification_processor.sh
head -n 8 ./proge-output/bus.dump
