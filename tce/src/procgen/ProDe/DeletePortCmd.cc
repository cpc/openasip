/**
 * @file DeletePortCmd.cc
 *
 * Definition of DeletePortCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "DeletePortCmd.hh"
#include "Port.hh"
#include "EditPart.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Port to delete.
 */
DeletePortCmd::DeletePortCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {
}


/**
 * The Destructor.
 */
DeletePortCmd::~DeletePortCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
DeletePortCmd::Do() {
    Port* port = dynamic_cast<Port*>(editPart_->model());
    assert (port != NULL);
    delete port;
    port = NULL;
    return true;
}
