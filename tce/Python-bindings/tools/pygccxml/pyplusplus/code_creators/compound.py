# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import code_creator

class compound_t(code_creator.code_creator_t):
    def __init__(self ):
        """Constructor.

        @param parent: Parent code creator.
        @type parent: L{code_creator_t}
        """
        code_creator.code_creator_t.__init__( self ) 
        self._creators = []
        
    def _get_creators(self):
        return self._creators    
    creators = property(_get_creators,
                        doc="""A list of children nodes.
                        @type: list of L{code_creator_t}""")

    def adopt_creator( self, creator, index=None):
        """Add a creator to the list of children creators.

        @param creator: Creator object
        @type creator: L{code_creator_t}
        @param index: Desired position of the creator or None to append it to the end of the list
        @type index: int
        """
        creator.parent = self
        if index or index == 0:
            self._creators.insert( index, creator )
        else:
            self._creators.append( creator )

    def adopt_creators( self, creators, index=None):
        """Add a creators to the list of children creators.

        @param creators: list of creators object
        @type creator: L{code_creator_t}
        @param index: Desired position of the creator or None to append it to the end of the list
        @type index: int
        """
        for pos, creator in enumerate( creators ):
            if index or index == 0:
                self.adopt_creator( creator, index + pos )
            else:
                self.adopt_creator( creator )

    def remove_creator( self, creator ):
        """Remove a children code creator object.

        @precondition: creator must be a children of self
        @param creator: The creator node to remove
        @type creator: L{code_creator_t}
        """
        creator.parent = None
        del self._creators[ self._creators.index( creator ) ]    

    @staticmethod
    def create_internal_code( creators ):
        """Concatenate the code from a list of code creators.

        @param creators: A list with code creators
        @type creators: list of L{code_creator_t}
        @rtype: str
        """
        internals = map( lambda expr: expr.create(), creators )
        internals = filter(None, internals )
        internals = map( lambda code: code_creator.code_creator_t.indent( code )
                         , internals )
        for index in range( len( internals ) - 1):
            internals[index] = internals[index] + os.linesep
        return os.linesep.join( internals )
        
    def get_system_headers( self, recursive=False, unique=False ):
        files = [ "boost/python.hpp" ]
        files.extend( self._get_system_headers_impl() )
        if recursive:
            for creator in self._creators: 
                files.extend( creator.get_system_headers(recursive, unique=False) )
        files = filter( None, files )
        if unique:
            files = self.unique_headers( files )
        return files
    
    def register_exposed( self, exposed_db ):
        """Register exposed declaration in L{exposed data base<utils.exposed_decls_db_t>}
        
        The default implementation of the function does nothing.
        """
        map( lambda creator: creator.register_exposed( exposed_db )
             , self._creators )
