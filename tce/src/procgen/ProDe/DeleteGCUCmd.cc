/**
 * @file DeleteGCUCmd.cc
 *
 * Definition of DeleteGCUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "DeleteGCUCmd.hh"
#include "ControlUnit.hh"
#include "EditPart.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Global control unit to delete.
 */
DeleteGCUCmd::DeleteGCUCmd(EditPart* editPart):
    ComponentCommand(), editPart_(editPart) {
}


/**
 * The Destructor.
 */
DeleteGCUCmd::~DeleteGCUCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
DeleteGCUCmd::Do() {
    ControlUnit* gcu = dynamic_cast<ControlUnit*>(editPart_->model());
    assert (gcu != NULL);
    delete gcu;
    return true;
}
