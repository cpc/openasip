pkglibdir = ${prefix}/tce-llvm/lib
pkglib_LIBRARIES = liboclhost-sa.a 

liboclhost_sa_a_SOURCES = oclhostsa.cc

TCECC_CPPFLAGS = -I${srcdir}/../include
TCECC = ../../src/bintools/Compiler/tcecc

.cc.o:
	$(TCECC) $(TCECC_CPPFLAGS) -g -c -O3 -o $@ $<
