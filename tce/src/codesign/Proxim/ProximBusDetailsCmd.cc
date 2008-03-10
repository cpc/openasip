/**
 * @file ProximBusDetailsCmd.cc
 *
 * Implementation of ProximBusDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximBusDetailsCmd.hh"
#include "ProximMachineStateWindow.hh"
#include "ProximToolbox.hh"
#include "Bus.hh"
#include "TracedSimulatorFrontend.hh"
#include "Conversion.hh"
#include "ProximConstants.hh"
#include "MachineState.hh"
#include "BusState.hh"
#include "UtilizationStats.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param bus Bus to display.
 */
ProximBusDetailsCmd::ProximBusDetailsCmd(const TTAMachine::Bus& bus) :
    ComponentCommand(), bus_(bus) {
}


/**
 * The Destructor.
 */
ProximBusDetailsCmd::~ProximBusDetailsCmd() {
}

/**
 * Executes the command.
 */
bool
ProximBusDetailsCmd::Do() {

    string busName = bus_.name();

    ProximMachineStateWindow* machineWin = ProximToolbox::machineStateWindow();
    MachineState& machineState = ProximToolbox::frontend()->machineState();
    const UtilizationStats& stats =
        ProximToolbox::frontend()->utilizationStatistics();

    // Append component type and name to the details widget.
    machineWin->appendDetails(ProximConstants::MACH_WIN_BUS_LABEL);
    machineWin->appendDetails(busName + "\n\n");

    // Append registers to the details widget.
    const BusState& busState = machineState.busState(busName);
    int busValue = busState.value().sIntWordValue();
    machineWin->appendDetails(ProximConstants::MACH_WIN_BUS_VALUE_LABEL);
    machineWin->appendDetails(Conversion::toString(busValue) + "\n");

    // Append utlization statistics.
    ClockCycleCount writes = stats.busWrites(busName);
    machineWin->appendUtilizationData(
        ProximConstants::MACH_WIN_BUS_WRITES_LABEL);
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
