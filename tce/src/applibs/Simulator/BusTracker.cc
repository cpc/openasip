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
