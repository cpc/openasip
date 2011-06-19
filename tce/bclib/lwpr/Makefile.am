pkglibdir = ${prefix}/tce-llvm/lib
pkglib_LIBRARIES = liblwpr.a 

liblwpr_a_SOURCES = lwpr.c

RANLIB = @LLVM_BINDIR@/llvm-ranlib
AR = @LLVM_BINDIR@/llvm-ar

TCECC_CPPFLAGS = -I$(srcdir)/../include
TCECC = ../../src/bintools/Compiler/tcecc

.c.o:
	$(TCECC) $(TCECC_CPPFLAGS) -g -c -O3 -o $@ $<
