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
 * @file EditAddressSpacesCmd.cc
 *
 * Definition of EditAddressSpacesCmd class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
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
