# Copyright 2006 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Initial author: Matthias Baas

"""This module contains the standard argument policy objects.

The following policies are available:

 - L{output_t}
 - L{input_t}
 - L{inout_t}
 - L{input_array_t}
 - L{output_array_t}
 - L{type_modifier_t}
"""
import os
import string
import transformer
import controllers
from pygccxml import declarations
from pyplusplus import code_repository

#TODO: pointers should be checked for NULL

def is_ref_or_ptr( type_ ):
    return declarations.is_pointer( type_ ) or declarations.is_reference( type_ )

def is_ptr_or_array( type_ ):
    return declarations.is_pointer( type_ ) or declarations.is_array( type_ )

def remove_ref_or_ptr( type_ ):
    if declarations.is_pointer( type_ ):
        return declarations.remove_pointer( type_ )
    elif declarations.is_reference( type_ ):
        return declarations.remove_reference( type_ )
    else:
        raise TypeError( 'Type should be reference or pointer, got %s.' % type_ )
    

# output_t
class output_t( transformer.transformer_t ):
    """Handles a single output variable.

    The specified variable is removed from the argument list and is turned
    into a return value.

    void getValue(int& v) -> v = getValue()
    """

    def __init__(self, function, arg_ref):
        transformer.transformer_t.__init__( self, function )
        """Constructor.

        The specified argument must be a reference or a pointer.

        @param arg_ref: Index of the argument that is an output value (the first arg has index 1).
        @type arg_ref: int
        """
        self.arg = self.get_argument( arg_ref )
        self.arg_index = self.function.arguments.index( self.arg )
        
        if not is_ref_or_ptr( self.arg.type ):
            raise ValueError( '%s\nin order to use "output" transformation, argument %s type must be a reference or a pointer (got %s).' ) \
                  % ( function, self.arg_ref.name, arg.type)

    def __str__(self):
        return "output(%d)"%(self.arg.name)

    def required_headers( self ):
        """Returns list of header files that transformer generated code depends on."""
        return [ code_repository.convenience.file_name ]

    def __configure_sealed( self, controller ):
        #removing arg from the function wrapper definition
        controller.remove_wrapper_arg( self.arg.name )
        #declaring new variable, which will keep result
        var_name = controller.declare_variable( remove_ref_or_ptr( self.arg.type ), self.arg.name )
        #adding just declared variable to the original function call expression
        controller.modify_arg_expression( self.arg_index, var_name )        
        #adding the variable to return variables list
        controller.return_variable( var_name )

    def __configure_v_mem_fun_default( self, controller ):
        self.__configure_sealed( controller )
        
    def __configure_v_mem_fun_override( self, controller ):
        controller.remove_py_arg( self.arg_index )
        tmpl = string.Template(
            '$name = boost::python::extract< $type >( pyplus_conv::get_out_argument( $py_result, "$name" ) );' )
        store_py_result_in_arg = tmpl.substitute( name=self.arg.name
                                                  , type=remove_ref_or_ptr( self.arg.type ).decl_string
                                                  , py_result=controller.py_result_variable.name )
        controller.add_py_post_call_code( store_py_result_in_arg )

    def configure_mem_fun( self, controller ):
        self.__configure_sealed( controller )
        
    def configure_free_fun(self, controller ):
        self.__configure_sealed( controller )
        
    def configure_virtual_mem_fun( self, controller ):
        self.__configure_v_mem_fun_default( controller.default_controller )
        self.__configure_v_mem_fun_override( controller.override_controller )

