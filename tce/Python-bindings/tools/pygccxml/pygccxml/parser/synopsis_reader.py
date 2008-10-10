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
