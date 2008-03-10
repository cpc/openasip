/**
 * @file ModifyFUCmd.cc
 *
 * Definition of ModifyFUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "ModifyFUCmd.hh"
#include "ModelConstants.hh"
#include "FunctionUnit.hh"
#include "EditPart.hh"
#include "FUDialog.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ModifyFUCmd::ModifyFUCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {
}


/**
 * The Destructor.
 */
ModifyFUCmd::~ModifyFUCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyFUCmd::Do() {

    FunctionUnit* fu = dynamic_cast<FunctionUnit*>(editPart_->model());

    assert (fu != NULL);
    assert (parentWindow() != NULL);

    FUDialog dialog(parentWindow(), fu);

    if (dialog.ShowModal() == wxID_OK) {
	// function unit was modified
	return true;
    } else {
	// modification was cancelled
	return false;
    }
}
