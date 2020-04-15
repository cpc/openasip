/*
    Copyright (c) 2002-2012 Tampere University.

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
 * @file MachineInstrDDG.cc
 *
 * @author Pekka Jääskeläinen 2012
 * @note rating: red
 */
#ifdef NDEBUG
#undef NDEBUG
#endif

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "tce_config.h"
#ifdef LLVM_OLDER_THAN_6_0
#include "llvm/Target/TargetRegisterInfo.h"
#else
#include "llvm/CodeGen/TargetRegisterInfo.h"
#endif
#include "llvm/IR/Function.h"
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetSubtargetInfo.h>
#else
#include <llvm/CodeGen/TargetSubtargetInfo.h>
#endif

#include "MachineInstrDDG.hh"

#include "AssocTools.hh"
#include "Application.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "TCETargetMachine.hh"
#include "OperationPool.hh"
#include "Operation.hh"

#include <utility>

POP_COMPILER_DIAGS

// #define DEBUG_MI_DDG

/**
 * Constructs a DDG out of MachineInstructions.
 *
 * Only true dependencies are supported at the moment.
 */
MachineInstrDDG::MachineInstrDDG(
    llvm::MachineFunction& mf, 
    bool onlyTrueDeps) :
    BoostGraph<MIDDGNode, MIDDGEdge>(
#ifdef LLVM_OLDER_THAN_6_0
        std::string(mf.getFunction()->getName().str()) + "_middg", true),
#else
        std::string(mf.getFunction().getName().str()) + "_middg", true),
#endif
    onlyTrueDeps_(onlyTrueDeps), mf_(mf), 
#if LLVM_OLDER_THAN_6_0
    regInfo_(mf_.getTarget().getSubtargetImpl(
                 *mf_.getFunction())->getRegisterInfo()) 
#else
    regInfo_(mf_.getTarget().getSubtargetImpl(
                 mf_.getFunction())->getRegisterInfo())
