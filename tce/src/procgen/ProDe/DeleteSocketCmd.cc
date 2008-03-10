/**
 * @file DeleteSocketCmd.cc
 *
 * Definition of DeleteSocketCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "DeleteSocketCmd.hh"
#include "Socket.hh"
#include "EditPart.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Socket to delete.
 */
DeleteSocketCmd::DeleteSocketCmd(EditPart* editPart):
    ComponentCommand(), editPart_(editPart) {
}


/**
 * The Destructor.
 */
DeleteSocketCmd::~DeleteSocketCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
DeleteSocketCmd::Do() {
    Socket* socket = dynamic_cast<Socket*>(editPart_->model());
    assert (socket != NULL);
    delete socket;
    return true;
}
