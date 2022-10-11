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
# Uses 'astyle' to format the code to cleanup some of the
# issues not matching the style code. Does not fix everything.
#
# The filename is given as argument.
#
# requires astyle v.1.9 or above

target=$(mktemp)
backupdir=$(mktemp -d)

# -a = Attach brackets (if (something) { <-)
# -p = Insert space paddings around operators only: a+b -> a + b
# 
# -U = Remove empty spaces from if brackets: if ( foo ) -> if (foo)
# -V = Convert tabs to spaces.
# -s4 = 4 spaces per indentation
# -S = Indent switch case-statements.


cp $1 $backupdir/$1
echo "backup written to $backupdir"
astyle -apUV -s4 -S < $1 > $target && mv $target $1 


