# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
Code repository package is used as a repository of C++ classes/functions.
Those classes/functions solve problems, that are typical to most projects.
Right now, this package contains set of classes that help to export one
dimensional static arrays. For example:

C{char data[23];}

"""

import array_1
import gil_guard
import named_tuple
import convenience
import return_range
import call_policies

all = [ array_1, gil_guard, convenience, call_policies, named_tuple, return_range ]

headers = map( lambda f: f.file_name, all )

