bin_PROGRAMS = tce-llvm-ld
tce_llvm_ld_SOURCES = llvm-ld.cpp Optimize.cpp
AM_CXXFLAGS = `@LLVM_CONFIG@ --cxxflags`
AM_LDFLAGS =  `@LLVM_CONFIG@ --ldflags` -lLLVM-`@LLVM_CONFIG@ --version`



