AC_INIT(LLVM-FRONTEND, 1.1, tce@cs.tut.fi)
AC_PREREQ(2.13)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.9 foreign])

if test "`which llvm-config`" = ""; then
   AC_MSG_ERROR([llvm-config was not found from PATH, please install llvm.])
else
   llvm_install_dir=`llvm-config --prefix`
fi

# If we're building in the source directory, error.
if test "${srcdir}" = "." ; then
  AC_MSG_ERROR([Building in the source directory is not supported in this release. Make separate directory for building the system. See http://gcc.gnu.org/install/configure.html for more details.])
fi

# Input llvm-gcc source dir
AC_ARG_WITH(
    [llvm-gcc-sources],
    [AS_HELP_STRING(
        [--with-llvm-gcc-sources],
        [Path to llvm-gcc-4.2 sources which are used for building frontend.])],
    [use_llvm_gcc=$withval], [])

## Check that dir is ok.
AC_CHECK_FILE($use_llvm_gcc/gcc/llvm-backend.cpp, [], 
    [AC_MSG_ERROR([Invalid llvm-gcc dir. Give path to your llvm-gcc sources with e.g. --with-llvm-gcc-sources=/usr/sources/llvm-gcc-4.2])])

# set prefix always to be something
if test x"$prefix" = xNONE; then
  prefix=/usr/local
fi

## Lets set target tools for compiling gcc
m4_pattern_allow([^AS_FOR_TARGET$])
llvmbindir=`llvm-config --bindir`
export AR_FOR_TARGET=$llvmbindir/llvm-ar 
export RANLIB_FOR_TARGET=$llvmbindir/llvm-ranlib 
export AS_FOR_TARGET=$llvmbindir/llvm-as 
export LD_FOR_TARGET=$llvmbindir/llvm-ld 
export NM_FOR_TARGET=$llvmbindir/llvm-nm 

# Configure llvm-gcc as cross compiler 
AC_CONFIG_COMMANDS_POST ([
echo "=== Configuring llvm-gcc-4.2 =====";
# Make ac_configure args to be nice for passing them to gcc configure.
mkdir -p llvm-gcc-4.2; 
cd llvm-gcc-4.2; 
echo $use_llvm_gcc/configure --prefix=$prefix --target=tce-llvm --enable-languages=c,c++ --disable-bootstrap --enable-llvm=$llvm_install_dir;
$use_llvm_gcc/configure --prefix=$prefix --target=tce-llvm --enable-languages=c,c++ --disable-bootstrap --enable-llvm=$llvm_install_dir;
])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
