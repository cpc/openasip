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
 * @file AddRFCmd.cc
 *
 * Definition of AddRFCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/docview.h>

#include "AddRFCmd.hh"
#include "RFDialog.hh"
#include "Model.hh"
#include "MDFDocument.hh"
#include "ProDeConstants.hh"
#include "ModelConstants.hh"
#include "ErrorDialog.hh"
#include "ProDe.hh"
#include "RegisterFile.hh"
#include "Machine.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 */
AddRFCmd::AddRFCmd() :
    EditorCommand(ProDeConstants::CMD_NAME_ADD_RF) {
}



/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
AddRFCmd::Do() {

    assert(parentWindow() != NULL);
    assert(view() != NULL);

    Model* model = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel();

    model->pushToStack();

    // Generate name for the new RF.
    Machine::RegisterFileNavigator navigator =
        model->getMachine()->registerFileNavigator();
    string newName = ProDeConstants::COMP_NEW_NAME_PREFIX_RF;
    int i = 1;
    while (navigator.hasItem(newName)) {
        newName = ProDeConstants::COMP_NEW_NAME_PREFIX_RF +
            Conversion::toString(i);
        i++;
    }

    RegisterFile* rf = new RegisterFile(
        newName, ModelConstants::DEFAULT_RF_SIZE,
        ModelConstants::DEFAULT_WIDTH,
        ModelConstants::DEFAULT_RF_MAX_READS,
        ModelConstants::DEFAULT_RF_MAX_WRITES,
        ModelConstants::DEFAULT_RF_GUARD_LATENCY, RegisterFile::NORMAL);
    
    rf->setMachine(*(model->getMachine()));

    RFDialog dialog(parentWindow(), rf);

    if (dialog.ShowModal() == wxID_OK) {
	model->notifyObservers();
	return true;
    } else {
	// register file creation was cancelled
	model->popFromStack();
	return false;
    }
}


/**
 * Returns id of this command.
 *
 * @return ID for this command to be used in menus and toolbars.
 */
int
AddRFCmd::id() const {
    return ProDeConstants::COMMAND_ADD_RF;
}


/**
 * Creates and returns a new instance of this command.
 *
 * @return Newly created instance of this command.
 */
AddRFCmd*
AddRFCmd::create() const {
    return new AddRFCmd();
}

/**
 * Returns short version of the command name.
 *
 * @return Short name of the command to be used in the toolbar.
 */
string
AddRFCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_ADD_RF;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
AddRFCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
