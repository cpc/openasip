CODE ;

# 1K R4 DIT FFT
# *********************************************************************************************************
# TTA ASSEMBLY with correct registers when jump latency = 4 and LSU latency = 3.
# Coefficient generator SFU is used in this code and the FIFO for operand addresses has been removed from
# the code. Due to removing the FIFO, the length of the kernel has to be increased from 4 to 12 
# instructions. Removing the address FIFO decreases power consumption since the address transports for 
# upkeeping it could be removed from the code.
# *********************************************************************************************************
# Latencies of other FUs:
# addrgen 2
# cmul    3
# cadd    1
# fgen    5
# add     1
# cmp     1
# *********************************************************************************************************
# Number of move buses = 18
# Number of instructions = 58 
# Cycle count = 5234
# *********************************************************************************************************
# Author: Risto Mäkinen <rmmakine@cs.tut.fi>
# Organization: Institute of Digital and Computer Systems, Tampere University of Technology (TUT), Finland
# Project: FlexDSP
# Date: 2005-07-12
# *********************************************************************************************************
# This assembly code has to be compiled with ttasm and proper machine configuration file.
# Then the generated binary code can be simulated with HW-simulator vsim. The processor model for vsim
# can be generated with the processor generator MOVEgen.
# *********************************************************************************************************


# BB0  
# purpose      : Initializations
# frequency    : 1 (* 9 = 9)
# moves/cycle  : xx

# Fill registers with initial parameters:
# r15 stage
# r16 zero register
# r17 one register
# r18 two register
# r19 three register
# rv four register, used in evaluating the end-condition of the 1k FFT computation: Is stage > 4?
# r21 store the end-index of the kernel (1004)
# r20 store base address of input buffer in BB0, after that this register is used for the storage of operand addresses.
# r22 base address of output buffer 
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., i1.0 -> integer4.1 [i1.0=0];
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., i1.0 -> integer5.1 [i1.0=0];
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., i1.0 -> integer6.1 [i1.0=1];
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., i1.0 -> integer7.1 [i1.0=2];
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., i1.0 -> integer8.1 [i1.0=3];
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., i1.0 -> integer0.0 [i1.0=4];
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., i1.0 -> integer9.1 [i1.0=0];
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., i1.0 -> integer0.2 [i1.0=4096];
..., integer9.1 -> fu1.o0.ag, integer0.2 -> fu1.o1.ag, ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., i1.0 -> integer10.1 [i1.0=1004];

# BB1  
# purpose      : Upkeeping of the outer loop
# frequency    : 5 (* 2 = 10)
# moves/cycle  : xx
..., integer4.1 -> fu1.o2.ag, integer5.1 -> fu8.o0.eq, integer4.1 -> fu8.trigger.eq, integer4.1 -> fu5.o0.fgen, ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ... ;
fu8.r0.eq -> boolean0.0, ..., integer5.1 -> fu4.o0.add, integer5.1 -> fu4.trigger.add, ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ... ;

# BB2  
# purpose      : Start the pipeline for FFT                     
# frequency    : 5 (* 14 = 70)
# moves/cycle  : xx
# comment      : Inner loop's prologue
#              : r1-r10, r20, and r12 is used for the temporary storage of the operand addresses.
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ..., ..., ..., ..., ..., fu4.r0.add -> fu5.trigger.fgen, ..., ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ..., ..., ..., ..., ..., fu4.r0.add -> fu5.trigger.fgen, ..., ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer1.0, ..., ..., fu4.r0.add -> fu5.trigger.fgen, ..., ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer2.0, ..., ..., fu4.r0.add -> fu5.trigger.fgen, ..., ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer3.0, ..., ..., fu4.r0.add -> fu5.trigger.fgen, ..., ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer4.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, ..., ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer5.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, ..., ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer6.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, ..., ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer7.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> fu3.o0.cadd, ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer8.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> fu3.o1.cadd, ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer9.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> fu3.o2.cadd, ..., ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer10.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> fu3.o3.cadd, integer5.1 -> fu3.trigger.cadd, ..., ..., ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer9.1, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer0.1, integer6.1 -> fu3.trigger.cadd, integer1.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer1.1, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer2.1, integer7.1 -> fu3.trigger.cadd, integer2.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;

