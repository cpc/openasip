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
 * @file BusTracker.cc
 *
 * Definition of BusTracker class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "BusTracker.hh"
#include "Application.hh"
#include "SimulationController.hh"
#include "SimulatorToolbox.hh"
#include "SimulationEventHandler.hh"
#include "SimulatorFrontend.hh"
#include "MachineState.hh"
#include "Machine.hh"
#include "MathTools.hh"
#include "Conversion.hh"

#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

const int BusTracker::COLUMN_WIDTH = 12;
const std::string BusTracker::COLUMN_SEPARATOR = " | ";

/**
 * Constructor.
 *
 * @param frontend The SimulationFrontend which is used to access simulation 
 *                 data.
 * @param traceStream Output stream where the trace data is written to.
 */
BusTracker::BusTracker(
    SimulatorFrontend& frontend, 
    std::ostream& traceStream) :
    Listener(), frontend_(frontend), traceStream_(traceStream) {
    // write the trace data at the end of simulation clock cycle
    frontend.eventHandler().registerListener(
        SimulationEventHandler::SE_CYCLE_END, this);
}

/**
 * Destructor.
 */
BusTracker::~BusTracker() {
    frontend_.eventHandler().unregisterListener(
        SimulationEventHandler::SE_CYCLE_END, this);
}

/**
 * Writes bus trace data to the trace stream.
 *
 * If any error happens while writing the data, aborts program with
 * an error message.
 */
void 
BusTracker::handleEvent() {

    TTAMachine::Machine::BusNavigator navigator = 
        frontend_.machine().busNavigator();

    ClockCycleCount currentCycle = frontend_.cycleCount();
    traceStream_ << std::right << std::setw(COLUMN_WIDTH) << currentCycle
                 << COLUMN_SEPARATOR;

    for (int i = 0; i < navigator.count(); ++i) {

        const std::string busName = navigator.item(i)->name();
        //int width = navigator.item(i)->width();

        BusState& bus = frontend_.machineState().busState(busName);

        if (!bus.isSquashed()) {
            // automatically extended to int width
            //int value = MathTools::signExtendTo(bus.value().intValue(), width);
            int value = bus.value().intValue(); 

            traceStream_ << std::setw(COLUMN_WIDTH)
                         << Conversion::toString(value)
                         << COLUMN_SEPARATOR;
        } else {
            // Squashed values are displayed as zeros.
            traceStream_ << std::setw(COLUMN_WIDTH) << "0" << COLUMN_SEPARATOR;
        }
    }

    traceStream_ << std::endl;
}
