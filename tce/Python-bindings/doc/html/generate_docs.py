#!/usr/bin/env python
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
# Traverse the TCE package to find out which packages and modules
# need to be documented.
#
# Pertti Kellomaki pertti.kellomaki@tut.fi, 2007

import TCE
import sys
import os

def traverse(entity):
    """Traverse a module and create pydoc invocations for it
    and any embedded modules.
    """

    if str(type(entity)) != "<type 'module'>":
        # At leaf
        return
    else:
        if entity.__name__ not in ['dl', 'sys']:
            f.write("pydoc -w %s\n" % entity.__name__)
            if '__all__' in dir(entity):
                # This seems to be a package, so traverse further
                for e in dir(entity):
                    traverse(entity.__dict__[e])
                return

f = open('generate_docs_aux.sh', 'w')
f.write('# Generated file, do not edit.\n')
f.write('# Use "python generate_docs.py" to refresh this file.\n')
traverse(TCE)
f.close()
os.system('/bin/sh generate_docs_aux.sh')
