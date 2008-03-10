/**
 * @file DeleteFUCmd.cc
 *
 * Definition of DeleteFUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "DeleteFUCmd.hh"
#include "FunctionUnit.hh"
#include "EditPart.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Function unit to delete.
 */
DeleteFUCmd::DeleteFUCmd(EditPart* editPart):
    ComponentCommand(),
    editPart_(editPart) {
}


/**
 * The Destructor.
 */
DeleteFUCmd::~DeleteFUCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
DeleteFUCmd::Do() {
    FunctionUnit* fu = dynamic_cast<FunctionUnit*>(editPart_->model());
    assert (fu != NULL);
    delete fu;
    return true;
}
