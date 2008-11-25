#!/bin/bash
# Runs the function pointer test.
./generate_func_ptr_processor.sh
./generate_func_ptr_program_image.sh
./simulate_func_ptr_processor.sh
cat proge-output/printchar_output.txt
