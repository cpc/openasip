/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
