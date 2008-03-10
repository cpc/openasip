# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""defines class that configure global and member variable exposing"""

import decl_wrapper
import python_traits
import call_policies
from pyplusplus import messages
from pygccxml import declarations

class variable_t(decl_wrapper.decl_wrapper_t, declarations.variable_t):
    """defines a set of properties, that will instruct Py++ how to expose the variable"""
    def __init__(self, *arguments, **keywords):
        declarations.variable_t.__init__(self, *arguments, **keywords )
        decl_wrapper.decl_wrapper_t.__init__( self )
        self._getter_call_policies = None
        self._setter_call_policies = None
        self._apply_smart_ptr_wa = False
        self._is_read_only = None
        self._use_make_functions = None
    
    __call_policies_doc__ = \
    """There are usecase, when exporting member variable forces Py++ to
    create accessors functions. Sometime, those functions requires call policies.
    To be more specific: when you export member variable that has reference or
    pointer type, you need to tell Boost.Python library how to manage object
    life-time. In all cases, Py++ will give reasonable default value. I am
    sure, that there are use cases, when you will have to change it. You should
    use this property to change it.
    """

    def get_getter_call_policies( self ):
        if None is self._getter_call_policies:
            if self.apply_smart_ptr_wa:
                value_policy = ''
                if self.is_read_only:
                   value_policy = call_policies.copy_const_reference
                else:
                    value_policy = call_policies.copy_non_const_reference
                self._getter_call_policies = call_policies.return_value_policy( value_policy )
            elif self.use_make_functions:
                self._getter_call_policies = call_policies.return_internal_reference()
            else:
                pass
        return self._getter_call_policies
    def set_getter_call_policies( self, call_policies ):
        self._getter_call_policies = call_policies
    getter_call_policies = property( get_getter_call_policies, set_getter_call_policies
                                     , doc=__call_policies_doc__ )

    def get_setter_call_policies( self ):
        if None is self._getter_call_policies:
            if self.apply_smart_ptr_wa or self.use_make_functions:
                self._setter_call_policies = call_policies.default_call_policies()       
        return self._setter_call_policies
    def set_setter_call_policies( self, call_policies ):
        self._setter_call_policies = call_policies
    setter_call_policies = property( get_setter_call_policies, set_setter_call_policies
                                     , doc=__call_policies_doc__ )

    __use_make_functions_doc__ = \
    """Generate code using make_getter and make_setter functions
    
    Basically you don't need to use this, untill you have one of the next use-cases:
    * member variable is smart pointer - in this case Boost.Python has small problem
      to expose it right. Using the functions is a work around to the problem.
    * member variable defined custom r-value converter - may be you don't know
      but the conversion is applied only on functions arguments. So you need to 
      use make_getter/make_setter in order to allow user to enjoy from the
      conversion.
      
    Setting "apply_smart_ptr_wa" and/or "use_make_functions" to "True" will tell
    Py++ to generate such code.
    """

    def get_apply_smart_ptr_wa( self ):
        return self._apply_smart_ptr_wa
    def set_apply_smart_ptr_wa( self, value):
        self._apply_smart_ptr_wa = value
    apply_smart_ptr_wa = property( get_apply_smart_ptr_wa, set_apply_smart_ptr_wa
                                     , doc=__use_make_functions_doc__ )

    def get_use_make_functions( self ):
        return self._use_make_functions
    def set_use_make_functions( self, value ):
        self._use_make_functions = value
    use_make_functions = property( get_use_make_functions, set_use_make_functions
                                   , doc=__use_make_functions_doc__)
    
    def __find_out_is_read_only(self):
        type_ = declarations.remove_alias( self.type )
        
        if isinstance( type_, declarations.const_t ):
            return True
        
        if declarations.is_pointer( type_ ):
            type_ = declarations.remove_pointer( type_ )

        if declarations.is_reference( type_ ):
            type_ = declarations.remove_reference( type_ )

        if isinstance( type_, declarations.const_t ):
            return True
        
        if self.apply_smart_ptr_wa:
            return False #all smart pointers has assign operator
            
        if isinstance( type_, declarations.declarated_t ) \
           and isinstance( type_.declaration, declarations.class_t ) \
           and not declarations.has_public_assign( type_.declaration ):
            return True
        return False
        
    def get_is_read_only( self ):
        if None is self._is_read_only:
            self._is_read_only = self.__find_out_is_read_only()
        return self._is_read_only
    def set_is_read_only( self, v ):
        self._is_read_only = v
    is_read_only = property( get_is_read_only, set_is_read_only )

    def _exportable_impl( self ):
        if not self.name:
            return messages.W1033
        if self.bits == 0 and self.name == "":
            return messages.W1034
        if declarations.is_array( self.type ) and declarations.array_size( self.type ) < 1:
            return messages.W1045
        type_ = declarations.remove_alias( self.type )
        type_ = declarations.remove_const( type_ )
        if declarations.is_pointer( type_ ):
            if self.type_qualifiers.has_static:
                return messages.W1035
            if python_traits.is_immutable( type_.base ):
                return messages.W1036

            units = declarations.decompose_type( type_ )
            ptr2functions = filter( lambda unit: isinstance( unit, declarations.calldef_type_t )
                                    , units )
            if ptr2functions:
                return messages.W1037
        type_ = declarations.remove_pointer( type_ )
        if declarations.class_traits.is_my_case( type_ ):
            cls = declarations.class_traits.get_declaration( type_ )
            if not cls.name:
                return messages.W1038
        if isinstance( self.parent, declarations.class_t ):
            if self.access_type != declarations.ACCESS_TYPES.PUBLIC:
                return messages.W1039
        return ''
