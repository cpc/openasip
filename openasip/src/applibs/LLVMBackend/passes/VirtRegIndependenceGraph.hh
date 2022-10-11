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
 * @file VirtRegIndependenceGraph.hh
 *
 * Declaration of VirtRegIndependence class.
 *
 * @author Pekka Jääskeläinen 2010
 * @note rating: red
 */

#ifndef TCE_VIRT_REG_IND_GRAPH_HH
#define TCE_VIRT_REG_IND_GRAPH_HH

#include <set>

#include "hash_set.hh"
#include "boost/graph/adjacency_list.hpp"
#include "MachineInstrDDG.hh"

namespace llvm {
    class LiveInterval;
    class MachineFunction;
    class VirtRegMap;
}

/**
 * Virtual register independence graph AKA False Dependence Prevention 
 * Graph (FDPG).
 *
 * Nodes in the graph are virtual registers. They have edges between if the
 * live ranges could be scheduled in parallel or reordered if there were no
 * false dependencies. In other words, two virtual registers / live ranges /
 * variables are considered independent in case they do not have real
 * data dependencies between them.
 *
 * @todo This approach has at least the following unsolved problems:
 *
 * - The data is not updated during the register allocation. In case
 *   a register needed to be shared and an false dep was added, the
 *   independence graph does not reflect this new dependence. This results
 *   in too greedy assignments because register sharing might not hurt
 *   after one register sharing has added a new antidependence. The DDG height
 *   delta-based allocator helped for this.
 *
 * - Actual schedulability is not taken in account. Even in case the nodes
 *   were independent, they could have min cycles such that it forces the
 *   ordering anyways. For example, a branch in DDG with node N1
 *   of height 4 and another independent branch with node N2 with height 2.
 *   N1 cannot be scheduled above N2 anyhow, regardless if there was an
 *   extra N2->N1 edge or not due to the register sharing. Currently the FDPG
 *   method blindly avoids sharing the reg between N2 and N1, thus increasing
 *   the register pressure for no benefit.
 *
 * - Memory dependencies are not taken in account. This results in a too
 *   free DDG thus too many falsely independent live ranges, ending with a too
 *   greedy allocation.
 */
class VirtRegIndependenceGraph {
public:
    VirtRegIndependenceGraph(llvm::MachineFunction& mf, llvm::VirtRegMap& vrm);

    int newFalseDepsFromAssign(
        llvm::LiveInterval* interval, 
        MachineInstrDDG::Register physReg);

    void addNode(MachineInstrDDG::Register virtReg);
    void addEdge(
        MachineInstrDDG::Register nodeA, 
        MachineInstrDDG::Register nodeB);
    
    std::set<MachineInstrDDG::Register> adjacentNodes(
        MachineInstrDDG::Register node);

private:
    typedef boost::adjacency_list<
    boost::vecS, boost::vecS, boost::undirectedS,
    boost::property<boost::vertex_name_t, MachineInstrDDG::Register> > FDPG;

    // VRM used to track phys to virt reg assignments during the register alloc
    llvm::VirtRegMap& vrm_;

    FDPG fdpg_;

    std::map<MachineInstrDDG::Register, FDPG::vertex_descriptor> vertexMap_;
    std::map<FDPG::vertex_descriptor, MachineInstrDDG::Register> vregMap_;
};

#endif

