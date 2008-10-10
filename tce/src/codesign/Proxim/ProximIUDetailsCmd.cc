/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ProximIUDetailsCmd.cc
 *
 * Implementation of ProximIUDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
