/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file ProDeConstants.cc
 *
 * Definition of ProDeConstants class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProDeConstants.hh"
#include "RegisterFile.hh"
#include "ProDe.hh"

using std::string;

const wxColour ProDeConstants::PINK_COLOR = wxColour(255, 190, 200);
const wxColour ProDeConstants::LIGHT_BLUE_COLOR = wxColour(130, 200, 250);
const wxColour ProDeConstants::BLACK_COLOR = wxColour(0, 0, 0);
const wxColour ProDeConstants::WHITE_COLOR = wxColour(255, 255, 255);
const wxColour ProDeConstants::DARK_GRAY_COLOR = wxColour(80, 80, 80);
const wxColour ProDeConstants::MEDIUM_GRAY_COLOR = wxColour(130, 130, 130);
const wxColour ProDeConstants::LIGHT_GRAY_COLOR = wxColour(215, 215, 215);

const string ProDeConstants::EMPTY = "";
const string ProDeConstants::SEPARATOR = "-- separator --";

const wxString ProDeConstants::NONE = _T("NONE");

const wxString ProDeConstants::EXTENSION_ZERO = _T("Zero");
const wxString ProDeConstants::EXTENSION_SIGN = _T("Sign");

const wxString ProDeConstants::SOCKET_DIRECTION_INPUT = _T("Input");
const wxString ProDeConstants::SOCKET_DIRECTION_OUTPUT = _T("Output");

const string ProDeConstants::CMD_NAME_NEW_DOC = "New Document";
const string ProDeConstants::CMD_NAME_OPEN_DOC = "Open Document";
const string ProDeConstants::CMD_NAME_CLOSE_DOC = "Close Document";
const string ProDeConstants::CMD_NAME_SAVE_DOC = "Save Document";
const string ProDeConstants::CMD_NAME_SAVEAS_DOC = "Save Document As";
const string ProDeConstants::CMD_NAME_PRINT = "Print";
const string ProDeConstants::CMD_NAME_PRINT_PREVIEW = "Print Preview";
const string ProDeConstants::CMD_NAME_EXPORT = "Export";
const string ProDeConstants::CMD_NAME_QUIT = "Quit";

const string ProDeConstants::CMD_NAME_ADD_FU = "Add Function Unit";
const string ProDeConstants::CMD_NAME_ADD_RF = "Add Register File";
const string ProDeConstants::CMD_NAME_ADD_BUS = "Add Transport Bus";
const string ProDeConstants::CMD_NAME_ADD_SOCKET = "Add Socket";
const string ProDeConstants::CMD_NAME_ADD_BRIDGE = "Add Bridge";
const string ProDeConstants::CMD_NAME_ADD_IU = "Add Immediate Unit";
const string ProDeConstants::CMD_NAME_ADD_GCU = "Add Global Control Unit";
const string ProDeConstants::CMD_NAME_ADD_AS = "Add Address Space";

const string ProDeConstants::CMD_NAME_ADD_RF_FROM_HDB = "Add RF From HDB";
const string ProDeConstants::CMD_NAME_ADD_IU_FROM_HDB = "Add IU From HDB";
const string ProDeConstants::CMD_NAME_ADD_FU_FROM_HDB = "Add FU From HDB";

const string ProDeConstants::CMD_NAME_DELETE_COMP = "Delete Component";
const string ProDeConstants::CMD_NAME_MODIFY_COMP = "Modify Component";

const string ProDeConstants::CMD_NAME_UNDO = "Undo";
const string ProDeConstants::CMD_NAME_REDO = "Redo";
const string ProDeConstants::CMD_NAME_CUT = "Cut";
const string ProDeConstants::CMD_NAME_COPY = "Copy";
const string ProDeConstants::CMD_NAME_PASTE = "Paste";

const string ProDeConstants::CMD_NAME_SELECT = "Select";
const string ProDeConstants::CMD_NAME_EDIT_CONNECTIONS = "Edit Connections";
const string ProDeConstants::CMD_NAME_EDIT_IMMEDIATE_SLOTS =
    "Edit Immediate Slots";
const string ProDeConstants::CMD_NAME_EDIT_BUS_ORDER = "Bus Order";
const string ProDeConstants::CMD_NAME_EDIT_ADDRESS_SPACES =
    "Edit Address Spaces";
const string ProDeConstants::CMD_NAME_EDIT_TEMPLATES = "Edit Templates";

const string ProDeConstants::CMD_NAME_ZOOM_IN = "Zoom In";
const string ProDeConstants::CMD_NAME_ZOOM_OUT = "Zoom Out";
const string ProDeConstants::CMD_NAME_ZOOM_FIT_WIN = "Fit Window";
const string ProDeConstants::CMD_NAME_ZOOM_FIT_HEI = "Fit Height";
const string ProDeConstants::CMD_NAME_ZOOM_FIT_WID = "Fit Width";
const string ProDeConstants::CMD_NAME_TOGGLE_UNIT_DETAILS = 
    "Toggle Unit Details";

const string ProDeConstants::CMD_NAME_FULLY_CONNECT_BUSSES =
    "Fully Connect IC";
