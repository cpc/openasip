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
 * @file FSAFUResourceConflictDetectorPimpl.hh
 *
 * Declaration of FSAFUResourceConflictDetectorPimpl (private implementation)
 * class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef FSA_FU_RESOURCE_CONFLICT_DETECTOR_PIMPL_HH
#define FSA_FU_RESOURCE_CONFLICT_DETECTOR_PIMPL_HH

#include <string>

#include "Exception.hh"
#include "FUResourceConflictDetector.hh"
#include "FUFiniteStateAutomaton.hh"
#include "FSAFUResourceConflictDetector.hh"

namespace TTAMachine {
    class FunctionUnit;
}

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
