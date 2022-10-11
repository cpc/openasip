# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import algorithm
import code_creator

class declaration_based_t:
    """Code creator that is based on a declaration.
    """
    def __init__(self, declaration ):
        """Constructor.

        @param declaration: Declaration object
        @type declaration: L{decl_wrapper_t<decl_wrappers.decl_wrapper_t>}
        @param parent: Parent code creator.
        @type parent: code_creator_t
        """
        self._decl = declaration
        
    def _generate_valid_name(self, name=None):
        if name == None:
            name = self.declaration.name
        return algorithm.create_valid_name( name )
            
    def _get_declaration(self):
        return self._decl
    declaration = property( _get_declaration,
                            doc="""The declaration this code creator is based on.
                            @type: L{decl_wrapper_t<decl_wrappers.decl_wrapper_t>}
                            """)

    def _get_alias_impl( self ):
        return self.declaration.alias
    
    def _get_alias(self):
        return self._get_alias_impl()
    
    def _set_alias(self, alias):
        self.declaration.alias = alias
    alias = property( _get_alias, _set_alias )
    
    @property
    def decl_identifier( self ):
        return algorithm.create_identifier( self, self.declaration.partial_decl_string )
    
    @property
    def documentation( self ):
        if None is self.declaration.documentation:
            return ''
        return self.declaration.documentation
    
    def get_user_headers( self, recursive=False, unique=False ):
        """return list of user header files to be included from the generated file"""
        return self.declaration.include_files
