/**
 * @file ProximIUDetailsCmd.cc
 *
 * Implementation of ProximIUDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximIUDetailsCmd.hh"
#include "ProximMachineStateWindow.hh"
#include "ProximToolbox.hh"
#include "ImmediateUnit.hh"
#include "TracedSimulatorFrontend.hh"
#include "Conversion.hh"
#include "ProximConstants.hh"
#include "MachineState.hh"
#include "LongImmediateUnitState.hh"
#include "UtilizationStats.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param iu Immediate unit to display.
 */
ProximIUDetailsCmd::ProximIUDetailsCmd(const TTAMachine::ImmediateUnit& iu) :
    ComponentCommand(), iu_(iu) {
}


/**
 * The Destructor.
 */
ProximIUDetailsCmd::~ProximIUDetailsCmd() {
}

/**
 * Executes the command.
 */
bool
ProximIUDetailsCmd::Do() {

    string iuName = iu_.name();

    ProximMachineStateWindow* machineWin = ProximToolbox::machineStateWindow();
    MachineState& machineState = ProximToolbox::frontend()->machineState();
    
    const UtilizationStats& stats =
        ProximToolbox::frontend()->utilizationStatistics();

    // Append component type and name to the details widget.
    machineWin->appendDetails(ProximConstants::MACH_WIN_IU_LABEL);
    machineWin->appendDetails(iuName + "\n\n");

    // Append registers to the details widget.
    machineWin->appendDetails(
        ProximConstants::MACH_WIN_IMMEDIATES_TITLE + "\n");
    LongImmediateUnitState& iuState =
        machineState.longImmediateUnitState(iuName);

    for (int i = 0; i < iuState.immediateRegisterCount(); i++) {
        machineWin->appendDetails("  " + Conversion::toString(i) + ":\t");
        int immValue = iuState.registerValue(i).sIntWordValue();
        machineWin->appendDetails(Conversion::toString(immValue) + "\n");
    }
    
    // Append register access statistics to the utilization widget.
    machineWin->appendUtilizationData("Register access statistics:\n\n");

    int usedRegCount = 0;
    for (int i = 0; i < iuState.immediateRegisterCount(); i++) {
        ClockCycleCount reads = stats.registerReads(iuName, i);
        ClockCycleCount writes = stats.registerWrites(iuName, i);
        if (reads > 0 || writes > 0) {
            usedRegCount++;
        }
        machineWin->appendUtilizationData(
            Conversion::toString(i) +  + ":  " +
            Conversion::toString(reads) + " reads, " +
            Conversion::toString(writes) + " writes.\n");
    }
    
    machineWin->appendUtilizationData(
        "\nTotal of " + Conversion::toString(usedRegCount) +
        " registers used.");

    return true;
}
