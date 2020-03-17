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
 * @file SimpleResourceManager.cc
 *
 * Implementation of SimpleResourceManager class.
 *
 * @author Ari Mets�alme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "SimpleResourceManager.hh"
#include "InputFUBroker.hh"
#include "OutputFUBroker.hh"
#include "ExecutionPipelineBroker.hh"
#include "InputPSocketBroker.hh"
#include "OutputPSocketBroker.hh"
#include "BusBroker.hh"
#include "IUBroker.hh"
#include "ITemplateBroker.hh"
#include "Instruction.hh"
#include "SequenceTools.hh"
#include "SimpleBrokerDirector.hh"
#include "ExecutionPipelineResourceTable.hh"

#include <sstream>

using namespace TTAProgram;

/**
 * Constructor.
 *
 * @param machine Target machine, ii Initiation interval.
 */
SimpleResourceManager::SimpleResourceManager(
    const TTAMachine::Machine& machine, unsigned int ii):
    ResourceManager(machine), director_(NULL),  initiationInterval_(ii) {

    buildResourceModel(machine);
}

/**
 * Factory method for creating resource managers.
 *
 * Checks a RM pool if a recyclable RM is found for same machine with same II,
 * if found gives that. If not found, creates a new RM.
 */
SimpleResourceManager* 
SimpleResourceManager::createRM(
    const TTAMachine::Machine& machine, unsigned int ii) {
    std::map<int, std::list< SimpleResourceManager*> >& pool =
        rmPool_[&machine];
    std::list<SimpleResourceManager*>& iipool = pool[ii];
    if (iipool.empty()) {
        return new SimpleResourceManager(machine,ii);
    } else {
        SimpleResourceManager* rm = iipool.back();
        iipool.pop_back();
        return rm;
    }
}

/*
 * Method which should be called when RM no longer needed. 
 *
 * This puts the RM into a pool of resource managers which can be recycled.
 */
void SimpleResourceManager::disposeRM(
    SimpleResourceManager* rm, bool allowReuse) {
    if (rm == NULL) return;
    if (allowReuse) {
        std::map<int, std::list< SimpleResourceManager*> >& pool =
            rmPool_[&rm->machine()];
        pool[rm->initiationInterval()].push_back(rm);
        rm->clear();
    } else {
        delete rm;
        ExecutionPipelineResourceTable::finalize();
    }
}


/**
 * Creates brokers and builds resource model.
 *
 */
void SimpleResourceManager::buildResourceModel(
    const TTAMachine::Machine& machine) {
    
    // instantiate brokers

    std::vector<ResourceBroker*> brokers;

    InputFUBroker* ifb = 
        new InputFUBroker("InputFUBroker", initiationInterval_);
    OutputFUBroker* ofb = 
        new OutputFUBroker("OutputFUBroker", initiationInterval_);
    brokers.push_back(ifb);
    brokers.push_back(ofb);
    brokers.push_back(new ExecutionPipelineBroker(
                          "ExecutionPipelineBroker", initiationInterval_));
    InputPSocketBroker* ipsb = new InputPSocketBroker(
        "InputPSocketBroker", *ifb, initiationInterval_);
    brokers.push_back(ipsb);
    brokers.push_back(new IUBroker("IUBroker", this, initiationInterval_));
    OutputPSocketBroker* opsb = new OutputPSocketBroker(
        "OutputPSocketBroker", *ofb, this, initiationInterval_);
    brokers.push_back(opsb);
    BusBroker* bb = new BusBroker(
        "BusBroker", *ipsb, *opsb, machine, 
        initiationInterval_);
    brokers.push_back(bb);
    brokers.push_back(
        new ITemplateBroker(
            "ITemplateBroker", *bb, this, initiationInterval_));

    ipsb->setBusBroker(*bb);
    opsb->setBusBroker(*bb);

    // build resource model and assignment plan

    for (unsigned int i = 0; i < brokers.size(); i++) {
        brokers[i]->setInitiationInterval(initiationInterval_);
        buildDirector_.addBroker(*brokers[i]);
        plan_.insertBroker(*brokers[i]);    
    }

    resources = brokers.size();
    
    buildDirector_.build(machine);

    director_ = new SimpleBrokerDirector(machine, plan_, initiationInterval_);
}

/**
 * Destructor.
 */
SimpleResourceManager::~SimpleResourceManager(){
    delete director_;
}

