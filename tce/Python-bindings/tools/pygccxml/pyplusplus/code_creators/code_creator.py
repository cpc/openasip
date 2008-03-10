# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import types

class code_creator_t(object):
    """
    code_creator_t is the base class for all code creators.
    
    This class defines the interface that every code creator should implement. 
    Also it provides few convenience functions.

    The purpose of a code creator is the generation of a block of C++
    source code as it will appear in the final source code for the
    extension module. The source code is obtained by calling the L{create()}
    method. Derived classes must implement the L{_create_impl()} method
    which is called by the create() method.
    """
    PYPLUSPLUS_NS_NAME = 'pyplusplus'
    __INDENTATION = '    '
    LINE_LENGTH = 80
    PARAM_SEPARATOR = ', '
    def __init__(self):
        """Constructor.

        @param parent: Parent code creator.
        @type parent: code_creator_t
        """
        object.__init__(self)
        self._parent = None
        self._target_configuration = None
        self._works_on_instance = True
        

    def _get_works_on_instance(self):
        return self._works_on_instance
    def _set_works_on_instance(self, works_on_instance):
        self._works_on_instance = works_on_instance
    works_on_instance = property( _get_works_on_instance, _set_works_on_instance )
        
    def _get_parent( self ):
        return self._parent 
    def _set_parent( self, new_parent ):
        if new_parent:
            assert isinstance( new_parent, code_creator_t )
        self._parent = new_parent
    """parent - reference to parent code creator"""
    parent = property( _get_parent, _set_parent,
                       doc="""Parent code creator or None if this is the root node.
                       @type: L{code_creator_t}
                       """)
    
    def _get_target_configuration( self ):
        return self._target_configuration
    def _set_target_configuration( self, config ):
        self._target_configuration = config
    """target_configuration - reference to target_configuration_t class instance"""
    target_configuration = property( _get_target_configuration, _set_target_configuration,
                                     doc="""Target configuration.
                                     @type: L{target_configuration_t}
                                     """)
    
    def _get_top_parent(self):
        parent = self.parent
        me = self
        while True:
            if not parent:
                return me
            else:
                me = parent
                parent = me.parent
    """top_parent - reference to top parent code creator"""
    top_parent = property( _get_top_parent,
                           doc="""Root of the code creator tree.
                           @type: L{code_creator_t}
                           """)

    def _create_impl(self):
        """
        function that all derived classes should implement. This function 
        actually creates code and returns it. Return value of this function is
        string.

        @rtype: str
        """
        raise NotImplementedError()
    
    def create(self):
        """
        this function should be used in order to get code that should be 
        generated.

        @returns: Returns a text block of C++ source code.
        @rtype: str
        """
        code = self._create_impl()
        assert isinstance( code, types.StringTypes )
        return self.beautify( code )

    @staticmethod
    def unique_headers( headers ):
        used = set()
        uheaders = []
        for h in headers:
            if h not in used:
                used.add( h )
                uheaders.append( h )
        return uheaders
    
    def _get_system_headers_impl( self ):
        """Return list of system header files the generated code depends on"""
        raise NotImplementedError(self.__class__.__name__)

    def get_system_headers( self, recursive=False, unique=False ):
        files = [ "boost/python.hpp" ]
        files.extend( self._get_system_headers_impl() )
        files = filter( None, files)
        if unique:
            files = self.unique_headers( files )
        return files

    def beautify( self, code ):
        """
        function that returns code without leading and trailing whitespaces.

        @param code: A code block with C++ source code.
        @type code: str
        @rtype: str
        """
        assert isinstance( code, types.StringTypes )
        return code.strip()
    
    @staticmethod
    def indent( code, size=1 ):
        """
        function that implements code indent algorithm.

        @param code: C++ code block.
        @type code: str
        @param size: The number of indentation levels that the code is shifted
        @type size: int
        @returns: Returns indented source code
        @rtype: str
        """
        assert isinstance( code, types.StringTypes )
        return code_creator_t.__INDENTATION * size\
               + code.replace( os.linesep
                               , os.linesep + code_creator_t.__INDENTATION * size )
    
    @staticmethod
    def unindent( code ):
        """
        function that implements code unindent algorithm. 

        @param code: C++ code block.
        @type code: str
        @rtype: str
        """
        assert isinstance( code, types.StringTypes )
        if code.startswith(code_creator_t.__INDENTATION):
            code = code[ len( code_creator_t.__INDENTATION ):]
        return code.replace( os.linesep + code_creator_t.__INDENTATION
                               , os.linesep )

    @staticmethod   
    def is_comment( line ):
        """
        function that returns true if content of the line is comment, otherwise
        false.

        @param line: C++ source code
        @type line: str
        @rtype: bool
        """
        assert isinstance( line, types.StringTypes )
        l = line.lstrip()
        return l.startswith( '//' ) or l.startswith( '/*' )

    def register_exposed( self, exposed_db ):
        """Register exposed declaration in L{exposed data base<utils.exposed_decls_db_t>}
        
        The default implementation of the function does nothing.
        """
        pass
    
    