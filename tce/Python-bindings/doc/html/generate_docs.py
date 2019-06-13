#!/usr/bin/env python
# Copyright (c) 2002-2009 Tampere University.
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
