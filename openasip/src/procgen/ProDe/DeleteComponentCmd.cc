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
 * @file DeleteComponentCmd.cc
 *
 * Implementation of DeleteComponentCmd class.
 *
 * @author Veli-Pekka Jääskeläinen (vjaaskel-no.spam-cs.tut.fi)
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
