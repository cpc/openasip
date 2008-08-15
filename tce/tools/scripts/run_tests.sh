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
# Runs all unit tests and filters output that is not relevant,
# that is, tests that pass, compiler output, etc.
#
# (c) 2005 pekka.jaaskelainen@tut.fi

last_test=""
test_count=0
fail_count=0
previous_error=0

function filter_successful {
    current_line=$*
    first_chars=$(echo ${current_line}|cut -c1-7)
    if [ "x${first_chars}" == "xRunning" ];
        then
        if [ $# -eq 2 ];
            then            
            last_test=$(echo ${current_line}|cut -c9-)
            let "test_count += 1"
        fi
    else
        echo "$last_test $current_line"
        if [ ! ${previous_error} -eq ${test_count} ];
            then
            let "fail_count += 1"
            previous_error=$test_count
        fi
    fi
}

function announce_error {
    echo -e $*
    echo -e "\a\a\a"
    if [ ! "x$(which kdialog)" == "x" ];
        then
        kdialog --error "$*" > /dev/null 2>&1
    fi
}

function announce_success {
    echo -e $*
    echo -e "\a"
    if [ ! "x$(which kdialog)" == "x" ];
        then
        kdialog --msgbox "$*" > /dev/null 2>&1
    fi
}

make 2>&1 | egrep -v \
"^make\[|^Making all|^rm -|^.*OK!|^mkdir|^g\+\+ |\
^ccache g\+\+|^cd |-lgcc|^data/|data/example.opb" | 
{
    while read line; 
      do filter_successful $line;
    done;

    if [ ${fail_count} -eq 0 ];
        then
        announce_success "Executed ${test_count} tests successfully."
        exit 0
    else
        announce_error "Failed ${fail_count} of ${test_count} tests!"
        exit 1
    fi
}