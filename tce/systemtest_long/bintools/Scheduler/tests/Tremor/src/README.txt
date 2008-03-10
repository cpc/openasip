In order to compile tremor, one needs to generate initial_data.c file using stuff in tests-pekka directory.

It is test input .ogg and correct output
generated with a pure Linux/PC compilation.


Here's how you can create the initial data for the MEM I/O version:
cd tests-pekka
gcc generate.c -o generate
./generate 33502976 < The_Clash_-_The_Guns_of_Brixton.ogg > ../initial_data.c


Next thing is to compile tremor using Makefile in TCE directory:
make -f TCE/Makefile
