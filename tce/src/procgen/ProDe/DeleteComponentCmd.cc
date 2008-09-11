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
 * @file DeleteComponentCmd.cc
 *
 * Implementation of DeleteComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "DeleteComponentCmd.hh"
#include "ComponentCommand.hh"
#include "MDFView.hh"
#include "Request.hh"
#include "MDFDocument.hh"
#include "EditPart.hh"
#include "ProDeConstants.hh"

using std::string;

/**
 * The Constructor.
 */
DeleteComponentCmd::DeleteComponentCmd():
    EditorCommand(ProDeConstants::CMD_NAME_DELETE_COMP) {

}



/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
DeleteComponentCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    // send delete request to the selected EditPart
    Request* deleteRequest = new Request(Request::DELETE_REQUEST);
    EditPart* selected = dynamic_cast<MDFView*>(view())->selection();
    if (selected == NULL) {
	return false;
    }

    ComponentCommand* command = selected->performRequest(deleteRequest);

    // execute the returned command
    if (command == NULL) {
	return false;
    } else {
	Model* model = dynamic_cast<MDFDocument*>(
	    wxGetApp().docManager()->GetCurrentDocument())->getModel();

	model->pushToStack();

        dynamic_cast<MDFView*>(view())->clearSelection();
        bool result = command->Do();

	if (result) {
	    // Model was modified.
	    model->notifyObservers();
	} else {
	    // Command was cancelled.
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
DeleteComponentCmd::id() const{
    return ProDeConstants::COMMAND_DELETE_COMP;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
DeleteComponentCmd*
DeleteComponentCmd::create() const {
    return new DeleteComponentCmd();
}


/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
DeleteComponentCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_DELETE_COMP;
}

/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
DeleteComponentCmd::icon() const {
    return ProDeConstants::CMD_ICON_DELETE_COMP;
}



/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a block is selected, and the
 * block can handle a delete request.
 *
 * @return True, if a deletable block is selected.
 */
bool
DeleteComponentCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();

    MDFView* mdfView = dynamic_cast<MDFView*>(manager->GetCurrentView());
    if (mdfView == NULL) {
	return false;
    }
    Request* deleteRequest = new Request(Request::DELETE_REQUEST);
    if (mdfView->selection() == NULL ||
        !mdfView->selection()->canHandle(deleteRequest)) {

	return false;
    }
    delete deleteRequest;
    return true;
}
