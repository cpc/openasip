/*
    Copyright (c) 2002-2012 Tampere University of Technology.

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

#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/Function.h"

#include "MachineInstrDDG.hh"

#include "AssocTools.hh"
#include "Application.hh"
#include "LLVMTCECmdLineOptions.hh"

#include <utility>

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
        std::string(mf.getFunction()->getName().str()) + "_middg", true),
    onlyTrueDeps_(onlyTrueDeps), mf_(mf), regInfo_(mf_.getTarget().getRegisterInfo()) {
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
#ifdef DEBUG_MI_DDG
            i.dump();
#endif
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

                if (llvm::TargetRegisterInfo::isPhysicalRegister(
                        operand.getReg())) {
                    // only physical reg at this point should be the stack pointer,
                    // which is a global reg we can ignore
#ifdef DEBUG_MI_DDG
                    Application::logStream() 
                        << "found a phys reg " << operand.getReg() 
                        << std::endl;
                    if (operand.getType() == llvm::MachineOperand::MO_FrameIndex)
                        Application::logStream() << "SP";
                    i.dump();
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
    CmdLineOptions *cmdLineOptions = Application::cmdLineOptions();
    LLVMTCECmdLineOptions* options = 
        dynamic_cast<LLVMTCECmdLineOptions*>(cmdLineOptions);
    if (options != NULL && options->dumpDDGsDot()) {
        writeToDotFile(mf.getFunction()->getName().str() + "_middg.dot");
    }    

    if (options != NULL && options->printResourceConstraints()) {
        Application::logStream() 
            << mf.getFunction()->getName().str() 
            << " MachineInstrDDG critical path length: "
            << height()
            << std::endl;
    }
}

MachineInstrDDG::~MachineInstrDDG() {
    CmdLineOptions *cmdLineOptions = Application::cmdLineOptions();
    LLVMTCECmdLineOptions* options = 
        dynamic_cast<LLVMTCECmdLineOptions*>(cmdLineOptions);
    if (options != NULL && options->dumpDDGsDot()) {
        writeToDotFile(mf_.getFunction()->getName().str() + "_middg-final.dot");
    }    

    if (options != NULL && options->printResourceConstraints()) {
        Application::logStream() 
            << mf_.getFunction()->getName().str() 
            << " final MachineInstrDDG critical path length: "
            << height()
            << std::endl;
    }
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
#ifdef LLVM_2_7
            instr->modifiesRegister(physReg)) {
#else
            instr->modifiesRegister(physReg, regInfo_)) {
#endif
            return true;
        } 
    }
    return false;
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

