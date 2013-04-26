pkglibdir = ${prefix}/tce-llvm/lib
pkglib_LIBRARIES = libcpp.a 

libcpp_a_SOURCES = exception_handling.c terminate.cc

RANLIB = @LLVM_BINDIR@/llvm-ranlib
AR = @LLVM_BINDIR@/llvm-ar

TCECC_CPPFLAGS = -I$(srcdir)/../include
TCECC = ../../src/bintools/Compiler/tcecc

.c.o:
	$(TCECC) $(TCECC_CPPFLAGS) -g -c -O3 -o $@ $<

.cc.o:
	$(TCECC) $(TCECC_CPPFLAGS) -g -c -O3 -o $@ $<
