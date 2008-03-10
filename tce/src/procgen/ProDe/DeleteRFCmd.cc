/**
 * @file DeleteRFCmd.cc
 *
 * Definition of DeleteRFCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#include "Application.hh"
#include "DeleteRFCmd.hh"
#include "RegisterFile.hh"
#include "EditPart.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Register file to delete.
 */
DeleteRFCmd::DeleteRFCmd(EditPart* editPart):
    ComponentCommand(), editPart_(editPart) {
}


/**
 * The Destructor.
 */
DeleteRFCmd::~DeleteRFCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
DeleteRFCmd::Do() {
    RegisterFile* rf = dynamic_cast<RegisterFile*>(editPart_->model());
    assert (rf != NULL);
    delete rf;
    return true;
}
