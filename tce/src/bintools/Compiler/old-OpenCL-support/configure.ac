#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(llvmopencl, 20091026, carlos.delalama@urjc.es)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(foreign)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_TESTDIR([tests])
AC_ARG_VAR(LLVMCC, [LLVM-enabled C compiler.])
AC_ARG_VAR(LLVMCFLAGS, [extra flags for LLVM C compiler.])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_PATH_PROG(LO_LLVM_CONFIG, llvm-config, AC_MSG_ERROR([installation of LLVM is required.]))
AC_PATH_PROGS(LLVMCC, [tcecc clang llvm-gcc], AC_MSG_ERROR([C-to-bytecode compiler is required.]))
AM_CONDITIONAL([CLANG], [test `basename $LLVMCC` = clang])

# Checks for libraries.

LLVM_VERSION=

AC_ARG_WITH(llvm,
[AS_HELP_STRING([--with-llvm],[absolute path to an LLVM installation])],
[llvmDir="${withval}"
 llvmConf="$llvmDir/bin/llvm-config"
],
[llvmDir=""
 llvmConf="$(which llvm-config)"
]
)

if test x"$llvmConf" == x; then
AC_MSG_NOTICE([
  Cannot find LLVM installation. Either LLVM is not
  installed at all or development support for it is missing. On certain
  systems, development support comes in separate '-dev' packages.])
else
 
AC_CHECK_FILE($llvmConf, 
[
LLVM_VERSION=$($llvmConf --version)
LLVM_LIBDIR=$($llvmConf --libdir)
LLVM_SHARED_LIB=$LLVM_LIBDIR/libLLVM-$LLVM_VERSION$LIBRARY_SUFFIX
LLVM_CPPFLAGS=$($llvmConf --cppflags)
LLVM_LDFLAGS="$($llvmConf --ldflags) $LLVM_SHARED_LIB"
LLVM_INCLUDEDIR=$($llvmConf --includedir)

# Hack for debian sid packages
if test x"$LLVM_INCLUDEDIR" == x/usr/include/llvm; then
LLVM_INCLUDEDIR=/usr/include
fi

case "$LLVM_VERSION" in
     2.7)
     AC_DEFINE([LLVM_2_7], [], "Using LLVM v2.7")
   ;;
     2.8)
     AC_DEFINE([LLVM_2_8], [], "Using LLVM v2.8")
   ;;
     2.9)
     AC_DEFINE([LLVM_2_9], [], "Using LLVM v2.9")
   ;;
     *svn)
     AC_DEFINE([LLVM_SVN], [], "Using LLVM svn")
   ;;   
     *)
   AC_MSG_NOTICE(
   [
Unsupported LLVM version. 
   ])
   LLVM_VERSION=
   ;;
esac
],

[
AC_MSG_NOTICE([
  You have to install LLVM to be able to compile from C.])
])

fi

AC_DEFINE_UNQUOTED([LLVM_VERSION], ["$LLVM_VERSION"], 
"The LLVM version string.")

AC_SUBST([LLVM_VERSION], ["$LLVM_VERSION"])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 examples/oclADDF8x8/Makefile.native
                 examples/oclADDF8x8/Makefile
                 include/Makefile
                 include/CL/Makefile
                 lib/Makefile
                 lib/CL/Makefile
                 scripts/Makefile
                 tests/Makefile
                 tests/atlocal])

AC_OUTPUT
