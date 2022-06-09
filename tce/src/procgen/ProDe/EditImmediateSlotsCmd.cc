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
 * @file EditImmediateSlotsCmd.cc
 *
 * Definition of EditImmediateSlotsCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "EditImmediateSlotsCmd.hh"
#include "ProDeConstants.hh"
#include "ImmediateSlotDialog.hh"
#include "ProDe.hh"
#include "Model.hh"
#include "MDFDocument.hh"

using std::string;


/**
 * The Constructor.
 */
EditImmediateSlotsCmd::EditImmediateSlotsCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EDIT_IMMEDIATE_SLOTS) {
}


/**
 * The Destructor.
 */
EditImmediateSlotsCmd::~EditImmediateSlotsCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
EditImmediateSlotsCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    ImmediateSlotDialog dialog(parentWindow(), model->getMachine());

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
        return true;
    } else {
        // Cancel button was pressed, immediate slot modifications are
	// cancelled.
	model->popFromStack();
        return false;
    }
}


/**
 * Returns id of this command.
 */
int
EditImmediateSlotsCmd::id() const {
    return ProDeConstants::COMMAND_EDIT_IMMEDIATE_SLOTS;
}


/**
 * Creates and returns a new instance of this command.
 */
EditImmediateSlotsCmd*
EditImmediateSlotsCmd::create() const {
    return new EditImmediateSlotsCmd();
}



/**
 * Returns path to the command's icon file.
 */
string
EditImmediateSlotsCmd::icon() const {
    return ProDeConstants::CMD_ICON_DEFAULT;
}


/**
 * Returns short version of the command name.
 */
string
EditImmediateSlotsCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_EDIT_IMMEDIATE_SLOTS;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
EditImmediateSlotsCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
