#!/bin/bash

export PATH=$LLVM_DIR/bin:$LLVM_FRONTEND_DIR/bin:$PATH
cd tce
tools/scripts/compiletest.sh -q -c $> test.log

# if there were errors
if [ -s compiletest.error.log ]
then
	exit 1
fi

exit 0
