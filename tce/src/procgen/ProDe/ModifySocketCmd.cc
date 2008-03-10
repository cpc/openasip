/**
 * @file ModifySocketCmd.cc
 *
 * Definition of ModifySocketCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "ModifySocketCmd.hh"
#include "Socket.hh"
#include "EditPart.hh"
#include "SocketDialog.hh"
#include "ErrorDialog.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 */
ModifySocketCmd::ModifySocketCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {

}


/**
 * The Destructor.
 */
ModifySocketCmd::~ModifySocketCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
ModifySocketCmd::Do() {

    Socket* socket = dynamic_cast<Socket*>(editPart_->model());
    assert (socket != NULL);

    assert (parentWindow() != NULL);

    SocketDialog dialog(parentWindow(), socket);

    if (dialog.ShowModal() == wxID_OK) {
	// socket was modified
	return true;
    } else {
	// modification was cancelled
	return false;
    }

    return false;
}
