
In this directory I have a test input .ogg and correct output
generated with a pure Linux/PC compilation.

play_output.sh uses 'sox' to play the raw output PCM file for 
fun (it's a good song) :)

Here's how you can create the initial data for the MEM I/O version:

gcc generate.c -o generate
./generate 33502976 < The_Clash_-_The_Guns_of_Brixton.ogg > ../initial_data.c

In order to use the special operation output, you have to compile 
the OSAL operation definitions in data/ by running 'make' in it.
You have to have TCE and 'buildopset' binary installed.
