/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ProDeConstants.hh
 *
 * Declaration of ProDeConstants class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PRODE_CONSTANTS_HH
#define TTA_PRODE_CONSTANTS_HH

#include <string>
#include <wx/wx.h>

/**
 * Constant command names, enum for mapping command event ids to
 * EditorCommands and color constants.
 */
class ProDeConstants {
public:

    // Color constants
    static const wxColour PINK_COLOR;
    static const wxColour LIGHT_BLUE_COLOR;
    static const wxColour BLACK_COLOR;
    static const wxColour WHITE_COLOR;
    static const wxColour DARK_GRAY_COLOR;
    static const wxColour MEDIUM_GRAY_COLOR;
    static const wxColour LIGHT_GRAY_COLOR;

    /// Constant for "None".
    static const wxString NONE;
    /// Constant for "".
    static const std::string EMPTY;
    // Constant for "-- separator --".
    static const std::string SEPARATOR;

    /// String for the zero extension.
    static const wxString EXTENSION_ZERO;
    /// String for the sign extension.
    static const wxString EXTENSION_SIGN;
    /// String for the socket direction: input.
    static const wxString SOCKET_DIRECTION_INPUT;
    /// String for the socket direction output.
    static const wxString SOCKET_DIRECTION_OUTPUT;

    // EditorCommand names

    /// Command name for the "New Document" command.
    static const std::string CMD_NAME_NEW_DOC;
    /// Command name for the "Open Document" command.
    static const std::string CMD_NAME_OPEN_DOC;
    /// Command name for the "Close Document" command.
    static const std::string CMD_NAME_CLOSE_DOC;
    /// Command name for the "Save Document" command.
    static const std::string CMD_NAME_SAVE_DOC;
    /// Command name for the "Save Document As" command.
    static const std::string CMD_NAME_SAVEAS_DOC;
    /// Command name for the "Print" command.
    static const std::string CMD_NAME_PRINT;
    /// Command name for the "Print Preview" command.
    static const std::string CMD_NAME_PRINT_PREVIEW;
    /// Command name for the "Export" command.
    static const std::string CMD_NAME_EXPORT;
    /// Command name for the "Quit" command.
    static const std::string CMD_NAME_QUIT;

    /// Command name for the "Add Function Unit" command.
    static const std::string CMD_NAME_ADD_FU;
    /// Command name for the "Add Register File" command.
    static const std::string CMD_NAME_ADD_RF;
    /// Command name for the "Add Bus" command.
    static const std::string CMD_NAME_ADD_BUS;
    /// Command name for the "Add Socket" command.
    static const std::string CMD_NAME_ADD_SOCKET;
    /// Command name for the "Add Bridge" command.
    static const std::string CMD_NAME_ADD_BRIDGE;
    /// Command name for the "Add Immediate Unit" command.
    static const std::string CMD_NAME_ADD_IU;
    /// Command name for the "Add Global Control Unit" command.
    static const std::string CMD_NAME_ADD_GCU;
    /// Command name for the "Add Address Space" command.
    static const std::string CMD_NAME_ADD_AS;

    /// Command name for the "Add RF From hdb" command.
    static const std::string CMD_NAME_ADD_RF_FROM_HDB;
    /// Command name for the "Add IU From hdb" command.
    static const std::string CMD_NAME_ADD_IU_FROM_HDB;
    /// Command name for the "Add FU From hdb" command.
    static const std::string CMD_NAME_ADD_FU_FROM_HDB;

    /// Command name for the "Delete Component" command.
    static const std::string CMD_NAME_DELETE_COMP;
    /// Command name for the "Modify Component" command.
    static const std::string CMD_NAME_MODIFY_COMP;

    /// Command name for the "Undo" command.
    static const std::string CMD_NAME_UNDO;
    /// Command name for the "Redo" command.
    static const std::string CMD_NAME_REDO;
    /// Command name for the "Cut" command.
    static const std::string CMD_NAME_CUT;
    /// Command name for the "Copy" command.
    static const std::string CMD_NAME_COPY;
    /// Command name for the "Paste" command.
    static const std::string CMD_NAME_PASTE;

    /// Command name for the "Select" command.
    static const std::string CMD_NAME_SELECT;
    /// Command name for the "Edit Connections" command.
    static const std::string CMD_NAME_EDIT_CONNECTIONS;
    /// Command name for the "Edit Address Space" command.
    static const std::string CMD_NAME_EDIT_ADDRESS_SPACES;
    /// Command name for the "Edit Tempaltes" command.
    static const std::string CMD_NAME_EDIT_TEMPLATES;
    /// Command name for the "Edit Immediate Slots" command.
    static const std::string CMD_NAME_EDIT_IMMEDIATE_SLOTS;
    /// Command name for the "Edit Bus Order" command.
    static const std::string CMD_NAME_EDIT_BUS_ORDER;
    /// Command name for the "Fully connect busses" command.
    static const std::string CMD_NAME_FULLY_CONNECT_BUSSES;

