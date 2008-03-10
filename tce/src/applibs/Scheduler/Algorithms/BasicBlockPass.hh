/**
 * @file BasicBlockPass.hh
 *
 * Declaration of BasicBlockPass interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BB_PASS_HH
#define TTA_BB_PASS_HH

#include "NullMachine.hh"
#include "Exception.hh"
#include "SchedulerPass.hh"

class DDGPass;
class DataDependenceGraph;
class SimpleResourceManager;

class BasicBlock;
namespace TTAMachine {
    class Machine;
}

/**
 * Interface for scheduler passes that handle basic blocks.
 */
class BasicBlockPass : public SchedulerPass {
public:
    BasicBlockPass(InterPassData& data);
    virtual ~BasicBlockPass();

    virtual void handleBasicBlock(
        BasicBlock& basicBlock,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    void executeDDGPass(
        BasicBlock& bb,
        const TTAMachine::Machine& targetMachine, 
        DDGPass& ddgPass)
        throw (Exception);
protected:
    virtual DataDependenceGraph* createDDGFromBB(BasicBlock& bb);
    virtual void deleteRM(SimpleResourceManager* rm, BasicBlock& bb);
};
#endif
