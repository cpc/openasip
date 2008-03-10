#!/bin/bash
#
# replaces max_rw with max_writes in hdb sql databases using sed and sqlite.
#
# Example usage with find:
# find . -iname "*.hdb" -exec hdb_update.sh '{}' ';'
#
# @author Esa Määttä 2007 (esa.maatta@tut.fi)


# create temp file for new hdb
TMPFILE=$(mktemp) 
if [ "$?" != 0 ]; then
    echo "Error tmp file creation failed."
    exit 1
fi

# command for sed
SEDCMD='s/max_rw/max_writes/g'

echo ".dump" | sqlite3 --batch "${1}" | sed "${SEDCMD}" | sqlite3 --batch ${TMPFILE}
mv -f ${TMPFILE} "${1}"
