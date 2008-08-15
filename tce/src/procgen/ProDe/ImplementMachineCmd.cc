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
 * @file ImplementMachineCmd.cc
 *
 * Definition of ImplementMachineCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2006 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <wx/docview.h>

#include "ProDe.hh"
#include "ImplementMachineCmd.hh"
#include "ProDeConstants.hh"
#include "MDFDocument.hh"
#include "ProcessorImplementationWindow.hh"
#include "MachineImplementation.hh"

using std::string;

/**
 * The Constructor.
 */
ImplementMachineCmd::ImplementMachineCmd():
    EditorCommand(ProDeConstants::CMD_NAME_IMPLEMENTATION) {

}


/**
 * The Destructor.
 */
ImplementMachineCmd::~ImplementMachineCmd() {}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 */
bool
ImplementMachineCmd::Do() {

    TTAMachine::Machine* machine = dynamic_cast<MDFDocument*>(
        view()->GetDocument())->getModel()->getMachine();

    IDF::MachineImplementation impl;
    ProcessorImplementationWindow dialog(parentWindow(), *machine, impl);
    dialog.ShowModal();

    return true;

}


/**
 * Returns id of this command.
 */
int
ImplementMachineCmd::id() const {
    return ProDeConstants::COMMAND_IMPLEMENTATION;
}


/**
 * Creates and returns a new instance of this command.
 */
ImplementMachineCmd*
ImplementMachineCmd::create() const {
    return new ImplementMachineCmd();
}



/**
 * Returns path to the command's icon file.
 */
string
ImplementMachineCmd::icon() const {
    return ProDeConstants::CMD_ICON_IMPLEMENTATION;
}


/**
 * Returns short version of the command name.
 */
string
ImplementMachineCmd::shortName() const {
    return ProDeConstants::CMD_SNAME_IMPLEMENTATION;
}


/**
 * Returns true when the command is executable, false when not.
 *
 * This command is executable when a document is open.
 *
 * @return True, if a document is open.
 */
bool
ImplementMachineCmd::isEnabled() {
    wxDocManager* manager = wxGetApp().docManager();
    if (manager->GetCurrentView() != NULL) {
	return true;
    }
    return false;
}
