# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os
import sys
import time

import warnings

from pygccxml import parser
from pygccxml import declarations as decls_package

from pyplusplus import utils
from pyplusplus import _logging_
from pyplusplus import decl_wrappers
from pyplusplus import file_writers
from pyplusplus import code_creators
from pyplusplus import module_creator as mcreator_package

class module_builder_t(object):
    """
    This class provides users with simple and intuitive interface to Py++
    and/or pygccxml functionality. If this is your first attempt to use Py++
    consider to read tutorials. You can find them on U{web site<http://www.language-binding.net>}.
    """

    def __init__( self
                  , files
                  , gccxml_path=''
                  , working_directory='.'
                  , include_paths=None
                  , define_symbols=None
                  , undefine_symbols=None
                  , start_with_declarations=None
                  , compilation_mode=None
                  , cache=None
                  , optimize_queries=True
                  , ignore_gccxml_output=False
                  , indexing_suite_version=1
                  , cflags=""
                  , encoding='ascii'):
        """
        @param files: list of files, declarations from them you want to export
        @type files: list of strings or L{file_configuration_t} instances

        @param gccxml_path: path to gccxml binary. If you don't pass this argument,
        pygccxml parser will try to locate it using you environment PATH variable
        @type gccxml_path: str

        @param include_paths: additional header files location. You don't have to
        specify system and standard directories.
        @type include_paths: list of strings

        @param define_symbols: list of symbols to be defined for preprocessor.
        @param define_symbols: list of strings

        @param undefine_symbols: list of symbols to be undefined for preprocessor.
        @param undefine_symbols: list of strings

        @param cflags: Raw string to be added to gccxml command line.
        """
        object.__init__( self )
        self.logger = _logging_.loggers.module_builder
        self.__encoding = encoding
        gccxml_config = parser.config_t(
            gccxml_path=gccxml_path
            , working_directory=working_directory
            , include_paths=include_paths
            , define_symbols=define_symbols
            , undefine_symbols=undefine_symbols
            , start_with_declarations=start_with_declarations
            , ignore_gccxml_output=ignore_gccxml_output
            , cflags=cflags)

        #may be in future I will add those directories to user_defined_directories
        #to self.__code_creator.
        self.__working_dir = os.path.abspath( working_directory )

        self.__parsed_files = map( decls_package.filtering.normalize_path
                                   , parser.project_reader_t.get_os_file_names( files ) )
        tmp = map( lambda file_: os.path.split( file_ )[0], self.__parsed_files )
        self.__parsed_dirs = filter( None, tmp )

        self.__global_ns = self.__parse_declarations( files
                                                      , gccxml_config
                                                      , compilation_mode
                                                      , cache
                                                      , indexing_suite_version)
        self.__code_creator = None
        if optimize_queries:
            self.run_query_optimizer()

        self.__declarations_code_head = []
        self.__declarations_code_tail = []

        self.__registrations_code_head = []
        self.__registrations_code_tail = []
        
    @property
    def global_ns( self ):
        """reference to global namespace"""
        return self.__global_ns

    @property
    def encoding( self ):
        return self.__encoding

    def register_module_dependency( self, other_module_generated_code_dir ):
        """``already_exposed`` solution is pretty good when you mix hand-written 
        modules with Py++ generated. It doesn't work/scale for "true" 
        multi-module development. This is exactly the reason why ``Py++``_ 
        offers "semi automatic" solution.

        For every exposed module, ``Py++``_ generates "exposed_decl.pypp.txt" file. 
        This file contains the list of all parsed declarations and whether they 
        were included or excluded. Later, when you work on another module, you 
        can tell ``Py++``_ that the current module depends on the previously 
        generated one. ``Py++``_ will load "exposed_decl.pypp.txt" file and 
        update the declarations.
        """
        
        db = utils.exposed_decls_db_t()
        db.load( other_module_generated_code_dir )
        db.update_decls( self.global_ns )

    def run_query_optimizer(self):
        """
        It is possible to optimze time that takes to execute queries. In most cases
        this is done from __init__ method. But there are use-case, when you need
        to disable optimizer at __init__ and run it later.
        """
        self.__global_ns.init_optimizer()

    def __parse_declarations( self, files, gccxml_config, compilation_mode, cache, indexing_suite_version ):
        if None is gccxml_config:
            gccxml_config = parser.config_t()
        if None is compilation_mode:
            compilation_mode = parser.COMPILATION_MODE.FILE_BY_FILE
        start_time = time.clock()
        self.logger.debug( 'parsing files - started' )
        reader = parser.project_reader_t( gccxml_config, cache, decl_wrappers.dwfactory_t() )
        decls = reader.read_files( files, compilation_mode )

        self.logger.debug( 'parsing files - done( %f seconds )' % ( time.clock() - start_time ) )
        self.logger.debug( 'settings declarations defaults - started' )

        global_ns = decls_package.matcher.get_single(
                decls_package.namespace_matcher_t( name='::' )
                , decls )
        if indexing_suite_version != 1:
            for cls in global_ns.classes():
                cls.indexing_suite_version = indexing_suite_version
            for cls in global_ns.decls(decl_type=decls_package.class_declaration_t):
                cls.indexing_suite_version = indexing_suite_version

        start_time = time.clock()
        self.__apply_decls_defaults(decls)
        self.logger.debug( 'settings declarations defaults - done( %f seconds )'
                           % ( time.clock() - start_time ) )
        return global_ns

    def __filter_by_location( self, flatten_decls ):
        for decl in flatten_decls:
            if not decl.location:
                continue
            fpath = decls_package.filtering.normalize_path( decl.location.file_name )
            if decls_package.filtering.contains_parent_dir( fpath, self.__parsed_dirs ):
                continue
            if fpath in self.__parsed_files:
                continue
            found = False
            for pfile in self.__parsed_files:
                if fpath.endswith( pfile ):
                    found = True
                    break
            if not found:
                decl.exclude()

    def __apply_decls_defaults(self, decls):
        flatten_decls = decls_package.make_flatten( decls )
        self.__filter_by_location( flatten_decls )
        call_policies_resolver = mcreator_package.built_in_resolver_t()
        calldefs = filter( lambda decl: isinstance( decl, decls_package.calldef_t )
                           , flatten_decls )
        map( lambda calldef: calldef.set_call_policies( call_policies_resolver( calldef ) )
             , calldefs )
        mem_vars = filter( lambda decl: isinstance( decl, decls_package.variable_t )
                                        and isinstance( decl.parent, decls_package.class_t )
                           , flatten_decls )
        map( lambda mem_var: mem_var.set_getter_call_policies( call_policies_resolver( mem_var, 'get' ) )
             , mem_vars )
        map( lambda mem_var: mem_var.set_setter_call_policies( call_policies_resolver( mem_var, 'set' ) )
             , mem_vars )

    @property
    def declarations_code_head( self ):
        "List of user code, that will be added to the head of the declarations section."
        return self.__declarations_code_head

    @property
    def declarations_code_tail( self ):
        "List of user code, that will be added to the tail of the declarations section."
        return self.__declarations_code_tail

    @property
    def registrations_code_head( self ):
        "List of user code, that will be added to the head of the registrations section."
        return self.__registrations_code_head

    @property
    def registrations_code_tail( self ):
        "List of user code, that will be added to the tail of the registrations section."
        return self.__registrations_code_tail

    def print_declarations(self, decl=None, detailed=True, recursive=True, writer=sys.stdout.write):
        """
        This function will print detailed description of all declarations or
        some specific one.

        @param decl: optional, if passed, then only it will be printed
        @type decl: instance of L{decl_wrappers.decl_wrapper_t} class
        """
        if None is decl:
            decl = self.global_ns
        decl_wrappers.print_declarations( decl, detailed, recursive, writer )

    def build_code_creator( self
                       , module_name
                       , boost_python_ns_name='bp'
                       , create_casting_constructor=True
                       , call_policies_resolver_=None
                       , types_db=None
                       , target_configuration=None
                       , enable_indexing_suite=True
                       , doc_extractor=None):
        """
        Creates L{module_t} code creator.

        @param module_name: module name
        @type module_name: string

        @param boost_python_ns_name: boost::python namespace alias, by default
        it is 'bp'
        @type boost_python_ns_name: string

        @param call_policies_resolver_: callable, that will be invoked on every
        calldef object. It should return call policies.
        @type call_policies_resolver_: callable
        @param doc_extractor: callable, that takes as argument reference to declaration
            and returns documentation string
        @type doc_extractor: callable or None
        """
        if not create_casting_constructor:
            msg = os.linesep.join([
                      "create_casting_constructor argument is deprecated."
                    , "If want to disable boost::python::implicitly_convertible code generation, consider to use allow_implicit_conversion constructor property"
                    , ">>> mb = module_builder_t(...)"
                    , ">>> mb.constructors().allow_implicit_conversion = False"])
            warnings.warn(msg, DeprecationWarning, stacklevel=2)

            self.global_ns.constructors(allow_empty=True).allow_implicit_conversion = False

        creator = mcreator_package.creator_t( self.global_ns
                                              , module_name
                                              , boost_python_ns_name
                                              , call_policies_resolver_
                                              , types_db
                                              , target_configuration
                                              , enable_indexing_suite
                                              , doc_extractor)
        self.__code_creator = creator.create()
        self.__code_creator.replace_included_headers(self.__parsed_files)
        #I think I should ask users, what they expect
        #self.__code_creator.user_defined_directories.append( self.__working_dir )
        #map( self.__code_creator.user_defined_directories.append
        #     , self.__parsed_dirs )

        return self.__code_creator

    @property
    def code_creator( self ):
        "reference to L{code_creators.module_t} instance"
        if not self.__code_creator:
            raise RuntimeError( "self.module is equal to None. Did you forget to call build_code_creator function?" )
        return self.__code_creator

    def has_code_creator( self ):
        """
        Function, that will return True if build_code_creator function has been
        called and False otherwise
        """
        return not ( None is self.__code_creator )

    def add_declaration_code( self, code, tail=True ):
        if tail:
            self.__declarations_code_tail.append( code )
        else:
            self.__declarations_code_head.append( code )

    def add_registration_code( self, code, tail=True ):
        if tail:
            self.__registrations_code_tail.append( code )
        else:
            self.__registrations_code_head.append( code )

    def __merge_user_code( self ):
        for code in self.__declarations_code_tail:
            self.code_creator.add_declaration_code( code, -1 )

        for code in self.__declarations_code_head:
            self.code_creator.add_declaration_code( code, 0 )

        body = self.code_creator.body

        for code in self.__registrations_code_tail:
            body.adopt_creator( code_creators.custom_text_t( code ), -1 )

        for code in self.__registrations_code_head:
            body.adopt_creator( code_creators.custom_text_t( code ), 0 )


    def write_module( self, file_name ):
        """
        Writes module to single file
        @param file_name: file name
        @type file_name: string
        """
        self.__merge_user_code()
        file_writers.write_file( self.code_creator, file_name, encoding=self.encoding )

    def __work_on_unused_files( self, dir_name, written_files, on_unused_file_found ):
        all_files = os.listdir( dir_name )
        all_files = map( lambda fname: os.path.join( dir_name, fname ), all_files )
        all_files = filter( file_writers.has_pypp_extenstion, all_files )

        unused_files = set( all_files ).difference( set( written_files ) )
        for fpath in unused_files:
            try:
                if on_unused_file_found is os.remove:
                    self.logger.info( 'removing file "%s"' % fpath )
                on_unused_file_found( fpath )
            except Exception, error:
                self.logger.exception( "Exception was catched, while executing 'on_unused_file_found' function."  )

    def split_module( self
                      , dir_name
                      , huge_classes=None
                      , on_unused_file_found=os.remove
                      , use_files_sum_repository=False):
        """
        Writes module to multiple files

        @param dir_name: directory name
        @type dir_name: string

        @param huge_classes: list that contains reference to classes, that should be split

        @param on_unused_file_found: callable object that represents the action that should be taken on
                                     file, which is no more in use

        @use_files_sum_repository: Py++ can generate file, which will contain md5 sum of every generated file.
                                   Next time you generate code, md5sum will be loaded from the file and compared.
                                   This could speed-up code generation process by 10-15%.
        """
        self.__merge_user_code()
        
        files_sum_repository = None        
        if use_files_sum_repository:
            cache_file = os.path.join( dir_name, self.code_creator.body.name + '.md5.sum' )
            files_sum_repository = file_writers.cached_repository_t( cache_file )
        
        written_files = []
        if None is huge_classes:
            written_files = file_writers.write_multiple_files( 
                                self.code_creator
                                , dir_name
                                , files_sum_repository=files_sum_repository
                                , encoding=self.encoding)
        else:
            written_files = file_writers.write_class_multiple_files( 
                                self.code_creator
                                , dir_name
                                , huge_classes 
                                , files_sum_repository=files_sum_repository
                                , encoding=self.encoding)
        self.__work_on_unused_files( dir_name, written_files, on_unused_file_found )

        return written_files

    def balanced_split_module( self
                               , dir_name
                               , number_of_files
                               , on_unused_file_found=os.remove
                               , use_files_sum_repository=False):
        """
        Writes module to fixed number of multiple cpp files

        @param number_of_files: the desired number of generated cpp files
        @type number_of_files: int

        @param dir_name: directory name
        @type dir_name: string

        @param on_unused_file_found: callable object that represents the action that should be taken on
                                     file, which is no more in use

        @use_files_sum_repository: Py++ can generate file, which will contain md5 sum of every generated file.
                                   Next time you generate code, md5sum will be loaded from the file and compared.
                                   This could speed-up code generation process by 10-15%.
        """
        self.__merge_user_code()
        
        files_sum_repository = None        
        if use_files_sum_repository:
            cache_file = os.path.join( dir_name, self.code_creator.body.name + '.md5.sum' )
            files_sum_repository = file_writers.cached_repository_t( cache_file )
        
        written_files = file_writers.write_balanced_files( self.code_creator
                                                           , dir_name
                                                           , number_of_buckets=number_of_files
                                                           , files_sum_repository=files_sum_repository
                                                           , encoding=self.encoding)
                                                           
        self.__work_on_unused_files( dir_name, written_files, on_unused_file_found )

        return written_files


    #select decl(s) interfaces
    def decl( self, name=None, function=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.decl( name=name
                                    , function=function
                                    , header_dir=header_dir
                                    , header_file=header_file
                                    , recursive=recursive)

    def decls( self, name=None, function=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.decls( name=name
                                     , function=function
                                     , header_dir=header_dir
                                     , header_file=header_file
                                     , recursive=recursive)

    def class_( self, name=None, function=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.class_( name=name
                                      , function=function
                                      , header_dir=header_dir
                                      , header_file=header_file
                                      , recursive=recursive)

    def classes( self, name=None, function=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.classes( name=name
                                       , function=function
                                       , header_dir=header_dir
                                       , header_file=header_file
                                       , recursive=recursive)

    def variable( self, name=None, function=None, type=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.variable( name=name
                                        , function=function
                                        , type=type
                                        , header_dir=header_dir
                                        , header_file=header_file
                                        , recursive=recursive)
    var = variable
    
    def variables( self, name=None, function=None, type=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.variables( name=name
                                         , function=function
                                         , type=type
                                         , header_dir=header_dir
                                         , header_file=header_file
                                         , recursive=recursive)
    vars = variables
    
    def calldef( self, name=None, function=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.calldef( name=name
                                       , function=function
                                       , return_type=return_type
                                       , arg_types=arg_types
                                       , header_dir=header_dir
                                       , header_file=header_file
                                       , recursive=recursive )

    def calldefs( self, name=None, function=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.calldefs( name=name
                                        , function=function
                                        , return_type=return_type
                                        , arg_types=arg_types
                                        , header_dir=header_dir
                                        , header_file=header_file
                                        , recursive=recursive)

    def operator( self, name=None, symbol=None, return_type=None, arg_types=None, decl_type=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.operator( name=name
                                        , symbol=symbol
                                        , function=function
                                        , decl_type=decl_type
                                        , return_type=return_type
                                        , arg_types=arg_types
                                        , header_dir=header_dir
                                        , header_file=header_file
                                        , recursive=recursive )

    def operators( self, name=None, symbol=None, return_type=None, arg_types=None, decl_type=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.operators( name=name
                                         , symbol=symbol
                                         , function=function
                                         , decl_type=decl_type
                                         , return_type=return_type
                                         , arg_types=arg_types
                                         , header_dir=header_dir
                                         , header_file=header_file
                                         , recursive=recursive )

    def member_function( self, name=None, function=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.member_function( name=name
                                               , function=function
                                               , return_type=return_type
                                               , arg_types=arg_types
                                               , header_dir=header_dir
                                               , header_file=header_file
                                               , recursive=recursive )
    mem_fun = member_function

    def member_functions( self, name=None, function=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.member_functions( name=name
                                                , function=function
                                                , return_type=return_type
                                                , arg_types=arg_types
                                                , header_dir=header_dir
                                                , header_file=header_file
                                                , recursive=recursive)

    mem_funs = member_functions

    def constructor( self, name=None, function=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.constructor( name=name
                                           , function=function
                                           , return_type=return_type
                                           , arg_types=arg_types
                                           , header_dir=header_dir
                                           , header_file=header_file
                                           , recursive=recursive )

    def constructors( self, name=None, function=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.constructors( name=name
                                            , function=function
                                            , return_type=return_type
                                            , arg_types=arg_types
                                            , header_dir=header_dir
                                            , header_file=header_file
                                            , recursive=recursive)

    def member_operator( self, name=None, function=None, symbol=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.member_operator( name=name
                                               , symbol=symbol
                                               , function=function
                                               , return_type=return_type
                                               , arg_types=arg_types
                                               , header_dir=header_dir
                                               , header_file=header_file
                                               , recursive=recursive )

    def member_operators( self, name=None, function=None, symbol=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.member_operators( name=name
                                                , symbol=symbol
                                                , function=function
                                                , return_type=return_type
                                                , arg_types=arg_types
                                                , header_dir=header_dir
                                                , header_file=header_file
                                                , recursive=recursive )

    def casting_operator( self, name=None, function=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.casting_operator( name=name
                                                , function=function
                                                , return_type=return_type
                                                , arg_types=arg_types
                                                , header_dir=header_dir
                                                , header_file=header_file
                                                , recursive=recursive )

    def casting_operators( self, name=None, function=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.casting_operators( name=name
                                                 , function=function
                                                 , return_type=return_type
                                                 , arg_types=arg_types
                                                 , header_dir=header_dir
                                                 , header_file=header_file
                                                 , recursive=recursive)

    def enumeration( self, name=None, function=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.enumeration( name=name
                                           , function=function
                                           , header_dir=header_dir
                                           , header_file=header_file
                                           , recursive=recursive)
    enum = enumeration

    def enumerations( self, name=None, function=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.scopedef_t} class documentation"""
        return self.global_ns.enumerations( name=name
                                            , function=function
                                            , header_dir=header_dir
                                            , header_file=header_file
                                            , recursive=recursive)

    enums = enumerations

    def namespace( self, name=None, function=None, recursive=None ):
        """Please see L{decl_wrappers.namespace_t} class documentation"""
        return self.global_ns.namespace( name=name
                                         , function=function
                                         , recursive=recursive )

    def namespaces( self, name=None, function=None, recursive=None ):
        """Please see L{decl_wrappers.namespace_t} class documentation"""
        return self.global_ns.namespaces( name=name
                                          , function=function
                                          , recursive=recursive )

    def free_function( self, name=None, function=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.namespace_t} class documentation"""
        return self.global_ns.free_function( name=name
                                             , function=function
                                             , return_type=return_type
                                             , arg_types=arg_types
                                             , header_dir=header_dir
                                             , header_file=header_file
                                             , recursive=recursive )
    free_fun = free_function
    
    def free_functions( self, name=None, function=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.namespace_t} class documentation"""
        return self.global_ns.free_functions( name=name
                                              , function=function
                                              , return_type=return_type
                                              , arg_types=arg_types
                                              , header_dir=header_dir
                                              , header_file=header_file
                                              , recursive=recursive)
    free_funs = free_functions
    
    def free_operator( self, name=None, function=None, symbol=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.namespace_t} class documentation"""
        return self.global_ns.free_operator( name=name
                                             , symbol=symbol
                                             , function=function
                                             , return_type=return_type
                                             , arg_types=arg_types
                                             , header_dir=header_dir
                                             , header_file=header_file
                                             , recursive=recursive )

    def free_operators( self, name=None, function=None, symbol=None, return_type=None, arg_types=None, header_dir=None, header_file=None, recursive=None ):
        """Please see L{decl_wrappers.namespace_t} class documentation"""
        return self.global_ns.free_operators( name=name
                                              , symbol=symbol
                                              , function=function
                                              , return_type=return_type
                                              , arg_types=arg_types
                                              , header_dir=header_dir
                                              , header_file=header_file
                                              , recursive=recursive )

    def _get_BOOST_PYTHON_MAX_ARITY( self ):
        return decl_wrappers.calldef_t.BOOST_PYTHON_MAX_ARITY
    def _set_BOOST_PYTHON_MAX_ARITY( self, value ):
        decl_wrappers.calldef_t.BOOST_PYTHON_MAX_ARITY = value
    BOOST_PYTHON_MAX_ARITY = property( _get_BOOST_PYTHON_MAX_ARITY, _set_BOOST_PYTHON_MAX_ARITY )
