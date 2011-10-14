pkglib_LIBRARIES = liboclhost.a liboclkernel.a

liboclhost_a_SOURCES = oclhost.cc

liboclkernel_a_SOURCES = oclkernel.cc
RANLIB = `@LO_LLVM_CONFIG@ --bindir`/llvm-ranlib
AR = `@LO_LLVM_CONFIG@ --bindir`/llvm-ar

AM_CPPFLAGS = -I$(top_srcdir)/include

if CLANG
.cc.o:
	$(LLVMCC) $(LLVMCFLAGS) $(AM_CPPFLAGS) -c -emit-llvm -o $@ $<
else
.cc.o:
	$(LLVMCC) $(LLVMCFLAGS) $(AM_CPPFLAGS) -c --emit-llvm -o $@ $<
endif
