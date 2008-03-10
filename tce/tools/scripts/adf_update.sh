#!/bin/bash
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
