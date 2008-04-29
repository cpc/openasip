AC_PREREQ(2.61)

AC_INIT(LLVM-FRONTEND, 1.0, tce@cs.tut.fi)

AM_INIT_AUTOMAKE([1.9 foreign])

AC_PROG_INSTALL
AC_PROG_YACC

if test "`which llvm-config`" = ""; then
   AC_MSG_ERROR([llvm-config was not found from PATH, please install llvm-2.1.])
fi

if test "$ac_cv_prog_YACC" = ""; then
   AC_MSG_ERROR([YACC not found, install bison or yacc.])
fi

# If we're building in the source directory, error.
if test "${srcdir}" = "." ; then
  AC_MSG_ERROR([Building in the source directory is not supported in this release. Make separate directory for building the system. See http://gcc.gnu.org/install/configure.html for more details.])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

if test ! "${srcdir}" = "." ; then
# configure llvm-gcc with special switches for TCE target
abssrcroot=`cd $srcdir;pwd`
echo "ABS: $abssrcroot"
#mkdir -p llvm-gcc4-2.0;cd llvm-gcc4-2.0;$abssrcroot/llvm-gcc4-2.0/configure --prefix=$prefix --enable-llvm=`llvm-config --prefix` --enable-languages=c,c++ --disable-shared --disable-multilib --build=i686-pc-linux-gnu --target=tce-llvm --with-as=llvm-as --with-ld=llvm-ld --with-rablib=llvm-ranlib --with-ar=llvm-ar --with-nm=llvm-nm --with-newlib --with-libgloss --disable-libstdcxx-pch --disable-newlib-io-long-long --enable-newlib-io-pos-args --disable-newlib-io-long-double --enable-newlib-hw-fp 

mkdir -p llvm-gcc4.2;cd llvm-gcc4.2;$abssrcroot/llvm-gcc4.2/configure --prefix=$prefix --enable-llvm=`llvm-config --prefix` --enable-languages=c --target=tce-llvm --disable-libssp --disable-multilib --with-build-time-tools=$abssrcroot/fake-binutils --with-newlib --with-libgloss --disable-libstdcxx-pch --disable-newlib-io-long-long --enable-newlib-io-pos-args --disable-newlib-io-long-double --enable-newlib-hw-fp 

fi
