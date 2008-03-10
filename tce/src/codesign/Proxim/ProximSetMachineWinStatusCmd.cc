/**
 * @file ProximSetMachineWinStatusCmd.cc
 *
 * Implementation of ProximSetMachineWinStatusCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ProximSetMachineWinStatusCmd.hh"
#include "EditPart.hh"
#include "ProximMainFrame.hh"
#include "ProximRegisterWindow.hh"
#include "MachinePart.hh"
#include "Application.hh"

/**
 * The Constructor.
 */
ProximSetMachineWinStatusCmd::ProximSetMachineWinStatusCmd(
    EditPart* part) :
    ComponentCommand(),
    part_(part) {

}


/**
 * The Destructor.
 */
ProximSetMachineWinStatusCmd::~ProximSetMachineWinStatusCmd() {
}


/**
 * Executes the command.
 *
 * @return True, if the command was succesfully executed, false otherwise.
 *
 */
bool
ProximSetMachineWinStatusCmd::Do() {

    ProximMachineStateWindow* machineWindow =
        ProximToolbox::machineStateWindow();

    if (machineWindow == NULL) {
        // Machine Window does not exist.
        return false;
    }

    TTAMachine::Bus* bus = dynamic_cast<Bus*>(part_->model());

    assert(bus != NULL);

    const Program& program = ProximToolbox::program();

    Instruction
}
