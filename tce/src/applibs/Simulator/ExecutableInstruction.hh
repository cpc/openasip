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
