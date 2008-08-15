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
 * @file EditImmediateSlotsCmd.cc
 *
 * Definition of EditImmediateSlotsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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