#endif
{
    int instructions = 0;
    for (llvm::MachineFunction::const_iterator bbi = mf.begin(); 
         bbi != mf.end(); ++bbi) {
        const llvm::MachineBasicBlock& bb = *bbi;
        for (llvm::MachineBasicBlock::const_iterator ii = bb.begin(); 
             ii != bb.end(); ++ii) {
            const llvm::MachineInstr& i = *ii;
            MIDDGNode* node = new MIDDGNode(i, instructions);
            nodes_.insert(node);
            addNode(*node);
            assert(hasNode(*node));
            ++instructions;
            for (unsigned oi = 0; oi < i.getNumOperands(); ++oi) {
                const llvm::MachineOperand& operand = i.getOperand(oi);
                if (!operand.isReg())
                    continue;

                if (operand.isUndef()) {
                    // this is probably a global register defined in some other
                    // function, thus it can be ignored (only reads from it in this func)
                    continue;
                }
                if (operand.isImplicit()) {
                    // the call clobbered regs
                    continue;
                }                    

#ifdef LLVM_OLDER_THAN_10
                if (llvm::TargetRegisterInfo::isPhysicalRegister(
                        operand.getReg())) {
#else
                if (llvm::Register::isPhysicalRegister(
                        operand.getReg())) {
#endif

                    // only physical reg at this point should be the stack pointer,
                    // which is a global reg we can ignore
#ifdef DEBUG_MI_DDG
                    Application::logStream() 
                        << "found a phys reg " << operand.getReg() 
                        << std::endl;
                    if (operand.getType() == llvm::MachineOperand::MO_FrameIndex)
                        Application::logStream() << "SP";
#endif
                    continue;
                }

                if (operand.isUse()) {
                    users_[operand.getReg()].insert(node);
#ifdef DEBUG_MI_DDG
                    Application::logStream()
                        << "uses: " << operand.getReg() << std::endl;
#endif
                } else if (operand.isDef()) {
                    if (definers_[operand.getReg()] != NULL) {
                        // in case we already have a definer, use the
                        // one from the different basic block to avoid loops
                        if (definers_[operand.getReg()]->machineInstr()->
                            getParent() != &bb) {
#ifdef DEBUG_MI_DDG
                            Application::logStream()
                                << "found a potential back edge case, "
                                << "using the definer from another BB"
                                << std::endl;
#endif
                            continue;
                        }
                    } else {
                        definers_[operand.getReg()] = node;
                    }
                } else {
#ifdef DEBUG_MI_DDG
                    Application::logStream()
                        << "unknown operand " << oi << std::endl;
#endif
                    continue;
                }
                allRegisters_.insert(operand.getReg());
            }
        }
    }


    for (DefinerMap::iterator i = definers_.begin(); i != definers_.end(); 
         ++i) {
        Register reg = (*i).first;
        MIDDGNode* source = (*i).second;
        NodeSet& users = users_[reg];
        for (std::set<MIDDGNode*>::iterator u = users.begin(); u != users.end();
             ++u) {
            MIDDGNode* dest = *u;

            if (hasPath(*dest, *source)) {
#ifdef DEBUG_MI_DDG
                Application::logStream() 
                    << "ignoring edge that would create a loop"
                    << std::endl;
#endif
                continue;
            }

            MIDDGEdge* edge = new MIDDGEdge(reg);
            edges_.insert(edge);
            connectNodes(*source, *dest, *edge);
        }
    }
}

MachineInstrDDG::~MachineInstrDDG() {
}


/**
 * Creates a false dependency edge introduced when the given virtual
 * reg is assigned the given physical register.
 *
 * Does not add the edge to the graph. Note, only creates a single edge
 * although in a multi-BB DDG there is usually many in case edge
 * spans CFG branch points. Returns a pair with NULLs in case no false dep is
 * introduced.
 */
std::pair<MIDDGNode*, MIDDGNode*>
MachineInstrDDG::createFalseDepEdge(Register vreg, Register physReg) const {

    MIDDGNode* null = NULL;
    std::pair<MIDDGNode*, MIDDGNode*> none = std::make_pair(null, null);


    if (lastPhysRegUsers_.find(physReg) == lastPhysRegUsers_.end()) {
        return none;
    }

    MIDDGNode* lastPhysRegUser = (*lastPhysRegUsers_.find(physReg)).second;

    if (this->vregDefiner(vreg) == NULL) {
        // could not find a definer for the given vreg, thus probably
        // a global register such as stack pointer, there won't be
        // many assignment possibilities for them anyways
        return none;
    }
    MIDDGNode* vregDefiner = this->vregDefiner(vreg);
    MIDDGNode* lastVregUser = this->lastVregUser(vreg);

    MIDDGNode* lastPhysRegDefiner = NULL;
    if (lastPhysRegDefiners_.find(physReg) != lastPhysRegDefiners_.end()) {
        lastPhysRegDefiner = (*lastPhysRegDefiners_.find(physReg)).second;
    }

    assert(vregDefiner != NULL);

    // the source and destination nodes for the introduced antidep
    MIDDGNode* source = NULL;
    MIDDGNode* dest = NULL;

    // the sequential instruction ordering defines the dep direction
    if (vregDefiner->sequentialAddress() > 
        lastPhysRegUser->sequentialAddress()) {
        source = lastPhysRegUser;
        dest = vregDefiner;
    } else {
        if (lastVregUser == NULL) {
            // only writes to the vreg, thus it would be WaW
            source = vregDefiner;
        } else {
            source = lastVregUser;
        }

        if (lastPhysRegDefiner != NULL) {
            dest = lastPhysRegDefiner;
        } else {
            dest = lastPhysRegUser;
        }
    }

    // ignore loop edges for now, but signal edges to itself as they are
    // cheap false deps which should be treated as such
    if (dest != source && hasPath(*dest, *source))
        return none; 

    return std::make_pair(source, dest);
}

/**
 * Returns the "height delta" of an antidep edge created in case the
 * given virtual register is assigned the given physical register.
 *
 * Height delta is the difference between the DDG height of the source
 * definer node and the DDG height of the latest read node of the physReg.
 * The direction of the introduced false dep edge is determined from the
 * sequential instruction order, direction is assumed to be from the
 * earlier instruction to the later. Thus, an edge with 0 or greater height dep
 * potentially constraints the schedule by potentially heightening the DDG.
 * However, this is not generally the case in case the critical path length
 * is not increased by the assignment.
 *
 * @param vreg The virtual register to test.
 * @param physReg The physical register assigment to test.
 * @return The height delta of the false dep from the assignment, or
 *         INT_MIN in case no false dep would be produced.
 */
int
MachineInstrDDG::falseDepHeightDelta(Register vreg, Register physReg) const {

    std::pair<MIDDGNode*, MIDDGNode*> fdep = 
        createFalseDepEdge(vreg, physReg);

    int hdelta = INT_MIN;
    if (fdep.first != NULL && fdep.second != NULL) {
        if (fdep.first == fdep.second)
            return -1; // treat fdep to itself as the least harmful one
        hdelta = 
            maxSourceDistance(*fdep.first) - maxSourceDistance(*fdep.second);
    }
    return hdelta;
}

MIDDGNode*
MachineInstrDDG::lastVregUser(Register vreg) const {
    int lastUse = -1;
    MIDDGNode* lastUser = NULL;
    if (users_.find(vreg) == users_.end())
        return NULL;

    NodeSet& users = (*users_.find(vreg)).second;
    for (NodeSet::const_iterator i = users.begin(); i != users.end(); 
         ++i) {
        MIDDGNode* node = *i;
        if (lastUse < node->sequentialAddress()) {
            lastUse = node->sequentialAddress();
            lastUser = node;
        }
    }
    return lastUser;
}

/**
 * Checks if there is at least one preceeding node to the given node that
 * defines or uses the given physical register.
 *
 * Also the uses in the same node are considered.
 */
bool
MachineInstrDDG::preceedingNodeUsesOrDefinesReg(
    const MIDDGNode& node, Register physReg) const {

    NodeSet pred = predecessors(node);
    pred.insert(const_cast<MIDDGNode*>(&node));
    for (NodeSet::const_iterator i = pred.begin(); i != pred.end(); ++i) {
        MIDDGNode& p = (**i);
        const llvm::MachineInstr* instr = p.machineInstr();
        for (unsigned operand = 0; operand < instr->getNumOperands(); 
             ++operand) {
            const llvm::MachineOperand& mo = instr->getOperand(operand);
            if (!mo.isReg())
                continue;
            if (mo.getReg() == physReg ||
                (regAssignments_.find(mo.getReg()) != regAssignments_.end() &&
                 (*regAssignments_.find(mo.getReg())).second == physReg)) {
                return true;
            }
        }
        
        if (instr->readsRegister(physReg) || 
            instr->modifiesRegister(physReg, regInfo_)) {
            return true;
        } 
    }
    return false;
}


/**
 * Computes optimal top-down schedule assuming infinite resources and that
 * each operation completes in one cycle.
 */
void
MachineInstrDDG::computeOptimalSchedule() {
    smallestCycle_ = 0;
    largestCycle_ = 0;
    schedule_.clear();
    for (int nc = 0; nc < nodeCount(); ++nc) {
        MIDDGNode& n = node(nc);
        int cycle = maxSourceDistance(n);
        n.setOptimalCycle(cycle);
        largestCycle_ = std::max(cycle, largestCycle_);
        schedule_[cycle].push_back(&n);
    }
}

TCEString
MachineInstrDDG::dotString() const {
    std::ostringstream s;
    s << "digraph " << name() << " {" << std::endl;

    const bool scheduled = nodeCount() > 1 && node(0).optimalCycle() != -1;
    
    if (scheduled) {
        // print the "time line" to visualize the schedule
        s << "\t{" << std::endl
          << "\t\tnode [shape=plaintext];" << std::endl
          << "\t\t";
        const int smallest = smallestCycle_;
        const int largest = largestCycle_;
        for (int c = smallest; c <= largest; ++c) {
            s << "\"cycle " << c << "\" -> ";
        }
        s << "\"cycle " << largest + 1 << "\"; " 
          << std::endl << "\t}" << std::endl;
    
        // print the nodes that have cycles
        for (int c = smallest; c <= largest; ++c) {
            std::list<MIDDGNode*> ops = schedule_[c];
            if (ops.size() > 0) {
                s << "\t{ rank = same; \"cycle " << c << "\"; ";
                for (std::list<MIDDGNode*>::iterator i = ops.begin(); 
                     i != ops.end(); ++i) {
                    MIDDGNode& n = **i;        
                    s << "n" << n.nodeID() << "; ";
                }
                s << "}" << std::endl;
            }        
        }


        typedef std::map<TCEString, int> OpCountMap;
        // Count how many times each operation could be potentially
        // executed in parallel in an optimal schedule. This can direct
        // the intial architecture design.
        OpCountMap maxParallelOps;
        // The operation mix. I.e., the static occurence of operations
        // in the code.
        OpCountMap operationMix;

        for (int c = smallest; c <= largest; ++c) {
            std::list<MIDDGNode*> ops = schedule_[c];
            if (ops.size() == 0) continue;

            std::map<TCEString, int> parallelOpsAtCycle;
            for (std::list<MIDDGNode*>::iterator i = ops.begin(); 
                 i != ops.end(); ++i) {
                MIDDGNode& n = **i;        
                TCEString opName = n.osalOperationName();
                if (opName == "" || opName == "?jump") continue;
                operationMix[opName]++;
                parallelOpsAtCycle[opName]++;
            }

            for (OpCountMap::const_iterator i = parallelOpsAtCycle.begin();
                 i != parallelOpsAtCycle.end(); ++i) {
                TCEString opName = (*i).first;
                int count = (*i).second;
                maxParallelOps[opName] = 
                    std::max(maxParallelOps[opName], count);
            }
        }

        const int COL_WIDTH = 14;
        // print statistics of the graph as a comment
        s << "/* statistics: " << std::endl << std::endl;
        s << std::setw(COL_WIDTH) << std::right << "virtual regs: ";
        s << definers_.size() << std::endl << std::endl;
        s << std::setw(COL_WIDTH) << std::right << "operation stats: ";
        s << std::endl << std::endl;
        
        for (OpCountMap::const_iterator i = maxParallelOps.begin();
             i != maxParallelOps.end(); ++i) {
            TCEString opName = (*i).first;
            int parCount = (*i).second;
            int total = operationMix[opName];
            s << std::setw(COL_WIDTH) << std::right << opName + ": ";
            s << std::setw(COL_WIDTH) << std::right << total;
            s << " total, " << std::setw(COL_WIDTH) << std::right 
              << parCount << " at most in parallel" << std::endl;
        }
        s << "*/" << std::endl;
    }
    // first print all the nodes and their properties
    for (int i = 0; i < nodeCount(); ++i) {
        Node& n = node(i);
        s << "\tn" << n.nodeID()
          << " [" << n.dotString();
        if (isInCriticalPath(n))
            s << ",shape=box,color=\"red\"";
        s  << "]; " << std::endl;
    }

    // edges
    for (int count = edgeCount(), i = 0; i < count ; ++i) {
        Edge& e = edge(i);
        Node& tail = tailNode(e);
        Node& head = headNode(e);

        s << "\tn" << tail.nodeID() << " -> n" 
          << head.nodeID() << "[" 
          << e.dotString();
        if (isInCriticalPath(tail) && isInCriticalPath(head))
            s << ",color=red";
        s << "];" << std::endl;
    }

    s << "}" << std::endl;   

    return s.str();    

}

/**
 * Assigns the given physical register to the given virtual register.
 *
 * Does not yet add false dependence edges, just updates the last
 * phys reg use bookkeeping.
 */
void
MachineInstrDDG::assignPhysReg(Register vreg, Register physReg) {

    regAssignments_[vreg] = physReg;

    MIDDGNode* lastDefiner = vregDefiner(vreg);

    if (lastPhysRegDefiners_.find(physReg) != lastPhysRegDefiners_.end()) {
        MIDDGNode* previousDefiner = lastPhysRegDefiners_[physReg];
        if (lastDefiner == NULL ||
            previousDefiner->sequentialAddress() >
            lastDefiner->sequentialAddress()) {
            lastDefiner = previousDefiner;            
        }
    }
    if (lastDefiner != NULL) {
        lastPhysRegDefiners_[physReg] = lastDefiner;
    }

    MIDDGNode* lastUser = NULL;
    if (lastPhysRegUsers_.find(physReg) != lastPhysRegUsers_.end()) {
        lastUser = lastPhysRegUsers_[physReg];
    }

    MIDDGNode* lastVregUser = this->lastVregUser(vreg);
    if (lastVregUser != NULL) {
        if (lastUser == NULL || 
            lastVregUser->sequentialAddress() > 
            lastUser->sequentialAddress()) {
            lastUser = lastVregUser;
        }
    }
    if (lastUser != NULL) {
        lastPhysRegUsers_[physReg] = lastUser;
    }

    std::pair<MIDDGNode*, MIDDGNode*> fdep = 
        createFalseDepEdge(vreg, physReg);

    if (fdep.first != NULL && fdep.second != NULL &&
        fdep.first != fdep.second) {
        MIDDGEdge* edge = new MIDDGEdge(physReg, MIDDGEdge::DEP_WAR);
        edges_.insert(edge);
#if 0
        Application::logStream()
            << "adding edge: " << edge->toString() << " from "
            << fdep.first->sequentialAddress() << " to "
            << fdep.second->sequentialAddress() << std::endl;
#endif
        connectNodes(*fdep.first, *fdep.second, *edge);
    }
}

/**
 * Try to figure out the name of the instruction opcode in OSAL,
 * if available.
 *
 * If the operation with the produced name is not found in OSAL,
 * llvm: is prepended to the name string.
 */
TCEString
MIDDGNode::osalOperationName() const {
#ifdef LLVM_OLDER_THAN_6_0
    const llvm::TargetInstrInfo *TII = 
        machineInstr()->getParent()->getParent()->getTarget().
        getSubtargetImpl(
            *machineInstr()->getParent()->getParent()->getFunction())->
        getInstrInfo();
#else
    const llvm::TargetInstrInfo *TII =
        machineInstr()->getParent()->getParent()->getTarget().
        getSubtargetImpl(
            machineInstr()->getParent()->getParent()->getFunction())->
        getInstrInfo();
#endif
    // If it's a custom operation call, try to figure out
    // the called operation name and use it instead as the
    // node label.
    TCEString opName;
    if (mi_->isInlineAsm()) {
        unsigned numDefs = 0;
        while (mi_->getOperand(numDefs).isReg() &&
               mi_->getOperand(numDefs).isDef())
            ++numDefs;
        opName = mi_->getOperand(numDefs).getSymbolName();
    } else {
#if LLVM_OLDER_THAN_4_0
        opName = TII->getName(mi_->getOpcode());
#else
        opName = TII->getName(mi_->getOpcode()).str();
#endif
    }

    // Clean up the operand type string encoded as 
    // lower case letter at the end of the string.
    TCEString upper = opName.upper();
    TCEString cleaned;
    for (size_t i = 0; i < opName.size(); ++i) {
        if (upper[i] == opName[i])
            cleaned += opName[i];
        else
            break;
    }

    OperationPool ops;
    Operation& operation = ops.operation(cleaned.c_str());
    if (operation.isNull())
        return TCEString("llvm:") + opName;

    return cleaned;
}

std::string 
MIDDGNode::dotString() const { 
    return (boost::format("label=\"%s\"") % osalOperationName()).str();
}
