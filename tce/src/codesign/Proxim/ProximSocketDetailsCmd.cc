/**
 * @file ProximSocketDetailsCmd.cc
 *
 * Implementation of ProximSocketDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximSocketDetailsCmd.hh"
#include "ProximMachineStateWindow.hh"
#include "ProximToolbox.hh"
#include "Socket.hh"
#include "TracedSimulatorFrontend.hh"
#include "Conversion.hh"
#include "ProximConstants.hh"
#include "UtilizationStats.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param bus Socket to display.
 */
ProximSocketDetailsCmd::ProximSocketDetailsCmd(
    const TTAMachine::Socket& socket) :
    ComponentCommand(), socket_(socket) {
}


/**
 * The Destructor.
 */
ProximSocketDetailsCmd::~ProximSocketDetailsCmd() {
}

/**
 * Executes the command.
 */
bool
ProximSocketDetailsCmd::Do() {

    string socketName = socket_.name();

    ProximMachineStateWindow* machineWin = ProximToolbox::machineStateWindow();
    const UtilizationStats& stats =
        ProximToolbox::frontend()->utilizationStatistics();

    // Append component type and name to the details widget.
    machineWin->appendDetails(ProximConstants::MACH_WIN_SOCKET_LABEL);
    machineWin->appendDetails(socketName + "\n\n");

    // Append utlization statistics.
    ClockCycleCount writes = stats.socketWrites(socketName);
    machineWin->appendUtilizationData(
        ProximConstants::MACH_WIN_SOCKET_WRITES_LABEL);
    machineWin->appendUtilizationData(Conversion::toString(writes));

    ClockCycleCount cycles = ProximToolbox::frontend()->cycleCount();
    double percent = 0;
    if (cycles > 0) {
        percent = (100.0 * writes) / cycles;
    }
    machineWin->appendUtilizationData(
        " (" + Conversion::toString(percent) + "%)");

    return true;
}
