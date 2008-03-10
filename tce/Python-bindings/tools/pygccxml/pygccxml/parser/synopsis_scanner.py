# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import types
import pprint
import warnings
from Synopsis import AST
from pygccxml import utils
from pygccxml.declarations import *

class scanner_t( AST.Visitor, object ):
    def __init__(self, ast, decl_factory ):
        self.logger = utils.loggers.cxx_parser
        self.ast = ast
        
        assert isinstance( decl_factory, decl_factory_t )
        self.__decl_factory = decl_factory
        
        #mapping from id -> decl
        self.__decl = self.__decl_factory.create_namespace( name='::' )
        self.global_ns = self.__decl
    

    def read_deaclaration( self, node, decl ):
        #this function should not be called for namespace
        decl.name = node.name()
        decl.location = location_t( file_name=node.file(), line=node.line() )

    def visitModule( self, node ):
        ns = self.__decl_factory.create_namespace( name=node.name() )
        self.__decl.adopt_declaration( ns )
        self.__decl = ns
        super( scanner_t, self ).visitModule( node )
        
    def visitEnum( self, node ):
        values = []
        for enumerator in node.enumerators():
            print enumerator.name(), ':',  enumerator.value() 
            values.append( ( enumerator.name(), enumerator.value() ) )
        enum = self.__decl_factory.create_enumeration( values=values )
        self.read_deaclaration( node, enum )
        self.__decl.adopt_declaration( enum )
        super( scanner_t, self ).visitEnum( node )
