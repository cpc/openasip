#!/bin/bash
# Copyright (c) 2002-2009 Tampere University of Technology.
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

# Set these optional variables, if reposity is not checkouted or configured yet
#
# DEFAULT_CONFIGURE_COMMAND
# SVN_REPO_URL

# example variable settings:
# 
# SVN_REPO_DIR="/home/elhigu/stow_sources/tce-1.0-llvm-svn/tce/tools/scripts/buildbot/test_update_svn_instalation/chekcout"
# SVN_REPO_URL="http://llvm.org/svn/llvm-project/llvm/trunk"
# DEFAULT_CONFIGURE_COMMAND="/home/elhigu/stow_sources/tce-1.0-llvm-svn/tce/tools/scripts/buildbot/test_update_svn_instalation/chekcout/configure --enable-optimized --prefix=/home/elhigu/stow_sources/tce-1.0-llvm-svn/tce/tools/scripts/buildbot/test_update_svn_instalation/install"
# BUILD_DIR="/home/elhigu/stow_sources/tce-1.0-llvm-svn/tce/tools/scripts/buildbot/test_update_svn_instalation/build"

# check that required parameters are defined SVN_REPO_DIR, BUILD_DIR 

# TODO: check that directories are absolute (first letter should be /)

if [ -z ${SVN_REPO_DIR} ]
then
    echo "ERROR: Define SVN_REPO_DIR where svn reposity will be checkouted if not already exists."
    return 1;
fi

if [ -z ${BUILD_DIR} ]
then
    echo "ERROR: Define BUILD_DIR where sources are configured and built."
    return 1;
fi

# make repo dir if not exist
if [ ! -d ${BUILD_DIR} ] 
then
    if [ -z ${SVN_REPO_URL} ]
    then
        echo "ERROR: Define SVN_REPO_URL where from reposity will be checkouted."
        return 1;
    fi
    echo "---------- SVN Checkout ---------------"
    echo "svn co ${SVN_REPO_URL} ${SVN_REPO_DIR}"
    svn co ${SVN_REPO_URL} ${SVN_REPO_DIR}
fi

cd ${SVN_REPO_DIR}

if [ ! -d .svn ]
then
    echo "${SVN_REPO_DIR} directory is not valid svn checkout"
    return 1
fi

# update sources
echo "---------- Updating svn directory ----------"
echo "$PWD\$ svn up"
svn up

# go to build dir
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

# clean installation (make uninstall)
echo "---------- Trying to uninstall old installation ----------"
echo "$PWD\$ make uninstall"
make uninstall

# get configure command if not defined
if [ -z "${DEFAULT_CONFIGURE_COMMAND}" ]
then
    if [ ! -e config.log ]
    then
        echo "ERROR: old configure was not found set DEFAULT_CONFIGURE_COMMAND variable!"
        return 1;
    else
        # NOTE: This is a bit fragile
        DEFAULT_CONFIGURE_COMMAND=`head -7 config.log|tail -1|cut -b5-`
        echo 'Found old configure command to use: '${DEFAULT_CONFIGURE_COMMAND}
    fi
fi


# run configure and install
echo "---------- Running configure ----------"
echo "$PWD\$" ${DEFAULT_CONFIGURE_COMMAND}
${DEFAULT_CONFIGURE_COMMAND}

echo "---------- Make and install ----------"
echo "$PWD\$ make"
make REQUIRES_RTTI=1 
make install
