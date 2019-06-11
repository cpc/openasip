/*
    Copyright (c) 2002-2009 Tampere University.

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
