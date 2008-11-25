#!/bin/bash
#
# Runs the FFT test case with short immediates
./generate_simm_processor.sh
./generate_simm_prog_image.sh
./simulate_simm_processor.sh
cat ./proge-output/bus.dump
