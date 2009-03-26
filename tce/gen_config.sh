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
#
# Generates the 'configure' script for TCE.
# 
# Uses kludges to get past issues between libtool versions etc.
#

rm -f acinclude.m4

autoreconf -i --force

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
