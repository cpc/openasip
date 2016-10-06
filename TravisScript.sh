export WORKDIR=$PWD
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
#tcl tk install region
wget --no-check-certificate http://prdownloads.sourceforge.net/tcl/tcl8.6.6-src.tar.gz
tar -xzf tcl8.6.6-src.tar.gz
cd tcl8.6.6/unix
./configure --prefix=$INSTALLDIR/tcl
make install
export PATH="$INSTALLDIR/tcl/bin:$PATH"
wget --no-check-certificate http://prdownloads.sourceforge.net/tcl/tk8.6.6-src.tar.gz
tar -xzf tk8.6.6-src.tar.gz
cd tk8.6.6/unix
./configure --prefix=$INSTALLDIR/tcl
make install
#tcl tk install region
fi
cd $WORKDIR/tce
./tools/scripts/install_llvm_$LLVM_VER_BUILD.sh $INSTALLDIR/llvm
export PATH="$HOME:$INSTALLDIR/llvm/bin:$PATH"
./gen_llvm_shared_lib.sh
./autogen.sh

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
./configure --prefix=$INSTALLDIR --with-tcl=$INSTALLDIR/tcl
else
./configure --prefix=$INSTALLDIR
fi
cat config.log

make
make install
