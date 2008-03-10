/**
 * @file DeleteBridgeCmd.cc
 *
 * Definition of DeleteBridgeCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#include "Application.hh"
#include "DeleteBridgeCmd.hh"
#include "Bridge.hh"
#include "EditPart.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Bridge to delete.
 */
DeleteBridgeCmd::DeleteBridgeCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {
}


/**
 * The Destructor.
 */
DeleteBridgeCmd::~DeleteBridgeCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
DeleteBridgeCmd::Do() {

    if (editPart_->childCount() == 0) {
        Bridge* bridge = dynamic_cast<Bridge*>(editPart_->model());
        assert (bridge != NULL);
	delete bridge;
        bridge = NULL;
    } else {
        // bidirectional bridge
        assert(editPart_->childCount() == 2);
        // delete bridges of both direction
        for (int i = 0; i < editPart_->childCount(); i++) {
            Bridge* bridge =
                dynamic_cast<Bridge*>(editPart_->child(i)->model());
            assert (bridge != NULL);
	    delete bridge;
            bridge = NULL;
        }
    }

    return true;
}
