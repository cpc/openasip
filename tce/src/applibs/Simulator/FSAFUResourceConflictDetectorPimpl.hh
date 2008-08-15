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
 * @file FSAFUResourceConflictDetectorPimpl.hh
 *
 * Declaration of FSAFUResourceConflictDetectorPimpl (private implementation)
 * class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef FSA_FU_RESOURCE_CONFLICT_DETECTOR_PIMPL_HH
#define FSA_FU_RESOURCE_CONFLICT_DETECTOR_PIMPL_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "FunctionUnit.hh"
#include "FUResourceConflictDetector.hh"
#include "FUFiniteStateAutomaton.hh"
#include "FSAFUResourceConflictDetector.hh"


class FSAFUResourceConflictDetectorPimpl {
public:
    friend class FSAFUResourceConflictDetector;
    ~FSAFUResourceConflictDetectorPimpl();
private:
    FSAFUResourceConflictDetectorPimpl(const TTAMachine::FunctionUnit& fu);
    /// Copying not allowed.
    FSAFUResourceConflictDetectorPimpl(
        const FSAFUResourceConflictDetectorPimpl&);
    /// Assignment not allowed.
    FSAFUResourceConflictDetectorPimpl& operator=(
        const FSAFUResourceConflictDetectorPimpl&);
    
    /// The FSA.
    FUFiniteStateAutomaton fsa_;
    /// Current state of the FSA.
    FiniteStateAutomaton::FSAStateIndex currentState_;
    /// The next state of the FSA (move to currentState in cycle advance).
    FiniteStateAutomaton::FSAStateIndex nextState_;
    /// True if operation was issued at the current cycle.
    bool operationIssued_;
    /// The transition index of a NOP operation.
    const FiniteStateAutomaton::FSAStateTransitionIndex NOP;
    /// The name of the FU that is being simulated (for debugging).
    const std::string fuName_;
};

#endif
