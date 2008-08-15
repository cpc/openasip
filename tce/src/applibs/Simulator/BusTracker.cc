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
 * @file BusTracker.cc
 *
 * Definition of BusTracker class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
