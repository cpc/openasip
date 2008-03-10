#!/bin/bash
# Runs the function pointer test.
./generate_func_ptr_program_image.sh
./generate_func_ptr_processor.sh
./simulate_func_ptr_processor.sh
cat ./printchar_output.txt
