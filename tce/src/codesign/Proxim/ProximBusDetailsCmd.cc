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
 * @file ProximBusDetailsCmd.cc
 *
 * Implementation of ProximBusDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
