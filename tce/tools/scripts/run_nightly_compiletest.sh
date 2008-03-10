#!/bin/bash
#
# Crontab entry should for executing this script be something like:
#
# 0 4,13 * * 1,2,3,4,5 ~/svnroot/trunk/tce/tools/scripts/run_nightly_compiletest.sh > /dev/null 
#
# You should have a TCE and LLVM/TCE installation at $HOME/tce-installation before
# executing this script the first time, otherwise systemtests that test 'tcecc'
# will fail. The script reinstalls TCE and LLVM/TCE only after the compiletest 
# to avoid extra recompilations.

export INSTALLATION_PATH=$HOME/tce-installation
export PATH="$INSTALLATION_PATH/bin:/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin"
export LD_LIBRARY_PATH=$INSTALLATION_PATH/lib

cd ~/svnroot
cd trunk
echo "Fetching updates from SVN..."
svn up
cd tce
autoreconf > /dev/null 2>&1

export ERROR_MAIL=yes
export ERROR_MAIL_ADDRESS=tce-logs@cs.tut.fi
export CXXFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros"
export TCE_CONFIGURE_SWITCHES="--prefix=$INSTALLATION_PATH --disable-python"

tools/scripts/compiletest.sh $*

# (Re)install TCE for building LLVM.
rm -fr $INSTALLATION_PATH

echo "Installing TCE..."
make install > /dev/null 2>&1
tools/scripts/install_all_headers

# Recompile and reinstall LLVM/TCE.

cd ~/svnroot/trunk/llvm-frontend
autoreconf > /dev/null 2>&1
mkdir -p build && cd build

# Only single word compilation commands are tolerated by the LLVM build scripts.
export CXX=g++
export CXXFLAGS="-O1 -g"
export CC=gcc
TARGET=$(tce-config --prefix)


function eexit {
    echo $1 >&2
    exit 1
}

echo "Installing LLVM/TCE to $TARGET..."
../configure --prefix=$TARGET >/dev/null 2>&1 || eexit "Configuration of LLVM/TCE failed!" 
make -s >/dev/null 2>&1 || eexit "Compilation of LLVM/TCE failed!" 
make install >/dev/null 2>&1 || eexit "Installation of LLVM/TCE failed!"

tcecc --clear-plugin-cache
