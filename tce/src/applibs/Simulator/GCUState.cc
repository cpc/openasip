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
 * @file GCUState.cc
 *
 * Definition of GCUState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "GCUState.hh"
#include "SequenceTools.hh"
#include "OperationContext.hh"
#include "GlobalLock.hh"
#include "Application.hh"

using std::string;

/**
 * Constructor.
 *
 * @param latency Latency of the unit.
 * @param nww Natural word width as MAUs.
 * @param lock Global lock signal.
 */
GCUState::GCUState(
    int latency, 
    int nww, 
    GlobalLock& lock) : 
    FUState(lock), naturalWordWidth_(nww), returnAddressRegister_(32), 
    programCounter_(0), newProgramCounter_(0), latency_(latency), 
    operationPending_(false), operationPendingTime_(0),
    operationContext_(
        NULL, programCounter_, returnAddressRegister_) {
    programCounter_ = 0;
    returnAddressRegister_ = 0;
}

/**
 * Destructor.
 */
GCUState::~GCUState() {
}

/**
 * Returns the operation context.
 *
 * This is basically a "template method" to allow differently initialized
 * OperationContext-classes in FUState subclasses.
 *
 * @return The operation context for the FU.
 */
OperationContext&
GCUState::context() {
    return operationContext_;
}

/**
 * Handles the actions when clock is advanced.
 *
 * The value of program counter is updated. Pipeline is advanced.
 */
void
GCUState::advanceClock() {

    if (operationPending_) {
        operationPendingTime_--;
    }
    if (operationPending_ && operationPendingTime_ <= 0) {
        programCounter_ = newProgramCounter_;
        operationPending_ = false;
    } 
    FUState::advanceClock();
    idle_ = !operationPending_;
}
