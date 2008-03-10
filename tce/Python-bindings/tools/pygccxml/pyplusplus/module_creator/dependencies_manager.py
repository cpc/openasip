# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""defines class, which informs user about used, but unexposed declarations"""

import os
from pyplusplus import utils 
from pyplusplus import messages
from pygccxml import declarations
from pyplusplus import decl_wrappers


class manager_t( object ):
    def __init__( self, logger, already_exposed=None ):
        object.__init__( self )
        self.__exported_decls = []
        self.__logger = logger
        self.__already_exposed_db = utils.exposed_decls_db_t()
        if already_exposed:
            map( self.__already_exposed_db.load, already_exposed )
            
    def add_exported( self, decl ):
        self.__exported_decls.append( decl )  

    def is_already_exposed( self, decl ):
        return decl.already_exposed or self.__already_exposed_db.is_exposed( decl )

    def __select_duplicate_aliases( self, decls ):
        duplicated = {}
        for decl in decls:
            if not duplicated.has_key( decl.alias ):
                duplicated[ decl.alias ] = set()
            duplicated[ decl.alias ].add( decl )        
        for alias, buggy_decls in duplicated.items():
            if 1 == len( buggy_decls ):
                del duplicated[ alias ]        
        return duplicated

    def __report_duplicate_aliases_impl( self, control_decl, duplicated ):
        if control_decl.alias in duplicated:
            buggy_decls = duplicated[control_decl.alias].copy()
            buggy_decls.remove( control_decl )
            warning = messages.W1047 % ( control_decl.alias
                                         , os.linesep.join( map( str, buggy_decls ) ) )
            self.__logger.warn( "%s;%s" % ( control_decl, warning ) )
            
        if isinstance( control_decl, declarations.class_t ):
            query = lambda i_decl: isinstance( i_decl, declarations.class_types ) \
                                   and i_decl.ignore == False
            i_decls = control_decl.classes( query, recursive=False, allow_empty=True )
            i_duplicated = self.__select_duplicate_aliases( i_decls )
            for i_decl in i_decls:
                self.__report_duplicate_aliases_impl( i_decl, i_duplicated )

    def __report_duplicate_aliases( self ):
        decls = filter( lambda decl: isinstance( decl, declarations.class_types ) \
                                     and isinstance( decl.parent, declarations.namespace_t )
                        , self.__exported_decls )
        duplicated = self.__select_duplicate_aliases( decls )
        for decl in decls:
            self.__report_duplicate_aliases_impl( decl, duplicated )
    
    def __is_std_decl( self, decl ):
        #Every class under std should be exported by Boost.Python and\\or Py++
        #Also this is not the case right now, I prefer to hide the warnings
        dpath = declarations.declaration_path( decl )
        if len( dpath ) < 3:
            return False
        if dpath[1] != 'std':
            return False
        if decl.name.startswith( 'pair<' ):
            #special case
            return False
        return True

    def __build_dependencies( self, decl ):
        if self.__is_std_decl( decl ):
            #TODO add element_type to the list of dependencies
            return [] #std declarations should be exported by Py++!
        if self.is_already_exposed( decl ):
            return []
        dependencies = decl.i_depend_on_them(recursive=False)
        if isinstance( decl, declarations.class_t ):
            dependencies = filter( lambda d: d.access_type != declarations.ACCESS_TYPES.PRIVATE
                                   , dependencies )
        return dependencies
    
    def __find_out_used_but_not_exported( self ):
        used_not_exported = []
        exported_ids = set( map( lambda d: id( d ), self.__exported_decls ) )
        for decl in self.__exported_decls:
            for dependency in self.__build_dependencies( decl ):
                depend_on_decl = dependency.find_out_depend_on_declaration()
                if None is depend_on_decl:
                    continue
                if self.__is_std_decl( depend_on_decl ):
                    continue
                if isinstance( depend_on_decl, decl_wrappers.decl_wrapper_t ):
                    if depend_on_decl.already_exposed:
                        continue
                    if isinstance( depend_on_decl, declarations.class_types ):
                        if depend_on_decl.opaque:
                            continue
                if id( depend_on_decl ) not in exported_ids:
                    report = messages.filter_disabled_msgs([messages.W1040], depend_on_decl.disabled_messaged )
                    if report:
                        used_not_exported.append( dependency )                    
        return used_not_exported

    def __group_by_unexposed( self, dependencies ):
        groups = {}
        for dependency in dependencies:
            depend_on_decl = dependency.find_out_depend_on_declaration()
            if not groups.has_key( id( depend_on_decl ) ):
                groups[ id( depend_on_decl ) ] = []
            groups[ id( depend_on_decl ) ].append( dependency )
        return groups

    def __create_dependencies_msg( self, dependencies ):
        depend_on_decl = dependencies[0].find_out_depend_on_declaration()
        decls = []
        for dependency in dependencies:
            decls.append( os.linesep + ' ' + str( dependency.declaration ) )
        return "%s;%s" % ( depend_on_decl, messages.W1040 % ''.join( decls ) )
        
    def inform_user( self ):
        used_not_exported_decls = self.__find_out_used_but_not_exported()
        groups = self.__group_by_unexposed( used_not_exported_decls )
        for group in groups.itervalues():
            self.__logger.warn( self.__create_dependencies_msg( group ) )
        self.__report_duplicate_aliases()