# BB3
# purpose      : 
# frequency    : 84 * 5 = 420 (* 12 = 5040)
# moves/cycle  : xx
# comment      : Inner loop's kernel: jump latency = 4 and LSU latency = 3
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer1.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer3.1, integer8.1 -> fu3.trigger.cadd, integer3.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer2.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> fu3.o3.cadd, integer5.1 -> fu3.trigger.cadd, integer4.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, integer0.1 -> fu3.o0.cadd, integer2.1 -> fu3.o1.cadd, integer3.1 -> fu3.o2.cadd, ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer3.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer0.1, integer6.1 -> fu3.trigger.cadd, integer5.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer4.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer2.1, integer7.1 -> fu3.trigger.cadd, integer6.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer5.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer3.1, integer8.1 -> fu3.trigger.cadd, integer7.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer6.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> fu3.o3.cadd, integer5.1 -> fu3.trigger.cadd, integer8.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, integer0.1 -> fu3.o0.cadd, integer2.1 -> fu3.o1.cadd, integer3.1 -> fu3.o2.cadd, ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer7.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer0.1, integer6.1 -> fu3.trigger.cadd, integer9.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, fu4.r0.add -> fu8.o0.gtu, integer10.1 -> fu8.trigger.gtu, ..., ... ;
fu8.r0.gtu -> boolean0.1, fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer8.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer2.1, integer7.1 -> fu3.trigger.cadd, integer10.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer9.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer3.1, integer8.1 -> fu3.trigger.cadd, integer9.1 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ! boolean0.1 i1.0 -> gcu.pc.jump [i1.0=25];
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer10.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> fu3.o3.cadd, integer5.1 -> fu3.trigger.cadd, integer1.1 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, integer0.1 -> fu3.o0.cadd, integer2.1 -> fu3.o1.cadd, integer3.1 -> fu3.o2.cadd, ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer9.1, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer0.1, integer6.1 -> fu3.trigger.cadd, integer1.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer1.1, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer2.1, integer7.1 -> fu3.trigger.cadd, integer2.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;


# BB4  
# purpose      : Stop the pipeline for FFT
# frequency    : 5 (* 14 = 70)
# moves/cycle  : xx
# comment      : Inner loop's epilogue
..., fu4.r0.add -> fu4.o0.add, fu4.r0.add -> fu1.trigger.ag, integer6.1 -> fu4.trigger.add, ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer1.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> integer3.1, integer8.1 -> fu3.trigger.cadd, integer3.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., ..., fu4.r0.add -> fu1.trigger.ag, ..., ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer2.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, fu4.r0.add -> fu5.trigger.fgen, fu2.r0.cmul -> fu3.o3.cadd, integer5.1 -> fu3.trigger.cadd, integer4.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, integer0.1 -> fu3.o0.cadd, integer2.1 -> fu3.o1.cadd, integer3.1 -> fu3.o2.cadd, ... ;
..., ..., ..., ..., ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer3.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, ..., fu2.r0.cmul -> integer0.1, integer6.1 -> fu3.trigger.cadd, integer5.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., ..., ..., ..., ? boolean0.0 fu1.r0.ag -> fu6.trigger.ld, ! boolean0.0 fu1.r1.ag -> fu6.trigger.ld, fu1.r1.ag -> integer4.0, fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, ..., fu2.r0.cmul -> integer2.1, integer7.1 -> fu3.trigger.cadd, integer6.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, ..., fu2.r0.cmul -> integer3.1, integer8.1 -> fu3.trigger.cadd, integer7.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, ..., fu2.r0.cmul -> fu3.o3.cadd, integer5.1 -> fu3.trigger.cadd, integer8.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, integer0.1 -> fu3.o0.cadd, integer2.1 -> fu3.o1.cadd, integer3.1 -> fu3.o2.cadd, ... ;
..., ..., ..., ..., ..., ..., ..., fu6.r0.ld -> fu2.o0.cmul, fu5.r0.fgen -> fu2.trigger.cmul, ..., fu2.r0.cmul -> integer0.1, integer6.1 -> fu3.trigger.cadd, integer9.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., fu2.r0.cmul -> integer2.1, integer7.1 -> fu3.trigger.cadd, integer10.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., fu2.r0.cmul -> integer3.1, integer8.1 -> fu3.trigger.cadd, integer9.1 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., fu2.r0.cmul -> fu3.o3.cadd, integer5.1 -> fu3.trigger.cadd, integer1.1 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, integer0.1 -> fu3.o0.cadd, integer2.1 -> fu3.o1.cadd, integer3.1 -> fu3.o2.cadd, ... ;
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., integer6.1 -> fu3.trigger.cadd, integer1.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., integer7.1 -> fu3.trigger.cadd, integer2.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., integer8.1 -> fu3.trigger.cadd, integer3.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., integer4.0 -> fu7.o0.st, fu3.r0.cadd -> fu7.trigger.st, ..., ..., ..., ... ;

