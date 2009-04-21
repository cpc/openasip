
Installing the frontend
------------------------

1. Make separate build directory.
   mkdir tce-frontend-build
   cd tce-frontend-build

2. Configure the compiler as following.

   <tce-frontend-dir>/configure --prefix=<installdir> --with-llvm-gcc-sources=<llvm-gcc-4.2-sources> --enable-languages=c,c++ --enable-llvm=<llvm-installdir> --target=tce-llvm --disable-bootstrap --disable-multilib --disable-libgloss

3. Make and install
   make
   make install