# input_t
class type_modifier_t(transformer.transformer_t):
    """Change/modify type of the argument.
    
    Right now compiler should be able to use implicit conversion
    """

    def __init__(self, function, arg_ref, modifier):
        """Constructor.

        modifier is callable, which take the type of the argument and should return 
        new type
        """
        transformer.transformer_t.__init__( self, function )
        self.arg = self.get_argument( arg_ref )
        self.arg_index = self.function.arguments.index( self.arg )
        self.modifier = modifier

    def __str__(self):
        return "type_modifier(%s)" % self.arg.name

    def __configure_sealed( self, controller ):
        w_arg = controller.find_wrapper_arg( self.arg.name )
        w_arg.type = self.modifier( self.arg.type )
        if not declarations.is_convertible( w_arg.type, self.arg.type ):
            casting_code = 'reinterpret_cast< %s >( %s )' % ( self.arg.type, w_arg.name )
            controller.modify_arg_expression(self.arg_index, casting_code)
            
    def __configure_v_mem_fun_default( self, controller ):
        self.__configure_sealed( controller )

    def configure_mem_fun( self, controller ):
        self.__configure_sealed( controller )
        
    def configure_free_fun(self, controller ):
        self.__configure_sealed( controller )

    def configure_virtual_mem_fun( self, controller ):
        self.__configure_v_mem_fun_default( controller.default_controller ) 

    def required_headers( self ):
        """Returns list of header files that transformer generated code depends on."""
        return []
        
# input_t
class input_t(type_modifier_t):
    """Handles a single input variable.

    The reference on the specified variable is removed.

    void setValue(int& v) -> setValue(v)
    """

    def __init__(self, function, arg_ref):
        """Constructor.

        The specified argument must be a reference or a pointer.

        @param idx: Index of the argument that is an output value (the first arg has index 1).
        @type idx: int
        """
        type_modifier_t.__init__( self, function, arg_ref, remove_ref_or_ptr )

        if not is_ref_or_ptr( self.arg.type ):
            raise ValueError( '%s\nin order to use "input" transformation, argument %s type must be a reference or a pointer (got %s).' ) \
                  % ( function, self.arg_ref.name, arg.type)

    def __str__(self):
        return "input(%s)"%(self.arg.name)

# inout_t
class inout_t(transformer.transformer_t):
    """Handles a single input/output variable.

    void foo(int& v) -> v = foo(v)
    """

    def __init__(self, function, arg_ref):
        """Constructor.

        The specified argument must be a reference or a pointer.

        @param idx: Index of the argument that is an in/out value (the first arg has index 1).
        @type idx: int
        """
        transformer.transformer_t.__init__( self, function )
        self.arg = self.get_argument( arg_ref )
        self.arg_index = self.function.arguments.index( self.arg )
        
        if not is_ref_or_ptr( self.arg.type ):
            raise ValueError( '%s\nin order to use "inout" transformation, argument %s type must be a reference or a pointer (got %s).' ) \
                  % ( function, self.arg_ref.name, arg.type)

    def __str__(self):
        return "inout(%s)"%(self.arg.name)

    def __configure_sealed(self, controller):
        w_arg = controller.find_wrapper_arg( self.arg.name )
        w_arg.type = remove_ref_or_ptr( self.arg.type )
        #adding the variable to return variables list
        controller.return_variable( w_arg.name )
    
    def __configure_v_mem_fun_default( self, controller ):
        self.__configure_sealed( controller )
        
    def __configure_v_mem_fun_override( self, controller ):
        tmpl = string.Template(
            '$name = boost::python::extract< $type >( pyplus_conv::get_out_argument( $py_result, "$name" ) );' )
        store_py_result_in_arg = tmpl.substitute( name=self.arg.name
                                                  , type=self.arg.type.decl_string
                                                  , py_result=controller.py_result_variable.name )
        controller.add_py_post_call_code( store_py_result_in_arg )

    def configure_mem_fun( self, controller ):
        self.__configure_sealed( controller )
        
    def configure_free_fun(self, controller ):
        self.__configure_sealed( controller )

    def configure_virtual_mem_fun( self, controller ):
        self.__configure_v_mem_fun_override( controller.override_controller )
        self.__configure_v_mem_fun_default( controller.default_controller )

    def required_headers( self ):
        """Returns list of header files that transformer generated code depends on."""
        return [ code_repository.convenience.file_name ]


_seq2arr = string.Template( os.linesep.join([
              'pyplus_conv::ensure_uniform_sequence< $type >( $pylist, $array_size );'
            , 'pyplus_conv::copy_sequence( $pylist, pyplus_conv::array_inserter( $native_array, $array_size ) );']))