# BB5
# purpose      : Upkeeping of the outer loop
# frequency    : 5 (* 7 = 35)
# moves/cycle  : xx
..., integer4.1 -> fu4.o0.add, ..., integer6.1 -> fu4.trigger.add, ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ... ;
..., ..., fu4.r0.add -> fu8.o0.gtu, integer0.0 -> fu8.trigger.gtu, ..., ..., fu4.r0.add -> integer4.1, ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ... ;
fu8.r0.gtu -> boolean0.0, ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ! boolean0.0 i1.0 -> gcu.pc.jump [i1.0=9];
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ... ;
..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ... ;

# Ready, jam into busy loop or do something
#..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., 58 -> jump [m19/i1/ir_1/pc];
#..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ...;
#..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ...;
#..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ...;


# Ready, jam into busy loop or do something
#..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., 58 -> jump [m19/i1/ir_1/pc];
#..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ...;
#..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ...;
#..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ..., ...;

# Move to TCE register map
# r4	= integer4.0
# r5	= integer5.0
# r6	= integer6.0
# r7	= integer7.0
# r8	= integer8.0
# r0	= integer0.0
# r9	= integer9.0
# r11	= integer0.1
# r10	= integer10.0
# b0	= boolean0.0
# r1	= integer1.0
# r2	= integer2.0
# r3	= integer3.0
# r15	= integer4.1
# r16	= integer5.1
# r17	= integer6.1
# r18	= integer7.1
# r19	= integer8.1
# r20	= integer9.1
# r21	= integer10.1
# r22	= integer0.2
# r12	= integer1.1
# r13	= integer2.1
# r14	= integer3.1
# b1	= boolean0.1

# MAU: 8bits
DATA iodata 0 ;

