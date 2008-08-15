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
 * @file ModifyComponentCmd.cc
 *
 * Implementation of ModifyComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "ModifyComponentCmd.hh"
#include "ComponentCommand.hh"
#include "WarningDialog.hh"
#include "Request.hh"
#include "MDFView.hh"
#include "MDFDocument.hh"
#include "EditPart.hh"
#include "ProDeConstants.hh"

using std::string;

/**
 * The Constructor.
 */
ModifyComponentCmd::ModifyComponentCmd():
    EditorCommand(ProDeConstants::CMD_NAME_MODIFY_COMP) {
}



/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
ModifyComponentCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    // send modify request to the selected EditPart
    Request* modifyRequest = new Request(Request::MODIFY_REQUEST);
    EditPart* selected = dynamic_cast<MDFView*>(view())->selection();
    if (selected == NULL) {
	return false;
    }

    ComponentCommand* command = selected->performRequest(modifyRequest);

    // execute the returned command
    if (command == NULL) {
	return false;
    } else {

	Model* model = dynamic_cast<MDFDocument*>(
	    wxGetApp().docManager()->GetCurrentDocument())->getModel();

	model->pushToStack();

	dynamic_cast<MDFView*>(view())->clearSelection();
	command->setParentWindow(parentWindow());
	bool result = command->Do();
	if (result) {
	    // Model was modified.
	    model->notifyObservers();
	} else {
	    // Modification was cancelled.
	    model->popFromStack();
	}
	return result;
    }
}


/**
 * Returns command identifier of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
ModifyComponentCmd::id() const {
    return ProDeConstants::COMMAND_MODIFY_COMP;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
ModifyComponentCmd*
ModifyComponentCmd::create() const {
    return new ModifyComponentCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
ModifyComponentCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_MODIFY_COMP;
}

/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
ModifyComponentCmd::icon() const {
    return ProDeConstants::CMD_ICON_MODIFY_COMP;
}



/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a block is selected, and the
 * selected block can handle modify requests.
 *
 * @return True, if a modifyable block is selected.
 */
bool
ModifyComponentCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();

    MDFView* mdfView = dynamic_cast<MDFView*>(manager->GetCurrentView());
    if (mdfView == NULL) {
	return false;
    }
    Request* modifyRequest = new Request(Request::MODIFY_REQUEST);
    if (mdfView->selection() == NULL ||
        !mdfView->selection()->canHandle(modifyRequest)) {

	return false;
    }
    return true;
}
