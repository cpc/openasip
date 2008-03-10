/**
 * @file ModifyBridgeCmd.cc
 *
 * Definition of ModifyBridgeCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#include <string>

#include "Application.hh"
#include "ModifyBridgeCmd.hh"
#include "Bridge.hh"
#include "EditPart.hh"
#include "BridgeDialog.hh"
#include "ErrorDialog.hh"

using std::string;
using namespace TTAMachine;


/**
 * The Constructor.
 *
 * @param editPart EditPart of the component to modify.
 */
ModifyBridgeCmd::ModifyBridgeCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {

}


/**
 * The Destructor.
 */
ModifyBridgeCmd::~ModifyBridgeCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifyBridgeCmd::Do() {

    Bridge* bridge = NULL;
    Bridge* opposite = NULL;

    if (editPart_->childCount() == 0) {
        bridge = dynamic_cast<Bridge*>(editPart_->model());
	editPart_->setModel(NULL);
        assert (bridge != NULL);
    } else {
        // bidirectional bridge
        assert(editPart_->childCount() == 2);
        // find bridges of both direction
        bridge = dynamic_cast<Bridge*>(editPart_->child(0)->model());
        opposite = dynamic_cast<Bridge*>(editPart_->child(1)->model());
	editPart_->setModel(NULL);
    }

    assert(opposite != bridge);

    BridgeDialog dialog(parentWindow(), bridge, opposite);

    if (dialog.ShowModal() == wxID_OK) {
	return true;
    } else {
	// modification was cancelled
	return false;
    }

    return false;
}
