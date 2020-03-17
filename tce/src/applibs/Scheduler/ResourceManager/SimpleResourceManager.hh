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
 * @file SimpleResourceManager.hh
 *
 * Declaration of SimpleResourceManager class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_RESOURCE_MANAGER_HH
#define TTA_SIMPLE_RESOURCE_MANAGER_HH

#include <list>
#include <map>
#include <memory>

#include "ResourceManager.hh"
#include "AssignmentPlan.hh"
#include "ResourceBuildDirector.hh"

class DataDependenceGraph;
class SimpleBrokerDirector;
namespace TTAProgram {
    class Instruction;
    class Immediate;
    class TerminalImmediate;
}

/**
 * A simple resource manager.
 */
class SimpleResourceManager : public ResourceManager {
public:
    static SimpleResourceManager* createRM(
        const TTAMachine::Machine& machine, unsigned int ii = 0);
    static void disposeRM(SimpleResourceManager* rm, bool allowReuse = true);

    virtual bool canAssign(
        int cycle, MoveNode& node,
        const TTAMachine::Bus* bus = NULL,
        const TTAMachine::FunctionUnit* srcFU = NULL,
        const TTAMachine::FunctionUnit* dstFU = NULL,
        int immWriteCycle = -1,
        const TTAMachine::ImmediateUnit* immu = nullptr,
        int immRegIndex = -1) const override;

    virtual bool canTransportImmediate(
        const MoveNode& node,
        const TTAMachine::Bus* preAssignedBus = NULL) const;
    virtual void assign(
        int cycle, MoveNode& node,
        const TTAMachine::Bus* bus = NULL,
        const TTAMachine::FunctionUnit* srcFU = NULL,
        const TTAMachine::FunctionUnit* dstFU = NULL,
        int immWriteCycle = -1,
	const TTAMachine::ImmediateUnit* immu = nullptr,
        int immRegIndex = -1) override;

    virtual void unassign(MoveNode& node) override;

    virtual int earliestCycle(MoveNode& node,
                              const TTAMachine::Bus* bus = NULL,
                              const TTAMachine::FunctionUnit* srcFU = NULL,
                              const TTAMachine::FunctionUnit* dstFU = NULL,
                              int immWriteCycle = -1,
                              const TTAMachine::ImmediateUnit* immu = nullptr,
                              int immRegIndex = -1) const override;

    virtual int earliestCycle(int cycle, MoveNode& node,
                              const TTAMachine::Bus* bus = NULL,
                              const TTAMachine::FunctionUnit* srcFU = NULL,
                              const TTAMachine::FunctionUnit* dstFU = NULL,
                              int immWriteCycle = -1,
                              const TTAMachine::ImmediateUnit* immu = nullptr,
                              int immRegIndex = -1) const override;

    virtual int latestCycle(MoveNode& node,
                            const TTAMachine::Bus* bus = NULL,
                            const TTAMachine::FunctionUnit* srcFU = NULL,
                            const TTAMachine::FunctionUnit* dstFU = NULL,
                            int immWriteCycle = -1,
                            const TTAMachine::ImmediateUnit* immu = nullptr,
                            int immRegIndex = -1) const override;

    virtual int latestCycle(int cycle, MoveNode& node,
                            const TTAMachine::Bus* bus = NULL,
                            const TTAMachine::FunctionUnit* srcFU = NULL,
                            const TTAMachine::FunctionUnit* dstFU = NULL,
                            int immWriteCycle = -1,
                            const TTAMachine::ImmediateUnit* immu = nullptr,
                            int immRegIndex = -1) const override;

    virtual bool hasGuard(const MoveNode& node) const;
    virtual TTAProgram::Instruction* instruction(int cycle) const override;
    virtual bool supportsExternalAssignments() const override;
    virtual int largestCycle() const override;
    virtual int smallestCycle() const override;
    virtual void loseInstructionOwnership(int cycle);
    virtual std::shared_ptr<TTAProgram::TerminalImmediate>
    immediateValue(const MoveNode&);
    virtual int immediateWriteCycle(const MoveNode&) const;
    virtual bool isTemplateAvailable(
        int, std::shared_ptr<TTAProgram::Immediate>) const;

    virtual unsigned int resourceCount() const;
    virtual void print(std::ostream& target) const;
    virtual std::string toString() const;
    
    virtual unsigned initiationInterval() const {return initiationInterval_;}
    virtual void clearOldResources();
    void setDDG(const DataDependenceGraph* ddg);
    void setMaxCycle(unsigned int maxCycle);

    unsigned int instructionIndex(unsigned int) const;
private:
    SimpleResourceManager(
        const TTAMachine::Machine& machine, unsigned int ii = 0);

    virtual ~SimpleResourceManager();
    /// Clears all bookkeeping done by this RM. 
    /// The RM can then be reused for different BB.
    void clear();

    void buildResourceModel(const TTAMachine::Machine& machine);

    /// Resource manager's broker director.
    SimpleBrokerDirector* director_;
    /// Resource assignment plan.
    AssignmentPlan plan_;
    /// Resource build director.
    ResourceBuildDirector buildDirector_;

    unsigned int initiationInterval_;

    unsigned int resources;
    
    
    static std::map<const TTAMachine::Machine*, 
                    std::map<int, std::list< SimpleResourceManager*> > >
    rmPool_;
};

#endif
