cd tce
./tools/scripts/install_llvm_$LLVM_VER_BUILD.sh $INSTALLDIR/llvm
export PATH="$HOME:$INSTALLDIR/llvm/bin:$PATH"
if [ ! -f `llvm-config --libdir`/libLLVM-* ]; then ./gen_llvm_shared_lib.sh ;fi
./autogen.sh
./configure --prefix=$INSTALLDIR
make
make install
