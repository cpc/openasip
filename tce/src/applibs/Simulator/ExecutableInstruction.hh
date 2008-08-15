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
 * @file ExecutableInstruction.hh
 *
 * Declaration of ExecutableInstruction class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