    /// Command name for the "Zoom In" command.
    static const std::string CMD_NAME_ZOOM_IN;
    /// Command name for the "Zoom Out" command.
    static const std::string CMD_NAME_ZOOM_OUT;
    /// Command name for the "Fit Window" command.
    static const std::string CMD_NAME_ZOOM_FIT_WIN;
    /// Command name for the "Fit Height" command.
    static const std::string CMD_NAME_ZOOM_FIT_HEI;
    /// Command name for the "Fit Width" command.
    static const std::string CMD_NAME_ZOOM_FIT_WID;
    /// Command name for the "Toggle Unit details" command.
    static const std::string CMD_NAME_TOGGLE_UNIT_DETAILS;

    /// Command name for the "Verify Machine" command.
    static const std::string CMD_NAME_VERIFY_MACHINE;
    /// Command name for the "Processor Implementation" command.
    static const std::string CMD_NAME_IMPLEMENTATION;

    /// Command name for the "Edit Options" command.
    static const std::string CMD_NAME_EDIT_OPTIONS;
    /// Command name for the "Load Options" command.
    static const std::string CMD_NAME_LOAD_OPTIONS;
    /// Command name for the "Save Options" command.
    static const std::string CMD_NAME_SAVE_OPTIONS;
    /// Command name for the "Save Options As" command.
    static const std::string CMD_NAME_SAVEAS_OPTIONS;

    /// Command name for the "About" command.
    static const std::string CMD_NAME_ABOUT;


    // short versions of the command names

    /// Command name for the "New Document" command.
    static const std::string CMD_SNAME_NEW_DOC;
    /// Command name for the "Save Document" command.
    static const std::string CMD_SNAME_SAVE_DOC;
    /// Command name for the "Save Document As" command.
    static const std::string CMD_SNAME_SAVEAS_DOC;
    /// Command name for the "Open Document" command.
    static const std::string CMD_SNAME_OPEN_DOC;
    /// Command name for the "Close Document" command.
    static const std::string CMD_SNAME_CLOSE_DOC;
    /// Command name for the "Print Preview" command.
    static const std::string CMD_SNAME_PRINT_PREVIEW;

    /// Command name for the "Add Function Unit" command.
    static const std::string CMD_SNAME_ADD_FU;
    /// Command name for the "Add Register File" command.
    static const std::string CMD_SNAME_ADD_RF;
    /// Command name for the "Add Bus" command.
    static const std::string CMD_SNAME_ADD_BUS;
    /// Command name for the "Add Socket" command.
    static const std::string CMD_SNAME_ADD_SOCKET;
    /// Command name for the "Add Bridge" command.
    static const std::string CMD_SNAME_ADD_BRIDGE;
    /// Command name for the "Add Immediate Unit" command.
    static const std::string CMD_SNAME_ADD_IU;
    /// Command name for the "Add Global Control Unit" command.
    static const std::string CMD_SNAME_ADD_GCU;
    /// Command name for the "Add Address Space" command.
    static const std::string CMD_SNAME_ADD_AS;

    /// Short command name for the "Add RF from hdb" command.
    static const std::string CMD_SNAME_ADD_RF_FROM_HDB;
    /// Short command name for the "Add IU from hdb" command.
    static const std::string CMD_SNAME_ADD_IU_FROM_HDB;
    /// Short command name for the "Add FU from hdb" command.
    static const std::string CMD_SNAME_ADD_FU_FROM_HDB;

    /// Command name for the "Edit Address Space" command.
    static const std::string CMD_SNAME_EDIT_ADDRESS_SPACES;
    /// Command name for the "Edit Templates" command.
    static const std::string CMD_SNAME_EDIT_TEMPLATES;
    /// Command name for the "Edit Immediate slots" command.
    static const std::string CMD_SNAME_EDIT_IMMEDIATE_SLOTS;
    /// Command name for the "Edit Bus Order" command.
    static const std::string CMD_SNAME_EDIT_BUS_ORDER;
    /// Command name for the "Edit Connections" command.
    static const std::string CMD_SNAME_EDIT_CONNECTIONS;
    /// Command name for the "Fully connect busses" command.
    static const std::string CMD_SNAME_FULLY_CONNECT_BUSSES;
    /// Command name for the "Verify Machine" command.
    static const std::string CMD_SNAME_VERIFY_MACHINE;
    /// Command name for the "Processor Implementation" command.
    static const std::string CMD_SNAME_IMPLEMENTATION;

