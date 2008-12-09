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
 * @file ExecutionTracker.cc
 *
 * Definition of ExecutionTracker class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
