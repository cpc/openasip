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
 * @file ProximRFDetailsCmd.cc
 *
 * Implementation of ProximRFDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
