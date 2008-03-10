# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import compound
import declaration_based

class scoped_t(declaration_based.declaration_based_t, compound.compound_t):
    def __init__( self, declaration ):
        declaration_based.declaration_based_t.__init__( self, declaration=declaration)
        compound.compound_t.__init__( self )
        
    def _create_impl(self):
        #template method pattern should be used.
        raise NotImplementedError()
    
    def register_exposed( self, exposed_db ):
        """Register exposed declaration in L{exposed data base<utils.exposed_decls_db_t>}"""
        exposed_db.expose( self.declaration )
        map( lambda creator: creator.register_exposed( exposed_db )
             , self._creators )

