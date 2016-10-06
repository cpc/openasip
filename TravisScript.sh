export WORKDIR=$PWD

#tcl tk install region
wget --no-check-certificate http://prdownloads.sourceforge.net/tcl/tcl8.6.6-src.tar.gz
tar -xzf tcl8.6.6-src.tar.gz
cd tcl8.6.6/unix
./configure --prefix=$INSTALLDIR/tcl
make install
export PATH="$INSTALLDIR/tcl/bin:$PATH"
wget --no-check-certificate http://prdownloads.sourceforge.net/tcl/tk8.5.19-src.tar.gz
tar -xzf tk8.5.19-src.tar.gz
cd tk8.5.19/unix
./configure --prefix=$INSTALLDIR/tk
make install
export PATH="$INSTALLDIR/tk/bin:$PATH"
#tcl tk install region

cd $WORKDIR/tce
./tools/scripts/install_llvm_$LLVM_VER_BUILD.sh $INSTALLDIR/llvm
export PATH="$HOME:$INSTALLDIR/llvm/bin:$PATH"
if [ ! -f '`llvm-config --libdir`/libLLVM-*' ]; then ./gen_llvm_shared_lib.sh ;fi
./autogen.sh
./configure --prefix=$INSTALLDIR
make
make install
