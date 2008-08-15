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
import os
import sys

from Synopsis import AST
from Synopsis.Parsers import Cxx

headers_dir = '/home/roman/language-binding/sources/pygccxml_dev/unittests/data' 

offset = 0
def print_decls( d ):
    global offset
    print offset * ' ', d.name(), d.__class__.__name__
    if hasattr( d, 'declarations' ):
        offset += 1
        for d1 in d.declarations():
            print_decls( d1 )
        offset -= 1

def parse( f ):
    global offset, headers_dir
    print 'file ', f
    cxx = Cxx.Parser(
            preprocess=True
            , cppflags=['-I %s' % headers_dir ] )
    
    ast = AST.AST()
    cxx.process( ast, input=[os.path.join(headers_dir, f )] )
    
    offset = 0
    for d in ast.declarations():
        print_decls( d )

parse( 'declarations_enums.hpp' )
#for x in os.listdir( headers_dir ):
    #if x.endswith( 'hpp' ):
        #parse( x )
