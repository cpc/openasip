# Copyright 2004 Roman Yakovenko, 2006 Allen Bierbaum, Matthias Baas
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
defines class, decl_printer_t that prints declarations tree in a user friendly format
"""

import os
import sys
import decl_visitor
import algorithm


class decl_printer_t( decl_visitor.decl_visitor_t ):
    """Helper class for printing decl tree.

    This class provides more information than the __str__() methods do.
    The class is not just meant to provide a unique "id" for a declaration
    but to inspect an entire declaration tree. This is particularly useful
    for new users who want to find out how Py++ works and how it
    stores its data.
    """
    JUSTIFY = 20
    INDENT_SIZE = 4

    def __init__( self, level=0, print_details=True, recursive=True, writer=None ):
        decl_visitor.decl_visitor_t.__init__(self)
        self.__inst = None
        self.__level = level
        self.__print_details = print_details
        self.__recursive = recursive
        self.__writer = writer
        if not self.__writer:
            self.__writer = sys.stdout.write

    def clone(self, increment_level=True):
        level = self.__level
        if increment_level:
            level += 1
        return decl_printer_t( level
                               , self.print_details
                               , recursive=self.recursive
                               , writer=self.writer )

    def _get_recursive(self):
        return self.__recursive
    def _set_recursive(self, recursive):
        self.__recursive = recursive
    recursive = property( _get_recursive, _set_recursive)

    def _get_level(self):
        return self.__level
    def _set_level(self, lvl):
        self.__level = lvl
    level = property( _get_level, _set_level )

    def _get_print_details(self):
        return self.__print_details
    def _set_print_details(self, details):
        self.__print_details = details
    print_details = property( _get_print_details, _set_print_details)

    def _get_writer(self):
        return self.__writer
    def _set_writer(self, writer):
        self.__writer = writer
    writer = property( _get_writer, _set_writer)

    def _get_inst(self):
        return self.__inst
    def _set_inst(self, inst):
        self.__inst = inst
    instance = property( _get_inst, _set_inst )

    def __nice_decl_name( self, inst ):
        name = inst.__class__.__name__
        return name
        #if name.endswith( '_t' ):
        #    name = name[:-len('_t')]
        #return name.replace( '_', ' ' )

    def print_decl_header(self):
        header = self.__nice_decl_name( self.__inst ) + ": '%s'" % self.__inst.name
        self.writer( ' ' * self.level * self.INDENT_SIZE + header.ljust( self.JUSTIFY ) + os.linesep )
        if self.__print_details:
            curr_level = self.level + 1
            if self.__inst.location:
                location = 'location: [%s]:%s'%(self.__inst.location.file_name, self.__inst.location.line)
                self.writer( ' ' * curr_level * self.INDENT_SIZE + location + os.linesep )
            artificial = 'artificial: ' + "'%s'" % str(self.__inst.is_artificial)
            self.writer( ' ' * curr_level * self.INDENT_SIZE + artificial.ljust( self.JUSTIFY ) + os.linesep )

    def __get_method_signature(self, decl=None):
       """ Returns function signature: [retval, [arg1, ..., argN]]. """
       if None is decl:
           decl = self.__inst
       retval = decl.return_type.decl_string
       args = []
       for arg in decl.arguments:
          args.append(arg.type.decl_string)
       return [retval, args]

    def visit_member_function( self ):
        self.print_decl_header()
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                        + "Signature: " + str( self.__get_method_signature() ) + os.linesep )

    def visit_constructor( self ):
        self.print_decl_header()
        if self.__print_details:
            self.writer( ' ' * ( self.level + 1 ) * self.INDENT_SIZE
                         + 'copy constructor: ' + str(self.__inst.is_copy_constructor) 
                         + os.linesep )
        
    def visit_destructor( self ):
        self.print_decl_header()

    def visit_member_operator( self ):
        self.print_decl_header()

    def visit_casting_operator( self ):
        self.print_decl_header()

    def visit_free_function( self ):
        self.print_decl_header()

    def visit_free_operator( self ):
        self.print_decl_header()

    def visit_class_declaration(self ):
        self.print_decl_header()

    def visit_class(self ):
        self.print_decl_header()
        curr_level = self.level + 1
        class_type = 'class type: ' + "'%s'" % str(self.__inst.class_type)
        self.writer( ' ' * curr_level * self.INDENT_SIZE + class_type.ljust( self.JUSTIFY ) + os.linesep )

        if self.__inst.aliases:
            aliases = map( lambda typedef: typedef.name, self.__inst.aliases )
            msg = 'aliases: ' + `aliases`
            self.writer( ' ' * curr_level * self.INDENT_SIZE + msg.ljust( self.JUSTIFY ) + os.linesep )

        def print_hierarchy(hierarchy_type, classes, curr_level):
            self.writer( ' ' * curr_level * self.INDENT_SIZE + hierarchy_type.ljust( self.JUSTIFY ) + os.linesep )
            curr_level += 1
            for class_ in classes:
                class_str = 'class: ' + "'%s'" % str(class_.related_class.decl_string)
                self.writer( ' ' * curr_level * self.INDENT_SIZE + class_str.ljust( self.JUSTIFY ) + os.linesep )
                access = 'access: ' + "'%s'" % str(class_.access)
                self.writer( ' ' * (curr_level + 1)* self.INDENT_SIZE + access.ljust( self.JUSTIFY ) + os.linesep )

        def print_members(members_type, members, curr_level):
            self.writer( ' ' * curr_level * self.INDENT_SIZE + members_type.ljust( self.JUSTIFY ) + os.linesep )
            if self.__recursive:
               curr_level += 1
               for member in members:
                  prn = self.clone()
                  prn.instance = member
                  algorithm.apply_visitor( prn, member )

        if self.__inst.bases:
            print_hierarchy( 'base classes: ', self.__inst.bases, curr_level )

        if self.__inst.derived:
            print_hierarchy( 'derived classes: ', self.__inst.derived, curr_level )

        print_members( 'public: ', self.__inst.public_members, curr_level )
        print_members( 'protected: ', self.__inst.protected_members, curr_level )
        print_members( 'private: ', self.__inst.private_members, curr_level )

    def visit_enumeration(self):
        self.print_decl_header()
        curr_level = self.level + 1
        self.writer( ' ' * curr_level * self.INDENT_SIZE + 'values:'.ljust( self.JUSTIFY ) )
        value_level = ' ' * ( curr_level + 1 )* self.INDENT_SIZE
        self.writer( os.linesep )
        for name, value in self.__inst.values:
            self.writer( value_level + "%s : %s"% (name, value) + os.linesep )

    def visit_namespace(self ):
        self.print_decl_header()
        if self.__recursive:
           for decl in self.__inst.declarations:
              prn = self.clone()
              prn.instance = decl
              algorithm.apply_visitor( prn, decl )

    def visit_typedef(self ):
        self.print_decl_header()
        curr_level = self.level + 1
        self.writer( ' ' * curr_level * self.INDENT_SIZE + 'alias to: ' + self.__inst.type.decl_string + os.linesep )

    def visit_variable(self ):
        self.print_decl_header()
        curr_level = self.level + 1
        self.writer( ' ' * curr_level * self.INDENT_SIZE + 'type: %s  value: %s'%(self.__inst.type.decl_string, self.__inst.value)  + os.linesep)

def print_declarations( decls, detailed=True, recursive=True, writer=sys.stdout.write ):
    """ Print decl tree rooted at each of the included nodes.
        decls - either a single decl or a list of decls.
    """
    prn = decl_printer_t(0, detailed, recursive, writer)
    if type(decls) is not list:
       decls = [decls]
    for d in decls:
       prn.level = 0
       prn.instance = d
       algorithm.apply_visitor(prn, d)
