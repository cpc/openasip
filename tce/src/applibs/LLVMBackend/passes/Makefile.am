pkglib_LTLIBRARIES = LowerIntrinsics.la LinkBitcode.la MachineDCE.la

LowerIntrinsics_la_SOURCES = LowerIntrinsics.cc 
# Static linking here is needed in FreeBSD and possibly other
# targets were --enable-shared does not compile llvm. In that
# case the TCE script is used to generate the LLVM dynamic library.
# This means that opt or llc do not have a reference to that
# dynamic library thus cannot find the symbols needed when the
# LowerIntrinsics.so is loaded in tcecc.
LowerIntrinsics_la_LDFLAGS = -module ${LLVM_LDFLAGS} -lLLVMCodeGen

LinkBitcode_la_SOURCES = LinkBitcode.cc 
LinkBitcode_la_LDFLAGS = -module

MachineDCE_la_SOURCES = MachineDCE.cc MachineDCE.hh
MachineDCE_la_LDFLAGS = -module

AM_CPPFLAGS = ${LLVM_CPPFLAGS}

# gives automake warning, but is necessary to disable
# compilation warnings from llvm headers...
AM_CXXFLAGS = ${CXXFLAGS} 
CXXFLAGS=-Wno-unused-parameter -g

## headers start
LowerIntrinsics_la_SOURCES += \
	MachineDCE.hh \
	RegisterClassInfo.h Spiller.h \
	LiveDebugVariables.h VirtRegRewriter.h \
	VirtRegMap.h LiveRangeEdit.h 
## headers end
