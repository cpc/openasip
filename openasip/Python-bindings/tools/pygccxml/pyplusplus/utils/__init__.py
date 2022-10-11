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
    #split sequence to buckets, where every will contain maximum bucket_size items
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
    class row_t( declarations.decl_visitor_t ):
        FIELD_DELIMITER = '@'                
        EXPOSED_DECL_SIGN = '+'
        UNEXPOSED_DECL_SIGN = '~'
        CALLDEF_SIGNATURE_DELIMITER = '#'
                
        def __init__( self, decl_or_string ):
            self.key = ''
            self.signature = ''
            self.exposed_sign = ''
            self.normalized_name = ''                        
            if isinstance( decl_or_string, declarations.declaration_t ):
                self.__init_from_decl( decl_or_string )
            else:
                self.__init_from_str( decl_or_string )

        def find_out_normalized_name( self, decl ):
            if decl.name: 
                return decl.name
            elif decl.location:#unnamed enums, classes, unions
                return str( decl.location.as_tuple() )
            elif isinstance( decl, declarations.namespace_t ):
                return '' #I don't really care about unnamed namespaces
            else: #this should nevere happen
                raise RuntimeError( "Unable to create normalized name for declaration: " + str(decl))

        def __init_from_str( self, row ):
            self.exposed_sign, self.key, self.normalized_name, self.signature \
                = row.split( self.FIELD_DELIMITER )

        def __init_from_decl( self, decl ):
            if decl.ignore:
                self.exposed_sign = self.UNEXPOSED_DECL_SIGN
            else:
                self.exposed_sign = self.EXPOSED_DECL_SIGN
            self.key = decl.__class__.__name__
            self.signature = decl.decl_string
            if isinstance( decl, declarations.calldef_t ):
                self.signature = self.signature + decl.function_type().decl_string
            self.normalized_name = self.find_out_normalized_name( decl )
            
        def __str__( self ):
            return self.FIELD_DELIMITER.join([ self.exposed_sign
                                               , self.key
                                               , self.normalized_name
                                               , self.signature])
        
        def does_refer_same_decl( self, other ):
            return self.key == other.key \
                   and self.signature == other.signature \
                   and self.normalized_name == other.normalized_name
        
    def __init__( self ):
        self.__registry = {} # key : { name : set(row) }
        self.__row_delimiter = os.linesep
        
    def save( self, fpath ):
        if os.path.isdir( fpath ):
            fpath = os.path.join( fpath, self.DEFAULT_FILE_NAME )
        f = file( fpath, 'w+b' )
        for name2rows in self.__registry.itervalues():
            for rows in name2rows.itervalues():
                for row in rows:
                    f.write( '%s%s' % ( str(row), self.__row_delimiter ) )
        f.close()

    def load( self, fpath ):
        if os.path.isdir( fpath ):
            fpath = os.path.join( fpath, self.DEFAULT_FILE_NAME )        
        f = file( fpath, 'r+b' )
        for line in f:
            row = self.row_t( line.replace( self.__row_delimiter, '' ) )            
            self.__update_registry( row )
    
    def __update_registry( self, row ):        
        if not self.__registry.has_key( row.key ):            
            self.__registry[ row.key ] = { row.normalized_name : [row] }
        else:       
            if not self.__registry[ row.key ].has_key( row.normalized_name ):
                self.__registry[ row.key ][row.normalized_name] = [row]
            else:
                self.__registry[ row.key ][row.normalized_name].append(row)
        
    def __find_in_registry( self, decl ):
        row = self.row_t( decl )
        try:
            decls = filter( lambda rrow: rrow.does_refer_same_decl( row )
                            , self.__registry[ row.key ][ row.normalized_name ] )
            if decls:
                return decls[0]
            else:
                return None
        except KeyError:
            return None
    
    def is_exposed( self, decl ):
        row = self.__find_in_registry( decl)
        return row and self.row_t.EXPOSED_DECL_SIGN == row.exposed_sign
        
    def update_decls( self, global_ns ):
        for decl in global_ns.decls():
            row = self.__find_in_registry( decl )
            if not row:
                continue
            if self.row_t.EXPOSED_DECL_SIGN == row.exposed_sign:
                decl.ignore = False
                decl.already_exposed = True
            else:
                decl.ignore = True
                decl.already_exposed = False

    def register_decls( self, global_ns ):
        for decl in global_ns.decls():
            self.__update_registry( self.row_t( decl ) )
