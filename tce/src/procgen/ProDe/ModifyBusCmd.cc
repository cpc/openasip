/**
 * @file ModifyBusCmd.cc
 *
 * Definition of ModifyBusCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "ModifyBusCmd.hh"
#include "ModelConstants.hh"
#include "Bus.hh"
#include "EditPart.hh"
#include "BusDialog.hh"
#include "ErrorDialog.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ModifyBusCmd::ModifyBusCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {

}


/**
 * The Destructor.
 */
ModifyBusCmd::~ModifyBusCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyBusCmd::Do() {

    Bus* bus = dynamic_cast<Bus*>(editPart_->model());
    assert (bus != NULL);

    assert (parentWindow() != NULL);

    BusDialog dialog(parentWindow(), bus);

    if (dialog.ShowModal() == wxID_OK) {
	// bus was modified
	return true;
    } else {
	// modification was cancelled
	return false;
    }
    return false;
}
