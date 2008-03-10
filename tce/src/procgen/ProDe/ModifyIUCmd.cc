/**
 * @file ModifyIUCmd.cc
 *
 * Definition of ModifyIUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "ModifyIUCmd.hh"
#include "ImmediateUnit.hh"
#include "EditPart.hh"
#include "IUDialog.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ModifyIUCmd::ModifyIUCmd(EditPart* editPart):
    editPart_(editPart) {

}


/**
 * The Destructor.
 */
ModifyIUCmd::~ModifyIUCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyIUCmd::Do() {

    ImmediateUnit* iu = dynamic_cast<ImmediateUnit*>(editPart_->model());
    assert (iu != NULL);
    assert (parentWindow() != NULL);

    IUDialog dialog(parentWindow(), iu);

    if (dialog.ShowModal() == wxID_OK) {
	// immediate unit was modified
	return true;
    } else {
	// modification was cancelled
	return false;
    }

    return false;
}
