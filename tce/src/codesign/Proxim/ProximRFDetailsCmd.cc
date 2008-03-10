/**
 * @file ProximRFDetailsCmd.cc
 *
 * Implementation of ProximRFDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximRFDetailsCmd.hh"
#include "ProximMachineStateWindow.hh"
#include "ProximToolbox.hh"
#include "RegisterFile.hh"
#include "TracedSimulatorFrontend.hh"
#include "Conversion.hh"
#include "ProximConstants.hh"
#include "MachineState.hh"
#include "PortState.hh"
#include "RegisterFileState.hh"
#include "UtilizationStats.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param rf Register file to display.
 */
ProximRFDetailsCmd::ProximRFDetailsCmd(const TTAMachine::RegisterFile& rf) :
    ComponentCommand(), rf_(rf) {
}


/**
 * The Destructor.
 */
ProximRFDetailsCmd::~ProximRFDetailsCmd() {
}

/**
 * Executes the command.
 */
bool
ProximRFDetailsCmd::Do() {

    string rfName = rf_.name();

    ProximMachineStateWindow* machineWin = ProximToolbox::machineStateWindow();
    MachineState& machineState = ProximToolbox::frontend()->machineState();

    const UtilizationStats& stats =
        ProximToolbox::frontend()->utilizationStatistics();

    // Append component type and name to the details widget.
    machineWin->appendDetails(ProximConstants::MACH_WIN_RF_LABEL);
    machineWin->appendDetails(rfName + "\n\n");

    // Append registers to the details widget.
    machineWin->appendDetails(
        ProximConstants::MACH_WIN_REGISTERS_TITLE + "\n");
    RegisterFileState& rfState = machineState.registerFileState(rfName);
    for (unsigned i = 0; i < rfState.registerCount(); i++) {
        machineWin->appendDetails("  " + Conversion::toString(i) + ":\t");
        int registerValue = rfState.registerState(i).value().unsignedValue();
        machineWin->appendDetails(Conversion::toString(registerValue) + "\n");
    }

    // Append register access statistics to the utilization widget.
    machineWin->appendUtilizationData("Register access statistics:\n\n");

    int usedRegCount = 0;
    for (unsigned i = 0; i < rfState.registerCount(); i++) {
        ClockCycleCount readCount = stats.registerReads(rfName, i);
        ClockCycleCount writeCount = stats.registerWrites(rfName, i);
        if (readCount > 0 || writeCount > 0) {
            usedRegCount++;
        }
        machineWin->appendUtilizationData(
            Conversion::toString(i) +  + ":  " +
            Conversion::toString(readCount) + " reads, " +
            Conversion::toString(writeCount) + " writes.\n");
    }
    
    machineWin->appendUtilizationData(
        "\nTotal of " + Conversion::toString(usedRegCount) +
        " registers used.");

    return true;
}