const string ProDeConstants::CMD_NAME_VLIW_CONNECT_IC =
    "VLIW Connect IC";
const string ProDeConstants::CMD_NAME_BLOCKS_CONNECT_IC = "Blocks Connect IC";
const string ProDeConstants::CMD_NAME_EDIT_MACHINE =
    "Processor Configurations";

const string ProDeConstants::CMD_NAME_VERIFY_MACHINE = "Verify Machine";
const string ProDeConstants::CMD_NAME_IMPLEMENTATION =
    "Processor Implementation";
    
const string ProDeConstants::CMD_CALL_EXPLORER_PLUGIN = "Call Explorer Plugin";

const string ProDeConstants::CMD_NAME_EDIT_OPTIONS = "Edit Options";
const string ProDeConstants::CMD_NAME_LOAD_OPTIONS = "Load Options";
const string ProDeConstants::CMD_NAME_SAVE_OPTIONS = "Save Options";
const string ProDeConstants::CMD_NAME_SAVEAS_OPTIONS = "Save Options As";

const string ProDeConstants::CMD_NAME_ABOUT = "About";

const string ProDeConstants::CMD_SNAME_NEW_DOC = "New";
const string ProDeConstants::CMD_SNAME_SAVE_DOC = "Save";
const string ProDeConstants::CMD_SNAME_SAVEAS_DOC = "Save As";
const string ProDeConstants::CMD_SNAME_OPEN_DOC = "Open";
const string ProDeConstants::CMD_SNAME_CLOSE_DOC = "Close";
const string ProDeConstants::CMD_SNAME_PRINT_PREVIEW = "Preview";

const string ProDeConstants::CMD_SNAME_ADD_FU = "Add FU";
const string ProDeConstants::CMD_SNAME_ADD_RF = "Add RF";
const string ProDeConstants::CMD_SNAME_ADD_BUS = "Add Bus";
const string ProDeConstants::CMD_SNAME_ADD_SOCKET = "Add Sck";
const string ProDeConstants::CMD_SNAME_ADD_BRIDGE = "Add Bdg";
const string ProDeConstants::CMD_SNAME_ADD_IU = "Add IU";
const string ProDeConstants::CMD_SNAME_ADD_GCU = "Add GCU";
const string ProDeConstants::CMD_SNAME_ADD_AS = "Add AS";

const string ProDeConstants::CMD_SNAME_ADD_RF_FROM_HDB = "HDB: RF";
const string ProDeConstants::CMD_SNAME_ADD_IU_FROM_HDB = "HDB: IU";
const string ProDeConstants::CMD_SNAME_ADD_FU_FROM_HDB = "HDB: FU";

const string ProDeConstants::CMD_SNAME_MODIFY_COMP = "Modify";
const string ProDeConstants::CMD_SNAME_DELETE_COMP = "Delete";
const string ProDeConstants::CMD_SNAME_ZOOM_IN = "Zoom +";
const string ProDeConstants::CMD_SNAME_ZOOM_OUT = "Zoom -";
const string ProDeConstants::CMD_SNAME_ZOOM_FIT_WIN = "Fit Win";
const string ProDeConstants::CMD_SNAME_ZOOM_FIT_HEI = "Fit Hgt";
const string ProDeConstants::CMD_SNAME_ZOOM_FIT_WID = "Fit Wid";
const string ProDeConstants::CMD_SNAME_TOGGLE_UNIT_DETAILS = "Details";
const string ProDeConstants::CMD_SNAME_EDIT_OPTIONS = "Options";
const string ProDeConstants::CMD_SNAME_LOAD_OPTIONS = "Ld Opt";
const string ProDeConstants::CMD_SNAME_SAVE_OPTIONS = "Sv Opt";
const string ProDeConstants::CMD_SNAME_SAVEAS_OPTIONS = "SvAs Opt";
const string ProDeConstants::CMD_SNAME_EDIT_ADDRESS_SPACES = "AS's";
const string ProDeConstants::CMD_SNAME_EDIT_TEMPLATES = "ITs";
const string ProDeConstants::CMD_SNAME_EDIT_MACHINE = "Mach Opt";
const string ProDeConstants::CMD_SNAME_EDIT_IMMEDIATE_SLOTS = "ImmSlots";
const string ProDeConstants::CMD_SNAME_EDIT_BUS_ORDER = "Bus Ord.";
const string ProDeConstants::CMD_SNAME_EDIT_CONNECTIONS = "Connect";
const string ProDeConstants::CMD_SNAME_FULLY_CONNECT_BUSSES = "Full IC";
const string ProDeConstants::CMD_SNAME_VLIW_CONNECT_IC = "VLIW IC";
const string ProDeConstants::CMD_SNAME_BLOCKS_CONNECT_IC = "Blocks IC";
const string ProDeConstants::CMD_SNAME_VERIFY_MACHINE = "Verify";
const string ProDeConstants::CMD_SNAME_IMPLEMENTATION = "Implement";
const string ProDeConstants::CMD_SNAME_EXPLORER_PLUGIN = "Explorer";

