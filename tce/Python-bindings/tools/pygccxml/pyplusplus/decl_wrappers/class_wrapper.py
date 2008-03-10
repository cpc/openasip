# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""defines class that configure class definition and class declaration exposing"""

import os
import user_text
import properties
import decl_wrapper
import scopedef_wrapper
from pyplusplus import messages
from pygccxml import declarations
import indexing_suite1 as isuite1
import indexing_suite2 as isuite2

ACCESS_TYPES = declarations.ACCESS_TYPES
VIRTUALITY_TYPES = declarations.VIRTUALITY_TYPES

class impl_details:
    class GUESS_VALUES: #guess always expose using scope values
        TRUE = 'true'
        FALSE = 'false'
        ALWAYS_TRUE = 'always true'
        all = [ TRUE,  FALSE, ALWAYS_TRUE ]


always_expose_using_scope_documentation = \
"""boolean, configures how Py++ should generate code for class.
Py can generate code using IDL like syntax:

    class_< ... >( ... )
        .def( ... );

Or it can generate code using more complex form:

    typedef bp::class_< my_class > my_class_exposer_t;
    my_class_exposer_t my_class_exposer = my_class_exposer_t( "my_class" );
    boost::python::scope my_class_scope( my_class_exposer );
    my_class_exposer.def( ... );

Also, the second way is much longer, it solves few problems:

    - you can not expose enums and internal classes defined within the class using first method
    - you will get much better compilation errors
    - the code looks like regular C++ code after all :-)

By default, this property is set to False. Also, Py++ knows pretty well
when it have to ignore this property and generate right code
"""

class class_common_details_t( object ):
    """defines few properties that are common to
    L{class declaration<pygccxml.declarations.class_declaration_t>} and
    L{definition<pygccxml.declarations.class_t>} classes
    """
    def __init__(self):
        object.__init__( self )
        self._always_expose_using_scope = None
        self._indexing_suite = None
        self._equality_comparable = None
        self._less_than_comparable = None
        self._isuite_version = 1
        self._opaque = False

    def _get_indexing_suite_version( self ):
        return self._isuite_version
    def _set_indexing_suite_version( self, version ):
        assert version in ( 1, 2 )
        if self._isuite_version != version:
            self._isuite_version = version
            self._indexing_suite = None
    indexing_suite_version = property( _get_indexing_suite_version, _set_indexing_suite_version
                                       , doc="indexing suite version")

    @property
    def indexing_suite( self ):
        """reference to indexing suite configuration class. 
        
        If the class is not STD container, this property will contain None"
        """
        if self._indexing_suite is None:
            if self.container_traits:
                if self._isuite_version == 1:
                    self._indexing_suite = isuite1.indexing_suite1_t( self )
                else:
                    self._indexing_suite = isuite2.indexing_suite2_t( self )
        return self._indexing_suite

    def guess_always_expose_using_scope_value( self ):
        if isinstance( self.indexing_suite, isuite2.indexing_suite2_t ) \
           and ( self.indexing_suite.disable_methods or self.indexing_suite.disabled_methods_groups ):
            return impl_details.GUESS_VALUES.ALWAYS_TRUE
        else:
            return impl_details.GUESS_VALUES.FALSE

    def _get_always_expose_using_scope( self ):
        tmp = self.guess_always_expose_using_scope_value()
        if tmp == impl_details.GUESS_VALUES.ALWAYS_TRUE:
            return True
        if None is self._always_expose_using_scope:
            if impl_details.GUESS_VALUES.TRUE == tmp:
                self._always_expose_using_scope = True
            else:
                self._always_expose_using_scope = False
        return self._always_expose_using_scope
        
    def _set_always_expose_using_scope( self, value ):
        self._always_expose_using_scope = value
    always_expose_using_scope = property( _get_always_expose_using_scope, _set_always_expose_using_scope
                                          , doc="please see L{class_wrapper.always_expose_using_scope_documentation} variable for documentation."  )

    def _get_equality_comparable( self ):
        if None is self._equality_comparable:
            self._equality_comparable = declarations.has_public_equal( self )
        return self._equality_comparable

    def _set_equality_comparable( self, value ):
        self._equality_comparable = value

    equality_comparable = property( _get_equality_comparable, _set_equality_comparable
                                    , doc="indicates existence of public operator=" \
                                         +"Default value is calculated, based on information presented in the declarations tree" )

    def _get_less_than_comparable( self ):
        if None is self._less_than_comparable:
            self._less_than_comparable = declarations.has_public_less( self )
        return self._less_than_comparable

    def _set_less_than_comparable( self, value ):
        self._less_than_comparable = value

    less_than_comparable = property( _get_less_than_comparable, _set_less_than_comparable
                                     , doc="indicates existence of public operator<. " \
                                          +"Default value is calculated, based on information presented in the declarations tree" )
        
    def _get_opaque( self ):
        return self._opaque

    def _set_opaque( self, value ):
        self._opaque = value        
        self.ignore = value #don't expose opaque type

    opaque = property( _get_opaque, _set_opaque
                      , doc="If True, Py++ will treat return types and arguments T* as opaque types." \
                            +"Thus it will be able to generate code, that uses " \
                            +" BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID macro in a right places." )

    @property
    def class_var_name(self):
        return self.alias + '_exposer'

