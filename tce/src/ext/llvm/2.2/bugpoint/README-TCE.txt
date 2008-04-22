The modified Bugpoint of LLVM in this directory
can be used to track LLVM optimizations that produce 
miscompiled TTA programs. It requires that the program 
compiles successfully without optimizations (tcecc -O3) 
but when some LLVM optimization is applied, the result 
gets broken.

I have added two new switches to the Bugpoint: -run-custom,
which instructs the Bugpoint to "run" the generated
bitcodes with an custom command that can be defined
with the added '-exec-command' switch.

For this to work with TCE, I wrote script 'tce-exec-bc' 
that "executes" LLVM bytecodes by first scheduling it to 
a target and then running ttasim with the resulting
TPEF.

For example, here is how I tried to track the LLVM 2.2
miscompilation bug with DSPstone/adpcm:

1) Generate a bytecode that works correctly with -O0, but not with -O3:

   tcecc src/board_test.c -O0 -o adpcm.bc

2) Create a script for executing the bytecode and to produce validation
   output:

   cat > run_bytecode
   tce-exec-bc -e "run; puts [x /u h /n 32 _Input]; quit;"  \
               1_bus_full_connectivity.adf $1
   ^D
   chmod +x run_bytecode

   The script given with '-e' is fed to ttasim and executed after it has 
   loaded the machine and the scheduled tpef. It is used to produce output for
   verification. I could have used also printf() in the C code but it
   slows down compilation quite a lot so I decided to generate the
   verification data using ttasim instead.

3) Generate reference output from a valid run of the unoptimized
   program:

   ./run_bytecode adpcm.bc > reference.txt

4) Start Bugpoint.

   tce-bugpoint -run-custom \
                -exec-command=$PWD/run_bytecode \
                -output=reference.txt \
                -mlimit=0 \
                -timeout=0 \
                -check-exit-code=0 \
                -find-bugs adpcm.bc

   This instructions the Bugpoint to try random passes on the bitcode and
   run the resulting bitcode with the tce-exec-bc script identically to
   how we produced the reference.txt in the previous text. It compares
   the output to the produced reference and yells if it finds a miscompilation
   with some optimization pass.
