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
 * @file ExecutableInstruction.hh
 *
 * Declaration of ExecutableInstruction class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXECUTABLE_INSTRUCTION_HH
#define TTA_EXECUTABLE_INSTRUCTION_HH

#include <vector>
#include "SimulatorConstants.hh"

class ExecutableMove;
class LongImmUpdateAction;

/**
 * Represents an interpreted TTA instruction.
 *
 * This class is used to simulate the data transports performed by a TTA
 * instruction.
 */
class ExecutableInstruction {
public:
    ExecutableInstruction();
    virtual ~ExecutableInstruction();

    void addExecutableMove(ExecutableMove* move);
    void addLongImmediateUpdateAction(LongImmUpdateAction* action);

    void execute();

    ClockCycleCount executionCount() const;
    ClockCycleCount moveExecutionCount(std::size_t moveIndex) const;
    void resetExecutionCounts();

    bool moveSquashed(std::size_t moveIndex) const;

    void setExitPoint(bool b);
    bool isExitPoint() const;
    
private:
    /// Copying not allowed.
    ExecutableInstruction(const ExecutableInstruction&);
    /// Assignment not allowed.
    ExecutableInstruction& operator=(const ExecutableInstruction&);
    /// Contains ExecutableMoves.
    typedef std::vector<ExecutableMove*> MoveContainer;
    /// Contains long immediate update actions.
    typedef std::vector<LongImmUpdateAction*> UpdateContainer;
    /// All moves of the instruction.
    MoveContainer moves_;
    /// All long immediate update actions.
    UpdateContainer updateActions_;
    /// The count of times this instruction has been executed.
    ClockCycleCount executionCount_;
    /// True in case the instruction is considered a program exit point.
    bool exitPoint_;
};

#include "ExecutableInstruction.icc"

#endif
