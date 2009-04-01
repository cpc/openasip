AC_INIT(LLVM-FRONTEND, 1.0, tce@cs.tut.fi)
AC_PREREQ(2.13)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([1.9 foreign])

if test "`which llvm-config`" = ""; then
   AC_MSG_ERROR([llvm-config was not found from PATH, please install llvm.])
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

## Apply tce patch to llvm-gcc if dir is not already patched
AC_CHECK_FILE($use_llvm_gcc/gcc/config/tce/tce.h, [],
    [AC_MSG_NOTICE([Patching llvm-gcc with tce target. Patch will not hurt old functionality of sources.]);
     cd $use_llvm_gcc;
     patch -p1 < $srcdir/llvm-gcc-tce-target.diff;
     AC_CHECK_FILE($use_llvm_gcc/gcc/config/tce/t-tce, 
         [AC_MSG_NOTICE([Patch applied to $use_llvm_gcc.])],
         [AC_MSG_ERROR([Failed patching llvm-gcc sources.])])
])

## Lets set target tools for compiling gcc and newlib
m4_pattern_allow([^AS_FOR_TARGET$])
llvmbindir=`llvm-config --bindir`
set > check_here.txt
export CC_FOR_TARGET=$ac_pwd/llvm-gcc-4.2/gcc/xgcc 
export AR_FOR_TARGET=$llvmbindir/llvm-ar 
export RANLIB_FOR_TARGET=$llvmbindir/llvm-ranlib 
export AS_FOR_TARGET=$llvmbindir/llvm-as 
export LD_FOR_TARGET=$llvmbindir/llvm-ld 
export NM_FOR_TARGET=$llvmbindir/llvm-nm 
export CFLAGS_FOR_TARGET="-emit-llvm -B$ac_pwd/llvm-gcc-4.2/gcc -ffreestanding -fno-unit-at-a-time -fno-inline"

# This does not work because AC_CONFIG_SUBDIRS passes extra 'target_alias=tce-llvm' parameter to configure and gcc (also mainline) 
# tries to recognize tce-llvm to be also --host and --build system. 

# AC_CONFIGURE_LINKS needs relative path so we do it manually
#AC_CONFIG_COMMANDS_POST (
#    [ln -s $use_llvm_gcc $srcdir/llvm-gcc-4.2])
#AC_CONFIG_SUBDIRS(llvm-gcc-4.2)

# Our replacement...
AC_CONFIG_COMMANDS_POST ([
echo "=== Configuring llvm-gcc-4.2 =====";
# Make ac_configure args to be nice for passing them to gcc configure.
conf_args_wo_target_alias=`echo $ac_configure_args | sed '$s/ *\([^ ]* *\)$//' | tr -d "'"` ;
mkdir -p llvm-gcc-4.2; 
cd llvm-gcc-4.2; 
echo $use_llvm_gcc/configure $conf_args_wo_target_alias;
$use_llvm_gcc/configure $conf_args_wo_target_alias;
])

AC_CONFIG_SUBDIRS(newlib-1.17.0)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
