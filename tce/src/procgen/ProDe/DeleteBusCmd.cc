/**
 * @file DeleteBusCmd.cc
 *
 * Definition of DeleteBusCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "DeleteBusCmd.hh"
#include "Bus.hh"
#include "EditPart.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param editPart Bus to delete.
 */
DeleteBusCmd::DeleteBusCmd(EditPart* editPart):
    ComponentCommand(), editPart_(editPart) {
}


/**
 * The Destructor.
 */
DeleteBusCmd::~DeleteBusCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
DeleteBusCmd::Do() {
    Bus* bus = dynamic_cast<Bus*>(editPart_->model());
    assert (bus != NULL);
    delete bus;
    return true;
}
