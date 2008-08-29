#!/bin/bash
# Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
#
# This file is part of TTA-Based Codesign Environment (TCE).
#
# TCE is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License version 2 as published by the Free
# Software Foundation.
#
# TCE is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin St,
# Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this file
# does not by itself cause the resulting executable to be covered by the GNU
# General Public License.  This exception does not however invalidate any
# other reasons why the executable file might be covered by the GNU General
# Public License.

# This script creates a tar.gz source package from clean tce directory.
# Run this script in the tce root directory, the package is created one 
# directory up
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
    echo "Run this script under tce root directory.. exiting."
    exit 1
fi

if [ ! -e "./configure.ac" ]; then
    echo "No configure.ac found.. exiting."
    exit 1
fi

if [ -e "./configure" ]; then
    echo "Script configure found in tce root, tce root is not clean, exiting.."
    exit 1
fi

autoreconf || exit 2

cd ..

tar cvvzf "${_package_name}-${_package_version}.tar.gz" tce || exit 2

echo "OK"
