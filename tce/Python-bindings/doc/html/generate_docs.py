#!/usr/bin/env python
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
