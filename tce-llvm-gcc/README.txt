tce-llvm-gcc cross-compiler installer
-------------------------------------

Compiles tce-llvm-gcc cross-compiler which tcecc uses for doing compiling C 
sources to llvm bitcode. This (or clang) must be installed before newlib can 
be compiled.

0. Download llvm-gcc sources for your tce installation llvm.org

1. Make separate build directory.
   mkdir tce-llvm-gcc-build
   cd tce-llvm-gcc-build

2. Configure the compiler as following.

   <tce-frontend-dir>/configure --prefix=<installdir> --with-llvm-gcc-sources=<llvm-gcc-4.2-sources>

3. Make and install
   make
   make install

This installer is just made for making building cross-compiler easier. You can
also get llvm-gcc sources (and patch them if the sources are older than llvm-2.6)
and compile them directly with following commands.

llvmbindir=`llvm-config --bindir`
AR_FOR_TARGET=$llvmbindir/llvm-ar 
RANLIB_FOR_TARGET=$llvmbindir/llvm-ranlib 
AS_FOR_TARGET=$llvmbindir/llvm-as 
LD_FOR_TARGET=$llvmbindir/llvm-ld 
NM_FOR_TARGET=$llvmbindir/llvm-nm 
llvm-gcc4.2-sources/configure --prefix=<installdir> --target=tce-llvm --enable-languages=c,c++ --disable-bootstrap --enable-llvm=<llvm-installdir>
