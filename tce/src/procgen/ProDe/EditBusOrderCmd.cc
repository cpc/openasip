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
 * @file EditBusOrderCmd.cc
 *
 * Definition of EditBusOrderCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
