bin_PROGRAMS = tce-llvm-ld
tce_llvm_ld_SOURCES = llvm-ld.cpp Optimize.cpp
tce_llvm_ld_LDADD = ../../libtce.la

PROJECT_ROOT = $(top_srcdir)

AM_CXXFLAGS = `@LLVM_CONFIG@ --cxxflags`
AM_CPPFLAGS = -I${PROJECT_ROOT} # Needed for config.h
AM_LDFLAGS = ${TCE_LDFLAGS}



