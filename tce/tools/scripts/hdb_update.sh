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
