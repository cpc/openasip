# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
defines logger classes and few convinience methods, not related to the declarations
tree
"""

import os
import sys
import logging
import tempfile

def _create_logger_( name ):
    """implementation details"""
    logger = logging.getLogger(name)
    handler = logging.StreamHandler(sys.stdout)
    handler.setFormatter( logging.Formatter( os.linesep + '%(levelname)s %(message)s' ) )
    logger.addHandler(handler)
    logger.setLevel(logging.INFO)
    return logger

class loggers:
    """class-namespace, defines few loggers classes, used in the project"""

    cxx_parser = _create_logger_( 'pygccxml.cxx_parser' )
    """logger for C++ parser functionality

    If you set this logger level to DEBUG, you will be able to see the exact
    command line, used to invoke GCC-XML  and errors that occures during XML parsing
    """

    gccxml = cxx_parser #backward compatability
    
    queries_engine = _create_logger_( 'pygccxml.queries_engine' )
    """logger for query engine functionality.

    If you set this logger level to DEBUG, you will be able to see what queries
    you do against declarations tree, measure performance and may be even to improve it.
    Query engine reports queries and whether they are optimized or not.
    """

    declarations_cache = _create_logger_( 'pygccxml.declarations_cache' )
    """logger for declarations tree cache functionality

    If you set this logger level to DEBUG, you will be able to see what is exactly
    happens, when you read the declarations from cache file. You will be able to
    decide, whether it worse for you to use this or that cache strategy.
    """

    root = logging.getLogger( 'pygccxml' )
    """root logger exists for your convinience only"""

    all = [ root, cxx_parser, queries_engine, declarations_cache ]
    """contains all logger classes, defined by the class"""

def remove_file_no_raise(file_name ):
    """removes file from disk, if exception is raised, it silently ignores it"""
    try:
        if os.path.exists(file_name):
            os.remove( file_name )
    except Exception, error:
        loggers.root.error( "Error ocured while removing temprorary created file('%s'): %s"
                            % ( file_name, str( error ) ) )

def create_temp_file_name(suffix, prefix=None, dir=None):
    """small convinience function that creates temporal file.

    This function is a wrapper aroung Python built-in function - tempfile.mkstemp
    """
    if not prefix:
        prefix = tempfile.template
    fd, name = tempfile.mkstemp( suffix=suffix, prefix=prefix, dir=dir )
    file_obj = os.fdopen( fd )
    file_obj.close()
    return name

def normalize_path( some_path ):
    """return os.path.normpath( os.path.normcase( some_path ) )"""
    return os.path.normpath( os.path.normcase( some_path ) )

def get_architecture():
    """returns computer architecture: 32 or 64.

    The guess is based on maxint.
    """
    if sys.maxint == 2147483647:
        return 32
    elif sys.maxint == 9223372036854775807:
        return 64
    else:
        raise RuntimeError( "Unknown architecture" )

