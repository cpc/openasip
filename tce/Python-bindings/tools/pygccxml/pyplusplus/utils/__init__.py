# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
This module is a collection of unrelated algorithms, that works on code creators
tree.
"""
import os
import math
from pygccxml import declarations
from pyplusplus import code_creators  

class missing_call_policies:
    @staticmethod
    def _selector( creator ):
        if not isinstance( creator, code_creators.declaration_based_t ):
            return False
        if not isinstance( creator.declaration, declarations.calldef_t ):
            return False
        if isinstance( creator.declaration, declarations.constructor_t ):
            return False
        return hasattr(creator, 'call_policies') and not creator.call_policies

    @staticmethod    
    def print_( extmodule ):
        creators = filter( missing_call_policies._selector
                           , code_creators.make_flatten_generator( extmodule.creators ) )
        for creator in creators:
            print creator.declaration.__class__.__name__, ': ', declarations.full_name( creator.declaration )
            print '  *** MISSING CALL POLICY', creator.declaration.function_type().decl_string
            print 

    @staticmethod    
    def exclude( extmodule ):
        creators = filter( missing_call_policies._selector
                           , code_creators.make_flatten_generator( extmodule.creators ) )
        for creator in creators:
            creator.parent.remove_creator( creator )
    
def split_sequence(seq, bucket_size):
    #split sequence to buclets, where every will contain maximum bucket_size items
    seq_len = len( seq )
    if seq_len <= bucket_size:
        return [ seq ] 
    buckets = []
    num_of_buckets = int( math.ceil( float( seq_len ) / bucket_size ) )
    for i in range(num_of_buckets):
        from_ = i * bucket_size
        to = min( ( i + 1) * bucket_size, seq_len )
        buckets.append( seq[ from_ : to ] )
    return buckets
    

class exposed_decls_db_t( object ):
    DEFAULT_FILE_NAME = 'exposed_decl.pypp.txt'
    class row_creator_t( declarations.decl_visitor_t ):
        def __init__( self, field_delimiter ):
            self.__decl = None
            self.__formatted = None
            self.__field_delimiter = field_delimiter

        def get_full_name(self):
            return declarations.full_name( self.__decl )

        def __call__( self, decl ):
            self.__decl = decl
            self.__formatted = None
            try:
                declarations.apply_visitor( self, decl )
            except NotImplementedError:
                pass
            return self.__formatted

        def visit_free_function( self ):
            self.__formatted = '%s%s%s' % ( self.get_full_name()
                                            , self.__field_delimiter
                                            , self.__decl.function_type().decl_string )

        def visit_class_declaration(self ):
            self.__formatted = self.get_full_name()
            
        def visit_class(self ):
            self.__formatted = self.get_full_name()
            
        def visit_enumeration(self ):
            self.__formatted = self.get_full_name()

        def visit_variable(self ):
            self.__formatted = self.get_full_name()
        
    def __init__( self ):
        self.__exposed = {}
        self.__row_creator = self.row_creator_t(field_delimiter='@')
        self.__key_delimiter = '?'
        self.__row_delimiter = os.linesep
        
    def save( self, fpath ):
        if os.path.isdir( fpath ):
            fpath = os.path.join( fpath, self.DEFAULT_FILE_NAME )
        f = file( fpath, 'w+b' )
        for key, items in self.__exposed.iteritems():
            for item in items:
                f.write( '%s%s%s%s' % ( key, self.__key_delimiter, item, self.__row_delimiter ) )
        f.close()
    
    def load( self, fpath ):
        if os.path.isdir( fpath ):
            fpath = os.path.join( fpath, self.DEFAULT_FILE_NAME )        
        f = file( fpath, 'r+b' )
        for line in f:
            key, row = line.split( self.__key_delimiter)
            row = row.replace( self.__row_delimiter, '' )
            if not self.__exposed.has_key( key ):
                self.__exposed[ key ] = set()
            self.__exposed[ key ].add( row )

    def __create_key( self, decl ):
        return decl.__class__.__name__ 
    
    def expose( self, decl ):
        if not isinstance( decl.parent, declarations.namespace_t ):
            return None #we don't want to dump class internal declarations
        row = self.__row_creator( decl )
        if row is None:
            return None
        key = self.__create_key( decl )
        if not self.__exposed.has_key( key ):
            self.__exposed[ key ] = set()
        self.__exposed[ key ].add( row )

    def __get_under_ns_decl( self, decl ):
        while True:
            if isinstance( decl.parent, declarations.namespace_t ):
                return decl
            else:
                decl = decl.parent

    def is_exposed( self, decl_ ):
        if isinstance( decl_, declarations.namespace_t ):
            return False#namespaces are always exposed
        decl = self.__get_under_ns_decl( decl_ )
        key = self.__create_key( decl )
        if not self.__exposed.has_key( key ):
            return False
        row = self.__row_creator( decl )
        return row in self.__exposed[ key ]


