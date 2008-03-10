/**
 *
 * @file DDGPass
 *
 * Definition of a DDGPass interface.
 * 
 * @author Heikki Kultala 2007 (hkultala@cs.tut.fi)
 * @note rating:red
 */

#ifndef TTA_DDG_PASS_HH
#define TTA_DDG_PASS_HH

#include "SchedulerPass.hh"
#include "Exception.hh"

class DataDependenceGraph;
class BasicBlock;
class SimpleResourceManager;

namespace TTAMachine {
    class Machine;
}

/*
 * Interface for a scheduler passes that handle data dependence graphs.
 */
class DDGPass : public SchedulerPass {
public:
    DDGPass(InterPassData& data);
    virtual ~DDGPass();
    virtual void handleDDG(
        DataDependenceGraph& ddg,
        SimpleResourceManager& rm,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);
};

#endif
