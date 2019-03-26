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
 * @file ProximFUPortDetailsCmd.cc
 *
 * Implementation of ProximFUPortDetailsCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "ProximFUPortDetailsCmd.hh"
#include "ProximMachineStateWindow.hh"
#include "ProximToolbox.hh"
#include "FUPort.hh"
#include "UtilizationStats.hh"
#include "TracedSimulatorFrontend.hh"
#include "Conversion.hh"
#include "StringTools.hh"
#include "ProximConstants.hh"
#include "MachineState.hh"
#include "PortState.hh"
#include "FunctionUnit.hh"

using std::string;
using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param fu Function unit port to display.
 */
ProximFUPortDetailsCmd::ProximFUPortDetailsCmd(
    const TTAMachine::FUPort& port) :
    ComponentCommand(), port_(port) {
}


/**
 * The Destructor.
 */
ProximFUPortDetailsCmd::~ProximFUPortDetailsCmd() {
}


/**
 * Executes the command.
 */
bool
ProximFUPortDetailsCmd::Do() {

    string fuName = port_.parentUnit()->name();
    string portName = port_.name();

    ProximMachineStateWindow* machineWin = ProximToolbox::machineStateWindow();

    MachineState& machineState = ProximToolbox::frontend()->machineState();

    // Append component type and name to the details widget.
    machineWin->appendDetails(ProximConstants::MACH_WIN_FU_PORT_LABEL);
    machineWin->appendDetails("\n" + fuName + "." + portName + "\n\n");

    const PortState& portState = machineState.portState(portName, fuName);
    machineWin->appendDetails(
        Conversion::toHexString(portState.value().unsignedValue()) + "\n");

    return true;
}
