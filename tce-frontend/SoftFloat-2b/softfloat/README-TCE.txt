LLVM-TCE versio for software floatingpoint lib
------------------------------------------------

Change log:
--------------
1.2.2008        M. Lepistö     Initial version


Quick testing:
-----------------

Make sure that your TCE and LLVM-frontend are armed.

1. cd SoftFloat-2b/softfloat/bits32/TCE-LLVM-GCC/
2. make
3. ttasim -a 1_bus.adf -p timesoftfloat.tpef


General notes about the system
-------------------------------

README.txt is good place to start digging.

TCE Notes
-------------

Added new files for LLVM customization to:

processors/TCE-GCC.h
softfloat/bits32/TCE-LLVM-GCC

There is also 1_bus.adf and 1_bus_seq.conf for testing.

For testing correctness of library functionality, 

softfloat/bits32/timesoftfloat.c 

was modified to output its calculation results with printf.
Before this file was for testing performance of the library.
Original timesoftfloat.c was moved to 

softfloat/bits32/timesoftfloat.c.original

Compilation of SPARC and i386 versions of the libraries are
still functional. So those versions can be used to generate 
correct reference data with host machine.

Integration with llvm-frontend
-------------------------------

All the code for software floatingpoint implemetation of 
llvm-frontend is copied from this directory directly to:

llvm-frontend/newlib-1.15.0/libgloss/tce/emulation_functions.c

Implementation was done by copying sources, because newlib/libgloss 
makefiles are such a mess that it would have been too hard to 
integrate this compilation structure to work with it  nicely.

Basically problem was that there is no Makefile.am files released 
with the libgloss and without Makefile.am it's pain to set up
builddir != srcdir + installation stuff.
