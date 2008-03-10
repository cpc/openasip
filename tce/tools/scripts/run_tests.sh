#!/bin/bash
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