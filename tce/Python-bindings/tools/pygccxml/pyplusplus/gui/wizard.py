"""generates Py++ code from the user data"""

CODE_TEMPLATE = \
"""
import os
from pyplusplus import module_builder

#Creating an instance of class that will help you to expose your declarations
mb = module_builder.module_builder_t( [r"%(file_path)s"]
                                      , gccxml_path=r"%(gccxml_path)s" 
                                      , working_directory=r"%(working_dir)s"
                                      , include_paths=%(include_paths)s
                                      , define_symbols=%(define_symbols)s )


#Well, don't you want to see what is going on?
mb.print_declarations()

#Creating code creator. After this step you should not modify/customize declarations.
mb.build_code_creator( module_name='pyplusplus' )

#Writing code to file.
mb.write_module( './bindings.cpp' )
"""

class wizard_t( object ):
    """code generator that creates Py++ code"""
    def __init__( self
                  , parser_configuration
                  , source_file ):
        object.__init__( self )
        self._parser_configuration = parser_configuration
        self._source_file = source_file
    
    def create_code( self ):
        global CODE_TEMPLATE
        return CODE_TEMPLATE % {
            'gccxml_path' : self._parser_configuration.gccxml_path
            , 'working_dir' : self._parser_configuration.working_directory
            , 'include_paths' : `self._parser_configuration.include_paths`
            , 'define_symbols' : `self._parser_configuration.define_symbols`
            , "file_path" : self._source_file
        }
        