# Copyright 2004 Roman Yakovenko, 2006 Allen Bierbaum, Matthias Baas
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""defines class that will print in a user friendly format declarations tree and
declarations Py++ configuration instructions"""

import os
import sys
from pygccxml import declarations

class decl_wrapper_printer_t( declarations.decl_printer_t ):
    """ Helper class for printing declarations tree and Py++ configuration instructions"""
    JUSTIFY = 20
    INDENT_SIZE = 4

    def __init__( self, level=0, print_details=True, recursive=True, writer=None ):
        declarations.decl_printer_t.__init__(self, level, print_details, recursive, writer)

    def clone( self, increment_level=True ):
        level = self.level
        if increment_level:
            level += 1
        return decl_wrapper_printer_t(level, self.print_details, self.recursive, self.writer)

    def print_decl_header(self):
        super( decl_wrapper_printer_t, self ).print_decl_header()
        if not self.print_details:
            return
        intend_txt = ' ' * (self.level+1) * self.INDENT_SIZE
        self.writer( intend_txt + "Alias: " + self.instance.alias + os.linesep )
        self.writer( intend_txt + "Ignore: " + str( self.instance.ignore ) + os.linesep )
        if not self.instance.ignore:
            msgs = self.instance.readme()
            if msgs:
                self.writer( intend_txt + "ReadMe: " + os.linesep )
                more_intend_txt = ' ' * (self.level+2) * self.INDENT_SIZE
                for msg in msgs:
                    self.writer( more_intend_txt + msg + os.linesep )

    def print_calldef_wrapper(self):
        if not self.print_details:
            return
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Call policies: " + str(self.instance.call_policies) + os.linesep )
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Use keywords: " + str(self.instance.use_keywords) + os.linesep )
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Use signature: " + str(self.instance.create_with_signature) + os.linesep )
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Use default arguments: " + str(self.instance.use_default_arguments) + os.linesep )

    def visit_member_function( self ):
        super( decl_wrapper_printer_t, self ).visit_member_function()
        self.print_calldef_wrapper()

    def visit_constructor( self ):
        super( decl_wrapper_printer_t, self ).visit_constructor()
        self.print_calldef_wrapper()

    def visit_destructor( self ):
        super( decl_wrapper_printer_t, self ).visit_destructor()
        self.print_calldef_wrapper()

    def visit_member_operator( self ):
        super( decl_wrapper_printer_t, self ).visit_member_operator()
        self.print_calldef_wrapper()

    def visit_casting_operator( self ):
        super( decl_wrapper_printer_t, self ).visit_casting_operator()
        self.print_calldef_wrapper()

    def visit_free_function( self ):
        super( decl_wrapper_printer_t, self ).visit_free_function()
        self.print_calldef_wrapper()

    def visit_free_operator( self ):
        super( decl_wrapper_printer_t, self ).visit_free_operator()
        self.print_calldef_wrapper()

    def visit_class_declaration(self ):
        super( decl_wrapper_printer_t, self ).visit_class_declaration()

    def visit_class(self ):
        super( decl_wrapper_printer_t, self ).visit_class()
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Expose using scope: " + str(self.instance.always_expose_using_scope) + os.linesep )
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Redefine operators: " + str(self.instance.redefine_operators) + os.linesep )
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Held type: " + str(self.instance.held_type) + os.linesep )
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Use noncopyable: " + str(self.instance.noncopyable) + os.linesep )
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Class wrapper alias: " + str(self.instance.wrapper_alias) + os.linesep )

    def visit_enumeration(self):
        super( decl_wrapper_printer_t, self ).visit_enumeration()
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Enumeration value aliases: " + str(self.instance.value_aliases) + os.linesep )
        self.writer(  ' ' * (self.level+1) * self.INDENT_SIZE
                      + "Enumeration export values: " + str(self.instance.export_values) + os.linesep )

    def visit_namespace(self ):
        super( decl_wrapper_printer_t, self ).visit_namespace()

    def visit_typedef(self ):
        super( decl_wrapper_printer_t, self ).visit_typedef()

    def visit_variable(self ):
        super( decl_wrapper_printer_t, self ).visit_variable()

def print_declarations( decls, detailed=True, recursive=True, writer=sys.stdout.write ):
    """ Print decl tree rooted at each of the included nodes.
        decls - either a single decl or a list of decls.
    """
    prn = decl_wrapper_printer_t(0, detailed, recursive, writer)
    if type(decls) is not list:
       decls = [decls]
    for d in decls:
       prn.level = 0
       prn.instance = d
       declarations.apply_visitor(prn, d)
