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
 * @file EditAddressSpacesCmd.cc
 *
 * Definition of EditAddressSpacesCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 */

#include <vector>
#include <wx/docview.h>

#include "EditAddressSpacesCmd.hh"
#include "ProDeConstants.hh"
#include "AddressSpacesDialog.hh"
#include "ProDe.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "Machine.hh"
#include "ModelConstants.hh"
#include "AddressSpace.hh"

using std::string;
using std::vector;


/**
 * The Constructor.
 */
EditAddressSpacesCmd::EditAddressSpacesCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EDIT_ADDRESS_SPACES) {
}


/**
 * The Destructor.
 */
EditAddressSpacesCmd::~EditAddressSpacesCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
EditAddressSpacesCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    AddressSpacesDialog dialog(parentWindow(), model->getMachine());

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
        return true;
    } else {
        // Cancel button was pressed, address spaces are not modified.
	model->popFromStack();
        return false;
    }
}


/**
 * Returns id of this command.
 */
int
EditAddressSpacesCmd::id() const {
    return ProDeConstants::COMMAND_EDIT_ADDRESS_SPACES;
}


/**
 * Creates and returns a new instance of this command.
 */
EditAddressSpacesCmd*
EditAddressSpacesCmd::create() const {
    return new EditAddressSpacesCmd();
}



/**
 * Returns path to the command's icon file.
 */
string
EditAddressSpacesCmd::icon() const {
    return ProDeConstants::CMD_ICON_EDIT_ADDRESS_SPACES;
}


/**
 * Returns short version of the command name.
 */
string
EditAddressSpacesCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_EDIT_ADDRESS_SPACES;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
EditAddressSpacesCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
