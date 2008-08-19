# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#TODO: find better place for it

"""defines logger classes"""

import os
import sys
import logging
from multi_line_formatter import multi_line_formatter_t

def _create_logger_( name ):    
    """implementation details"""
    logger = logging.getLogger(name)
    handler = logging.StreamHandler()
    handler.setFormatter( multi_line_formatter_t( os.linesep + '%(levelname)s: %(message)s' ) )
    logger.addHandler(handler)
    logger.setLevel(logging.INFO)
    return logger

class loggers:
    """class-namespace, defines few loggers classes, used in the project"""
    
    file_writer = _create_logger_( 'pyplusplus.file_writer' )
    """logger for classes that write code to files"""
    
    declarations = _create_logger_( 'pyplusplus.declarations' )
    """logger for declaration classes
    
    This is very import logger. All important messages: problems with declarations,
    warnings or hints are written to this logger.
    """
    
    module_builder = _create_logger_( 'pyplusplus.module_builder' )
    """logger that in use by L{module_builder_t} class.

    Just another logger. It exists mostly for Py++ developers.
    """
    
    #root logger exists for configuration purpose only
    root = logging.getLogger( 'pyplusplus' )
    """root logger exists for your convinience only"""
    
    all = [ root, file_writer, module_builder, declarations ]
    """contains all logger classes, defined by the class"""
    