_seq2vector = string.Template( os.linesep.join([
                 'pyplus_conv::ensure_uniform_sequence< $type >( $pylist );'
               , 'pyplus_conv::copy_sequence( $pylist, std::back_inserter( $native_array), boost::type< $type >() );']))

_arr2seq = string.Template( 
            'pyplus_conv::copy_container( $native_array, $native_array + $array_size, pyplus_conv::list_inserter( $pylist ) );' )

class input_static_array_t(transformer.transformer_t):
    """Handles an input array with fixed size.

    void setVec3(double* v) ->  setVec3(object v)
    # v must be a sequence of 3 floats
    """

    def __init__(self, function, arg_ref, size):
        """Constructor.

        @param size: The fixed size of the input array
        @type size: int
        """
        transformer.transformer_t.__init__( self, function )
        
        self.arg = self.get_argument( arg_ref )
        self.arg_index = self.function.arguments.index( self.arg )
        
        if not is_ptr_or_array( self.arg.type ):
            raise ValueError( '%s\nin order to use "input_array" transformation, argument %s type must be a array or a pointer (got %s).' ) \
                  % ( function, self.arg.name, self.arg.type)

        self.array_size = size
        self.array_item_type = declarations.remove_const( declarations.array_item_type( self.arg.type ) )

    def __str__(self):
        return "input_array(%s,%d)"%( self.arg.name, self.array_size)

    def required_headers( self ):
        """Returns list of header files that transformer generated code depends on."""
        return [ code_repository.convenience.file_name ]

    def __configure_sealed(self, controller):
        global _seq2arr
        w_arg = controller.find_wrapper_arg( self.arg.name )
        w_arg.type = declarations.dummy_type_t( "boost::python::object" )

        # Declare a variable that will hold the C array...
        native_array = controller.declare_variable( self.array_item_type
                                                    , "native_" + self.arg.name
                                                    , '[%d]' % self.array_size )
            
        copy_pylist2arr = _seq2arr.substitute( type=self.array_item_type
                                                , pylist=w_arg.name
                                                , array_size=self.array_size
                                                , native_array=native_array )
        
        controller.add_pre_call_code( copy_pylist2arr )
        
        controller.modify_arg_expression( self.arg_index, native_array )        

    def __configure_v_mem_fun_default( self, controller ):
        self.__configure_sealed( controller )
        
    def __configure_v_mem_fun_override( self, controller ):
        global _arr2seq
        pylist = controller.declare_py_variable( declarations.dummy_type_t( 'boost::python::list' )
                                                 , 'py_' + self.arg.name )
        
        copy_arr2pylist = _arr2seq.substitute( native_array=self.arg.name
                                                , array_size=self.array_size
                                                , pylist=pylist )
                            
        controller.add_py_pre_call_code( copy_arr2pylist )

    def configure_mem_fun( self, controller ):
        self.__configure_sealed( controller )
        
    def configure_free_fun(self, controller ):
        self.__configure_sealed( controller )

    def configure_virtual_mem_fun( self, controller ):
        self.__configure_v_mem_fun_override( controller.override_controller )
        self.__configure_v_mem_fun_default( controller.default_controller )
        

