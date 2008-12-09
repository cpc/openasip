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


function remLicense {

    # create temp file for awk
    TMPFILE=$(mktemp) 
    if [ "$?" != 0 ]; then
        echo "Error tmp file creation failed."
        exit 1
    fi

awk '
    /^\/\*$/ { 
        if (found < 1) {
            getline temp_line
            if (index(temp_line, "Copyright 2002-2008 Tampere")) {
                cpp_copyr = 1
                ++found
            } 
        }
    }

    /?? Copyright 2002-2008 Tampere University of Technology./ {
        cpp_copyr = 1  
        ++found
    }

    /the GNU General Public License.$|?? Public License.$/ {
        cpp_copyr = 0
        getline temp_line
        if (temp_line != "*/") {
            print temp_line
        } 
        next
    }

    {
        #print "line proc: ", $0
        if (cpp_copyr != 1) {
            print $0
        }
    }

    ' ${1} > ${TMPFILE}
    cp ${TMPFILE} ${1}
    rm -f ${TMPFILE}
}

# export function and variables needed by them
export -f remLicense 

PREPEND_REGEX='.*\.(h|hh|cc|icc|sh|py|php|vhd[l]?|tex)$'

find . -regextype posix-extended -type f -regex "$PREPEND_REGEX" -exec bash -c "\
remLicense {}" \;
