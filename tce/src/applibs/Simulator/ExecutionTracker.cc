/**
 * @file ExecutionTracker.cc
 *
 * Definition of ExecutionTracker class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "ExecutionTracker.hh"
#include "ExecutionTrace.hh"
#include "Application.hh"
#include "TTASimulationController.hh"
#include "SimulatorToolbox.hh"
#include "SimulationEventHandler.hh"
#include "SimulatorFrontend.hh"

/**
 * Constructor.
 *
 * @param subject The SimulationController which is observed.
 * @param traceDB The Execution Trace Database instance in which the
 *                trace is stored. Expects that the database is open
 *                for writing.
 */
ExecutionTracker::ExecutionTracker(
    TTASimulationController& subject,
    ExecutionTrace& traceDB) : Listener(),
    subject_(subject), traceDB_(traceDB) {
    subject.frontend().eventHandler().registerListener(
        SimulationEventHandler::SE_CYCLE_END, this);
}

/**
 * Destructor.
 */
ExecutionTracker::~ExecutionTracker() {
    subject_.frontend().eventHandler().unregisterListener(
        SimulationEventHandler::SE_CYCLE_END, this);
}

/**
 * Stores instruction execution data in the trace database.
 *
 * If any error happens while writing the data, aborts program with
 * an error message.
 */
void 
ExecutionTracker::handleEvent() {
    try {
        traceDB_.addInstructionExecution(
            subject_.clockCount(), subject_.lastExecutedInstruction());
    } catch (const Exception& e) {
        debugLog("Error while writing TraceDB: " + e.errorMessage());
    }
}
