/**
 * @file ProximFUPortDetailsCmd.cc
 *
 * Implementation of ProximFUPortDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximFUPortDetailsCmd.hh"
#include "ProximMachineStateWindow.hh"
#include "ProximToolbox.hh"
#include "FUPort.hh"
#include "UtilizationStats.hh"
#include "TracedSimulatorFrontend.hh"
#include "Conversion.hh"
#include "StringTools.hh"
#include "ProximConstants.hh"
#include "MachineState.hh"
#include "PortState.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param fu Function unit port to display.
 */
ProximFUPortDetailsCmd::ProximFUPortDetailsCmd(
    const TTAMachine::FUPort& port) :
    ComponentCommand(), port_(port) {
}


/**
 * The Destructor.
 */
ProximFUPortDetailsCmd::~ProximFUPortDetailsCmd() {
}


/**
 * Executes the command.
 */
bool
ProximFUPortDetailsCmd::Do() {

    string fuName = port_.parentUnit()->name();
    string portName = port_.name();

    ProximMachineStateWindow* machineWin = ProximToolbox::machineStateWindow();

    MachineState& machineState = ProximToolbox::frontend()->machineState();

    // Append component type and name to the details widget.
    machineWin->appendDetails(ProximConstants::MACH_WIN_FU_PORT_LABEL);
    machineWin->appendDetails("\n" + fuName + "." + portName + "\n\n");

    const PortState& portState = machineState.portState(portName, fuName);
    machineWin->appendDetails(
        Conversion::toHexString(portState.value().unsignedValue()) + "\n");

    return true;
}
