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
#
# Generates the 'configure' script for TCE.
# 
# Uses kludges to get past issues between libtool versions etc.
#

rm -f acinclude.m4

autoreconf

# In case of libtool 2.x (at least in Ubuntu Intrepid), m4 dir
# should now contain libtool.m4. If that's the case, we do not
# need to include libtool.m4 in autoconf as it's found from there.
# Otherwise, rename the old working acinclude.m4 containing the
# libtool.m4 for libtool1.5x.
#
# TODO: find out why the new libtool script produces the linkage
# error with the older Ubuntu's (hidden 'stat' symbol or something).
if ! test -e m4/libtool.m4;
then

echo Assuming acinclude.m4 is needed. Using the old one.
ln -s acinclude.m4.lt15 acinclude.m4

else

echo Assuming acinclude.m4 is not needed.

fi
