/**
 * @file ProximComponentStatusCmd.cc
 *
 * Implementation of ProximComponentStatusCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximComponentStatusCmd.hh"
#include "ProximToolbox.hh"
#include "ProximMachineStateWindow.hh"


/**
 * The Constructor.
 *
 * @param part Machine component to be displayed in the status widgets of the
 *             MachineWindow.
 */
ProximComponentStatusCmd::ProximComponentStatusCmd(
    std::string status) :
    ComponentCommand(),
    status_(status) {

}


/**
 * The Destructor.
 */
ProximComponentStatusCmd::~ProximComponentStatusCmd() {
}


/**
 * Executes the command.
 */
bool
ProximComponentStatusCmd::Do() {

    ProximMachineStateWindow* machineWindow =
        ProximToolbox::machineStateWindow();

    assert(machineWindow != NULL);
    
    machineWindow->setStatusText(status_);
    return true;
}
