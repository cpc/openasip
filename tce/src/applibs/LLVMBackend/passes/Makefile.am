pkglib_LTLIBRARIES = LowerIntrinsics.la LinkBitcode.la MachineDCE.la

LowerIntrinsics_la_SOURCES = LowerIntrinsics.cc 
LowerIntrinsics_la_LDFLAGS = -module 

LinkBitcode_la_SOURCES = LinkBitcode.cc 
LinkBitcode_la_LDFLAGS = -module

MachineDCE_la_SOURCES = MachineDCE.cc MachineDCE.hh
MachineDCE_la_LDFLAGS = -module

AM_CPPFLAGS = ${LLVM_CPPFLAGS}

# gives automake warning, but is necessary to disable
# compilation warnings from llvm headers...
AM_CXXFLAGS = ${CXXFLAGS} 
CXXFLAGS=-Wno-unused-parameter -g
