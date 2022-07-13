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
 * @file HDBEditorConstants.cc
 *
 * Implementation of HDBEditorConstants class.
 *
 * @author Veli-Pekka Jääskeläinen 2006 (vjaaskel-no.spam-cs.tut.fi)
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

const std::string
    HDBEditorConstants::COMMAND_NAME_ADD_OPERATION_IMPLEMENTATION_RESOURCE =
        "Add Operation Implementation Resource";
const std::string
    HDBEditorConstants::COMMAND_NAME_ADD_OPERATION_IMPLEMENTATION =
        "Add FUGEN Operation Implementation";

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

