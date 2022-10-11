# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
defines few algorithms, that deals with different properties of std containers
"""

import types
import string
import calldef
import cpptypes
import namespace
import templates
import type_traits
import class_declaration

std_namespaces = ( 'std', 'stdext', '__gnu_cxx' )

class defaults_eraser:
    @staticmethod
    def normalize( type_str ):
        return type_str.replace( ' ', '' )

    @staticmethod
    def replace_basic_string( cls_name ):
        strings = {
              'std::string' : ( 'std::basic_string<char,std::char_traits<char>,std::allocator<char> >'
                                , 'std::basic_string<char, std::char_traits<char>, std::allocator<char> >' )
            , 'std::wstring' : ( 'std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> >'
                                 , 'std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> >' ) }

        new_name = cls_name
        for short_name, long_names in strings.iteritems():
            for lname in long_names:
                new_name = new_name.replace( lname, short_name )
        return new_name

    class recursive_impl:
        @staticmethod
        def decorated_call_prefix( cls_name, text, doit ):
            has_text = cls_name.startswith( text )
            if has_text:
                cls_name = cls_name[ len( text ): ]
            answer = doit( cls_name )
            if has_text:
                answer = text + answer
            return answer

        @staticmethod
        def decorated_call_suffix( cls_name, text, doit ):
            has_text = cls_name.endswith( text )
            if has_text:
                cls_name = cls_name[: len( text )]
            answer = doit( cls_name )
            if has_text:
                answer = answer + text
            return answer

        @staticmethod
        def erase_call( cls_name ):
            global find_container_traits
            c_traits = find_container_traits( cls_name )
            if not c_traits:
                return cls_name
            return c_traits.remove_defaults( cls_name )

        @staticmethod
        def erase_recursive( cls_name ):
            ri = defaults_eraser.recursive_impl
            no_std = lambda cls_name: ri.decorated_call_prefix( cls_name, 'std::', ri.erase_call )
            no_stdext = lambda cls_name: ri.decorated_call_prefix( cls_name, 'stdext::', no_std )
            no_gnustd = lambda cls_name: ri.decorated_call_prefix( cls_name, '__gnu_cxx::', no_stdext )
            no_const = lambda cls_name: ri.decorated_call_prefix( cls_name, 'const ', no_gnustd )
            no_end_const = lambda cls_name: ri.decorated_call_suffix( cls_name, ' const', no_const )
            return no_end_const( cls_name )

    @staticmethod
    def erase_recursive( cls_name ):
        return defaults_eraser.recursive_impl.erase_recursive( cls_name )

    @staticmethod
    def erase_allocator( cls_name, default_allocator='std::allocator' ):
        cls_name = defaults_eraser.replace_basic_string( cls_name )
        c_name, c_args = templates.split( cls_name )
        if 2 != len( c_args ):
            return
        value_type = c_args[0]
        tmpl = string.Template( "$container< $value_type, $allocator<$value_type> >" )
        tmpl = tmpl.substitute( container=c_name, value_type=value_type, allocator=default_allocator )
        if defaults_eraser.normalize( cls_name ) == defaults_eraser.normalize( tmpl ):
            return templates.join( c_name, [defaults_eraser.erase_recursive( value_type )] )

    @staticmethod
    def erase_container( cls_name, default_container_name='std::deque' ):
        cls_name = defaults_eraser.replace_basic_string( cls_name )
        c_name, c_args = templates.split( cls_name )
        if 2 != len( c_args ):
            return
        value_type = c_args[0]
        dc_no_defaults = defaults_eraser.erase_recursive( c_args[1] )
        if defaults_eraser.normalize( dc_no_defaults ) \
           != defaults_eraser.normalize( templates.join( default_container_name, [value_type] ) ):
            return
        return templates.join( c_name, [defaults_eraser.erase_recursive( value_type )] )

    @staticmethod
    def erase_container_compare( cls_name, default_container_name='std::vector', default_compare='std::less' ):
        cls_name = defaults_eraser.replace_basic_string( cls_name )
        c_name, c_args = templates.split( cls_name )
        if 3 != len( c_args ):
            return
        dc_no_defaults = defaults_eraser.erase_recursive( c_args[1] )
        if defaults_eraser.normalize( dc_no_defaults ) \
           != defaults_eraser.normalize( templates.join( default_container_name, [c_args[0]] ) ):
            return
        dcomp_no_defaults = defaults_eraser.erase_recursive( c_args[2] )
        if defaults_eraser.normalize( dcomp_no_defaults ) \
           != defaults_eraser.normalize( templates.join( default_compare, [c_args[0]] ) ):
            return
        value_type = defaults_eraser.erase_recursive( c_args[0] )
        return templates.join( c_name, [value_type] )

    @staticmethod
    def erase_compare_allocator( cls_name, default_compare='std::less', default_allocator='std::allocator' ):
        cls_name = defaults_eraser.replace_basic_string( cls_name )
        c_name, c_args = templates.split( cls_name )
        if 3 != len( c_args ):
            return
        value_type = c_args[0]
        tmpl = string.Template( "$container< $value_type, $compare<$value_type>, $allocator<$value_type> >" )
        tmpl = tmpl.substitute( container=c_name
                                , value_type=value_type
                                , compare=default_compare
                                , allocator=default_allocator )
        if defaults_eraser.normalize( cls_name ) == defaults_eraser.normalize( tmpl ):
            return templates.join( c_name, [defaults_eraser.erase_recursive( value_type )] )

    @staticmethod
    def erase_map_compare_allocator( cls_name, default_compare='std::less', default_allocator='std::allocator' ):
        cls_name = defaults_eraser.replace_basic_string( cls_name )
        c_name, c_args = templates.split( cls_name )
        if 4 != len( c_args ):
            return
        key_type = c_args[0]
        mapped_type = c_args[1]
        tmpls = [
            string.Template( "$container< $key_type, $mapped_type, $compare<$key_type>, $allocator< std::pair< const $key_type, $mapped_type> > >" )
            , string.Template( "$container< $key_type, $mapped_type, $compare<$key_type>, $allocator< std::pair< $key_type const, $mapped_type> > >" )
            , string.Template( "$container< $key_type, $mapped_type, $compare<$key_type>, $allocator< std::pair< $key_type, $mapped_type> > >" )]
        for tmpl in tmpls:
            tmpl = tmpl.substitute( container=c_name
                                    , key_type=key_type
                                    , mapped_type=mapped_type
                                    , compare=default_compare
                                    , allocator=default_allocator )
            if defaults_eraser.normalize( cls_name ) == defaults_eraser.normalize( tmpl ):
                return templates.join( c_name
                                       , [ defaults_eraser.erase_recursive( key_type )
                                           , defaults_eraser.erase_recursive( mapped_type )] )


    @staticmethod
    def erase_hash_allocator( cls_name ):
        cls_name = defaults_eraser.replace_basic_string( cls_name )
        c_name, c_args = templates.split( cls_name )
        if len( c_args ) < 3:
            return

        default_hash=None
        default_less='std::less'
        default_equal_to='std::equal_to'
        default_allocator='std::allocator'

        tmpl = None
        if 3 == len( c_args ):
            default_hash='hash_compare'
            tmpl = "$container< $value_type, $hash<$value_type, $less<$value_type> >, $allocator<$value_type> >"
        elif 4 == len( c_args ):
            default_hash='hash'
            tmpl = "$container< $value_type, $hash<$value_type >, $equal_to<$value_type >, $allocator<$value_type> >"
        else:
            return

        value_type = c_args[0]
        tmpl = string.Template( tmpl )
        for ns in std_namespaces:
            inst = tmpl.substitute( container=c_name
                                    , value_type=value_type
                                    , hash= ns + '::' + default_hash
                                    , less=default_less
                                    , equal_to=default_equal_to
                                    , allocator=default_allocator )
            if defaults_eraser.normalize( cls_name ) == defaults_eraser.normalize( inst ):
                return templates.join( c_name, [defaults_eraser.erase_recursive( value_type )] )


    @staticmethod
    def erase_hashmap_compare_allocator( cls_name ):
        cls_name = defaults_eraser.replace_basic_string( cls_name )
        c_name, c_args = templates.split( cls_name )

        default_hash=None
        default_less='std::less'
        default_allocator='std::allocator'
        default_equal_to = 'std::equal_to'

        tmpl = None
        key_type = None
        mapped_type = None
        if 2 < len( c_args ):
            key_type = c_args[0]
            mapped_type = c_args[1]
        else:
            return

        if 4 == len( c_args ):
            default_hash = 'hash_compare'
            tmpl = string.Template( "$container< $key_type, $mapped_type, $hash<$key_type, $less<$key_type> >, $allocator< std::pair< const $key_type, $mapped_type> > >" )
            if key_type.startswith( 'const ' ) or key_type.endswith( ' const' ):
                tmpl = string.Template( "$container< $key_type, $mapped_type, $hash<$key_type, $less<$key_type> >, $allocator< std::pair< $key_type, $mapped_type> > >" )
        elif 5 == len( c_args ):
            default_hash = 'hash'
            tmpl = string.Template( "$container< $key_type, $mapped_type, $hash<$key_type >, $equal_to<$key_type>, $allocator< $mapped_type> >" )
            if key_type.startswith( 'const ' ) or key_type.endswith( ' const' ):
                tmpl = string.Template( "$container< $key_type, $mapped_type, $hash<$key_type >, $equal_to<$key_type>, $allocator< $mapped_type > >" )
        else:
            return

        for ns in std_namespaces:
            inst = tmpl.substitute( container=c_name
                                    , key_type=key_type
                                    , mapped_type=mapped_type
                                    , hash=ns + '::' + default_hash
                                    , less=default_less
                                    , equal_to = default_equal_to
                                    , allocator=default_allocator )
            if defaults_eraser.normalize( cls_name ) == defaults_eraser.normalize( inst ):
                return templates.join( c_name
                                       , [ defaults_eraser.erase_recursive( key_type )
                                           , defaults_eraser.erase_recursive( mapped_type )] )


class container_traits_impl_t:
    """this class implements the functionality needed for convinient work with
    STD container classes.

    Implemented functionality:
        - find out whether a declaration is STD container or not
        - find out container value( mapped ) type

    This class tries to be useful as much, as possible. For example, for class
    declaration( and not definition ) it parsers the class name in order to
    extract all the information.
    """
    def __init__( self, container_name, element_type_index, element_type_typedef ):
        """
        container_name - std container name
        element_type_index - position of value\\mapped type within template
          arguments list
        element_type_typedef - class typedef to the value\\mapped type
        """
        self.name = container_name
        self.element_type_index = element_type_index
        self.element_type_typedef = element_type_typedef

    def get_container_or_none( self, type ):
        """returns reference to the class declaration or None"""
        type = type_traits.remove_alias( type )
        type = type_traits.remove_cv( type )

        cls = None
        if isinstance( type, cpptypes.declarated_t ):
            cls = type_traits.remove_alias( type.declaration )
        elif isinstance( type, class_declaration.class_t ):
            cls = type
        elif isinstance( type, class_declaration.class_declaration_t ):
            cls = type
        else:
            return

        if not cls.name.startswith( self.name + '<' ):
            return

        for ns in std_namespaces:
            if type_traits.impl_details.is_defined_in_xxx( ns, cls ):
                return cls

    def is_my_case( self, type ):
        """checks, whether type is STD container or not"""
        return bool( self.get_container_or_none( type ) )

    def class_declaration( self, type ):
        """returns reference to the class declaration"""
        cls = self.get_container_or_none( type )
        if not cls:
            raise TypeError( 'Type "%s" is not instantiation of std::%s' % ( type.decl_string, self.name ) )
        return cls

    def element_type( self, type ):
        """returns reference to the class value\\mapped type declaration"""
        cls = self.class_declaration( type )
        if isinstance( cls, class_declaration.class_t ):
            value_type = cls.typedef( self.element_type_typedef, recursive=False ).type
            return type_traits.remove_declarated( value_type )
        else:
            value_type_str = templates.args( cls.name )[self.element_type_index]
            ref = type_traits.impl_details.find_value_type( cls.top_parent, value_type_str )
            if None is ref:
                raise RuntimeError( "Unable to find out %s '%s' value type."
                                    % ( self.name, cls.decl_string ) )
            return ref



def create_traits_class( container_name
                         , element_type_index
                         , element_type_typedef
                         , remove_defaults_=None ):
    """ creates concrete container traits class """

    impl_tmp = container_traits_impl_t( container_name, element_type_index, element_type_typedef )

    class xxx_traits:
        """extract information from the container"""

        impl = None

        @staticmethod
        def name():
            return xxx_traits.impl.name

        @staticmethod
        def is_my_case( type ):
            """returns True if type is the container class, otherwise False"""
            return xxx_traits.impl.is_my_case( type )

        @staticmethod
        def class_declaration( type ):
            """returns reference to the container class"""
            return xxx_traits.impl.class_declaration( type )

        @staticmethod
        def element_type( type ):
            """returns reference to container name value\\mapped type class"""
            return xxx_traits.impl.element_type( type )

        @staticmethod
        def remove_defaults( type_or_string ):
            name = None
            if not isinstance( type_or_string, types.StringTypes ):
                name = xxx_traits.class_declaration( type_or_string ).name
            else:
                name = type_or_string
            if not remove_defaults_:
                return name
            no_defaults = remove_defaults_( name )
            if not no_defaults:
                return name
            else:
                return no_defaults
            
    xxx_traits.impl = impl_tmp
    
    return xxx_traits

list_traits = create_traits_class( 'list', 0, 'value_type', defaults_eraser.erase_allocator )

deque_traits = create_traits_class( 'deque', 0, 'value_type', defaults_eraser.erase_allocator )

queue_traits = create_traits_class( 'queue', 0, 'value_type', defaults_eraser.erase_container )

priority_queue_traits = create_traits_class( 'priority_queue', 0, 'value_type', defaults_eraser.erase_container_compare )

vector_traits = create_traits_class( 'vector', 0, 'value_type', defaults_eraser.erase_allocator )

stack_traits = create_traits_class( 'stack', 0, 'value_type', defaults_eraser.erase_container )

map_traits = create_traits_class( 'map', 1, 'mapped_type', defaults_eraser.erase_map_compare_allocator )
multimap_traits = create_traits_class( 'multimap', 1, 'mapped_type', defaults_eraser.erase_map_compare_allocator )

hash_map_traits = create_traits_class( 'hash_map', 1, 'mapped_type', defaults_eraser.erase_hashmap_compare_allocator )
hash_multimap_traits = create_traits_class( 'hash_multimap', 1, 'mapped_type', defaults_eraser.erase_hashmap_compare_allocator )

set_traits = create_traits_class( 'set', 0, 'value_type', defaults_eraser.erase_compare_allocator)
multiset_traits = create_traits_class( 'multiset', 0, 'value_type', defaults_eraser.erase_compare_allocator )

hash_set_traits = create_traits_class( 'hash_set', 0, 'value_type', defaults_eraser.erase_hash_allocator )
hash_multiset_traits = create_traits_class( 'hash_multiset', 0, 'value_type', defaults_eraser.erase_hash_allocator )

container_traits = (
      list_traits
    , deque_traits
    , queue_traits
    , priority_queue_traits
    , vector_traits
    , stack_traits
    , map_traits
    , multimap_traits
    , hash_map_traits
    , hash_multimap_traits
    , set_traits
    , hash_set_traits
    , multiset_traits
    , hash_multiset_traits )
"""tuple of all STD container traits classes"""

def find_container_traits( cls_or_string ):
    if isinstance( cls_or_string, types.StringTypes ):
        if not templates.is_instantiation( cls_or_string ):
            return None
        name = templates.name( cls_or_string )
        if name.startswith( 'std::' ):
            name = name[ len( 'std::' ): ]
        for cls_traits in container_traits:
            if cls_traits.name() == name:
                return cls_traits
    else:
        for cls_traits in container_traits:
            if cls_traits.is_my_case( cls_or_string ):
                return cls_traits


