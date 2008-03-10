/**
 * @file DeleteIUCmd.cc
 *
 * Definition of DeleteIUCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "DeleteIUCmd.hh"
#include "ImmediateUnit.hh"
#include "EditPart.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Function unit to delete.
 */
DeleteIUCmd::DeleteIUCmd(EditPart* editPart):
    ComponentCommand(), editPart_(editPart) {
}


/**
 * The Destructor.
 */
DeleteIUCmd::~DeleteIUCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
DeleteIUCmd::Do() {
    ImmediateUnit* iu = dynamic_cast<ImmediateUnit*>(editPart_->model());
    assert (iu != NULL);
    delete iu;
    return true;
}
