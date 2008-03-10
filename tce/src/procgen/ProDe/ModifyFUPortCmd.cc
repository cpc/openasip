/**
 * @file ModifyFUPortCmd.cc
 *
 * Definition of ModifyFUPortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "ModifyFUPortCmd.hh"
#include "ModelConstants.hh"
#include "EditPart.hh"
#include "FUPortDialog.hh"
#include "FUPort.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Port to modify.
 */
ModifyFUPortCmd::ModifyFUPortCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {
}


/**
 * The Destructor.
 */
ModifyFUPortCmd::~ModifyFUPortCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyFUPortCmd::Do() {

    FUPort* port = dynamic_cast<FUPort*>(editPart_->model());

    assert (port != NULL);
    assert (parentWindow() != NULL);

    FUPortDialog dialog(parentWindow(), port);

    if (dialog.ShowModal() == wxID_OK) {
	// port was modified
	return true;
    } else {
	return false;
    }
}
