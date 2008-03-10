/**
 * @file ProximFUDetailsCmd.cc
 *
 * Implementation of ProximFUDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximFUDetailsCmd.hh"
#include "ProximMachineStateWindow.hh"
#include "ProximToolbox.hh"
#include "FunctionUnit.hh"
#include "UtilizationStats.hh"
#include "TracedSimulatorFrontend.hh"
#include "HWOperation.hh"
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
 * @param fu Function unit to display.
 */
ProximFUDetailsCmd::ProximFUDetailsCmd(const TTAMachine::FunctionUnit& fu) :
    ComponentCommand(), fu_(fu) {
}


/**
 * The Destructor.
 */
ProximFUDetailsCmd::~ProximFUDetailsCmd() {
}

/**
 * Executes the command.
 */
bool
ProximFUDetailsCmd::Do() {

    string fuName = fu_.name();

    ProximMachineStateWindow* machineWin = ProximToolbox::machineStateWindow();

    const UtilizationStats& stats =
        ProximToolbox::frontend()->utilizationStatistics();

    MachineState& machineState = ProximToolbox::frontend()->machineState();

    // Append component type and name to the details widget.
    machineWin->appendDetails(ProximConstants::MACH_WIN_FU_LABEL);
    machineWin->appendDetails(fuName + "\n\n");

    // Append ports to the details widget.
    machineWin->appendDetails(ProximConstants::MACH_WIN_PORTS_TITLE + "\n");
    for (int i = 0; i < fu_.portCount(); i++) {
        string portName = fu_.port(i)->name();
        const PortState& portState = machineState.portState(portName, fuName);
        machineWin->appendDetails("  " + portName + ":\t");
        machineWin->appendDetails(
            Conversion::toString(portState.value().uIntWordValue()) + "\n");
    }

    // Append trigger count to the utlization widget.
    machineWin->appendUtilizationData(
        ProximConstants::MACH_WIN_TRIGGER_COUNT_LABEL);
    double triggerCount = stats.triggerCount(fuName);
    machineWin->appendUtilizationData(Conversion::toString(triggerCount));
    machineWin->appendUtilizationData("\n\n");

    // Append operation usage to the utilization widget.
    machineWin->appendUtilizationData(
        ProximConstants::MACH_WIN_EXECUTION_COUNT_LABEL);
    machineWin->appendUtilizationData("\n");

    for (int i = 0; i < fu_.operationCount(); i++) {
        string opName = StringTools::stringToUpper(fu_.operation(i)->name());
        ClockCycleCount execCount = stats.operationExecutions(fuName, opName);
        double percent = 0;
        if (triggerCount > 0) {
            percent = 100 * (execCount / triggerCount);
        }
        machineWin->appendUtilizationData(
            "  " + opName + ": " +
            Conversion::toString(execCount) + "  (" +
            Conversion::toString(percent) + "%)\n");
    }

    return true;
}
