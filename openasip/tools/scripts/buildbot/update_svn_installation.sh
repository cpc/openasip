#!/bin/bash
# Copyright (c) 2002-2009 Tampere University.
#
# This file is part of TTA-Based Codesign Environment (TCE).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

#
# Checkouts or updates requested SVN repo revision to requested directory
# and runs configure, make and make install to repo.
#
# Set following variables:
#
# SVN_REPO_DIR="/tmp/llvm-trunk"
# BUILD_DIR="/tmp/llvm-build"
# SVN_REPO_URL="http://llvm.org/svn/llvm-project/llvm/trunk"
# CLANG_REPO_URL="http://llvm.org/svn/llvm-project/cfe/trunk"
# SVN_REV="-r 8301" empty if latest revision is wanted
# CONFIGURE_COMMAND="$SVN_REPO_DIR/configure --enable-optimized --enable-shared --prefix=/tmp/llvm_install"

#
# author Mikael Lepistö 2009 
#

# check that required parameters are defined SVN_REPO_DIR, BUILD_DIR 

# TODO: check that directories are absolute (first letter should be /)
if [ -z ${SVN_REPO_DIR} ]
then
    echo "ERROR: Define SVN_REPO_DIR where svn reposity will be checkouted if not already exists."
    exit 1;
fi

if [ -z ${BUILD_DIR} ]
then
    echo "ERROR: Define BUILD_DIR where sources are configured and built."
    exit 1;
fi

# make repo dir if not exist
if [ ! -d ${BUILD_DIR} ] 
then
    if [ -z ${SVN_REPO_URL} ]
    then
        echo "ERROR: Define SVN_REPO_URL where from reposity will be checkouted."
        exit 1;
    fi
    echo "---------- SVN Checkout ---------------"
    echo "svn co ${SVN_REPO_URL} ${SVN_REPO_DIR}"
    svn co ${SVN_REPO_URL} ${SVN_REPO_DIR}
    svn co ${CLANG_REPO_URL} ${SVN_REPO_DIR}/tools/clang
fi

cd ${SVN_REPO_DIR}

if [ ! -d .svn ]
then
    echo "${SVN_REPO_DIR} directory is not valid svn checkout"
    exit 1
fi

# update sources
echo "---------- Updating svn directory ----------"
echo "$PWD\$ svn up $SVN_REV"
svn up $SVN_REV

echo "---------- Updating clang ----------"
cd tools/clang
echo "$PWD\$ svn up $SVN_REV"
svn up $SVN_REV
cd ../..

# go to build dir
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

# clean installation (make uninstall)
echo "---------- Trying to uninstall old installation ----------"
echo "$PWD\$ make uninstall"
make uninstall

# get configure command if not defined
if [ -z "${CONFIGURE_COMMAND}" ]
then
    if [ ! -e config.log ]
    then
        echo "ERROR: old configure was not found set CONFIGURE_COMMAND variable!"
        exit 1;
    else
        # NOTE: This is a bit fragile
        CONFIGURE_COMMAND=`head -7 config.log|tail -1|cut -b5-`
        echo 'Found old configure command to use: '${CONFIGURE_COMMAND}
    fi
fi

# run configure and install
echo "---------- Running configure ----------"
echo "$PWD\$" ${CONFIGURE_COMMAND}
${CONFIGURE_COMMAND}

echo "---------- Make and install ----------"
echo "$PWD\$ make REQUIRES_RTTI=1 -j 3"
make REQUIRES_RTTI=1 -j 3 > build.log 
make install >install.log
