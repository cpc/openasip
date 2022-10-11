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

# This script creates a tar.gz source package from clean _package_dir_name
# directory.  Run this script in the _package_dir_name directory, the actual
# package is created one directory up.
#
# $1    version number for the package

_package_name="tce"
_package_dir_name="tce"
_package_version="1.0beta1"

if [ "${1}x" == "x" ]; then 
    echo "Give version number for the package as the first parameter."
    echo "Using the default package version: ${_package_version}"
else
    _package_version="${1}"
fi

if [ "$(basename "$(pwd)")x" != "${_package_dir_name}x" ]; then
    echo "Run this script under tce root directory.. Exiting."
    exit 1
fi

if [ ! -e "./configure.ac" ]; then
    echo "No configure.ac found.. Exiting."
    exit 1
fi

if [ "$(bzr status .)x" != "x" ]; then
    echo "${_package_dir_name} is not clean.. Exiting."
    exit 1
fi

sed -i 's/DISTRIBUTED_VERSION, false,/DISTRIBUTED_VERSION, true,/g' configure.ac
autoreconf || exit 2

cd ..

tar czf "${_package_name}-${_package_version}.tar.gz" ${_package_dir_name} || exit 2

echo "OK"