# replace next line with your own init data if needed.
DA 4096
4:0x733bbb2b
4:0x1b5afc69
4:0x642c4318
4:0xf4db84bc
4:0x5247f1d8
4:0x1d8c4abc
4:0x6bfc3cfb
4:0xad1fe7dc
4:0x6f7a6aba
4:0xe90764c6
4:0x8ed1da55
4:0x502b8286
4:0xa38eb3e8
4:0xb2df1a91
4:0xc5aeb2e5
4:0x83e93f2d
4:0xf1f16e8b
4:0xf74beb2c
4:0x58a10670
4:0xb3e02c11
4:0x568f8507
4:0x2e68e125
4:0x54f000b8
4:0x359fedcb
4:0xcdfbb08d
4:0xb1842ea6
4:0xcd820aab
4:0xa69f32a9
4:0xe0dd5c29
4:0x5a8917f3
4:0xff1e6657
4:0x52562518
4:0x51662904
4:0xd78bca2b
4:0xd75808b9
4:0x3a24cf2d
4:0x56a7116d
4:0xded333e6
4:0x0becf1e3
4:0x31cf1f0e
4:0x4b7974f3
4:0x05c86150
4:0xac467ad0
4:0xc57dc098
4:0x60303cc0
4:0xa2f28302
4:0x64d6b2fa
4:0xcc792954
4:0xc8cff81f
4:0x90957d03
4:0x1531ec6a
4:0x03f8d57d
4:0xeed2b9d7
4:0x146e42a7
4:0x07a223f9
4:0xb585e13b
4:0x48882e4b
4:0xf60a115d
4:0x4b518f26
4:0x1a558cde
4:0xea56ce14
4:0x5fd683d7
4:0x44987889
4:0x7d7649f2
4:0xf04cff91
4:0xb6c624bb
4:0xd1ed75c9
4:0x3a04e975
4:0x3e9bc498
4:0xf09e6ef1
4:0x2eeeb66a
4:0x56d820f8
4:0xa23fb506
4:0x1b712140
4:0xded7133d
4:0xf3908b3c
4:0x86f5d00c
4:0x834be24b
4:0x2ee097c4
4:0x890c1cc5
4:0x1bc98408
4:0x842fb0a8
4:0x16408ebd
4:0xde1921a6
4:0x37b63152
4:0x9586f450
4:0xf11bda6e
4:0xa7522cf7
4:0x32ff3a3e
4:0xfa770e09
4:0x9efcf364
4:0x37446491
4:0xc5e9c138
4:0x5d98bb7b
4:0x4e0b688c
4:0xbb5dbd43
4:0x8cbc9411
4:0x240cb0dd
4:0x5807ac84
4:0xabb97e8a
4:0xf096d70d
4:0xd070dd75
4:0xe4ab176e
4:0x9ea789c2
4:0xf5665eaf
4:0x6f2ac3b2
4:0xa9095f73
4:0xbce52555
4:0x77852a38
4:0x5ed1828a
4:0xa3135199
4:0xee1f63ec
4:0x3c232ff4
4:0xd89aaa81
4:0xa7d6b0ec
4:0xec255b21
4:0xfd8150e1
4:0xf5f5f515
4:0xf360e987
4:0x66cf816e
4:0xcc228c95
4:0x3174266d
4:0x7ba50d7c
4:0xe66bb2e3
4:0x200d3bbd
4:0xe0398287
4:0xeb7b40f0
4:0x4b3b6b82
4:0x583fde25
4:0x1eec3b34
4:0xb1a367a1
4:0x11b721bc
4:0xbc020c7c
4:0x6e7cd5cf
4:0x27d0e453
4:0x209732f6
4:0xe5ade9e3
4:0x27bc566b
4:0xdf21ecdd
4:0x183b10d4
4:0x376f02e5
4:0x46c2fd45
4:0xaf97335c
4:0x7b924e7f
4:0x341dfc26
4:0x9d572a33
4:0xdd89a3d9
4:0x111852b0
4:0x2c877fdb
4:0x762d8f11
4:0xdc3d0c6b
4:0xc30318eb
4:0x8c9d1230
4:0x336b7658
4:0x40223d70
4:0xee8f225f
4:0x4d939579
4:0x72096a7b
4:0x1a1bc0e9
4:0x5f9a036e
4:0x3b8eec16
4:0x761c9272
4:0x0daccabf
4:0x5ba3d5f4
4:0x2e218dae
4:0xdb4dff90
4:0xef370ffd
4:0x1dda9d03
4:0x65f3412a
4:0x4a8750a0
4:0x2b85b36c
4:0xc5e92050
4:0x096f8f3b
4:0x96c6c574
4:0xe8b8f95a
4:0x68b318a3
4:0xd436fa6a
4:0x18e0a954
4:0x545874c4
4:0x1875875c
4:0x4fe71c30
4:0x3395979a
4:0xecc5e025
4:0xaa895549
4:0x56b1f39c
4:0x74e3a5ab
4:0x5eb444f9
4:0xf1b41ee0
4:0x73a123d7
4:0xbf51da4a
4:0xb017fd9a
4:0xe8c6f6a9
4:0x1c669238
4:0xd0741bbf
4:0xacce1efb
4:0xbef7165d
4:0x018cf6fb
4:0x0a9a713c
4:0xd77de6dc
4:0xcec4e95c
4:0xc933e4e5
4:0x00c538d3
4:0xce639cb6
4:0xf17bf77d
4:0x83c129ff
4:0x395bc819
4:0xc306355e
4:0x48ab7c74
4:0xf92c671f
4:0xf3784df4
4:0x5430aa90
4:0xe4d60550
4:0x37d711b6
4:0xf5f7f1ff
4:0x9676f188
4:0xddc5cd72
4:0x5a12426d
4:0x73230ed5
4:0x83a4189e
4:0x50f27a22
4:0xb8cf3424
4:0x05a56ed2
4:0x369eba60
4:0xf31bac15
4:0x7804db10
4:0x8c8e415d
4:0x6512c941
4:0xc04e6ec7
4:0xa18770d9
4:0x33ac5901
4:0xb592f480
4:0x94c159e3
4:0x0fe2d1bd
4:0xdff95e27
4:0xdf4692dd
4:0xb3288cab
4:0x11139f36
4:0x05a99df7
4:0x4519e003
4:0x52c98bf0
4:0x191072fb
4:0xc9ee638a
4:0x9a0190b8
4:0xbbfb6edf
4:0x9029c3a3
4:0x7fe1b644
4:0xff97ca5d
4:0x2c39753e
4:0x443c2a87
4:0xa185986d
4:0x83cec9c7
4:0x51157c48
4:0x847251c3
4:0x1f030f6a
4:0xbe78526f
4:0xc36140ee
4:0x28deb6cc
4:0x1a241add
4:0x28d4aef1
4:0x22f4ab99
4:0x0a231f96
4:0x2f962d66
4:0x6077834c
4:0xcf764771
4:0xceaa6d3a
4:0x2dc09306
4:0x9217830d
4:0xba260429
4:0xf54c3405
4:0x151d025b
4:0x9304b177
4:0xe12dc6c4
4:0x4558d05d
4:0x23607c8f
4:0x00bc729c
4:0x53f96ae4
4:0x9cf24fe7
4:0x6884a808
4:0x9f43433e
4:0x38c726d1
4:0x410729c4
4:0x622cc5ab
4:0xeb60b686
4:0x891d94c7
4:0x59bed717
4:0xf75569ec
4:0xba835cae
4:0x28186424
4:0xfcf67e1d
4:0xdf920808
4:0xae6a007f
4:0xec142911
4:0x2c747513
4:0xb11e9c78
4:0x10a7781b
4:0x86145ec6
4:0x86e104ff
4:0xb1393737
4:0xc02c6f11
4:0xa31e0589
4:0x652b7140
4:0xd5c8eff7
4:0xf89da639
4:0xa2c80851
4:0x39cde611
4:0xdbc1c908
4:0x5e5e205c
4:0xbdbd7a63
4:0x23f7bad7
4:0x2e6b2a73
4:0xa27d85c2
4:0xc31f9dd3
4:0x91be5a59
4:0xae2a884c
4:0x3be20959
4:0xc6a9de53
4:0x834c63a2
4:0x5db3c116
4:0x11c9a8c5
4:0x1828d4c2
4:0x289a5d17
4:0x114f7b00
4:0x4ab5a710
4:0x5541b11e
4:0x23942b43
4:0x45a7e13b
4:0xf10bfba9
4:0x1bacad0e
4:0x80844a4c
4:0x037bb696
4:0x9a7ba847
4:0xe852e862
4:0x8d7d711a
4:0xa664e266
4:0xcfa1ab25
4:0x658ad29e
4:0x3be7e931
4:0xe6580169
4:0xab570655
4:0x24268425
4:0x563b4daf
4:0x32a2f63e
4:0x9526521a
4:0xb169f202
4:0x8351cf09
4:0x601655d3
4:0xd5456175
4:0xfacc0f91
4:0x1dac2972
4:0x1ddb2f65
4:0x029936c6
4:0x03e41b19
4:0x77795276
4:0xd1581673
4:0xa154c11d
4:0x4d932af7
4:0x837c0fc3
4:0xf45e67aa
4:0xc83b90a6
4:0xfa017bd4
4:0x6c1f0faa
4:0x26fe45ce
4:0x9b2e8046
4:0x0ab181c1
4:0xf38ab216
4:0x49821e5a
4:0x83f9640f
4:0x42ff6833
4:0x4231e177
4:0xd4c3010b
4:0x10874466
4:0x47a5fbed
4:0x4d5df894
4:0xb3e81461
4:0x2a9f2d40
4:0x71484528
4:0x3cc65dc3
4:0x7dae0101
4:0x210b4ae8
4:0xf2da063c
4:0xabe5a173
4:0xb8019b00
4:0xa434f4fb
4:0x49c3c7f3
4:0xb98b68ac
4:0x81e016b7
4:0x0ac8274d
4:0xd03dbb2d
4:0xea83cc7e
4:0x2c247031
4:0xd7d8101e
4:0x9e6fab44
4:0xc7650e8b
4:0xfc4f73c5
4:0xbb5ffa89
4:0x06ca4aef
4:0xb16968db
4:0x6c148365
4:0x447e7285
4:0x50346c80
4:0xb2f12c9c
4:0x6d56d804
4:0x18301d90
4:0x80dd7b65
4:0x66463158
4:0xf08d3376
4:0x1c16ccc5
4:0x5b259cb0
4:0xcaa398f2
4:0xe5bfd554
4:0x71b956ab
4:0xc2288afb
4:0x8181130d
4:0x3e704e8c
4:0x2337c053
4:0xa4f226cb
4:0x723250dd
4:0x6e24cf58
4:0xc4d10954
4:0xa9ccb603
4:0xb78026db
4:0x8d82bab2
4:0x2adccf99
4:0xce7f387e
4:0x7453a192
4:0x917aa00e
4:0xaa896952
4:0xa2e11df3
4:0xc4dbb87d
4:0x36800c8b
4:0x70fcd472
4:0x34597184
4:0x14e36151
4:0x3fe6e12e
4:0x39bea9ad
4:0x74c9b23c
4:0x46b41d00
4:0xa98b87f3
4:0xc9e37897
4:0x7353ba61
4:0x75612e0b
4:0x8e12198f
4:0xe4a5b722
4:0xaeb193a6
4:0x81e649ed
4:0x848d60c0
4:0xda4038de
4:0x77eca7d9
4:0xa9b7d03a
4:0x8785db8d
4:0x86f54b2d
4:0x7fcd9c38
4:0x1f62a1f0
4:0xcf5ea281
4:0xb92be583
4:0xa298bdb6
4:0x6d71e41f
4:0x02e297c7
4:0x858ea8d7
4:0x58326110
4:0xafde7dc6
4:0x36475f11
4:0xfac9fefa
4:0xc99b8f99
4:0xc331afae
4:0x6ac69f8f
4:0x8371dea4
4:0x32da63ac
4:0x1801a81c
4:0xd112bbbf
4:0x8227e59b
4:0x265d95c2
4:0x44d0783e
4:0x36fc482e
4:0xbcd1b21b
4:0xc36236ba
4:0x7a43231a
4:0x0bc1591a
4:0x4d562b16
4:0x2bc55215
4:0x7870fca7
4:0x5145243d
4:0xce6c2932
4:0xdba6702d
4:0xfcd7974b
4:0x2c8003cf
4:0xb8b9399a
4:0x917876d0
4:0xb528a93f
4:0x2362800f
4:0xd5ecc66c
4:0x8b669809
4:0xe8f5511f
4:0x5eda85c6
4:0x3a285917
4:0x3a857481
4:0x28063e07
4:0xd84f624f
4:0xd8e58f39
4:0x37ea754d
4:0xa826ea96
4:0x9812f32f
4:0x5e80e440
4:0xc0b6dab8
4:0x3e33269c
4:0x70965532
4:0xf850213e
4:0x8ee50acc
4:0xf4aa5cf3
4:0x5aeef8e5
4:0x497327ee
4:0x8002a198
4:0xfeb089d0
4:0xba39d3f0
4:0x6643d050
4:0xc06deed8
4:0x57a6af3a
4:0x0218f3c5
4:0xd359e14c
4:0x62f042e1
4:0x623ef518
4:0x4c98a252
4:0x90b8e009
4:0xdf9ffbe8
4:0x782ed791
4:0xc0b015bb
4:0x0611a9d5
4:0xfc84fefd
4:0x57db4e63
4:0x5b971c18
4:0x10d31ca5
4:0x9a5ca887
4:0xe9e40f77
4:0xc4c748c4
4:0xe34b87ee
4:0x15e30efd
4:0xb3609661
4:0x6ee8c266
4:0xb4448c99
4:0x1b2d0bdd
4:0x98882312
4:0xf16590fe
4:0xdfd1bfc5
4:0x6cc42126
4:0x60d82444
4:0x4c63ef5d
4:0x7b2b9890
4:0x07090baf
4:0xc8cadeed
4:0x908f0b78
4:0x561ca533
4:0xabe8916b
4:0x52f2a24b
4:0x628103c5
4:0x76b09ed8
4:0x8c5ce151
4:0xe9ace6c1
4:0xebc6e080
4:0x68472b8f
4:0x763ba9b9
4:0x3fa7dfc2
4:0xf44889df
4:0x0ffbdf4f
4:0x4af34b94
4:0xe206c0b6
4:0xd7ca77c2
4:0xfad4de4a
4:0x43bae08c
4:0x667aaef5
4:0xde496ade
4:0x0413971e
4:0x3c3d8134
4:0x1a6674f5
4:0xe5bd3b46
4:0x2f447a7f
4:0xb42b17e2
4:0x7399c2a1
4:0x03c122e6
4:0xe6a5fc92
4:0x401ea04e
4:0x8b05def5
4:0x317c6f92
4:0xfa42a10c
4:0xfbdc7212
4:0xde24d417
4:0x45dbcc1a
4:0xad8730d8
4:0xc391f52e
4:0x57fc61a9
4:0x33344174
4:0x7979e6f3
4:0xa19b3988
4:0x6646abb2
4:0x8b04faaa
4:0x980c2669
4:0x73c8f52c
4:0x09719105
4:0xfe6eeae3
4:0xcad1ca27
4:0x40fb98c7
4:0x93b0388d
4:0x43d1286f
4:0x4f77dfce
4:0xce65dee6
4:0x34edab19
4:0x504ff75a
4:0x38e77eaf
4:0xdccd3b17
4:0x26502e6c
4:0x81f22776
4:0x71fa1cff
4:0x486e80ce
4:0x4c05244d
4:0xadb00786
4:0xb7ff0c4d
4:0x8ee8166b
4:0xea85afba
4:0x905c9325
4:0xcf5e71af
4:0x7b100e1c
4:0x7d0f310a
4:0xbddd4f50
4:0x6f3ca0f9
4:0x2fd3cc18
4:0x25b0f6b8
4:0x6c3bbde1
4:0x28ffd510
4:0xfa0cf801
4:0x34b7bd6d
4:0x379a5d7d
4:0xe936ecbe
4:0x744c61a4
4:0x32d6ce2e
4:0x544e7879
4:0xccd37f80
4:0xf0d48197
4:0xcab9311f
4:0xfe2b955e
4:0xb21e7a43
4:0xddbfa3b1
4:0x83c62400
4:0x3cdb86d9
4:0x9a2664b1
4:0x438d1529
4:0x2f787796
4:0x0ce346f6
4:0x1c3f7d14
4:0x8c5a7c42
4:0xb4696997
4:0x2a61f65a
4:0x8c5ff5df
4:0x4cc9ca18
4:0x31f2c260
4:0x3696386a
4:0x3bb71f51
4:0x7d62a702
4:0xb40d51be
4:0x8ef509db
4:0xb0ad1977
4:0xcad1975e
4:0x01bb6256
4:0x1d958be0
4:0x73b1ab45
4:0x53a01c83
4:0x58e89d37
4:0x26339d64
4:0xf9312ee4
4:0xa21ff6cd
4:0x923d14ca
4:0x10e4c158
4:0xbd0e8415
4:0xe27e41de
4:0x133de878
4:0xb21a031d
4:0x369d5e0c
4:0x3de67ead
4:0x5de3fc5e
4:0x673e8533
4:0x230471ad
4:0x8a70c921
4:0x2d60160f
4:0x90a16113
4:0xfce76437
4:0x432527c2
4:0x78b2abcf
4:0xa2ce4155
4:0xd02ef697
4:0xd6e58276
4:0xd573bf2f
4:0x7e7a67d3
4:0xd98ac732
4:0x97c9bd93
4:0x66d2d301
4:0x129caf60
4:0x3ce79c9b
4:0xe23d5cc3
4:0x5b6951c1
4:0xbee565c8
4:0x1aa7a3c5
4:0xa94a6216
4:0x7a8419f0
4:0xdc9fbb3d
4:0xf32124a2
4:0xbec6f7cf
4:0xff579330
4:0x443f8b9d
4:0xaa4446f6
4:0xb556c074
4:0xe582fb0e
4:0x02601ff2
4:0x20237dbc
4:0xdbf5c6a7
4:0x2d98023e
4:0xc6e0142e
4:0x52a57103
4:0xf1beec58
4:0x7f061d36
4:0x71ae697e
4:0x50a2308b
4:0xcf040ee5
4:0x230244e2
4:0x8dd49d65
4:0x5892ac21
4:0x8977d003
4:0x5139bc11
4:0x16626c97
4:0xd47eb49e
4:0xdd4e79c8
4:0x378624af
4:0x4856f76b
4:0xbb76d160
4:0x49ee226a
4:0x28e7099f
4:0x6b5147cf
4:0xd3a33d1e
4:0xf00f177f
4:0x9d50d1b4
4:0x1f316501
4:0x76ad8522
4:0x9c53e72c
4:0x98e1cb05
4:0x2dbf7ad3
4:0xe6415225
4:0xa1b1584a
4:0xbb939131
4:0xd4d91342
4:0xa3065e28
4:0xd44afced
4:0xee79c2b2
4:0x7ba67738
4:0x72d1f3a4
4:0xd4e981fc
4:0xaa745402
4:0x24b5d280
4:0x86c0dafe
4:0x3e6fcc75
4:0xae64ea49
4:0x5e091ffc
4:0x8e21e772
4:0xcd4da6fe
4:0xcf2580d5
4:0xeffa2d29
4:0x52aa417f
4:0xa9a20d52
4:0x066c6b5a
4:0xf1208b78
4:0x76ef8372
4:0x0d516f2e
4:0x660b1e34
4:0x332e7068
4:0xf3bbad3a
4:0x1de404b4
4:0xdd4f45f7
4:0x5409d182
4:0x18924825
4:0x88995b62
4:0x2ebf4d46
4:0x6aeee35b
4:0xf8d3968b
4:0x6cd32db0
4:0x241a7ef3
4:0xffe2ee17
4:0x54449ddf
4:0x1943b8a2
4:0xe60fcd18
4:0x223071ea
4:0x8180510c
4:0x30dd7b21
4:0x91da8b82
4:0x3d6d51b1
4:0xc2c6343a
4:0xcbe59510
4:0x601993d3
4:0x3db1a004
4:0xf19717ad
4:0x22e70861
4:0xb56e2c79
4:0xbded3874
4:0xe664e4df
4:0xe958a655
4:0x60d48591
4:0x6c8021f3
4:0x912121f6
4:0x40f54508
4:0x4edb0b17
4:0xfc2ef0f3
4:0x206293e9
4:0x0b47b587
4:0xc51ecaab
4:0x527de958
4:0x06e66b92
4:0xe10e238e
4:0xac7a492a
4:0xdd9946e5
4:0xd7964630
4:0xa5d8a5f3
4:0x239897e7
4:0x6dc5fc31
4:0xa9cac6be
4:0xa18785f1
4:0xbb365862
4:0x39f231d7
4:0xe213fad6
4:0x77924895
4:0xe8e47515
4:0xb26d0783
4:0x0112fbc0
4:0xe0ab050c
4:0x50d6b568
4:0xe3888bd0
4:0xce075057
4:0x2e9d61e7
4:0x94f0feac
4:0xee2ad870
4:0x69e5a900
4:0xe34b779b
4:0x9114d115
4:0xa0719da5
4:0xaa13980d
4:0x0839432c
4:0xeae055e5
4:0x4e028258
4:0x250c9cc3
4:0xb73868c6
4:0xa509b51d
4:0x59c3368e
4:0x94965196
4:0xd955a99c
4:0x0a2d6c5f
4:0xefe70a45
4:0x32c78086
4:0x562b63b1
4:0x850e9932
4:0xdfc877ac
4:0x1e9d2b87
4:0xf4c75689
4:0x52b60ac7
4:0x97a6cb97
4:0x1a2caece
4:0x3bb10c84
4:0xa7fa6cd1
4:0xb3c08229
4:0x3069e6a5
4:0xe12c6737
4:0x717ff956
4:0x848b51e8
4:0x8eb19712
4:0x3257a5ab
4:0x8ebd9038
4:0x390b88a1
4:0x321ab89e
4:0x20190367
4:0x1361c988
4:0x648797b4
4:0xcae05268
4:0x7e78915c
4:0x791c6c3b
4:0x756f102b
4:0x7355d177
4:0xc4118fa3
4:0xd530cadc
4:0xe70988bb
4:0xc997b839
4:0x10e05ebe
4:0xc1461780
4:0x05cfa10f
4:0x80f7d368
4:0xdc5b7e79
4:0x70a1ef23
4:0x6f7dff30
4:0x7898752a
4:0x388125e1
4:0x5cd387e9
4:0xddaa9804
4:0xd5ab81e0
4:0x50c68667
4:0xec9888a7
4:0x2d3cd3f4
4:0x0f60ecc0
4:0xa66c907a
4:0x1dcf2106
4:0xad38e86d
4:0x34c5ee06
4:0xb5ff174b
4:0x56536366
4:0x62b77681
4:0x4506de31
4:0x91719be0
4:0x6fd302d4
4:0xb616ebcd
4:0x545ca478
4:0xd2565162
4:0xcb1b9477
4:0x6da5aab1
4:0xe303fc6b
4:0x4f95e7dc
4:0x4b8684f5
4:0x74e75adb
4:0x7c7fc02d
4:0xfb86d319
4:0x8eb1ca7d
4:0x2d1373c0
4:0x8a4f5255
4:0xb93f7fec
4:0xc2b1baf3
4:0x066c0f88
4:0x5376c9d8
4:0xc0b1bd81
4:0x0d2eb720
4:0x2a5dcf11
4:0xf843017c
4:0x3039b15f
4:0xc6086d23
4:0xbc17a0c3
4:0xe9a1227d
4:0xae79e73a
4:0x3052d36d
4:0x6c5c41d3
4:0xe6afb234
4:0xe678a78b
4:0x90879ece
4:0x87825d7d
4:0xea094abb
4:0x50ffe533
4:0x5371d66e
4:0xb2850294
4:0xb5bb1479
4:0x7aadf795
4:0x4d57eed5
4:0xbdbe2276
4:0xddffee74
4:0xcd4aeea3
4:0x30c60325
4:0x7dcaaf1f
4:0x0f18fcaf
4:0xe5c1fe2b
4:0x780653b8
4:0xa0dd579e
4:0x4a1bd846
4:0x3d0e8f3c
4:0x0f034860
4:0xa4090ec2
4:0xb4985c49
4:0xc1e2c713
4:0xc9c53c1a
4:0xff3c8dff
4:0xb0f8397d
4:0x79413459
4:0x4d30b4cc
4:0x45cd5f69
4:0x4eb141b2
4:0x6794ebc6
4:0x288c8054
4:0x7b64e98d
4:0xd8fc2b0d
4:0xf1945701
4:0x078ec1d3
4:0xa8030ecc
4:0x76177386
4:0x3c20b8f9
4:0xff2381cc
4:0xd0f13d08
4:0x4a53e679
4:0xe65fc06e
4:0x97871a2a
4:0x551bcb21
4:0xf72d8ba7
4:0x95f2035d
4:0x37268912
4:0x7ac5ff58
4:0x33644d99
4:0x044400b0
4:0x4136bcf4
4:0x716d96b0
4:0x0b026748
4:0x5c7f21a8
4:0xc32ae107
4:0xd65ad089
4:0x72750900
4:0x33b36002
4:0x7c7a62a3
4:0xe7a3208c
4:0xe2ad590d
4:0x06df4eb2
4:0xe4bc7634
4:0x87b57427
4:0x36e12584
4:0xf257acb4
4:0x55d27858
4:0xa28dc040
4:0x68f62d2c
4:0x1f870320
4:0x80e4ba15
4:0x7a7c5c7a
4:0x83aefc5b
4:0xea9945dc
4:0xfcf705c6
4:0x4834175a
4:0xa05b9c16
4:0x29b47f41
4:0xd89fad11
4:0x9161cf34
4:0xd5b2e04c
4:0x73c33825;
DA 4096;
