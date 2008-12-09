#!/bin/bash
cd llvm-frontend

if [ ! -e build ]
then
	mkdir build
fi

cd build
../configure --prefix=$LLVM_FRONTEND_DIR $> compile.log
make -j2 1>> compile.log 2>> compile.log || exit 1
make install 1>> compile.log 2>> compile.log || exit 1
exit $?
