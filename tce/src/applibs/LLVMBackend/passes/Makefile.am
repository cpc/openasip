BUILD_PASS = ./buildllvmpass

SRC_ROOT_DIR = $(top_srcdir)/src

basedir = ${prefix}/share/tce/llvm/passes/
nobase_base_DATA = LowerIntrinsics.so LinkBitcode.so MachineDCE.so

.PHONY: ${nobase_base_DATA}

all-local: LowerIntrinsics.so LinkBitcode.so MachineDCE.so

LowerIntrinsics.so: LowerIntrinsics.cc
	${BUILD_PASS} $@ $^

LinkBitcode.so: LinkBitcode.cc
	${BUILD_PASS} LinkBitcode.so LinkBitcode.cc

MachineDCE.so: MachineDCE.cc
	${BUILD_PASS} MachineDCE.so MachineDCE.cc

clean:
	rm -f *.so *~