/**
 * Return true if given node can be assigned without resource
 * conflicts in given cycle.
 *
 * For those parts of the node that have
 * been already assigned to a resource, the manager simply keeps the
 * assignment and verifies that the resource is available. For those
 * parts that are not yet assigned, the resource manager looks for any
 * compatible resource that could be assigned.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return True if given node can be assigned without resource
 * conflicts in given cycle, false otherwise.
 */
bool
SimpleResourceManager::canAssign(int cycle, MoveNode& node,
                                 const TTAMachine::Bus* bus,
                                 const TTAMachine::FunctionUnit* srcFU,
                                 const TTAMachine::FunctionUnit* dstFU,
                                 int immWriteCycle,
                                 const TTAMachine::ImmediateUnit* immu,
                                 int immRegIndex) const {
#ifdef DEBUG_RM
    Application::logStream() << "\tCanAssign: " << cycle << " " << 
        node.toString() << std::endl;
#endif
    return director_->canAssign(
	cycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
}

/**
 * Assign all resources needed by a given node starting from given
 * cycle, and place the node in that cycle.
 *
 * The manager keeps any pre-existing assignment and verifies that
 * assigned resource is available. For all unassigned parts of the
 * node, the resource manager looks for and then assigns the necessary
 * resources. If the node is not fully assigned after the invocation
 * of this operation, it means the assignment attempt failed.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @exception InvalidData exception if given node is already placed in a
 * cycle different from given cycle.
 */
void
SimpleResourceManager::assign(int cycle, MoveNode& node,
                              const TTAMachine::Bus* bus,
                              const TTAMachine::FunctionUnit* srcFU,
                              const TTAMachine::FunctionUnit* dstFU,
                              int immWriteCycle,
                              const TTAMachine::ImmediateUnit* immu,
                              int immRegIndex) {
#ifdef DEBUG_RM
    Application::logStream() << "\tAssign: " << cycle << " " << 
        node.toString() << std::endl;
#endif
    director_->assign(
	cycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
#ifdef DEBUG_RM
    Application::logStream() << "\tAssign: " << cycle << " " << 
        node.toString() << " OK!" << std::endl;
#endif
}

/**
 * Free all resource assignments of the given node.
 *
 * If the node is only partially assigned, the resource manager
 * ignores those parts of the node that are already unassigned.
 *
 * @param node Node to unassign.
 * @exception InvalidData If the given node is not placed in any
 * cycle. Assigned but not placed nodes are not considered by the
 * resource manager.
 */
void
SimpleResourceManager::unassign(MoveNode& node)
{
#ifdef DEBUG_RM
    Application::logStream() << "\tUnAssign: " << node.toString() << std::endl;
#endif
    director_->unassign(node);
}

/**
 * Return the earliest cycle in the scope where all required resources
 * can be assigned to the given node.
 *
 * If the node is partially assigned, the manager keeps existing
 * assignments. This means that a client can apply arbitrary
 * constraints to resource allocation.
 *
 * @param node Node.
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return The earliest cycle in the scope where all required resources
 * can be assigned to the given node. -1 if assignment is not possible
 *
 */
int
SimpleResourceManager::earliestCycle(MoveNode& node,
                                     const TTAMachine::Bus* bus,
                                     const TTAMachine::FunctionUnit* srcFU,
                                     const TTAMachine::FunctionUnit* dstFU,
                                     int immWriteCycle,
                                     const TTAMachine::ImmediateUnit* immu,
                                     int immRegIndex) const {
    int ec =  director_->earliestCycle(
        node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
#ifdef DEBUG_RM
    Application::logStream() << "\tEC:" << node.toString() << std::endl;
    Application::logStream() << "\t\tEC result is: " << ec << std::endl;
#endif
    return ec;
}

/**
 * Return the earliest cycle starting from the given cycle in which
 * required resources can be assigned to given node.
 *
 * If the node is partially assigned, the manager keeps existing
 * assignments. This means that a client can apply arbitrary
 * constraints to resource allocation.
 *
 * @param cycle Cycle to start from.
 * @param node Node.
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return The earliest cycle starting from the given cycle in which
 * required resources can be assigned to given node. Returns -1 if assignment
 * is not possible.
 */
int
SimpleResourceManager::earliestCycle(int cycle, MoveNode& node,
                                     const TTAMachine::Bus* bus,
                                     const TTAMachine::FunctionUnit* srcFU,
                                     const TTAMachine::FunctionUnit* dstFU,
                                     int immWriteCycle,
                                     const TTAMachine::ImmediateUnit* immu,
                                     int immRegIndex) const {

    int ec = director_->earliestCycle(
        cycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
#ifdef DEBUG_RM
    Application::logStream() << "\tEC: " << cycle << " " << node.toString()
                             << std::endl;
    Application::logStream() << "\t\tEC result is: " << ec << std::endl;
#endif
    return ec;
}

/**
 * Return the latest cycle in the scope where all required resources
 * can be assigned to the given node.
 *
 * If the node is partially assigned, the manager keeps existing
 * assignments. This means that a client can apply arbitrary
 * constraints to resource allocation.
 *
 * @param node Node.
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return The latest cycle in the scope where all required resources
 * can be assigned to the given node. -1 if assignment is not possible.
 * INT_MAX if there is no upper boundary for assignment.
 */
int
SimpleResourceManager::latestCycle(MoveNode& node,
                                   const TTAMachine::Bus* bus,
                                   const TTAMachine::FunctionUnit* srcFU,
                                   const TTAMachine::FunctionUnit* dstFU,
                                   int immWriteCycle,
                                   const TTAMachine::ImmediateUnit* immu,
                                   int immRegIndex) const {
    int lc = director_->latestCycle(
        node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
#ifdef DEBUG_RM
    Application::logStream() << "\tLC: " << node.toString() << std::endl;
    Application::logStream() << "\t\tLC result is: " << lc << std::endl;                             
#endif
    return lc;
}

/**
 * Return the latest cycle starting from the given cycle in which
 * required resources can be assigned to given node.
 *
 * If the node is partially assigned, the manager keeps existing
 * assignments. This means that a client can apply arbitrary
 * constraints to resource allocation.
 *
 * @param cycle Cycle to start from.
 * @param node Node.
 * @param bus if not null, bus that has to be used.
 * @param srcFU if not null, srcFu that has to be used.
 * @param dstFU if not null, dstFU that has to be used.
 * @param immWriteCycle if not -1 and src is imm, write cycle of limm.
 * @return The latest cycle starting from the given cycle in which
 * required resources can be assigned to given node. -1 if assignment is not
 * possible.
 */
int
SimpleResourceManager::latestCycle(int cycle, MoveNode& node,
                                   const TTAMachine::Bus* bus,
                                   const TTAMachine::FunctionUnit* srcFU,
                                   const TTAMachine::FunctionUnit* dstFU,
                                   int immWriteCycle,
                                   const TTAMachine::ImmediateUnit* immu,
                                   int immRegIndex) const {
    int lc = director_->latestCycle(
        cycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
#ifdef DEBUG_RM
    Application::logStream() << "\tLC: " << cycle << " " << node.toString()
        << std::endl;
    Application::logStream() << "\t\tLC result is: " << lc << std::endl;                             
#endif
    return lc;
}

/**
 * Tests if immediate of MoveNode can be transported by any of
 * buses of present machine.
 *
 * @param node MoveNode that contains immediate that we test
 * @param bus if non-null, only check immediate from that bus
 * @return true if the immediate in node can be transported by
 * some bus
 */
bool
SimpleResourceManager::canTransportImmediate(
    const MoveNode& node, const TTAMachine::Bus* preassignedBus) const {
    return director_->canTransportImmediate(node, preassignedBus);
}

/**
 * Tests if any of a buses of machine supports guard needed
 * by a node. Should always return true! Otherwise, scheduler generated
 * code for different machine.
 *
 * @param node MoveNode to test
 * @return true if any of buses supports guard needed by node, or if move
 * is not conditional.
 */
bool
SimpleResourceManager::hasGuard(const MoveNode& node) const {
    return director_->hasGuard(node);
}

/**
 * Returns instruction that holds a moves for given cycle
 * and has correct template set.
 *
 * The instruction ownedship stays in the resource manager until
 * loseInstructionOwnership() is called.
 *
 * @param cycle Cycle for which to get instruction
 * @return the Instruction with moves and template assigned
 * @note This method should be called when whole scope is scheduled
 */
TTAProgram::Instruction*
SimpleResourceManager::instruction(int cycle) const {
    return director_->instruction(cycle);
}

/**
 * Defines if Resource Manager implementation supports node with resources
 * pre assigned by clients.
 *
 * @return For SimpleResourceManager allways false.
 */
bool
SimpleResourceManager::supportsExternalAssignments() const {
    return director_->supportsExternalAssignments();
}

/**
 * Returns largest cycle known to be used by any of the resources.
 *
 * @return Largest cycle resource manager assigned any resource to.
 */
int
SimpleResourceManager::largestCycle() const{
#ifdef DEBUG_RM
    Application::logStream() << "\tLC." << std::endl;
    int lc = director_->largestCycle();
    Application::logStream() << "\tLC got: " << lc << std::endl;
    return lc;
#else
    return director_->largestCycle();
#endif
}

/**
 * Returns smallest cycle known to be used by any of the resources.
 *
 * @return Smallest cycle resource manager assigned any resource to.
 */
int
SimpleResourceManager::smallestCycle() const{
#ifdef DEBUG_RM
    Application::logStream() << "\tSC." << std::endl;
    int sc = director_->smallestCycle();
    Application::logStream() << "\tSC got: " << sc << std::endl;
    return sc;
#else
    return director_->smallestCycle();
#endif
}

/**
 * Transfer the instruction ownership away from this object,
 *
 * If this method is called, resource manager does not delete it's
 * instructions when it it destroyed.
 */
void 
SimpleResourceManager::loseInstructionOwnership(int cycle) {
    director_->loseInstructionOwnership(cycle);
}

/**
 * Finds the original terminal with value of immediate.
 * 
 * @param node node with immediate register source
 * @return terminal with immediate value
 */
std::shared_ptr<TTAProgram::TerminalImmediate>
SimpleResourceManager::immediateValue(const MoveNode& node) {
    return director_->immediateValue(node);
}

/**
 * Finds cycle in which the immediate that is read by node is written.
 * 
 * @param node with source immediate register
 * @return cycle in which immediate is written to register
 */
int
SimpleResourceManager::immediateWriteCycle(const MoveNode& node) const {
    return director_->immediateWriteCycle(node);
}

/**
 * Return the instruction index corresponding to cycle.
 *
 * If modulo scheduling is not used (ie. initiation interval is 0), then
 * index is equal to cycle.
 *
 * @param cycle Cycle to get instruction index.
 * @return Return the instruction index for cycle.
 */
unsigned int
SimpleResourceManager::instructionIndex(unsigned int cycle) const {
    if (initiationInterval_ != 0) {
        return cycle % initiationInterval_;
    } else {
        return cycle;
    }
}

bool 
SimpleResourceManager::isTemplateAvailable(
    int defCycle, 
    std::shared_ptr<TTAProgram::Immediate> immediate) const {
    
    return director_->isTemplateAvailable(defCycle, immediate);
}

/**
 * Return the total number of resources.
 *
 * @return Return the total number of resources.
 */
unsigned int 
SimpleResourceManager::resourceCount() const {
    return resources;
}

/**
 * Print the contents of resource manager.
 *
 * @param stream target stream where to print
 */
void
SimpleResourceManager::print(std::ostream& target) const {
    if (initiationInterval_ == 0) {
        buildDirector_.print(target, 10);
    } else {
        buildDirector_.print(target, initiationInterval_);
    }

}

/**
 *
 * Print the contents of resource manager.
 *
 * @return Return string containing the resource table.
 */
std::string
SimpleResourceManager::toString() const {

    std::stringstream temp;

    if (initiationInterval_ == 0) {
        buildDirector_.print(temp, 10);
    } else {
        buildDirector_.print(temp, initiationInterval_);
    }

    std::string target = "";
    std::string result = "";

    while (getline(temp, target)) {
        // read temp to target
        target += "\n";
        result += target;
    };

    return result;
}

/**
 * Clears bookkeeping which is needed for unassigning previously assigned
 * moves. After this call these cannot be unassigned, but new moves which
 * are assigned after this call can still be unassigned.
 */
void 
SimpleResourceManager::clearOldResources() {
    director_->clearOldResources();
}

/**
 * Clears a resource manager so that it can be reused for different BB.
 * 
 * After this call the state of the RM should be identical to a new RM.
 */
void SimpleResourceManager::clear() {
    director_->clearOldResources();
    buildDirector_.clear();
    director_->clear();
    setDDG(NULL);
}

void
SimpleResourceManager::setDDG(const DataDependenceGraph* ddg) {
    director_->setDDG(ddg);
}

std::map<const TTAMachine::Machine*, 
         std::map<int, std::list< SimpleResourceManager*> > >
SimpleResourceManager::rmPool_;

void SimpleResourceManager::setMaxCycle(unsigned int maxCycle) {
    director_->setMaxCycle(maxCycle);
}

