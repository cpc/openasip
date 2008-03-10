#! /usr/bin/python
# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""Py++ user interface runner

The main purpose of this module is to run ui.py from development tree, so the
only work it does is adding new paths to sys.path variable.
"""

import os
import sys

#pygccxml
sys.path.append( os.path.abspath( './../../../pygccxml_dev' ) )
#pyplusplus
sys.path.append( os.path.abspath( './../..' ) )

import ui
if __name__ == '__main__':
    ui.show_demo()