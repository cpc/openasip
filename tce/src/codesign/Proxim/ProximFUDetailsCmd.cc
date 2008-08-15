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
