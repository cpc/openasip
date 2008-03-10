/**
 * @file OSEdConstants.cc
 *
 * Definition of OSEdConstants.cc class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#include <wx/treectrl.h>
#include "OSEdConstants.hh"

using std::string;

const string OSEdConstants::CMD_NAME_QUIT = "Quit";
const string OSEdConstants::CMD_NAME_ABOUT = "About";
const string OSEdConstants::CMD_NAME_PROPERTIES = "Operation properties";
const string OSEdConstants::CMD_NAME_ADD_MODULE = "Add module";
const string OSEdConstants::CMD_NAME_ADD_OPERATION = "Add operation";
const string OSEdConstants::CMD_NAME_SIMULATE = "Simulate";
const string OSEdConstants::CMD_NAME_OPTIONS = "Options";
const string OSEdConstants::CMD_NAME_BUILD = "Build";
const string OSEdConstants::CMD_NAME_BUILD_ALL = "Build all";
const string OSEdConstants::CMD_NAME_USER_MANUAL = "User manual";
const string OSEdConstants::CMD_NAME_REMOVE_MODULE = "Remove module";
const string OSEdConstants::CMD_NAME_MODIFY_BEHAVIOR = "Modify behavior";
const string OSEdConstants::CMD_NAME_REMOVE_OPERATION = "Remove operation";
const string OSEdConstants::CMD_NAME_MEMORY = "Memory";

const string OSEdConstants::CONF_FILE_NAME = "OSEd.conf";
const string OSEdConstants::USER_MANUAL_NAME = "index.html";
const string OSEdConstants::BEHAVIOR_TEMPLATE_FILE_NAME = 
"behavior_definition_template.cc";

const string OSEdConstants::DEFAULT_EDITOR = "/usr/bin/emacs";

const wxString OSEdConstants::APPLICATION_NAME = _T("OSEd");

const wxFont* OSEdConstants::BOLD = new wxFont(12, wxROMAN, wxNORMAL, wxBOLD);

const int OSEdConstants::DEFAULT_COLUMN_WIDTH = 200;

const string OSEdConstants::LOGO_NAME = "logo.png";

const wxString OSEdConstants::OSED_COPYRIGHT = 
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


