#!/bin/bash
#
# Runs the FFT test case with short immediates
./generate_simm_processor.sh
./simulate_simm_processor.sh
cat ./proge-output/bus.dump