const string ProDeConstants::CMD_ICON_DEFAULT = "default.png";
const string ProDeConstants::CMD_ICON_OPEN_DOC = "fileopen.png";
const string ProDeConstants::CMD_ICON_CLOSE_DOC = "fileclose.png";
const string ProDeConstants::CMD_ICON_NEW_DOC = "filenew.png";
const string ProDeConstants::CMD_ICON_SAVE_DOC = "filesave.png";
const string ProDeConstants::CMD_ICON_SAVEAS_DOC = "filesaveas.png";
const string ProDeConstants::CMD_ICON_QUIT = "quit.png";
const string ProDeConstants::CMD_ICON_PRINT = "fileprint.png";
const string ProDeConstants::CMD_ICON_PRINT_PREVIEW = "printpreview.png";
const string ProDeConstants::CMD_ICON_SELECT = "select.png";
const string ProDeConstants::CMD_ICON_MODIFY_COMP = "edit.png";
const string ProDeConstants::CMD_ICON_DELETE_COMP = "delete.png";
const string ProDeConstants::CMD_ICON_ZOOM_IN = "zoomin.png";
const string ProDeConstants::CMD_ICON_ZOOM_OUT = "zoomout.png";
const string ProDeConstants::CMD_ICON_ZOOM_FIT_WIN = "fitwindow.png";
const string ProDeConstants::CMD_ICON_ZOOM_FIT_HEI = "fitheight.png";
const string ProDeConstants::CMD_ICON_ZOOM_FIT_WID = "fitwidth.png";
const string ProDeConstants::CMD_ICON_EDIT_OPTIONS = "options.png";
const string ProDeConstants::CMD_ICON_SAVE_OPTIONS = "filesave.png";
const string ProDeConstants::CMD_ICON_EDIT_ADDRESS_SPACES = "memory.png";
const string ProDeConstants::CMD_ICON_EDIT_TEMPLATES = "default.png";
const string ProDeConstants::CMD_ICON_EDIT_CONNECTIONS = "connect.png";
const string ProDeConstants::CMD_ICON_EDIT_MACHINE = "default.png";

const string ProDeConstants::CMD_ICON_VERIFY_MACHINE = "verify_machine.png";
const string ProDeConstants::CMD_ICON_IMPLEMENTATION = "default.png";
const string ProDeConstants::CMD_ICON_EXPLORER_PLUGIN = "default.png";
const string ProDeConstants::CMD_ICON_UNDO = "undo.png";
const string ProDeConstants::CMD_ICON_REDO = "redo.png";
const string ProDeConstants::CMD_ICON_COPY = "editcopy.png";
const string ProDeConstants::CMD_ICON_CUT = "editcut.png";
const string ProDeConstants::CMD_ICON_PASTE = "editpaste.png";
const string ProDeConstants::CMD_ICON_ABOUT = "about.png";

const string ProDeConstants::EDITOR_LOGO_PATH = "logo.png";

const wxString ProDeConstants::EDITOR_NAME = _T("TTA Processor Designer");
const wxString ProDeConstants::EDITOR_COPYRIGHT_TEXT = 
    _T("Portions of this program are copyright\n"
        "\n"
        "Xerces (c) 1999-2005 The Apache Software\n" 
        "wxWidgets (c) 1998 Julian Smart, Robert Roebling \n"
        "Tcl (c) 1998-1999 by Scriptics Corporation\n" 
        "Editline (c) 1997 The NetBSD Foundation, Inc.\n"
        "Boost C++ Libraries (c) 2003 Beman Dawes, David Abrahams\n" 
        "\n" 
        "For more information see User manual chapter \"Copyright notices\""
    );

const string ProDeConstants::COMP_DEFAULT_NAME_GCU = "gcu";
const string ProDeConstants::COMP_DEFAULT_NAME_SEGMENT = "seg1";

const string ProDeConstants::COMP_NEW_NAME_PREFIX_FU = "FU";
const string ProDeConstants::COMP_NEW_NAME_PREFIX_RF = "RF";
const string ProDeConstants::COMP_NEW_NAME_PREFIX_IU = "IMM";
const string ProDeConstants::COMP_NEW_NAME_PREFIX_SOCKET = "S";
const string ProDeConstants::COMP_NEW_NAME_PREFIX_BUS = "B";
const string ProDeConstants::COMP_NEW_NAME_PREFIX_PORT = "P";
const string ProDeConstants::COMP_NEW_NAME_PREFIX_BRIDGE = "Br";
const string ProDeConstants::COMP_NEW_NAME_PREFIX_OPERATION = "operation";
const string ProDeConstants::COMP_NEW_NAME_PREFIX_AS = "AS";

const string ProDeConstants::PROCESSOR_ARCH_FILE_EXTENSION = ".adf";
const string ProDeConstants::PROCESSOR_CONFIG_FILE_EXTENSION = ".cfg";

const std::string ProDeConstants::ICON_SORT_ASC = "sort_asc.png";
const std::string ProDeConstants::ICON_SORT_DESC = "sort_desc.png";
