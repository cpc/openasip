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
