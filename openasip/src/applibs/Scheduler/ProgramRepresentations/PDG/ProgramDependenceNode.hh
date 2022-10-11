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
 * @file ProgramDependenceNode.hh
 *
 * Declaration of prototype of graph-based program representation:
 * declaration of the program dependence node.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_PROGRAM_DEPENDENCE_NODE_HH
#define TTA_PROGRAM_DEPENDENCE_NODE_HH

#include "ControlDependenceNode.hh"

class MoveNode;
class BasicBlockNode;

/**
*/
class ProgramDependenceNode : public GraphNode {
public:
    enum NodeType {
        PDG_NODE_REGION, // Region nodes of PDG
        PDG_NODE_PREDICATE, // Predicate nodes of PDG
        PDG_NODE_MOVE,   // Single statements nodes
        PDG_NODE_LOOPENTRY, // Region that is loop entry node
        PDG_NODE_LOOPCLOSE // Region that is loop entry node
    };

    ProgramDependenceNode(NodeType type = PDG_NODE_REGION);
    ProgramDependenceNode(
        ControlDependenceNode& cdgNode,
        NodeType type = PDG_NODE_REGION);
    ProgramDependenceNode(
        MoveNode& mNode,
        NodeType type = PDG_NODE_MOVE);
    virtual ~ProgramDependenceNode();

    // Stores information about region and eec
    // Duplicates NodeSet which is available only in Graph :(
    typedef std::set<ProgramDependenceNode*> NodesInfo;
    bool isRegionNode() const { return type_ == PDG_NODE_REGION;}
    bool isPredicateMoveNode() const { return type_ == PDG_NODE_PREDICATE;}
    bool isMoveNode() const { return type_ == PDG_NODE_MOVE;}
    bool isLoopEntryNode() const { return type_ == PDG_NODE_LOOPENTRY;}
    bool isLoopEntryNode(int component) const {
        return type_ == PDG_NODE_LOOPENTRY && component_ == component;}
    bool isLoopCloseNode() const { return type_ == PDG_NODE_LOOPCLOSE;}
    bool isLastNode() const { return lastNode_;}
    void setLoopEntryNode(int component);
    void setComponent(int component) { component_ = component;}
    void setLastNode() { lastNode_ = true; }
    int component() const { return component_;}

    MoveNode& moveNode();
    const MoveNode& moveNode() const;
    ControlDependenceNode& cdgNode();
    const ControlDependenceNode& cdgNode() const;
    std::string dotString() const;
    std::string toString() const;
    const NodesInfo& region();
    const NodesInfo& eec();
    void printRelations() const;
    BasicBlockNode* newFirstBB() { return newFirstBB_;}
    void setNewFirstBB(BasicBlockNode* newBB) { newFirstBB_ = newBB;}
    std::vector<BasicBlockNode*> leafBlocks() { return leafBlocks_;}
    void addLeafBlock(BasicBlockNode* newLeaf) {
        leafBlocks_.push_back(newLeaf); }
    void addLeafBlocks(std::vector<BasicBlockNode*> newLeafs) {
        leafBlocks_.insert(leafBlocks_.end(), newLeafs.begin(), newLeafs.end());
    }

protected:
    friend class ProgramDependenceGraph;
    void setPredicateMoveNode();
    void addToRegion(ProgramDependenceNode& node);
    void addToEEC(ProgramDependenceNode& node);
private:
    MoveNode* mNode_;
    ControlDependenceNode* cdgNode_;
    NodeType type_;
    /// Stores "region" information for computing serialization information
    NodesInfo region_;
    /// Stores "eec" information for computing serialization information
    NodesInfo eec_;
    /// Number of strong component the node belongs to.
    /// Node can be part of several strong components so this value
    /// have practical meaning only for loop close nodes - close just one
    /// loop, and loop entry nodes - marks a component in which node is
    /// loop entry. Still can be regular region of larger loop
    /// Full list of nodes that belongs to actual components is available
    /// in strongComponents_ vector in PDG
    int component_;
    /// First basic block of a subgraph of a region
    BasicBlockNode* newFirstBB_;
    /// Vector of basic blocks of a subgraph that do not have outgoing
    /// edged and will need to get connected
    std::vector<BasicBlockNode*> leafBlocks_;
    /// Indicates that node must be scheduled last between it's siblings
    /// because it's subgraph contains close node and it's parent is entry
    bool lastNode_;
};

#endif
