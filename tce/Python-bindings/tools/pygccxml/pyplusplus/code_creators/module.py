# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import custom
import license
import include
import namespace    
import compound
import algorithm
import module_body
import include_directories

class module_t(compound.compound_t):
    """This class represents the source code for the entire extension module.

    The root of the code creator tree is always a module_t object.
    """
    def __init__(self, global_ns):
        """Constructor.
        """
        compound.compound_t.__init__(self)
        self.__body = None
        self.__global_ns = global_ns
        
    @property
    def global_ns(self):
        "reference to global_ns ( namespace_t ) declaration"
        return self.__global_ns
            
    def _get_include_dirs(self):
        include_dirs = algorithm.creator_finder.find_by_class_instance( 
            what=include_directories.include_directories_t
            , where=self.creators
            , recursive=False)        
        if 0 == len( include_dirs ):
            include_dirs = include_directories.include_directories_t()
            if self.license:
                self.adopt_creator( include_dirs, 1 )
            else:
                self.adopt_creator( include_dirs, 0 )
            return include_dirs
        elif 1 == len( include_dirs ):
            return include_dirs[0]
        else: 
            assert not "only single instance of include_directories_t should exist"
            
    def _get_std_directories(self):
        include_dirs = self._get_include_dirs()
        return include_dirs.std
    std_directories = property( _get_std_directories )

    def _get_user_defined_directories(self):
        include_dirs = self._get_include_dirs()
        return include_dirs.user_defined
    user_defined_directories = property( _get_user_defined_directories )

    @property 
    def body(self):
        """Return reference to L{module_body_t} code creator"""
        if None is self.__body:
            found = algorithm.creator_finder.find_by_class_instance( what=module_body.module_body_t
                                                                 , where=self.creators
                                                                 , recursive=False )
            if found:
                self.__body = found[0]
        return self.__body

    def _get_license( self ):
        if isinstance( self.creators[0], license.license_t ):
            return self.creators[0]
        return None
    
    def _set_license( self, license_text ):
        if not isinstance( license_text, license.license_t ):
            license_inst = license.license_t( license_text )
        if isinstance( self.creators[0], license.license_t ):
            self.remove_creator( self.creators[0] )
        self.adopt_creator( license_inst, 0 )
    license = property( _get_license, _set_license,
                        doc="""License text.

                        The license text will always be the first children node.
                        @type: str or L{license_t}""")

    def last_include_index(self):
        """Return the children index of the last L{include_t} object.

        An exception is raised when there is no include_t object among
        the children creators.

        @returns: Children index
        @rtype: int
        """
        for i in range( len(self.creators) - 1, -1, -1 ):
            if isinstance( self.creators[i], include.include_t ):
                return i
        else:
            return 0

    def replace_included_headers( self, headers, leave_system_headers=True ):
        to_be_removed = []
        for creator in self.creators:
            if isinstance( creator, include.include_t ):
                to_be_removed.append( creator )
            elif isinstance( creator, module_body.module_body_t ):
                break
        
        for creator in to_be_removed:
            if creator.is_system: 
                if not leave_system_headers:
                    self.remove_creator( creator )
            elif creator.is_user_defined:
                pass
            else:
                self.remove_creator( creator )
        map( lambda header: self.adopt_include( include.include_t( header=header ) )
             , headers )

    def adopt_include(self, include_creator):
        """Insert an L{include_t} object.

        The include creator is inserted right after the last include file.

        @param include_creator: Include creator object
        @type include_creator: L{include_t}
        """
        lii = self.last_include_index()
        if lii == 0:
            if not self.creators:
                lii = -1
            elif not isinstance( self.creators[0], include.include_t ):
                lii = -1
            else:
                pass
        self.adopt_creator( include_creator, lii + 1 )

    def do_include_dirs_optimization(self):
        include_dirs = self._get_include_dirs()
        includes = filter( lambda creator: isinstance( creator, include.include_t )
                           , self.creators )
        for include_creator in includes:
            include_creator.include_dirs_optimization = include_dirs

    def _get_system_headers_impl( self ):
        return []

    def _create_impl(self):
        self.do_include_dirs_optimization()
        index = 0
        includes = []
        for index in range( len( self.creators ) ):
            if not isinstance( self.creators[index], include.include_t ):
                break
            else:
                includes.append( self.creators[index].create() )
        code = compound.compound_t.create_internal_code( self.creators[index:] )
        code = self.unindent(code)        
        return os.linesep.join( includes ) + 2 * os.linesep + code + os.linesep
    
    def add_include( self, header, user_defined=True, system=False ):
        creator = include.include_t( header=header, user_defined=user_defined, system=system )
        self.adopt_include( creator )
    
    def add_namespace_usage( self, namespace_name ):
        self.adopt_creator( namespace.namespace_using_t( namespace_name )
                            , self.last_include_index() + 1 )

    def add_namespace_alias( self, alias, full_namespace_name ):
        self.adopt_creator( namespace.namespace_alias_t( 
                                alias=alias
                                , full_namespace_name=full_namespace_name )
                            , self.last_include_index() + 1 )

    def adopt_declaration_creator( self, creator ):
        self.adopt_creator( creator, self.creators.index( self.body ) )

    def add_declaration_code( self, code, position ):        
        self.adopt_declaration_creator( custom.custom_text_t( code ) )
