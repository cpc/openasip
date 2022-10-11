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
