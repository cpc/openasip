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
