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