# s - static
class output_static_array_t(transformer.transformer_t):
    """Handles an output array of a fixed size.

    void getVec3(double* v) -> v = getVec3()
    # v will be a list with 3 floats
    """

    def __init__(self, function, arg_ref, size):
        """Constructor.

        @param idx: Index of the argument that is an output array (the first arg has index 1).
        @type idx: int
        @param size: The fixed size of the output array
        @type size: int
        """
        transformer.transformer_t.__init__( self, function )
        self.arg = self.get_argument( arg_ref )
        self.arg_index = self.function.arguments.index( self.arg )

        if not is_ptr_or_array( self.arg.type ):
            raise ValueError( '%s\nin order to use "input_array" transformation, argument %s type must be a array or a pointer (got %s).' ) \
                  % ( function, self.arg.name, self.arg.type)

        self.array_size = size
        self.array_item_type = declarations.array_item_type( self.arg.type )

    def __str__(self):
        return "output_array(%s,%d)"%( self.arg.name, self.array_size)

    def required_headers( self ):
        """Returns list of header files that transformer generated code depends on."""
        return [ code_repository.convenience.file_name ]

    def __configure_sealed(self, controller):
        global _arr2seq
        #removing arg from the function wrapper definition
        controller.remove_wrapper_arg( self.arg.name )

        # Declare a variable that will hold the C array...
        native_array = controller.declare_variable( self.array_item_type
                                                    , "native_" + self.arg.name
                                                    , '[%d]' % self.array_size )

        #adding just declared variable to the original function call expression
        controller.modify_arg_expression( self.arg_index, native_array )        

        # Declare a Python list which will receive the output...
        pylist = controller.declare_variable( declarations.dummy_type_t( "boost::python::list" )
                                              , 'py_' + self.arg.name )
    
        copy_arr2pylist = _arr2seq.substitute( native_array=native_array
                                               , array_size=self.array_size
                                               , pylist=pylist )
        
        controller.add_post_call_code( copy_arr2pylist )

        #adding the variable to return variables list
        controller.return_variable( pylist )

    def __configure_v_mem_fun_default( self, controller ):
        self.__configure_sealed( controller )

    def __configure_v_mem_fun_override( self, controller ):
        global _seq2arr
        seq = controller.declare_py_variable( declarations.dummy_type_t( 'boost::python::object' )
                                              , 'py_' + self.arg.name )
        controller.remove_py_arg( self.arg_index )
        tmpl = string.Template( '$seq = pyplus_conv::get_out_argument( $py_result, "$name" );' )
        get_ref_to_seq = tmpl.substitute( seq=seq
                                          , py_result=controller.py_result_variable.name
                                          , name=self.arg.name )
        controller.add_py_post_call_code( get_ref_to_seq )
        
        copy_pylist2arr = _seq2arr.substitute( type=self.array_item_type
                                               , pylist=seq
                                               , array_size=self.array_size
                                               , native_array=self.arg.name )
        controller.add_py_post_call_code( copy_pylist2arr )
        
    def configure_mem_fun( self, controller ):
        self.__configure_sealed( controller )
        
    def configure_free_fun(self, controller ):
        self.__configure_sealed( controller )

    def configure_virtual_mem_fun( self, controller ):
        self.__configure_v_mem_fun_override( controller.override_controller )
        self.__configure_v_mem_fun_default( controller.default_controller )


