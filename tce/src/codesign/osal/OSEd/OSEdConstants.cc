/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file OSEdConstants.cc
 *
 * Definition of OSEdConstants.cc class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/treectrl.h>
#include "OSEdConstants.hh"
#include "Environment.hh"

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

const string OSEdConstants::DEFAULT_EDITOR = Environment::defaultTextEditorPath();

const wxString OSEdConstants::APPLICATION_NAME = _T("OSEd");

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


