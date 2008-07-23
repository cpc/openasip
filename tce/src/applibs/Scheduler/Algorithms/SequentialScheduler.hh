/**
 * @file SequentialScheduler.hh
 *
 * Declaration of SequentialScheduler class.
 *
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEQUENTIAL_SCHEDULER_HH
#define TTA_SEQUENTIAL_SCHEDULER_HH

#include "Program.hh"
#include "MoveNode.hh"
#include "MoveNodeGroup.hh"
#include "SequentialMoveNodeSelector.hh"
#include "FunctionUnit.hh"
#include "BasicBlockPass.hh"
#include "ControlFlowGraphPass.hh"
#include "ProcedurePass.hh"
#include "ProgramPass.hh"
#include "RegisterCopyAdder.hh"

class BasicBlockNode;
class SimpleResourceManager;

/**
 * A class that implements the functionality of a basic block scheduler.
 *
 * Schedules the program one basic block at a time. Does not fill delay slots
 * if they couldn't be filled with the basic block's contents itself (no
 * instruction importing).
 */
class SequentialScheduler :
    public BasicBlockPass, public ControlFlowGraphPass, public ProcedurePass,
    public ProgramPass {
public:
    SequentialScheduler(
        InterPassData& data);

    virtual ~SequentialScheduler();

    virtual void handleBasicBlock(
        BasicBlock& bb, const TTAMachine::Machine& targetMachine)
        throw (Exception);

    virtual void handleControlFlowGraph(
        ControlFlowGraph& cfg,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    virtual void handleProcedure(
        TTAProgram::Procedure& procedure,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    virtual void handleProgram(
        TTAProgram::Program& program,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

private:
    int scheduleOperation(MoveNodeGroup& moves, int earliestCycle)
        throw (Exception);

    int scheduleOperandWrites(
        int cycle, MoveNodeGroup& moves,
        RegisterCopyAdder::AddedRegisterCopies& regCopies)
        throw (Exception);

    int scheduleResultReads(
        int triggerCycle, MoveNodeGroup& moves,
        RegisterCopyAdder::AddedRegisterCopies& regCopies)
        throw (Exception);

    int scheduleMove(int earliestCycle, MoveNode& move)
        throw (Exception);

    int scheduleInputOperandTempMoves(
        int cycle, MoveNode& operandMove, 
        RegisterCopyAdder::AddedRegisterCopies& regCopies)
        throw (Exception);

    int scheduleResultTempMoves(
        int cycle, MoveNode& resultMove, 
        RegisterCopyAdder::AddedRegisterCopies& regCopies)
        throw (Exception);
        

    void unschedule(MoveNode& moveNode);

    /// The target machine we are scheduling the program against.
    const TTAMachine::Machine* targetMachine_;
    /// Resource Manager of the currently scheduled BB.
    SimpleResourceManager* rm_;
    /// Stores the MoveNodes that were scheduled as temp moves during
    /// scheduling of the operand move.

};

#endif
