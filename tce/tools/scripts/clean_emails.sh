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

# This script changes all commented @author lines in non binary files with 
# obfuscated email address. In this case '@' is replaced with '-no.spam-".
# Files are found recursively starting from working directory.
# note: iconv might be needed in case of utf8 files.

SEARCH_DIR="."

FILES=($(\grep -IRilZE '[[:space:]]*(\*|//)[[:space:]]+@author' ${SEARCH_DIR} | xargs -0))
for file in ${FILES[@]}; do 
{ \rm $file && awk ' 
    BEGIN { nop=1 }
    /[[:space:]]*(\*|\/\/)[[:space:]]+@author/ { 
        print gensub("@", "-no.spam-", 2)
        nop=0
    }

    { 
        if (nop == 1) {
            print $0
        }
        nop=1
    }
    ' > $file
} < $file
done
