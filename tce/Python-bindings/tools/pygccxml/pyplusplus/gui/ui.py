#! /usr/bin/python
# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""this module implements GUI.

TODO: It could be nice, if GUI will save the user settings beetwen runs.
"""

import os
import time
import Tkinter
import tkFileDialog
import tkSimpleDialog

from pygccxml import parser
from pyplusplus import module_builder

import wizard


class gui_config_t:   
    
    @staticmethod
    def config_file():
        config_path = os.getenv( 'HOME', os.path.abspath( os.getcwd() ) )
        return os.path.join( os.path.abspath( config_path ), ".pyplusplus" )
    
    @staticmethod
    def read_header_name():        
        config_path = gui_config_t.config_file()
        if os.path.exists(config_path):    
            return open(config_path).read()
        else:
            return ''
            
    @staticmethod
    def save_header_name( header_file ):
        config_path = gui_config_t.config_file()
        open(config_path, "w").write(header_file)        

#TODO: add configuration file and ability to start\run it.

class custom_frame_t(Tkinter.Frame):
    def __init__(self, parent, sticky=Tkinter.NSEW ):
        Tkinter.Frame.__init__( self, parent )
        self.grid( sticky=sticky)

        self.rowconfigure( 1, weight=1 )
        self.columnconfigure( 0, weight=1 )
        

class parser_configuration_ui_t(custom_frame_t):
    def __init__(self, parent ):
        custom_frame_t.__init__( self, parent, sticky=Tkinter.NW )

        temp = Tkinter.Label( self, text="Parser configuration", justify=Tkinter.LEFT)
        temp.grid( row=0, sticky=Tkinter.NW + Tkinter.E, columnspan=3)
        temp.configure( relief=Tkinter.GROOVE, borderwidth=1 )
        
        temp = Tkinter.Label( self, text="GCC_XML location:")
        temp.grid( row=1, sticky=Tkinter.NW)
        
        self._gccxml_location = Tkinter.Entry( self, width=35 )
        self._gccxml_location.grid(row=2, column=0, columnspan=2)

        temp = Tkinter.Button( self, text="...", command=self._select_gccxml_executable )
        temp.grid( row=2, column=2)

        temp = Tkinter.Label( self, text="Include paths:", justify=Tkinter.LEFT)
        temp.grid( row=3, sticky=Tkinter.NW)
        
        self._include_paths = Tkinter.Listbox( self, height=9, width=35 )
        vscrollbar = Tkinter.Scrollbar( self, orient=Tkinter.VERTICAL)
        hscrollbar = Tkinter.Scrollbar( self, orient=Tkinter.HORIZONTAL)
        
        self._include_paths.config( yscrollcommand=vscrollbar.set 
                                     , xscrollcommand=hscrollbar.set )
        vscrollbar.config( command=self._include_paths.yview )
        hscrollbar.config( command=self._include_paths.xview )
        
        self._include_paths.grid( row=4, column=0, columnspan=2, sticky=Tkinter.NW )
        vscrollbar.grid( row=4, column=2, sticky=Tkinter.NS + Tkinter.W )
        hscrollbar.grid( row=5, column=0, columnspan=2, sticky=Tkinter.EW )            
            
        temp = Tkinter.Button( self, text="add", command=self._add_include_path )
        temp.grid( row=6, column=0, sticky=Tkinter.NW + Tkinter.E)
        temp = Tkinter.Button( self, text="remove", command=self._remove_include_path )
        temp.grid( row=6, column=1, columnspan=2, sticky=Tkinter.NE + Tkinter.W)

        temp = Tkinter.Label( self, text="Defines:", justify=Tkinter.LEFT)
        temp.grid( row=7, sticky=Tkinter.NW)

        self._defines = Tkinter.Listbox( self, height=9, width=35 )
        vscrollbar = Tkinter.Scrollbar( self, orient=Tkinter.VERTICAL)
        hscrollbar = Tkinter.Scrollbar( self, orient=Tkinter.HORIZONTAL)
        
        self._defines.config( yscrollcommand=vscrollbar.set 
                              , xscrollcommand=hscrollbar.set )
        vscrollbar.config( command=self._defines.yview )
        hscrollbar.config( command=self._defines.xview )
        
        self._defines.grid( row=8, column=0, columnspan=2, sticky=Tkinter.NW )
        vscrollbar.grid( row=8, column=2, sticky=Tkinter.NS + Tkinter.W )
        hscrollbar.grid( row=9, column=0, columnspan=2, sticky=Tkinter.EW )            
            
        temp = Tkinter.Button( self, text="add", command=self._add_define )
        temp.grid( row=10, column=0, sticky=Tkinter.NW + Tkinter.E)
        temp = Tkinter.Button( self, text="remove", command=self._remove_define )
        temp.grid( row=10, column=1, columnspan=2, sticky=Tkinter.NE + Tkinter.W)

        map( lambda i:  self.rowconfigure( i, weight=1 ), range(11) )
            
    def _select_gccxml_executable( self ):
        file_name = tkFileDialog.askopenfilename()
        if not file_name:
            return
        self._gccxml_location.delete( 0, Tkinter.END )
        self._gccxml_location.insert( 0, file_name )
        
    def _add_include_path( self ):
        directory = tkFileDialog.askdirectory()
        if directory:
            self._include_paths.insert( Tkinter.END, directory )
        
    def _remove_include_path(self):
        selected = list( self._include_paths.curselection() )
        selected = map( int, selected )
        selected.sort()
        selected.reverse()
        map( self._include_paths.delete, selected )

    def _add_define( self ):
        symbol = tkSimpleDialog.askstring(title='Py++ - code generator demo'
                                             , prompt="Define symbol ( example 'max(a,b)=a<b?b:a' ):")
        if symbol:
            self._defines.insert( Tkinter.END, symbol )
        
    def _remove_define(self):
        selected = list( self._defines.curselection() )
        selected = map( int, selected )
        selected.sort()
        selected.reverse()
        map( self._defines.delete, selected )

    def parser_configuration(self):
        return parser.config_t( gccxml_path=self._gccxml_location.get()
                                , include_paths=list(self._include_paths.get( 0, Tkinter.END ) )
                                , define_symbols=list(self._defines.get( 0, Tkinter.END ) ) )

class header_file_ui_t(custom_frame_t):
    def __init__(self, parent ):
        custom_frame_t.__init__( self, parent, sticky=Tkinter.NW )

        temp = Tkinter.Label( self, text="Header file:", justify=Tkinter.LEFT)
        temp.grid( row=0, sticky=Tkinter.NW + Tkinter.E, columnspan=3)
        temp.configure( relief=Tkinter.GROOVE, borderwidth=1 )
        
        self._header_file = Tkinter.Entry( self, width=35 )
        self._header_file.grid(row=1, column=0, sticky=Tkinter.NW + Tkinter.E)
        
        initial_header = gui_config_t.read_header_name()
        if initial_header:
            self._header_file.delete( 0, Tkinter.END )
            self._header_file.insert( 0, initial_header )
            
        temp = Tkinter.Button( self, text="...", command=self._select_header_file )
        temp.grid( row=1, column=2)
        
        map( lambda i:  self.rowconfigure( i, weight=1 ), range(2) )
        
        
    def _select_header_file( self ):        
        file_name = tkFileDialog.askopenfilename()
        if not file_name:
            return
        self._header_file.delete( 0, Tkinter.END )
        self._header_file.insert( 0, file_name )

    def header_file(self):
        hf = self._header_file.get()  
        if not os.path.isabs( hf ):
            hf = os.path.abspath( hf )
        return hf

class actions_ui_t(custom_frame_t):
    def __init__(self
                 , parent
                 , on_generate_code
                 , on_generate_pyplusplus
                 , on_create_gccxml):
        custom_frame_t.__init__( self, parent, sticky=Tkinter.NW )
        
        temp = Tkinter.Button( self
                               , width=37
                               , text="generate code"
                               , command=on_generate_code )
        temp.grid( row=0, columnspan=3, sticky=Tkinter.NW + Tkinter.E )

        temp = Tkinter.Button( self
                               , text="generate Py++ code"
                               , command=on_generate_pyplusplus )
        temp.grid( row=1, sticky=Tkinter.NW + Tkinter.E)

        temp = Tkinter.Button( self
                               , text="create XML"
                               , command=on_create_gccxml )
        temp.grid( row=2, sticky=Tkinter.NW + Tkinter.E)

        map( lambda i:  self.rowconfigure( i, weight=1 ), range(3) )
        #self.columnconfigure( 0, weight=1 )
        
class generated_code_ui_t(custom_frame_t):
    def __init__(self, parent):
        custom_frame_t.__init__( self, parent )
        
        temp = Tkinter.Label( self, text="Generated code")
        temp.grid( row=0, sticky=Tkinter.NW + Tkinter.E, columnspan=2)
        temp.configure( relief=Tkinter.GROOVE, borderwidth=1 )
    
        self._generated_code = Tkinter.Text( self, width=80, height=40)
        vscrollbar = Tkinter.Scrollbar( self, orient=Tkinter.VERTICAL)
        hscrollbar = Tkinter.Scrollbar( self, orient=Tkinter.HORIZONTAL)
        
        self._generated_code.config( yscrollcommand=vscrollbar.set 
                                     , xscrollcommand=hscrollbar.set
                                     , wrap=Tkinter.NONE )
        vscrollbar.config( command=self._generated_code.yview )
        hscrollbar.config( command=self._generated_code.xview )

        self._generated_code.grid( row=1, column=0, sticky=Tkinter.NSEW)
        vscrollbar.grid( row=1, column=1, sticky=Tkinter.N + Tkinter.S)
        hscrollbar.grid( row=2, column=0, sticky=Tkinter.W + Tkinter.E) 
        
        self.rowconfigure( 1, weight=1 )
        self.columnconfigure( 0, weight=1 )

    def set_generated_code(self, code ):
        self._generated_code.delete( 1.0, Tkinter.END )
        self._generated_code.insert( Tkinter.END, code )
    
class statistics_t(custom_frame_t):
    def __init__(self, parent):
        custom_frame_t.__init__( self, parent )
        
        temp = Tkinter.Label( self, text="Statistics", justify=Tkinter.LEFT)        
        temp.grid( row=0, sticky=Tkinter.NW + Tkinter.E, columnspan=3)
        temp.configure( relief=Tkinter.GROOVE, borderwidth=1 )
        
        temp = Tkinter.Label( self, text="Parse time:")
        temp.grid( row=1, column=0, sticky=Tkinter.NW)

        self._parse_time = Tkinter.Label( self, text="0 seconds")
        self._parse_time.grid( row=1, column=1, sticky=Tkinter.NW)

        temp = Tkinter.Label( self, text="Code generation time:")
        temp.grid( row=2, column=0, sticky=Tkinter.NW)

        self._code_generation_time = Tkinter.Label( self, text="0 seconds")
        self._code_generation_time.grid( row=2, column=1, sticky=Tkinter.NW)

    def set_parse_time(self, time ):
        self._parse_time.config( text='%0.2f seconds' % time )
    
    def set_code_generation_time(self, time ):
        self._code_generation_time.config( text='%0.2f seconds' % time )
    
class main_widget_ui_t(custom_frame_t):
    def __init__(self, parent=None ):
        custom_frame_t.__init__( self, parent )

        top = self.winfo_toplevel()
        top.rowconfigure( 0, weight=1 )
        top.columnconfigure( 0, weight=1 )
        
        self.master.title('Py++ - code generator demo')

        self._header_file_configurator = header_file_ui_t( self )
        self._header_file_configurator.grid( row=0, column=0, sticky=Tkinter.NW, padx=2, pady=2)
        
        self._parser_configurator = parser_configuration_ui_t( self )
        self._parser_configurator.grid( row=1, column=0, sticky=Tkinter.NW, padx=2)
        
        self._actions_ui = actions_ui_t( self
                                         , on_generate_code=lambda:self._generate_code()
                                         , on_generate_pyplusplus=lambda: self._generate_pyplusplus()
                                         , on_create_gccxml=lambda:self._create_xml())
        
        self._actions_ui.grid( row=3, column=0, rowspan=4, sticky=Tkinter.SE +Tkinter.W, padx=2 )

        self._generated_code = generated_code_ui_t(self)
        self._generated_code.grid( row=0, column=1, rowspan=5, sticky=Tkinter.NSEW, pady=2)

        self._statistics = statistics_t( self )
        self._statistics.grid( row=6, column=1, sticky=Tkinter.EW + Tkinter.S  )

        self.rowconfigure( 0, weight=1 )
        self.rowconfigure( 1, weight=1 )
        self.rowconfigure( 2, weight=1 )
        self.rowconfigure( 3, weight=1 )
        self.rowconfigure( 4, weight=1 )
        self.rowconfigure( 5, weight=0 )
        self.rowconfigure( 6, weight=0 )

        self.columnconfigure( 0, weight=0 )        
        self.columnconfigure( 1, weight=1 )

    def _generate_pyplusplus(self):
        config = self._parser_configurator.parser_configuration()
        header_file = self._header_file_configurator.header_file()
        config.include_paths.append( os.path.split( header_file )[0] )
        config.working_directory = os.path.split( header_file )[0]        
        w = wizard.wizard_t( config, header_file )
        self._generated_code.set_generated_code( w.create_code() )

    def _create_xml( self ):
        try:
            start_time = time.clock()        
            config = self._parser_configurator.parser_configuration()
            header_file = self._header_file_configurator.header_file()
            config.include_paths.append( os.path.split( header_file )[0] )
            config.working_directory = os.path.split( header_file )[0]
            reader = parser.source_reader_t( config=config )
            xml_file = reader.create_xml_file( header_file )
            parsed_time = time.clock() - start_time
            xml_file_obj = file( xml_file )
            self._generated_code.set_generated_code( xml_file_obj.read() )
            xml_file_obj.close()
            os.remove( xml_file )
            self._statistics.set_parse_time( parsed_time )
            self._statistics.set_code_generation_time( 0 )
        except Exception, error:
            user_msg = [ 'Error occured during code generation process!' ]
            user_msg.append( 'Error:' )
            user_msg.append( str( error ) )
            self._generated_code.set_generated_code( '\n'.join( user_msg ) )
            
    def _generate_code(self):
        global save_header_name
        
        try:
            config = self._parser_configurator.parser_configuration()
            header_file = self._header_file_configurator.header_file()
            if not header_file or not os.path.isfile( header_file ):
                raise RuntimeError( 'Header file "%s" does not exist or should be valid file name.' % header_file )
            gui_config_t.save_header_name( header_file )
            config.include_paths.append( os.path.split( header_file )[0] )
            
            start_time = time.clock()        
            mb = module_builder.module_builder_t( 
                    [ header_file ]
                    , gccxml_path=config.gccxml_path
                    , working_directory=os.path.split( header_file )[0]
                    , include_paths=config.include_paths
                    , define_symbols=config.define_symbols )

            parsed_time = time.clock() - start_time

            mb.build_code_creator( "pyplusplus" )
            mb.code_creator.user_defined_directories.extend( config.include_paths )
            code = mb.code_creator.create()
            code = code.replace( '\n\r', '\n' )
            code = code.replace( '\r\n', '\n' )
            code_generated_time = time.clock() - start_time - parsed_time
            self._generated_code.set_generated_code( code )
            self._statistics.set_parse_time( parsed_time )
            self._statistics.set_code_generation_time( code_generated_time )
        except Exception, error:
            user_msg = [ 'Error occured during code generation process!' ]
            user_msg.append( 'Error:' )
            user_msg.append( str( error ) )
            self._generated_code.set_generated_code( '\n'.join( user_msg ) )
            
    def _copy_to_clipboard(self):
        pass

def show_demo():
    root = Tkinter.Tk()
    mw = main_widget_ui_t(root)
    root.geometry("%dx%d%+d%+d" % (1024, 768, 0, 0)) # (width,height, x, y)
    mw.mainloop()
    
if __name__ == "__main__":
    show_demo()

