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
 * @file CutComponentCmd.cc
 *
 * Implementation of CutComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <wx/docview.h>

#include "Application.hh"
#include "CutComponentCmd.hh"
#include "ComponentCommand.hh"
#include "MDFView.hh"
#include "MDFDocument.hh"
#include "EditPart.hh"
#include "ProDeConstants.hh"
#include "Request.hh"
#include "ProDe.hh"

using std::string;

/**
 * The Constructor.
 */
CutComponentCmd::CutComponentCmd():
    EditorCommand(ProDeConstants::CMD_NAME_CUT) {
}



/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
CutComponentCmd::Do() {

    assert(view() != NULL);

    Request* copyRequest = new Request(Request::COPY_REQUEST);
    Request* deleteRequest = new Request(Request::DELETE_REQUEST);

    EditPart* selected = dynamic_cast<MDFView*>(view())->selection();
    if (selected == NULL) {
        return false;
    }

    // Request copy and delete commands.
    ComponentCommand* copyCommand = selected->performRequest(copyRequest);
    ComponentCommand* deleteCommand = selected->performRequest(deleteRequest);

    // Execute the returned commands.
    if (copyCommand == NULL || deleteCommand == NULL) {
        return false;
    } else {

        dynamic_cast<MDFView*>(view())->clearSelection();

	Model* model = dynamic_cast<MDFDocument*>(
	    wxGetApp().docManager()->GetCurrentDocument())->getModel();

	model->pushToStack();

        // copy
        copyCommand->setParentWindow(parentWindow());
        copyCommand->Do();

        // delete
        deleteCommand->setParentWindow(parentWindow());
        deleteCommand->Do();

	model->notifyObservers();

        return false;
    }
}


/**
 * Returns command identifier of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
CutComponentCmd::id() const {
    return ProDeConstants::COMMAND_CUT;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
CutComponentCmd*
CutComponentCmd::create() const {
    return new CutComponentCmd();
}



/**
 * Returns path to the command's icon file.
 *
 * @return Full path to the command's icon file.
 */
string
CutComponentCmd::icon() const {
    return ProDeConstants::CMD_ICON_CUT;
}



/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a component is selected, and the
 * selected component can be copied and deleted.
 *
 * @return True, if a component is selected.
 */
bool
CutComponentCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();

    MDFView* mdfView = dynamic_cast<MDFView*>(manager->GetCurrentView());
    if (mdfView == NULL) {
	return false;
    }

    EditPart* selected = mdfView->selection();
    Request* copyRequest = new Request(Request::COPY_REQUEST);
    Request* deleteRequest = new Request(Request::DELETE_REQUEST);
    if (selected == NULL ||
        !(selected->canHandle(copyRequest)) ||
        !(selected->canHandle(deleteRequest))) {

	delete copyRequest;
	return false;
    }
    delete copyRequest;
    delete deleteRequest;
    return true;
}
