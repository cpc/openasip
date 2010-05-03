Sources:
-----------------------------------
fft_simm.asm
fft_limm.asm
verify_limm_operation.asm


How to generate new TPEFs:
----------------------------------
First create data-symlink to scheduler testbench operations (contains fft
operation definitions)

tceasm -o fft_simm.tpef fft_simm.adf fft_simm.asm
tceasm -o fft_limm.tpef fft_limm.adf fft_limm.asm
tceasm -o fft_limm_opt.tpef fft_limm_opt.adf fft_limm.asm
tceasm -o limm_test.tpef limm_test.adf verify_limm_operation.asm


Create new verification data
----------------------------------
Create reference bustraces with ttasim (setting bus_trace 1) for each tpef.

cat fft_simm.tpef.bustrace > ../test_fft_case_with_simm/1_output.txt
cat fft_simm.tpef.bustrace > ../test_fft_case_with_compression/1_output.txt
cat fft_limm.tpef.bustrace > ../test_fft_case_with_limm/1_output.txt
cat fft_limm_opt.tpef.bustrace > ../test_fft_case_with_limm_opt/1_output.txt
cat limm_test.tpef.bustrace > ../verify_limm_operation/1_output.txt

