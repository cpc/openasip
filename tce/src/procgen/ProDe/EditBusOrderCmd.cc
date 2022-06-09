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
 * @file EditBusOrderCmd.cc
 *
 * Definition of EditBusOrderCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */


#include <wx/docview.h>

#include "EditBusOrderCmd.hh"
#include "ProDeConstants.hh"
#include "ProDeBusOrderDialog.hh"
#include "ProDe.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "Machine.hh"

using std::string;


/**
 * The Constructor.
 */
EditBusOrderCmd::EditBusOrderCmd():
    EditorCommand(ProDeConstants::CMD_NAME_EDIT_BUS_ORDER) {
}


/**
 * The Destructor.
 */
EditBusOrderCmd::~EditBusOrderCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
EditBusOrderCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    ProDeBusOrderDialog dialog(parentWindow(), *model->getMachine());

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
EditBusOrderCmd::id() const {
    return ProDeConstants::COMMAND_EDIT_BUS_ORDER;
}


/**
 * Creates and returns a new instance of this command.
 */
EditBusOrderCmd*
EditBusOrderCmd::create() const {
    return new EditBusOrderCmd();
}


/**
 * Returns short version of the command name.
 */
string
EditBusOrderCmd::shortName() const {
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
EditBusOrderCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