class input_c_buffer_t(transformer.transformer_t):
    """Handles an input of C buffere:

    void write( byte *buffer, int size ) -> void write( python sequence )
    """

    def __init__(self, function, buffer_arg_ref, size_arg_ref):
        """Constructor.

        @param buffer_arg_ref: "reference" to the buffer argument 
        @param buffer_arg_ref: "reference" to argument, which holds buffer size
        """
        transformer.transformer_t.__init__( self, function )
        
        self.buffer_arg = self.get_argument( buffer_arg_ref )
        self.buffer_arg_index = self.function.arguments.index( self.buffer_arg )

        self.size_arg = self.get_argument( size_arg_ref )
        self.size_arg_index = self.function.arguments.index( self.size_arg )

        if not is_ptr_or_array( self.buffer_arg.type ):
            raise ValueError( '%s\nin order to use "input_c_buffer" transformation, "buffer" argument %s type must be a array or a pointer (got %s).' ) \
                  % ( function, self.buffer_arg.name, self.buffer_arg.type)

        if not declarations.is_integral( self.size_arg.type ):
            raise ValueError( '%s\nin order to use "input_c_buffer" transformation, "size" argument %s type must be an integral type (got %s).' ) \
                  % ( function, self.size_arg.name, self.size_arg.type)

        self.buffer_item_type = declarations.remove_const( declarations.array_item_type( self.buffer_arg.type ) )

    def __str__(self):
        return "input_c_buffer(buffer arg=%s, size arg=%s)" \
               % ( self.buffer_arg.name, self.size_arg.name)

    def required_headers( self ):
        """Returns list of header files that transformer generated code depends on."""
        return [ code_repository.convenience.file_name, '<vector>', '<iterator>' ]

    def __configure_sealed(self, controller):
        global _seq2arr
        w_buffer_arg = controller.find_wrapper_arg( self.buffer_arg.name )
        w_buffer_arg.type = declarations.dummy_type_t( "boost::python::object" )
        
        controller.remove_wrapper_arg( self.size_arg.name )

        size_var = controller.declare_variable( 
                          declarations.remove_const( self.size_arg.type )
                        , self.size_arg.name
                        , ' = boost::python::len(%s)' % w_buffer_arg.name )
        
        # Declare a variable that will hold the C array...
        buffer_var = controller.declare_variable( 
                          declarations.dummy_type_t( "std::vector< %s >" % self.buffer_item_type.decl_string )
                        , "native_" + self.buffer_arg.name )

        controller.add_pre_call_code( '%s.reserve( %s );' % ( buffer_var, size_var ) )
        
        copy_pylist2arr = _seq2vector.substitute( type=self.buffer_item_type
                                                  , pylist=w_buffer_arg.name
                                                  , native_array=buffer_var )
        
        controller.add_pre_call_code( copy_pylist2arr )
        
        controller.modify_arg_expression( self.buffer_arg_index, '&%s[0]' % buffer_var )        
        controller.modify_arg_expression( self.size_arg_index, '%s' % size_var )        

    def __configure_v_mem_fun_default( self, controller ):
        self.__configure_sealed( controller )
        
    def __configure_v_mem_fun_override( self, controller ):
        raise NotImplementedError()
        #global _arr2seq
        #pylist = controller.declare_py_variable( declarations.dummy_type_t( 'boost::python::list' )
                                                 #, 'py_' + self.arg.name )
        
        #copy_arr2pylist = _arr2seq.substitute( native_array=self.arg.name
                                                #, array_size=self.array_size
                                                #, pylist=pylist )
                            
        #controller.add_py_pre_call_code( copy_arr2pylist )

    def configure_mem_fun( self, controller ):
        self.__configure_sealed( controller )
        
    def configure_free_fun(self, controller ):
        self.__configure_sealed( controller )

    def configure_virtual_mem_fun( self, controller ):
        self.__configure_v_mem_fun_override( controller.override_controller )
        self.__configure_v_mem_fun_default( controller.default_controller )
        

class transfer_ownership_t(type_modifier_t):
    """see http://boost.org/libs/python/doc/v2/faq.html#ownership
    """
    def __init__(self, function, arg_ref):
        """Constructor."""
        transformer.transformer_t.__init__( self, function )
        self.arg = self.get_argument( arg_ref )
        self.arg_index = self.function.arguments.index( self.arg )
        if not declarations.is_pointer( self.arg.type ):
            raise ValueError( '%s\nin order to use "transfer ownership" transformation, argument %s type must be a pointer (got %s).' ) \
                  % ( function, self.arg_ref.name, arg.type)

    def __str__(self):
        return "transfer_ownership(%s)" % self.arg.name

    def __configure_sealed( self, controller ):
        w_arg = controller.find_wrapper_arg( self.arg.name )
        naked_type = declarations.remove_pointer( self.arg.type )
        naked_type = declarations.remove_declarated( naked_type )
        w_arg.type = declarations.dummy_type_t( 'std::auto_ptr< %s >' % naked_type.decl_string )
        controller.modify_arg_expression(self.arg_index, w_arg.name + '.release()' )
            
    def __configure_v_mem_fun_default( self, controller ):
        self.__configure_sealed( controller )

    def configure_mem_fun( self, controller ):
        self.__configure_sealed( controller )
        
    def configure_free_fun(self, controller ):
        self.__configure_sealed( controller )

    def configure_virtual_mem_fun( self, controller ):
        raise NotImplementedError(self.__class__.__name__)

#TODO: FT for constructor
    #~ def configure_constructor( self, controller ):
        #~ self.__configure_sealed( controller )
        
    def required_headers( self ):
        """Returns list of header files that transformer generated code depends on."""
        return []

