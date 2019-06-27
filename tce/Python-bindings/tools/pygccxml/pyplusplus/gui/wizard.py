# Copyright (c) 2002-2009 Tampere University.
#
# This file is part of TTA-Based Codesign Environment (TCE).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
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
        
