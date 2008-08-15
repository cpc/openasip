/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ControlDependenceNode.hh
 *
 * Prototype control dependence graph of TTA program representation:
 * declaration of graph node (basic block or region node).
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONTROL_DEPENDENCE_NODE_HH
#define TTA_CONTROL_DEPENDENCE_NODE_HH

#include <string>

#include "GraphNode.hh"
#include "CodeSnippet.hh"
#include "BasicBlockNode.hh"

namespace TTAProgram {
class Instruction;
class CodeSnippet;
}
/**
 * Node of the control dependence graph. Each node represents one basic block
 * or region node.
 *
 * A basic block is defined as an ordered sequence of adjacent instructions
 * in which only the first can be target of jump and ony the last one (save
 * jump delay slots) can contain jumps.
 *
 * @note Instructions that contain function calls are control flow bariers
 * also. For now they split basic blocks into two, with special type of edge
 * connecting them.
 */
class ControlDependenceNode : public GraphNode {
public:
    enum NodeType {
        CDEP_NODE_REGION, // Region nodes of CDG
        CDEP_NODE_PREDICATE, // Predicate nodes of CDG
        CDEP_NODE_BB   // Basic block nodes containing code snippets
    };

    ControlDependenceNode(
        const int nodeID,
        const NodeType type = CDEP_NODE_BB,
        BasicBlockNode* bblock = NULL) :
        GraphNode(nodeID), type_(type), code_(bblock) {}
    virtual ~ControlDependenceNode() {}

    bool isControlDependenceNode() const;
    bool isRegionNode() const;
    bool isPredicateNode() const;
    bool isBBNode() const;
    bool isEntryNode() const;
    bool isExitNode() const;

    int instructionCount() const;
    TTAProgram::Instruction& instruction(int index) const;
    BasicBlockNode* basicBlockNode() const;

    NodeType type() const;
    std::string toString() const;

private:
    NodeType type_;
    BasicBlockNode* code_;
};

#endif
