# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""Py++ - Boost.Python code generator
========================================

This package (together with the accompanying pygccxml package and
U{Boost.Python<http://www.boost.org/libs/python/doc/index.html>})
assists you in creating Python bindings for a C/C++ library. This is
done by parsing a set of header files that contain all the
stuff you want to expose in Python. The result of this parsing
step is a I{declaration tree} that represents all declarations found
in the headers. You can then modify (decorate) this tree to customize
the bindings. After that, a I{code creators} tree is created where
each node represents a block of C++ source code. So you can change any piece of
code befor it is written to disk. As a last step, these source code blocks are
finally written into one or more C++ source files, which can then be compiled to
generate the final Python module.

If you are just starting with U{Py++<http://www.language-binding.net>},
then consider to read documentation of L{module_builder} package.
"""

import code_creators
import file_writers
import module_creator
import code_repository
import utils
import decl_wrappers
import module_builder
import messages

from _logging_ import multi_line_formatter_t

__version__ = '0.9.0'

import pygccxml
if not hasattr( pygccxml, '__revision__' ) or pygccxml.__revision__ < 1080:
    msg = 'This revision of Py++ requieres pygccxml revision to be ' \
          'greater or equal to %d. ' \
          'Please install right pygccxml version.'          
    raise AssertionError( msg % pygccxml.__revision__ )

#Known issues:
#3.
#~ > > 2. An other difference: when Py++ creates bindings for a set of
#~ > > declarations, it
#~ > > should (?) see all declarations that are going to be exported:
#~ > >     reasons:
#~ > >         to decide what class holder is.
#~ > >             In one header file you define class, in an other you
#~ > > define function that takes
#~ > >             as argument shared_ptr to the class.
#~ >
#~ > You don't need to use a shared_ptr holder for that purpose.  The
#~ > *only* reason you'd ever want to use a share_ptr holder is if you
#~ > expect people to wrap functions taking non-const references to these
#~ > shared_ptrs.  That is very rare -- it only happens when users want to
#~ > replace one shared_ptr with another (possibly NULL) shared_ptr.

