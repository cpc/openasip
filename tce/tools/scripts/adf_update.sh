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
# This script reformats adf files by replacing "max-rw" element with
# "max-writes" and adds "max-writes", "max-reads" and "type" to
# immediate-units (so that they resemble register files). Also removes
# "cycles" from immediate-units.
#
# This script illustrates how (more than one line) context dependent file 
# editing can be done with awk.
# 
# Example usage with find:
# find . -iname "*.mdf" -o -iname "*.adf" -exec adf_update.sh '{}' ';'
#
# @author Esa Määttä 2007 (esa.maatta@tut.fi)


# create temp file for awk
TMPFILE=$(mktemp) 
if [ "$?" != 0 ]; then
    echo "Error tmp file creation failed."
    exit 1
fi

# replace max-rw with max-writes in register files
sed -i 's#<max-rw>\(.*\)</max-rw>#<max-writes>\1</max-writes>#' ${1}

# removes cycles from immediate-units and adds new entries
awk '
    /<immediate-unit.*>/    {inside_imm=1}
    /<cycles>/              { 
                                if (inside_imm == 1) { 
                                    cycles=1; 
                                    # after cycles comes slot width that
                                    # should not be confused with width that
                                    # is matched
                                    inside_imm=0; # last edit in imm unit
                                } 
                            }
    /<width>/               { 
                                if (inside_imm == 1) {
                                    width=1;
                                }
                            }
    /<\/immediate-unit>/    { inside_imm=0; infoadded=0 } 
    
                            { 
                                if (cycles == 0 ) {
                                    print $0;
                                } else {
                                    cycles=0;     
                                }
                                # add new fields to immediate-unit
                                if (inside_imm == 1 && infoadded == 0) {
                                    print "    <type>normal</type>";
                                    infoadded=1;
                                }
                                if (width == 1) {
                                    print "    <max-reads>1</max-reads>"
                                    print "    <max-writes>1</max-writes>"
                                    width=0;
                                }
                            } 
    ' ${1} > ${TMPFILE}

ORGSIZE=$(du -b "${1}" | awk '{print $1}')
NEWSIZE=$(du -b "${TMPFILE}" | awk '{print $1}')

if [ "${NEWSIZE}" -ge "${ORGSIZE}" ]; then
    mv -f "${TMPFILE}" "${1}"
else
    echo "Error: New adf file was smaller than orginal."
    exit 2
fi
