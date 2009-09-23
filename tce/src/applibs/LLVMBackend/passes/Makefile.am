pkglib_LTLIBRARIES = LinkBitcode.la MachineDCE.la

LinkBitcode_la_SOURCES = LinkBitcode.cc
LinkBitcode_la_LDFLAGS = -module

MachineDCE_la_SOURCES = MachineDCE.cc
MachineDCE_la_LDFLAGS = -module

AM_CPPFLAGS = ${LLVM_CPPFLAGS}
AM_LDFLAGS = ${LLVM_LDFLAGS} -lLLVMCodeGen

# gives automake warning, but is necessary to disable
# compilation warnings from llvm headers...
AM_CXXFLAGS = ${CXXFLAGS} -Wno-unused-parameter
CXXFLAGS=""
