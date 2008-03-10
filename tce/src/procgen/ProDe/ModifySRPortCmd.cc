/**
 * @file ModifySRPortCmd.cc
 *
 * Definition of ModifySRPortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "ModifySRPortCmd.hh"
#include "ModelConstants.hh"
#include "EditPart.hh"
#include "SRPortDialog.hh"
#include "SpecialRegisterPort.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Port to modify.
 */
ModifySRPortCmd::ModifySRPortCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {
}


/**
 * The Destructor.
 */
ModifySRPortCmd::~ModifySRPortCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifySRPortCmd::Do() {

    SpecialRegisterPort* port =
        dynamic_cast<SpecialRegisterPort*>(editPart_->model());

    assert (port != NULL);
    assert (parentWindow() != NULL);

    SRPortDialog dialog(parentWindow(), port);

    if (dialog.ShowModal() == wxID_OK) {
	// port was modified
	return true;
    } else {
	return false;
    }
}
