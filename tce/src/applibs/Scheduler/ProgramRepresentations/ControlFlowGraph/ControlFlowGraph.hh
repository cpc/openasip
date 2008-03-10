/**
 * @file ControlFlowGraph.hh
 *
 * Declaration of prototype control flow graph of TTA program
 * representation.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONTROL_FLOW_GRAPH_HH
#define TTA_CONTROL_FLOW_GRAPH_HH


#include <map>
#include <vector>
#include <string>

#include <boost/graph/reverse_graph.hpp>

#include "Exception.hh"
#include "BoostGraph.hh"
#include "BasicBlockNode.hh"
#include "ControlFlowEdge.hh"
#include "Program.hh"
#include "Address.hh"
#include "NullAddress.hh"
#include "CFGStatistics.hh"

namespace TTAProgram {
    class Program;
    class Procedure;
    class Instruction;
    class Move;
    class InstructionReferenceManager;
    class POMRelocBookkeeper;
    class Address;
    class NullAddress;
}
using boost::reverse_graph;

/**
 * Control Flow Graph.
 *
 * The basic blocks are initially in the original program order when traversed
 * with nodeCount()/node().
 */
class ControlFlowGraph : public BoostGraph<BasicBlockNode, ControlFlowEdge> {
public:
    ControlFlowGraph(const TTAProgram::Procedure& procedure);
    virtual ~ControlFlowGraph();

    
    std::string procedureName() const;
    int alignment() const;
    TTAProgram::Program* program() const;

    BasicBlockNode& entryNode();
    BasicBlockNode& exitNode();
    std::string printStatistics();
    const CFGStatistics& statistics();
    friend class ControlDependenceGraph;
    
    
private:
    // For temporary storage
    typedef std::map<InstructionAddress, const TTAProgram::Instruction*>
        InstructionAddressMap;
    typedef std::vector<InstructionAddress> InstructionAddressVector;
    // Type of reversed underlying graph, needed for control dependence
    // analysis
    typedef reverse_graph<ControlFlowGraph::Graph> ReversedGraph;
    typedef BoostGraph<BasicBlockNode, ControlFlowEdge>::NodeDescriptor
        NodeDescriptor;
 
    ReversedGraph& reversedGraph() const;

    void computeLeadersFromRefManager(
        InstructionAddressMap& leaders,
        const TTAProgram::Procedure& procedure);
    void computeLeadersFromJumpSuccessors(
        InstructionAddressMap& leaders,
        const TTAProgram::Procedure& procedure);
    void computeLeadersFromRelocations(
        InstructionAddressMap& leaderSet,
        InstructionAddressMap& dataCodeRellocations,
        const TTAProgram::Procedure& procedure);

    void createAllBlocks(
        InstructionAddressMap& leaders,
        const TTAProgram::Procedure& procedure);
    BasicBlockNode& createBlock(
        const TTAProgram::Instruction& leader,
        const TTAProgram::Instruction& endBlock);
    ControlFlowEdge& createControlFlowEdge(
        const TTAProgram::Instruction& iTail,
        const TTAProgram::Instruction& iHead,
        ControlFlowEdge::CFGEdgePredicate edgePredicate =
            ControlFlowEdge::CFLOW_EDGE_NORMAL,
        bool isJump = true);

    void directJump(
        InstructionAddressMap& leaders,
        const InstructionAddress& leaderAddr,
        const TTAProgram::Instruction& instruction,
        const TTAProgram::Instruction& instructionTarget,
        const TTAProgram::Procedure& procedure);
    void indirectJump(
        InstructionAddressMap& leaders,
        const InstructionAddress& leaderAddr,
        InstructionAddressMap& dataCodeRellocations,
        const TTAProgram::Instruction& instruction,
        const TTAProgram::Procedure& procedure);
    void createJumps(
        InstructionAddressMap& leaders,
        const InstructionAddress& leaderAddr,
        InstructionAddressMap& dataCodeRellocations,
        const TTAProgram::Instruction& instruction,
        const TTAProgram::Procedure& procedure,
        int moveIndex);

    void addExit();
    void addEntryExitEdge();
    void removeEntryExitEdge();
    // Data saved from original procedure object
    std::string procedureName_;
    TTAProgram::Program* program_;
    TTAProgram::Address startAddress_;
    TTAProgram::Address endAddress_;
    int alignment_;

    // Collection of all basic blocks of the control flow graph indexed by
    // the address of their start instruction (leader).
    std::map<InstructionAddress, BasicBlockNode*> blocks_;

};
#endif
