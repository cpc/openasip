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
 * @file MachineInstrDDG.hh
 *
 * Declaration of MachineInstrDDG class.
 *
 * @author Pekka Jääskeläinen 2010-2012
 * @note rating: red
 */

#ifndef TCE_MACHINE_INSTR_DDG_HH
#define TCE_MACHINE_INSTR_DDG_HH

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")
#ifdef __clang__
IGNORE_COMPILER_WARNING("-Wunused-private-field")
#endif

#include "BoostGraph.hh"
#include "GraphNode.hh"
#include "GraphEdge.hh"
#include <set>
#include <map>
#include <sstream>
#ifdef LLVM_OLDER_THAN_6_0
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"
#else
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#endif
#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class MachineFunction;
    class MachineInstr;
}

struct MIDDGNode : public GraphNode {
    MIDDGNode() : GraphNode(), mi_(NULL), address_(-1), optimalCycle_(-1) {}
    MIDDGNode(const llvm::MachineInstr& mi, int sequentialAddress) : 
        GraphNode(), mi_(&mi), address_(sequentialAddress) {}

    virtual ~MIDDGNode() {}

    bool operator<(const MIDDGNode& other) const {
        return other.sequentialAddress() < this->sequentialAddress();
    }
    bool operator==(const MIDDGNode& other) const {
        return other.mi_ == this->mi_;
    }
    
    const llvm::MachineInstr* machineInstr() const { return mi_; }
    int sequentialAddress() const { return address_; }

    std::string dotString() const;
    TCEString osalOperationName() const;

    void setOptimalCycle(int cycle) { optimalCycle_ = cycle; }
    int optimalCycle() const { return optimalCycle_; }
private:
    const llvm::MachineInstr* mi_;
    int address_;
    int optimalCycle_;
};

struct MIDDGEdge : public GraphEdge {
    enum DependenceType {
        DEP_UNKNOWN = 0,
        DEP_RAW     = 1,
        DEP_WAR     = 2,
        DEP_WAW     = 3};

    enum EdgeReason {
        EDGE_REGISTER,
        EDGE_MEMORY};

    MIDDGEdge(unsigned reg) : 
        GraphEdge(), reg_(reg), dependenceType_(DEP_RAW) {}

    MIDDGEdge(unsigned reg, DependenceType type) : 
        GraphEdge(), reg_(reg), dependenceType_(type) {}


    virtual ~MIDDGEdge() {}

    TCEString dotString() const { 
        return (boost::format("label=\"%d %s\"") % reg_ % typeAsString()).
            str();
    }

    TCEString toString() const {
        return (boost::format("%d %s") % reg_ % typeAsString()).str();
    }

private:

    std::string typeAsString() const {
        std::string type = "unknown";
        if (dependenceType_ == DEP_RAW)
            type = "RaW";
        else if (dependenceType_ == DEP_WAR) 
            type = "WaR";
        else if (dependenceType_ == DEP_WAW) 
            type = "WaW";
        return type;
    }
    
    unsigned reg_;
    unsigned char dependenceType_; // DependenceType
};

/**
 * Data Dependence Graph constructed from non-register allocated LLVM
 * MachineInstructions.
 *
 * Only true dependencies supported at the moment. Later we can might add 
 * support for adding the false dependencies introduced by the register 
 * allocator, if needed.
 */
class MachineInstrDDG : 
    public BoostGraph<MIDDGNode, MIDDGEdge> {
public:
    typedef unsigned Register;
    typedef std::set<Register> RegisterSet;

    MachineInstrDDG(
        llvm::MachineFunction& mf, 
        bool onlyTrueDeps=true);

    virtual ~MachineInstrDDG();

    RegisterSet allRegisters() const { return allRegisters_; }

    MIDDGNode* vregDefiner(Register vreg) const { return definers_[vreg]; };
    MIDDGNode* lastVregUser(Register vreg) const;
    
    int falseDepHeightDelta(Register vreg, Register physReg) const;
    void assignPhysReg(Register vreg, Register physReg);

    bool preceedingNodeUsesOrDefinesReg(
        const MIDDGNode& node, 
        Register physReg) const;

    void computeOptimalSchedule();

    TCEString dotString() const;

private:
    typedef std::map<Register, MIDDGNode*> DefinerMap;
    typedef std::map<Register, NodeSet> UserMap;
    typedef std::map<Register, Register> RegisterMap;

    std::pair<MIDDGNode*, MIDDGNode*> 
    createFalseDepEdge(Register vreg, Register physReg) const;

    // all register indices in the DDG
    RegisterSet allRegisters_;
    // the MachineInstructions* that define the virtual regs
    mutable DefinerMap definers_;
    mutable UserMap users_;

    NodeSet nodes_;
    std::set<MIDDGEdge*> edges_;
    std::map<Register, MIDDGNode*> lastPhysRegUsers_;
    std::map<Register, MIDDGNode*> lastPhysRegDefiners_;
    RegisterMap regAssignments_;

    // do not add any false deps in case this is true
    const bool onlyTrueDeps_;

    // in case a schedule has been computed, these contain the limits
    int smallestCycle_;
    int largestCycle_;
    mutable std::map<int, std::list<MIDDGNode*> > schedule_;

    llvm::MachineFunction& mf_;
    const llvm::TargetRegisterInfo* regInfo_;
};

POP_COMPILER_DIAGS


#endif

