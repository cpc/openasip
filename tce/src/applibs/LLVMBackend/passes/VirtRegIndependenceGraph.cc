/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file VirtRegIndependenceGraph.cc
 *
 * @author Pekka J‰‰skel‰inen 2010
 * @note rating: red
 */
#include "hash_set.hh"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/CodeGen/LiveIntervalAnalysis.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/Function.h"
#include "boost/graph/graphviz.hpp"
#include "VirtRegMap.h"
#include "VirtRegIndependenceGraph.hh"
#include "MachineInstrDDG.hh"
#include "MapTools.hh"

// #define DEBUG_FDPG
/**
 * Constructs the graph from an LLVM MachineFunction.
 */
VirtRegIndependenceGraph::VirtRegIndependenceGraph(
    llvm::MachineFunction& mf, llvm::VirtRegMap& vrm) :
    vrm_(vrm) {

    MachineInstrDDG tddg(*vrm.TII, *vrm.TRI, mf,  true); 
    MachineInstrDDG::RegisterSet regs = tddg.allRegisters();
#ifdef DEBUG_FDPG    
    Application::logStream() << "building FDPG" << std::endl;
#endif
    // add the virtual regs as nodes 
    for (MachineInstrDDG::RegisterSet::const_iterator i = regs.begin(); 
         i != regs.end(); ++i) {
        MachineInstrDDG::Register reg = *i;

        if (llvm::TargetRegisterInfo::isPhysicalRegister(reg)) 
            continue;

        addNode(reg);
    }

#ifdef DEBUG_FDPG
    Application::logStream() << "finding TDDG paths" << std::endl;
#endif
    // this should speed up hasPath() as all known paths are
    // computed only once
    tddg.findAllPaths();
#ifdef DEBUG_FDPG
    Application::logStream() << "finding TDDG paths done" << std::endl;
#endif
    MachineInstrDDG::RegisterSet unhandledRegs = regs;
    for (MachineInstrDDG::RegisterSet::const_iterator i = regs.begin(); 
         i != regs.end(); ++i) {
        MachineInstrDDG::Register reg = *i;
        MIDDGNode* instrA = tddg.vregDefiner(reg);
        for (MachineInstrDDG::RegisterSet::const_iterator u = 
                 unhandledRegs.begin(); 
             u != unhandledRegs.end(); ++u) {
            MachineInstrDDG::Register otherReg = *u;
            MIDDGNode* instrB = tddg.vregDefiner(otherReg);
            if (instrB == NULL) {
                Application::logStream() 
                    << "could not find definer for " << otherReg << std::endl;
                abortWithError("Cannot proceed.");
            }
            if (!tddg.hasPath(*instrA, *instrB) &&
                !tddg.hasPath(*instrB, *instrA)) {
                addEdge(reg, otherReg);
            }
        }
        unhandledRegs.erase(reg);
    }
#ifdef DEBUG_FDPG
    Application::logStream() << "building FDPG done" << std::endl;
#endif
#if 0
    // this is exremely slow as there are often huge number of edges in FDPG
    std::string s(mf.getFunction()->getNameStr() + "_fdpg.dot");
    std::ofstream o(s.c_str());
    boost::write_graphviz(
        o, fdpg_, make_label_writer(get(boost::vertex_name, fdpg_)));
#endif
}

/** 
 * Returns the number of new false deps introduced by assigning the given
 * physical register to the given live interval.
 */
int
VirtRegIndependenceGraph::newFalseDepsFromAssign(
    llvm::LiveInterval* interval, MachineInstrDDG::Register physReg) {

    if (llvm::TargetRegisterInfo::isPhysicalRegister(interval->reg)) {
        Application::logStream() 
            << "Should not see assigned intervals here. Got: " 
            << interval->reg
            << std::endl;
        return true;
    }

    // look at all connected live Intervals in the FDPG and see if they have
    // been assigned the same PhysReg we are trying to assign this one

    int foundFalseDeps = 0;
    std::set<MachineInstrDDG::Register> adjacent = adjacentNodes(interval->reg);
    for (std::set<MachineInstrDDG::Register>::const_iterator i = 
             adjacent.begin(); i != adjacent.end(); ++i) {
        MachineInstrDDG::Register virtReg = *i;
        bool isPhysRegAssigned = vrm_.hasPhys(virtReg);
        if (isPhysRegAssigned && vrm_.getPhys(virtReg) == physReg) {
            ++foundFalseDeps;
        }
    }
    return foundFalseDeps;
}

void 
VirtRegIndependenceGraph::addNode(MachineInstrDDG::Register virtReg) {    
    vertexMap_[virtReg] = boost::add_vertex(fdpg_);
    vregMap_[vertexMap_[virtReg]] = virtReg;
    boost::put(boost::vertex_name, fdpg_, vertexMap_[virtReg], virtReg);
}

void 
VirtRegIndependenceGraph::addEdge(
    MachineInstrDDG::Register nodeA, MachineInstrDDG::Register nodeB) {
    boost::add_edge(vertexMap_[nodeA], vertexMap_[nodeB], fdpg_);
}

std::set<MachineInstrDDG::Register>
VirtRegIndependenceGraph::adjacentNodes(MachineInstrDDG::Register node) {
    std::set<MachineInstrDDG::Register> nodes;

    boost::graph_traits<FDPG>::adjacency_iterator i, end;
    i = boost::adjacent_vertices(vertexMap_[node], fdpg_).first;
    end = boost::adjacent_vertices(vertexMap_[node], fdpg_).second;
    for (; i != end; ++i) {
        MachineInstrDDG::Register virtReg = vregMap_[*i];
        nodes.insert(virtReg);
    }

    return nodes;
}