    /// Command name for the "Edit Options" command.
    static const std::string CMD_SNAME_EDIT_OPTIONS;
    /// Command name for the "Load Options" command.
    static const std::string CMD_SNAME_LOAD_OPTIONS;
    /// Command name for the "Save Options" command.
    static const std::string CMD_SNAME_SAVE_OPTIONS;
    /// Command name for the "Save Options As" command.
    static const std::string CMD_SNAME_SAVEAS_OPTIONS;

    /// Command name for the "Delete Component" command.
    static const std::string CMD_SNAME_MODIFY_COMP;
    /// Command name for the "Modify Component" command.
    static const std::string CMD_SNAME_DELETE_COMP;

    /// Command name for the "Zoom In" command.
    static const std::string CMD_SNAME_ZOOM_IN;
    /// Command name for the "Zoom Out" command.
    static const std::string CMD_SNAME_ZOOM_OUT;
    /// Command name for the "Fit Window" command.
    static const std::string CMD_SNAME_ZOOM_FIT_WIN;
    /// Command name for the "Fit Height" command.
    static const std::string CMD_SNAME_ZOOM_FIT_HEI;
    /// Command name for the "Fit Width" command.
    static const std::string CMD_SNAME_ZOOM_FIT_WID;
    /// Command name for the "Toggle Unit Details" command.
    static const std::string CMD_SNAME_TOGGLE_UNIT_DETAILS;


    // EditorCommand icon paths

    /// Icon path for default icon of commands.
    static const std::string CMD_ICON_DEFAULT;
    /// Icon location for the "Open Document" command.
    static const std::string CMD_ICON_OPEN_DOC;
    /// Icon location for the "Close Document" command.
    static const std::string CMD_ICON_CLOSE_DOC;
    /// Icon location for the "Save Document" command.
    static const std::string CMD_ICON_SAVE_DOC;
    /// Icon location for the "Save Document As" command.
    static const std::string CMD_ICON_SAVEAS_DOC;
    /// Icon location for the "New Document" command.
    static const std::string CMD_ICON_NEW_DOC;
    /// Icon location for the "Quit" command.
    static const std::string CMD_ICON_QUIT;
    /// Icon location for the "Print" command.
    static const std::string CMD_ICON_PRINT;
    /// Icon location for the "Print Preview" command.
    static const std::string CMD_ICON_PRINT_PREVIEW;
    /// Icon location for the "Select" command.
    static const std::string CMD_ICON_SELECT;
    /// Icon location for the "Undo" command.
    static const std::string CMD_ICON_UNDO;
    /// Icon location for the "Redo" command.
    static const std::string CMD_ICON_REDO;
    /// Icon location for the "Copy" command.
    static const std::string CMD_ICON_COPY;
    /// Icon location for the "Cut" command.
    static const std::string CMD_ICON_CUT;
    /// Icon location for the "Paste" command.
    static const std::string CMD_ICON_PASTE;
    /// Icon location for the "Modify Component" command.
    static const std::string CMD_ICON_MODIFY_COMP;
    /// Icon location for the "Delete Component" command.
    static const std::string CMD_ICON_DELETE_COMP;
    /// Icon location for the "Edit Options" command.
    static const std::string CMD_ICON_EDIT_OPTIONS;
    /// Icon location for the "Load Options" command.
    static const std::string CMD_ICON_LOAD_OPTIONS;
    /// Icon location for the "Save Options" command.
    static const std::string CMD_ICON_SAVE_OPTIONS;
    /// Icon location for the "Save Options As" command.
    static const std::string CMD_ICON_SAVEAS_OPTIONS;
    /// Icon location for the "Zoom In" command.
    static const std::string CMD_ICON_ZOOM_IN;
    /// Icon location for the "Zoom Out" command.
    static const std::string CMD_ICON_ZOOM_OUT;
    /// Icon location for the "Fit Window" command.
    static const std::string CMD_ICON_ZOOM_FIT_WIN;
    /// Icon location for the "Fit Height" command.
    static const std::string CMD_ICON_ZOOM_FIT_HEI;
    /// Icon location for the "Fit Width" command.
    static const std::string CMD_ICON_ZOOM_FIT_WID;
    /// Icon location for the "Edit Address Space" command.
    static const std::string CMD_ICON_EDIT_ADDRESS_SPACES;
    /// Icon location for the "Edit Templates" command.
    static const std::string CMD_ICON_EDIT_TEMPLATES;
    /// Icon location for the "Edit Connections" command.
    static const std::string CMD_ICON_EDIT_CONNECTIONS;
    /// Icon location for the "Verify Machine" command.
    static const std::string CMD_ICON_VERIFY_MACHINE;
    /// Icon location for the "Implementation" command.
    static const std::string CMD_ICON_IMPLEMENTATION;
    /// Icon location for the "About" command.
    static const std::string CMD_ICON_ABOUT;

