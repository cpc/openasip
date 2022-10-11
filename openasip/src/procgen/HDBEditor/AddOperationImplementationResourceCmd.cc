/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file AddOperationImplementationResourceCmd.cc
 *
 * Implementation of AddOperationImplementationResourceCmd.
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */

#include "AddOperationImplementationResourceCmd.hh"
#include "HDBEditorConstants.hh"
#include "HDBEditor.hh"
#include "OperationImplementationResourceDialog.hh"

#include <wx/app.h>

/// Constructor.
AddOperationImplementationResourceCmd::AddOperationImplementationResourceCmd()
    : GUICommand(
          HDBEditorConstants::
              COMMAND_NAME_ADD_OPERATION_IMPLEMENTATION_RESOURCE,
          NULL) {}

/// Execute command.
bool
AddOperationImplementationResourceCmd::Do() {
    OperationImplementationResourceDialog* dialog =
        new OperationImplementationResourceDialog();
    dialog->Show(true);

    return true;
}

/**
 * Returns name of the command icon file.
 *
 * @return Command icon file name.
 */
std::string
AddOperationImplementationResourceCmd::icon() const {
    return "";
}

/**
 * Returns command id.
 *
 * @return Command identifier.
 */
int
AddOperationImplementationResourceCmd::id() const {
    return HDBEditorConstants::COMMAND_ADD_OPERATION_IMPLEMENTATION_RESOURCE;
}

/**
 * Creates a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddOperationImplementationResourceCmd*
AddOperationImplementationResourceCmd::create() const {
    return new AddOperationImplementationResourceCmd();
}

/**
 * Returns true if the command should be enabled in the menu/toolbar.
 *
 * @return True if the command is enabled, false if not.
 */
bool
AddOperationImplementationResourceCmd::isEnabled() {
    HDB::HDBManager* manager = wxGetApp().mainFrame().hdbManager();

    if (manager == NULL) {
        return false;
    }

    return true;
}
