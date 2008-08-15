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
 * @file HDBEditorConstants.cc
 *
 * Implementation of HDBEditorConstants class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "HDBEditorConstants.hh"



const std::string HDBEditorConstants::COMMAND_NAME_OPEN_HDB = "Open HDB";
const std::string HDBEditorConstants::COMMAND_NAME_CREATE_HDB = "Create HDB";
const std::string HDBEditorConstants::COMMAND_NAME_QUIT = "Quit";

const std::string HDBEditorConstants::COMMAND_NAME_ADD_FU_ENTRY =
    "Add FU Entry";
const std::string HDBEditorConstants::COMMAND_NAME_ADD_RF_ENTRY =
    "Add RF Entry";
const std::string HDBEditorConstants::COMMAND_NAME_ADD_BUS_ENTRY =
    "Add Bus Entry";
const std::string HDBEditorConstants::COMMAND_NAME_ADD_SOCKET_ENTRY =
    "Add Socket Entry";
const std::string HDBEditorConstants::COMMAND_NAME_DELETE = "Delete";
const std::string HDBEditorConstants::COMMAND_NAME_ADD_FU_FROM_ADF =
    "Add FU Arch from ADF";
const std::string HDBEditorConstants::COMMAND_NAME_ADD_RF_ARCHITECTURE =
    "Add RF Architecture";
const std::string HDBEditorConstants::COMMAND_NAME_ADD_COST_PLUGIN =
    "Add Cost Function Plugin";

const std::string HDBEditorConstants::COMMAND_NAME_SET_FU_ARCH =
    "Set FU Architecture";
const std::string HDBEditorConstants::COMMAND_NAME_SET_RF_ARCH =
    "Set RF Architecture";
const std::string HDBEditorConstants::COMMAND_NAME_SET_COST_PLUGIN =
    "Set Cost Function Plugin";
const std::string HDBEditorConstants::COMMAND_NAME_ADD_FU_IMPLEMENTATION =
    "Add FU Implementation";
const std::string HDBEditorConstants::COMMAND_NAME_ADD_RF_IMPLEMENTATION =
    "Add RF Implementation";
const std::string HDBEditorConstants::COMMAND_NAME_MODIFY = "Modify";
const std::string HDBEditorConstants::COMMAND_NAME_ABOUT = "About";

const wxString HDBEditorConstants::HDB_EDITOR_COPYRIGHT =
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

