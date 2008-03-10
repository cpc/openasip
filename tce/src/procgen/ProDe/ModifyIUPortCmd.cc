/**
 * @file ModifyIUPortCmd.cc
 *
 * Definition of ModifyIUPortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "ModifyIUPortCmd.hh"
#include "ModelConstants.hh"
#include "EditPart.hh"
#include "IUPortDialog.hh"
#include "Port.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Port to modify.
 */
ModifyIUPortCmd::ModifyIUPortCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {
}


/**
 * The Destructor.
 */
ModifyIUPortCmd::~ModifyIUPortCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyIUPortCmd::Do() {

    Port* port = dynamic_cast<Port*>(editPart_->model());

    assert (port != NULL);
    assert (parentWindow() != NULL);

    IUPortDialog dialog(parentWindow(), port);

    if (dialog.ShowModal() == wxID_OK) {
	// port was modified
	return true;
    } else {
	return false;
    }
}
