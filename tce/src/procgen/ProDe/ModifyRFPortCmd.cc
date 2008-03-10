/**
 * @file ModifyRFPortCmd.cc
 *
 * Definition of ModifyRFPortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "ModifyRFPortCmd.hh"
#include "ModelConstants.hh"
#include "EditPart.hh"
#include "RFPortDialog.hh"
#include "Port.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Port to modify.
 */
ModifyRFPortCmd::ModifyRFPortCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {
}


/**
 * The Destructor.
 */
ModifyRFPortCmd::~ModifyRFPortCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyRFPortCmd::Do() {

    Port* port = dynamic_cast<Port*>(editPart_->model());

    assert (port != NULL);
    assert (parentWindow() != NULL);

    RFPortDialog dialog(parentWindow(), port);

    if (dialog.ShowModal() == wxID_OK) {
	// port was modified
	return true;
    } else {
	return false;
    }
}