#this will only be exported if indexing suite is not None and only when needed
class class_declaration_t( class_common_details_t
                           , decl_wrapper.decl_wrapper_t
                           , declarations.class_declaration_t ):
    def __init__(self, *arguments, **keywords):
        class_common_details_t.__init__( self )
        declarations.class_declaration_t.__init__(self, *arguments, **keywords )
        decl_wrapper.decl_wrapper_t.__init__( self )

class class_t( class_common_details_t
               , scopedef_wrapper.scopedef_t
               , declarations.class_t):
    
    class EXPOSED_CLASS_TYPE:
        DECLARED = 'declared'
        WRAPPER = 'wrapper'
        ALL = ( DECLARED, WRAPPER )
        
    def __init__(self, *arguments, **keywords):
        class_common_details_t.__init__( self )
        declarations.class_t.__init__(self, *arguments, **keywords )
        scopedef_wrapper.scopedef_t.__init__( self )

        self._redefine_operators = False
        self._held_type = None
        self._noncopyable = None
        self._wrapper_alias = self._generate_valid_name() + "_wrapper"
        self._registration_code = []
        self._declaration_code = []
        self._wrapper_code = []
        self._null_constructor_body = ''
        self._copy_constructor_body = ''
        self._exception_translation_code = None
        self._properties = []
        self._redefined_funcs = None
        self._require_self_reference  = False
        self._exposed_class_type = self.EXPOSED_CLASS_TYPE.DECLARED
        
    def _get_redefine_operators( self ):
        return self._redefine_operators
    def _set_redefine_operators( self, new_value ):
        self._redefine_operators = new_value
    redefine_operators = property( _get_redefine_operators, _set_redefine_operators
                                   , doc="tells Py++ to redefine operators from base class in this class, False by default")

    def _get_exposed_class_type(self):
        return self._exposed_class_type
    def _set_exposed_class_type(self, class_type):
        assert class_type in self.EXPOSED_CLASS_TYPE.ALL
        self._class_type = class_type
    exposed_class_type = property( _get_exposed_class_type, _set_exposed_class_type
                          , doc="set this value to CLASS_TYPE.WRAPPER, if you need to transfer ownership of" \
                                "polymorphic class" )

    def _get_held_type(self):
        return self._held_type
    def _set_held_type(self, held_type):
        self._held_type = held_type
    held_type = property( _get_held_type, _set_held_type
                          , doc="string, this property tells Py++ what HeldType this class has" \
                               +"Default value is calculated, based on information presented in exposed declarations" )

    def _get_noncopyable(self):
        if self._noncopyable is None:
            self._noncopyable = declarations.is_noncopyable( self )
        return self._noncopyable
    def _set_noncopyable(self, noncopyable):
        self._noncopyable= noncopyable
    noncopyable = property( _get_noncopyable, _set_noncopyable
                            , doc="True if the class is noncopyable, False otherwies" \
                                 +"Default value is calculated, based on information presented in the declarations tree" )

    def _get_wrapper_alias( self ):
        return self._wrapper_alias
    def _set_wrapper_alias( self, walias ):
        self._wrapper_alias = walias
    wrapper_alias = property( _get_wrapper_alias, _set_wrapper_alias
                              , doc="class-wrapper name")

    @property
    def declaration_code( self ):
        """
        List of strings, that contains valid C++ code, that will be added to
        the class registration section
        """
        return self._declaration_code

    @property
    def registration_code( self ):
        """
        List of strings, that contains valid C++ code, that will be added to
        the class registration section
        """
        return self._registration_code

    @property
    def wrapper_code( self ):
        """
        List of strings, that contains valid C++ code, that will be added to
        the class wrapper.
        """
        return self._wrapper_code

    def _get_null_constructor_body(self):
        return self._null_constructor_body
    def _set_null_constructor_body(self, body):
        self._null_constructor_body = body
    null_constructor_body = property( _get_null_constructor_body, _set_null_constructor_body
                                      , doc="null constructor code, that will be added as is to the null constructor of class-wrapper")

    def _get_copy_constructor_body(self):
        return self._copy_constructor_body
    def _set_copy_constructor_body(self, body):
        self._copy_constructor_body = body
    copy_constructor_body = property( _get_copy_constructor_body, _set_copy_constructor_body
                                      , doc="copy constructor code, that will be added as is to the copy constructor of class-wrapper")

    @property
    def exception_argument_name( self ):
        """exception argument name for translate exception function

        If you don't understand what this argument is, please take a look on
        Boost.Python documentation: http://www.boost.org/libs/python/doc/v2/exception_translator.html
        """
        return 'exc'

    def _get_exception_translation_code( self ):
        return self._exception_translation_code
    def _set_exception_translation_code( self, code ):
        self._exception_translation_code = code
    exception_translation_code = property( _get_exception_translation_code, _set_exception_translation_code
                                           , doc="C++ exception to Python exception translation code" \
                                                +"\nExample: PyErr_SetString(PyExc_RuntimeError, exc.what()); " \
                                                +"\nPy++ will generate the rest of the code." \
                                                +"\nPay attention: the exception variable name is exc." )

    def translate_exception_to_string( self, python_exception_type, to_string ):
        """registers exception translation to string

        @param python_exception_type: Python exception type, for example PyExc_RuntimeError
        @type python_exception_type: str

        @param to_string: C++ expression that extracts information from exception.
                          The type of expression should be char*.
        @type to_string: str
        """
        #NICE TO HAVE:
        #1. exception\assert\warning should be raised if python_exception_type
        #   does not contain valid Python exception
        #2. Py++ can validate, that member function returns char*
        code = "PyErr_SetString( %(exception_type)s, %(to_string)s ); " \
               % { 'exception_type' : python_exception_type, 'to_string' : to_string }
        self.exception_translation_code = code

    def add_declaration_code( self, code ):
        """adds the code to the declaration section"""
        self.declaration_code.append( user_text.user_text_t( code ) )

    def add_registration_code( self, code, works_on_instance=True ):
        """adds the code to the class registration section

        works_on_instance: If true, the custom code can be applied directly to obj inst.
        Example: ObjInst."CustomCode"
        """
        self.registration_code.append( user_text.class_user_text_t( code, works_on_instance ) )
    #preserving backward computability
    add_code = add_registration_code

    def add_wrapper_code( self, code ):
        """adds code to the class wrapper class definition"""
        self.wrapper_code.append( user_text.user_text_t( code ) )

    def set_constructors_body( self, body ):
        """Sets the body for all constructors"""
        self.constructors().body = body
        self.null_constructor_body = body
        self.copy_constructor_body = body

    def _exportable_impl( self ):
        if not self.name:
            return messages.W1018
            #it is possible to do so, but not for unnamed classes defined under namespace.
        if isinstance( self.parent, declarations.namespace_t ):
            return ''
        if not self in self.parent.public_members:
            return messages.W1019
        return ''

    def get_exportable_members( self, sort=None ):
        """returns list of internal declarations that should\\could be exported"""
        #TODO: obviously this function should be shorter. Almost all logic of this class
        #      should be spread between decl_wrapper classes
        members = filter( lambda mv: mv.ignore == False and mv.exportable, self.public_members )
        #protected and private virtual functions that not overridable and not pure
        #virtual should not be exported
        for member in self.protected_members:
            if not isinstance( member, declarations.calldef_t ):
                continue
            else:
                members.append( member )

        vfunction_selector = lambda member: isinstance( member, declarations.member_function_t ) \
                                            and member.virtuality == declarations.VIRTUALITY_TYPES.PURE_VIRTUAL
        members.extend( filter( vfunction_selector, self.private_members ) )
        
        def is_exportable( decl ):
            #filter out non-public member operators - Py++ does not support them right now
            if isinstance( decl, declarations.member_operator_t ) \
               and decl.access_type != declarations.ACCESS_TYPES.PUBLIC:
                return False
            #remove artificial constructors
            if isinstance( decl, declarations.constructor_t ) and decl.is_artificial:
                return False
            if decl.ignore == True or decl.exportable == False:
                return False
            return True                
        #-#if declarations.has_destructor( self ) \
        #-#   and not declarations.has_public_destructor( self ):
        members = filter( is_exportable, members )
        sorted_members = members
        if sort:
            sorted_members = sort( members )
        return sorted_members

    @property
    def properties( self ):
        """list of properties"""
        return self._properties

    def add_property( self, name, fget, fset=None, doc='' ):
        """adds new property to the class

        @param name: name of the property
        @type name: str

        @param fget: reference to the class member function
        @param fset: reference to the class member function, could be None
        @param doc: documentation string
        """
        self._properties.append( properties.property_t( name, fget, fset, doc ) )

    def add_properties( self, recognizer=None, exclude_accessors=False ):
        props = properties.find_properties( self, recognizer, exclude_accessors )
        self.properties.extend( props )
                        
    def add_static_property( self, name, fget, fset=None, doc='' ):
        """adds new static property to the class"""
        self._properties.append( properties.property_t( name, fget, fset, doc, True ) )

    def redefined_funcs( self ):
        """returns list of member functions that should be defined in class wrapper

        It comes useful in 3 tier hierarchy:
        struct base{
            virtual void do_nothing() = 0;
        };

        struct derived{
            virtual void do_something() = 0;
        };

        struct concrete{
            virtual void do_nothing(){}
            virtual void do_something(){}
        };

        derived_wrapper should define do_nothing function, otherwise the generated
        code will not compile
        """

        if isinstance( self._redefined_funcs, list ):
            return self._redefined_funcs

        all_included = declarations.custom_matcher_t( lambda decl: decl.ignore == False and decl.exportable )
        all_protected = declarations.access_type_matcher_t( 'protected' ) & all_included
        all_pure_virtual = declarations.virtuality_type_matcher_t( VIRTUALITY_TYPES.PURE_VIRTUAL )
        all_not_pure_virtual = ~all_pure_virtual

        query = all_protected | all_pure_virtual
        relevant_opers = declarations.custom_matcher_t( lambda decl: decl.symbol in ('()', '[]') )
        funcs = set()
        defined_funcs = set()

        for base in self.recursive_bases:
            if base.access == ACCESS_TYPES.PRIVATE:
                continue
            base_cls = base.related_class
            funcs.update( base_cls.member_functions( query, allow_empty=True ) )
            funcs.update( base_cls.member_operators( relevant_opers & query, allow_empty=True ) )

            defined_funcs.update( base_cls.member_functions( all_not_pure_virtual, allow_empty=True ) )
            defined_funcs.update( base_cls.member_operators( all_not_pure_virtual & relevant_opers, allow_empty=True ) )

        not_reimplemented_funcs = set()
        is_same_function = declarations.is_same_function
        for f in funcs:
            cls_fs = self.calldefs( name=f.name, recursive=False, allow_empty=True )
            for cls_f in cls_fs:
                if is_same_function( f, cls_f ):
                    break
            else:
                #should test whether this function has been added or not
                for f_impl in not_reimplemented_funcs:
                    if is_same_function( f, f_impl ):
                        if declarations.is_base_and_derived( f_impl.parent, f.parent ):
                            #add function from the most derived class
                            not_reimplemented_funcs.remove( f_impl )
                            not_reimplemented_funcs.add( f )                       
                        break
                else:
                    #should test whether this function is implemented in base class
                    if f.virtuality != VIRTUALITY_TYPES.PURE_VIRTUAL:
                        not_reimplemented_funcs.add( f )
                    else:
                        for f_defined in defined_funcs:
                            if is_same_function( f, f_defined ):
                                break
                        else:
                            not_reimplemented_funcs.add( f )
        functions = list( not_reimplemented_funcs )
        functions.sort( cmp=lambda f1, f2: cmp( ( f1.name, f1.location.as_tuple() )
                                                , ( f2.name, f2.location.as_tuple() ) ) )
        self._redefined_funcs = functions
        return self._redefined_funcs

    def is_wrapper_needed(self):
        """returns an explanation( list of str ) why wrapper is needed.

        If wrapper is not needed than [] will be returned.
        """
        explanation = []
        if self.wrapper_code:
            explanation.append( messages.W1020 )

        if self.null_constructor_body:
            explanation.append( messages.W1021 )

        if self.copy_constructor_body:
            explanation.append( messages.W1022 )

        redefined_funcs = self.redefined_funcs()
        if redefined_funcs:
            funcs = map( lambda f: f.name, redefined_funcs )
            explanation.append( messages.W1023 % ', '.join(funcs) )

        for member in self.get_exportable_members():
            if isinstance( member, declarations.destructor_t ):
                continue
            if isinstance( member, declarations.variable_t ):
                if member.bits:
                    explanation.append( messages.W1024 % member.name )
                if declarations.is_pointer( member.type ):
                    explanation.append( messages.W1025 % member.name )
                if declarations.is_reference( member.type ):
                    explanation.append( messages.W1026 % member.name )
                if declarations.is_array( member.type ):
                    explanation.append( messages.W1027 % member.name)
            if isinstance( member, declarations.class_t ) and member.is_wrapper_needed():
                explanation.append( messages.W1028 % member.name)
            if isinstance( member, declarations.calldef_t ):
                if isinstance( member, declarations.constructor_t ) and member.body:
                    explanation.append( messages.W1029 )
                if member.virtuality != VIRTUALITY_TYPES.NOT_VIRTUAL:
                    explanation.append( messages.W1030 % member.name )
                if member.access_type in ( ACCESS_TYPES.PROTECTED, ACCESS_TYPES.PRIVATE ):
                    explanation.append( messages.W1031 % member.name)
        return explanation

    def _readme_impl( self ):
        return self.is_wrapper_needed()

    def guess_always_expose_using_scope_value( self ):
        def is_assign( oper ):
            if oper.symbol != '=':
                return False
            if oper.is_artificial:
                return False
            if oper.access_type != ACCESS_TYPES.PUBLIC:
                return False
            return True
        #MSVC 7.1 has problem with taking reference to operator=
        if self.member_operators( is_assign, allow_empty=True, recursive=False ):
            return impl_details.GUESS_VALUES.ALWAYS_TRUE
        return super(class_t, self).guess_always_expose_using_scope_value()    

    def _get_require_self_reference(self):
        return self._require_self_reference
    def _set_require_self_reference(self, require_self_reference):
        self._require_self_reference = require_self_reference
    require_self_reference = property( _get_require_self_reference, _set_require_self_reference
                     , doc="boolean, if True the first argument to the constructor will be reference to self object" )
