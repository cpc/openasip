/**
 * @file ModifyGCUCmd.cc
 *
 * Definition of ModifyGCUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "ModifyGCUCmd.hh"
#include "ModelConstants.hh"
#include "ControlUnit.hh"
#include "EditPart.hh"
#include "GCUDialog.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ModifyGCUCmd::ModifyGCUCmd(EditPart* editPart):
    editPart_(editPart) {

}


/**
 * The Destructor.
 */
ModifyGCUCmd::~ModifyGCUCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyGCUCmd::Do() {

    ControlUnit* gcu = dynamic_cast<ControlUnit*>(editPart_->model());
    assert (gcu != NULL);
    assert (parentWindow() != NULL);

    GCUDialog dialog(parentWindow(), gcu);

    if (dialog.ShowModal() == wxID_OK) {
	return true;
    } else {
	// modification was cancelled
	return false;
    }

    return false;
}