    /// Default name for new control units.
    static const std::string COMP_DEFAULT_NAME_GCU;
    /// Name for the default segments of busses.
    static const std::string COMP_DEFAULT_NAME_SEGMENT;

    // Prefixes for generated names of the components.
    /// Prefix for new function unit names.
    static const std::string COMP_NEW_NAME_PREFIX_FU;
    /// Prefix for new register file names.
    static const std::string COMP_NEW_NAME_PREFIX_RF;
    /// Prefix for new immediate unit names.
    static const std::string COMP_NEW_NAME_PREFIX_IU;
    /// Prefix for new socket names..
    static const std::string COMP_NEW_NAME_PREFIX_SOCKET;
    /// Prefix for new bus names.
    static const std::string COMP_NEW_NAME_PREFIX_BUS;
    /// Prefix for new port names.
    static const std::string COMP_NEW_NAME_PREFIX_PORT;
    /// Prefix for new bridge names.
    static const std::string COMP_NEW_NAME_PREFIX_BRIDGE;
    /// Prefix for new operation names.
    static const std::string COMP_NEW_NAME_PREFIX_OPERATION;
    /// Prefix for new address space names.
    static const std::string COMP_NEW_NAME_PREFIX_AS;

    // zooming constants

    /// Minimum zoom factor.
    static const double MIN_ZOOM_FACTOR;
    /// Maximum zoom factor.
    static const double MAX_ZOOM_FACTOR;
    /// Step between zoom levels.
    static const double ZOOM_STEP;


    // editor constants

    /// Copyright text of the Editor.
    static const wxString EDITOR_COPYRIGHT_TEXT;
    /// Full name of the Editor.
    static const wxString EDITOR_NAME;
    /// Location of the logo picture of the Editor.
    static const std::string EDITOR_LOGO_PATH;

    /// File name extensions.
    /// Processor architecture file extension.
    static const std::string PROCESSOR_ARCH_FILE_EXTENSION;
    /// Processor configuration file extension.
    static const std::string PROCESSOR_CONFIG_FILE_EXTENSION;

    /// Unique identifiers for the commands.
    enum CommandID {
	COMMAND_NEW_DOC = 20000,
	COMMAND_OPEN_DOC,
	COMMAND_CLOSE_DOC,
	COMMAND_SAVE_DOC,
	COMMAND_SAVEAS_DOC,
        COMMAND_PRINT,
        COMMAND_PRINT_PREVIEW,
        COMMAND_EXPORT,
	COMMAND_QUIT,

	COMMAND_ADD_FU,
	COMMAND_ADD_RF,
	COMMAND_ADD_BUS,
	COMMAND_ADD_SOCKET,
	COMMAND_ADD_BRIDGE,
	COMMAND_ADD_IU,
	COMMAND_ADD_GCU,
	COMMAND_ADD_AS,

        COMMAND_ADD_RF_FROM_HDB,
        COMMAND_ADD_IU_FROM_HDB,
        COMMAND_ADD_FU_FROM_HDB,

	COMMAND_DELETE_COMP,
	COMMAND_MODIFY_COMP,

        COMMAND_UNDO,
        COMMAND_REDO,
	COMMAND_CUT,
	COMMAND_COPY,
	COMMAND_PASTE,

        COMMAND_EDIT_CONNECTIONS,
        COMMAND_EDIT_ADDRESS_SPACES,
        COMMAND_EDIT_TEMPLATES,
	COMMAND_EDIT_IMMEDIATE_SLOTS,
        COMMAND_EDIT_BUS_ORDER,

	COMMAND_ZOOM_IN,
	COMMAND_ZOOM_OUT,
	COMMAND_ZOOM_FIT_WIN,
	COMMAND_ZOOM_FIT_HEI,
	COMMAND_ZOOM_FIT_WID,
        COMMAND_TOGGLE_UNIT_DETAILS,

        COMMAND_FULLY_CONNECT_BUSSES,
        COMMAND_VERIFY_MACHINE,
        COMMAND_IMPLEMENTATION,
	COMMAND_SELECT,

	COMMAND_EDIT_OPTIONS,
	COMMAND_LOAD_OPTIONS,
	COMMAND_SAVE_OPTIONS,
	COMMAND_SAVEAS_OPTIONS,

        COMMAND_ABOUT,
        COMMAND_NOT_IMPLEMENTED_UNDO,
        COMMAND_NOT_IMPLEMENTED_REDO
    };
};

#endif
