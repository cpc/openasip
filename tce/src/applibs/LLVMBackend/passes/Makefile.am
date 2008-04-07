BUILD_PASS = ./buildllvmpass

SRC_ROOT_DIR = ../../..

basedir = ${prefix}/share/tce/llvm/passes/
nobase_base_DATA = LowerIntrinsics.so

.PHONY: ${nobase_base_DATA}

all-local: LowerIntrinsics.so

LowerIntrinsics.so: LowerIntrinsics.cc
	${BUILD_PASS} LowerIntrinsics.so LowerIntrinsics.cc

clean:
	rm -f *.so *~
