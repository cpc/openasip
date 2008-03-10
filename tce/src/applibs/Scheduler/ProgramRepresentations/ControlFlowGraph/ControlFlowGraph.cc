/**
 * @file ControlFlowGraph.cc
 *
 * Implementation of prototype control flow graph of TTA program
 * representation.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include <vector>
#include <algorithm>
#include <functional>

#include "ControlFlowGraph.hh"

#include "BaseType.hh"
#include "MapTools.hh"

#include "Program.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "NullInstruction.hh"
#include "Immediate.hh"
#include "Move.hh"
#include "MoveGuard.hh"
#include "Terminal.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"
#include "Address.hh"
#include "DataMemory.hh"
#include "DataDefinition.hh"
#include "Exception.hh"
#include "Application.hh"
#include "Operation.hh"
#include "SpecialRegisterPort.hh"
#include "POMDisassembler.hh"
#include "FunctionUnit.hh"
#include "ControlUnit.hh"
#include "Machine.hh"
#include "NullProgram.hh"
#include "TerminalRegister.hh"
#include "TerminalImmediate.hh"
#include "BasicBlock.hh"
#include "CFGStatistics.hh"

using TTAProgram::Program;
using TTAProgram::Procedure;
using TTAProgram::Instruction;
using TTAProgram::NullInstruction;
using TTAProgram::Move;
using TTAProgram::InstructionReference;
using TTAProgram::InstructionReferenceManager;
using TTAProgram::DataMemory;
using TTAProgram::DataDefinition;
using TTAProgram::Address;
using TTAProgram::CodeSnippet;
using TTAMachine::SpecialRegisterPort;
using TTAMachine::Port;
using TTAMachine::FunctionUnit;
using TTAMachine::ControlUnit;



/**
 * Destructor. Removes nodes and edge from a graph.
 * Removal of node automatically free also the edges.
 */
ControlFlowGraph::~ControlFlowGraph() {
    while (nodeCount() > 0) {
        BasicBlockNode* b = &node(0);
        removeNode(*b);
        delete b;
    }
}
/**
 * Constructor.
 * Read a procedure of TTA program and build a control flow graph
 * out of it.
 *
 * Create a control flow graph using the procedure of POM. Procedure must be
 * registered in Program to get access to rellocations...
 *
 * @param procedure The procedure to analyse.
 */

ControlFlowGraph::ControlFlowGraph(
    const TTAProgram::Procedure& procedure) :
    program_(&TTAProgram::NullProgram::instance()),
    startAddress_(TTAProgram::NullAddress::instance()),
    endAddress_(TTAProgram::NullAddress::instance()) {

    procedureName_ = procedure.name();
    if (procedure.isInProgram()) {
        program_ = &procedure.parent();
    }
    startAddress_ = procedure.startAddress();
    endAddress_ = procedure.endAddress();
    alignment_ = procedure.alignment();

    // Set of instructions that start a new basic block
    InstructionAddressMap leaders;
    InstructionAddressMap dataCodeRellocations;

    computeLeadersFromRefManager(leaders, procedure);
    computeLeadersFromJumpSuccessors(leaders, procedure);
    computeLeadersFromRelocations(leaders, dataCodeRellocations, procedure);
    createAllBlocks(leaders, procedure);
    // Creates edges between basic blocks
    InstructionAddress leaderAddr;
    Instruction& iEnd = procedure.lastInstruction();
    Instruction* instruction = &procedure.firstInstruction();
    int delaySlots = 0;
    bool hasCFMove = false;
    while (instruction->address().location() <= iEnd.address().location()) {
        if (MapTools::containsValue(leaders, instruction)) {
            leaderAddr = instruction->address().location();
            if (!MapTools::containsKey(blocks_, leaderAddr)) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__, "Basic block is missing!");
            }
            hasCFMove = false;
        }
        // We only deal with one JUMP or CALL per instruction,
        // there is restriction that there can be no control flow
        // operations in delay slots of previous operation
        for (int i = 0, count = instruction->moveCount(); i < count; i++) {
            if (instruction->move(i).isCall()) {
                // There is some CF move in a basic block
                hasCFMove = true;
                // Find delay slots of call, can be different then delays
                // slots of target machine in case partially scheduled code
                const Instruction* iNext = instruction;
                // Partially scheduled POM is possible input, so each
                // call must check what is the number of delay slots in
                // target machine
                FunctionUnit& unit =
                    const_cast<FunctionUnit&>(
                    instruction->move(i).destination().functionUnit());
                ControlUnit& control = dynamic_cast<ControlUnit&>(unit);
                delaySlots = control.delaySlots();
                for (int i = 0; i < delaySlots; i++) {
                    // moves through delay slots and checks if there is
                    // no another CF operation
                    if (procedure.hasNextInstruction(*iNext)) {
                        iNext = &procedure.nextInstruction(*iNext);
                    }
                    if (iNext->hasControlFlowMove()) {
                        throw InvalidData(
                          __FILE__, __LINE__, __func__,
                            (boost::format(
                                "Control flow operation in delay slot"
                                " of CALL in %d!")
                                % iNext->address().location()).str());
                    }
                }
                if (procedure.hasNextInstruction(*iNext)) {
                    iNext = &procedure.nextInstruction(*iNext);
                    createControlFlowEdge(
                        *leaders[leaderAddr], *iNext,
                        ControlFlowEdge::CFLOW_EDGE_NORMAL, false);

                }
                break;
            }
            if (instruction->move(i).isJump()) {
                // There is some CF move in basic block
                hasCFMove = true;
                createJumps(
                    leaders, leaderAddr, dataCodeRellocations, *instruction,
                    procedure,i);
                break;
            }
        }
        if (procedure.hasNextInstruction(*instruction)) {
            Instruction* nextInstruction =
                &procedure.nextInstruction(*instruction);
            // Look if next instruction is beginning of basic block
            // and if there was no CF move in current basic block
            // add fall true edge in such case
            if (MapTools::containsValue(leaders, nextInstruction) &&
                hasCFMove == false) {
                BasicBlockNode& blockSource(*blocks_[leaderAddr]);
                BasicBlockNode& blockTarget(
                    *blocks_[nextInstruction->address().location()]);
                if (!hasEdge(blockSource, blockTarget)) {
                    createControlFlowEdge(
                        *leaders[leaderAddr],*nextInstruction);
                    }
            }
            instruction = nextInstruction;
        } else {
            break;
        }
    }
    addExit();

#if 0
	// Causes problems with test case with infinite loop and no
	// gcc optimization. References are not removed correctly.
    // remove unreachable basic blocks recursivelly,
    std::list<BasicBlockNode*> candidates;
    InstructionReferenceManager& refManager =
        procedure.parent().instructionReferenceManager();
    
    while(1){
        typedef std::pair<NodeIter, NodeIter> NodeIterPair;
        NodeIterPair nodes = boost::vertices(graph_);
        for (NodeIter i = nodes.first; i != nodes.second; i++) {
            NodeDescriptor nd = descriptor(*graph_[*i]);
            int degree = boost::in_degree(nd, graph_); 
            if (degree == 0 &&
                !graph_[*i]->isEntryBB() &&
                graph_[*i]->basicBlock().instructionCount() != 0) {
                candidates.push_back(graph_[*i]);            
            }
        }
        if (candidates.size() == 0) {
            break;
        }
        std::list<BasicBlockNode*>::iterator itr = candidates.begin();
        while (itr != candidates.end()) {
            BasicBlock& b = (*itr)->basicBlock();
            b.clear();
            itr++;
        }
        candidates.clear();
    }
#endif
}


/**
 * Internal helper. Find the leader instructions from program
 * reference manager. Should also find startAddress of procedure.
 *
 * @param leaders Set of leader instructions to update.
 * @param procedure The procedure to analyse, uses it's parent() program to
 *                  find referenceManager.
 */
void
ControlFlowGraph::computeLeadersFromRefManager(
    InstructionAddressMap& leaders,
    const Procedure& procedure) {

    if (!procedure.isInProgram()) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "For access to reference manager procedure \
            must be registered in Program!");
    }
    InstructionReferenceManager& refManager =
        procedure.parent().instructionReferenceManager();
    // Add first instruction of procedure by default,
    // testing starts from second
    leaders[startAddress_.location()] = &procedure.firstInstruction();
    for (int i = 0; i < refManager.referenceCount(); i++) {
        Instruction& instruction(refManager.reference(i).instruction());
        InstructionAddress insAddr = instruction.address().location();
        if (insAddr > startAddress_.location() &&
            insAddr < endAddress_.location()) {
            leaders[insAddr] = &instruction;
        }
    }
}


/**
 * Internal helper. Finds the leader instructions that are referred to via
 * addressed stored in data memory and recorded as data-to-code relocation
 * entries.
 *
 * Adds to a given instruction set all instructions whose address is stored
 * in data memory (thus are potential targets of an indirect jump) and is
 * recorded in a data-to-code relocation entry.
 *
 * @param leaders Set of leader instructions to update.
 * @param dataCodeRellocations Set of dataCodeRellocations that applies for
 *                  given procedure
 * @param procedure The procedure to analyse, uses it's parent() program to
 *                  access data memory
 */
void
ControlFlowGraph::computeLeadersFromRelocations(
    InstructionAddressMap& leaders,
    InstructionAddressMap& dataCodeRellocations,
    const Procedure& procedure) {

    if (!procedure.isInProgram()) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "For access to Relocations procedure \
            must be registered in Program!");
    }
    Program& program = procedure.parent();
    for (int j = 0; j < program.dataMemoryCount(); j++) {
        const DataMemory& d(program.dataMemory(j));
        for (int i = 0, count = d.dataDefinitionCount();
            i < count;
            i++) {
            const DataDefinition& dataDef(d.dataDefinition(i));
            if (!dataDef.isInstructionAddress()) {
                continue;
            }
            const Address& targetAddress(
                dataDef.destinationAddress());
            if (targetAddress.location() >= startAddress_.location() &&
                targetAddress.location() < endAddress_.location()) {
                Instruction& iTarget(
                    procedure.instructionAt(targetAddress.location())) ;
                leaders[targetAddress.location()] = &iTarget;
                dataCodeRellocations[targetAddress.location()] = &iTarget;
            }
        }
    }
}


/**
 * Computes starts of basic blocks which follows jumps or calls or are
 * targets of jump.
 *
 * @param leaders Set of leader instructions to update.
 * @param procedure The procedure to analyse.
 */
void
ControlFlowGraph::computeLeadersFromJumpSuccessors(
    InstructionAddressMap& leaders,
    const Procedure& procedure) {

    // record target instructions of jumps, because they are
    // leaders of basic block too
    InstructionAddressMap targets;
    // The procedure start point is always a block leader.
    const Instruction* instruction = &procedure.firstInstruction();
    bool increase = true;
    bool hasNext = true;
    int delaySlots = 0;
    while (1) {
        // Only one control flow operation per cycle
        increase = true;
        for (int i = 0; i < instruction->moveCount(); i++) {
            Move& m(instruction->move(i));
            if (m.isControlFlowMove()) {
                // if it is direct jump we store target address
                if (m.source().isInstructionAddress() && m.isJump()) {
                    Instruction& iTarget =
                        m.source().instructionReference().instruction();
                    InstructionAddress targetAddr =
                        iTarget.address().location();
                    // If an instruction that is target of a jump has not
                    // been yet identified as a leader, do it here.
                    leaders[targetAddr] = &iTarget;
                }
                increase = false;
                // POM can contain scheduled and unscheduled code
                // so each jump must test form it's target machine
                // how many delay slots it needs
                FunctionUnit& unit =
                    const_cast<FunctionUnit&>(
                        m.destination().functionUnit());
                ControlUnit& control = dynamic_cast<ControlUnit&>(unit);
                delaySlots = control.delaySlots();

                for (int i = 0; i < delaySlots + 1; i++) {
                    if (procedure.hasNextInstruction(*instruction)) {
                        instruction =
                            &procedure.nextInstruction(*instruction);
                        if (instruction->hasControlFlowMove() &&
                            i < delaySlots) {
                        //test if there  is no another CF operation in delay
                        // slots
                            throw InvalidData(
                                __FILE__, __LINE__, __func__,
                                (boost::format(
                                "Control flow operation in delay slot"
                                " in %d!")
                                % instruction->address().location()).str());
                        }
                    } else {
                        hasNext = false;
                        break;
                    }
                }
                if (hasNext) {
                    leaders[instruction->address().location()] = instruction;
                }
                break;
            }
        }

        if (procedure.hasNextInstruction(*instruction)) {
            if (increase) {
                instruction = &procedure.nextInstruction(*instruction);
            }
            hasNext = true;
        } else {
            break;
        }
    }
}

/**
 * Split Procedure into the set of basic blocks.
 *
 * @param leaders Set of instructions which starts basic block.
 * @param procedure Procedure that is analysed.
 */
void
ControlFlowGraph::createAllBlocks(
    InstructionAddressMap& leaders,
    const Procedure& procedure) {

    // leaders are not sorted so to create basic blocks we need
    // to sort beginning addresses of blocks
    std::set<InstructionAddress> iAddresses;
    iAddresses = MapTools::keys<InstructionAddress>(leaders);
    InstructionAddressVector
        sortedLeaders(iAddresses.begin(), iAddresses.end());
    sort(sortedLeaders.begin(), sortedLeaders.end());
    int blockSize = sortedLeaders.size();
    if (blockSize > 0) {
        int i;
        for (i = 0; i < blockSize - 1; i++) {
            createBlock(
                procedure.instructionAt(sortedLeaders[i]),
                procedure.instructionAt(sortedLeaders[i+1] - 1));
        }
        createBlock(
            procedure.instructionAt(sortedLeaders[i]),
            procedure.lastInstruction());
    }
}
/**
 * Internal helper. Create a basic block.
 *
 * Given the first and last instructions of a basic block, create a new basic
 * block of graph. It is assumed that the graph does not have a basic block
 * for the given pair of instructions yet. If such a block exists,
 * this method aborts.
 *
 * @param leader The first instruction of the basic block to create.
 * @param endBlock The last instruction of the basic block to create.
 * @return The created basic block node.
 */
BasicBlockNode&
ControlFlowGraph::createBlock(
    const Instruction& leader,
    const Instruction& endBlock) {

    InstructionAddress blockStart = leader.address().location();
    InstructionAddress blockEnd = endBlock.address().location();
    CodeSnippet& proc = leader.parent();

    if (MapTools::containsKey(blocks_, blockStart)) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Basic block with given start address already exists!");
    }
    if (blockStart > blockEnd) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Basic block start address is higher then it's end address!");
    }

    BasicBlockNode* node = new BasicBlockNode(blockStart, blockEnd);
    const Instruction* addInstruction = &leader;
    while (addInstruction->address().location() <= blockEnd) {
        Instruction* newInstruction = addInstruction->copy();
        node->basicBlock().add(newInstruction);
        if (proc.hasNextInstruction(*addInstruction)) {
            addInstruction = &proc.nextInstruction(*addInstruction);
        } else {
            break;
        }
    }
    addNode(*node);
    // Create entry node and add edge from it into current BB
    // if it's start is also start of procedure
    if (leader.address().location() == startAddress_.location()){
        BasicBlockNode* entry = new BasicBlockNode(0, 0, true);
        addNode(*entry);
        ControlFlowEdge* edge = new ControlFlowEdge(edgeCount());
        connectNodes(*entry, *node, *edge);
    }
    blocks_[blockStart] = node;
    return *node;
}

/**
 * Internal helper. Create a control flow edge between two basic blocks.
 *
 * Given the leader instructions of two basic blocks, create a new control
 * flow edge connecting the blocks. The basic blocks are assumed to be
 * already existing and added to the graph. If either basic block does not
 * exist, this method aborts. A new edge is created even if the blocks are
 * already connected. Thus, parallel edges are possible.
 *
 * @param iTail The first instruction of the tail basic block (from).
 * @param iHead The first instruction of the head basic block (to).
 * @param edgePredicate The value of an edge (true, false, normal, call)
 * @param isJumpEdge Defines if edge represents jump or call, default jump
 * @return The created control flow edge.
 */
ControlFlowEdge&
ControlFlowGraph::createControlFlowEdge(
    const Instruction& iTail,
    const Instruction& iHead,
    ControlFlowEdge::CFGEdgePredicate edgePredicate,
    bool isJumpEdge) {

    InstructionAddress sourceAddr = iTail.address().location();
    InstructionAddress targetAddr = iHead.address().location();
    if (!MapTools::containsKey(blocks_, sourceAddr)) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Source basic block is missing!");
    }
    if (!MapTools::containsKey(blocks_, targetAddr)) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Destination basic block is missing!");
    }

    BasicBlockNode& blockSource(*blocks_[sourceAddr]);
    BasicBlockNode& blockTarget(*blocks_[targetAddr]);

    ControlFlowEdge* theEdge;
    if (hasEdge(blockSource, blockTarget)) {
        theEdge = graph_[connectingEdge(blockSource, blockTarget)];
        if (theEdge->isNormalEdge() &&
            edgePredicate != ControlFlowEdge::CFLOW_EDGE_NORMAL) {
            disconnectNodes(blockSource, blockTarget);
            theEdge = new ControlFlowEdge(
                edgeCount(), edgePredicate, isJumpEdge);
            connectNodes(blockSource, blockTarget, *theEdge);
        }
    } else {
        theEdge = new ControlFlowEdge(
            edgeCount(), edgePredicate, isJumpEdge);
        connectNodes(blockSource, blockTarget, *theEdge);
    }
    return *theEdge;
}

/**
 * Creates edges for direct jump (source is immediate value).
 *
 * @param leaders Set of beginnings of basic blocks
 * @param leaderAddr Address of beginning of current basic block
 * @param instruction Instruction to analyse (only one move in instruction)
 * @param procedure Procedure we are working with
 */
void
ControlFlowGraph::directJump(
    InstructionAddressMap& leaders,
    const InstructionAddress& leaderAddr,
    const TTAProgram::Instruction& instruction,
    const TTAProgram::Instruction& instructionTarget,
    const TTAProgram::Procedure& procedure) {

    int cfIndex = 0;
    int delaySlots = 0;
    for (int i = 0; i < instruction.moveCount(); i++) {
        if (instruction.move(i).isControlFlowMove()) {
            cfIndex = i;
            break;
        }
    }

    TTAProgram::Move& move = instruction.move(cfIndex);
    InstructionAddress targetAddr = instructionTarget.address().location();
    if (!MapTools::containsKey(leaders, targetAddr)) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Target basic block of jump is missing!");
    }
    // POM allows mixed scheduled an unscheduled code, so each jump
    // must check from machine how many delay slots it needs
    FunctionUnit& unit =
        const_cast<FunctionUnit&>(move.destination().functionUnit());
    ControlUnit& control = dynamic_cast<ControlUnit&>(unit);
    delaySlots = control.delaySlots() ;

    if (!move.isUnconditional()) {
        // if jump is conditional we consider guard
        // we add also fall-through edge to next block,
        // for inverted value of guard
        Instruction* iNext = const_cast<Instruction*>(&instruction);
        for (int i = 0; i < delaySlots + 1; i++) {
            if (procedure.hasNextInstruction(*iNext)) {
                iNext = &procedure.nextInstruction(*iNext);
            }
            if (iNext->hasControlFlowMove() &&
                i < delaySlots) {
                //test if there  is no another CF operation in delay
                // slots
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format(
                    "Control flow operation in delay slot"
                    " in %d!")
                    % iNext->address().location()).str());
            }
        }
        if (!MapTools::containsValue(leaders, iNext)) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Fall through basic block is missing!");
        }
        if (move.guard().isInverted()) {
            // jumps on !bool, fall-through on bool
            createControlFlowEdge(
                *leaders[leaderAddr], instructionTarget,
                ControlFlowEdge::CFLOW_EDGE_FALSE);
            createControlFlowEdge(
                *leaders[leaderAddr], *iNext,
                ControlFlowEdge::CFLOW_EDGE_TRUE);

        } else {
            createControlFlowEdge(
                *leaders[leaderAddr], instructionTarget,
                ControlFlowEdge::CFLOW_EDGE_TRUE);
            createControlFlowEdge(
                *leaders[leaderAddr], *iNext,
                ControlFlowEdge::CFLOW_EDGE_FALSE);
        }
    } else {
        createControlFlowEdge(*leaders[leaderAddr], instructionTarget);
    }
}

/**
 * Creates edges for indirect jump (source is NOT immediate value).
 *
 * @param leaders Set of beginnings of basic blocks
 * @param leaderAddr Address of beginning of current basic block
 * @param dataCodeRellocations Rellocation information for targets of jump
 * @param instruction Instruction to analyse (only one move in instruction)
 * @param procedure Procedure we are working with
 */
void
ControlFlowGraph::indirectJump(
    InstructionAddressMap& leaders,
    const InstructionAddress& leaderAddr,
    InstructionAddressMap& dataCodeRellocations,
    const TTAProgram::Instruction& instruction,
    const TTAProgram::Procedure& procedure) {

    InstructionAddressMap::iterator dataCodeIterator =
        dataCodeRellocations.begin();
    int cfIndex = 0;
    int delaySlots = 0;
    for (int i = 0; i < instruction.moveCount(); i++) {
        if (instruction.move(i).isControlFlowMove()) {
            cfIndex = i;
            break;
        }
    }
    Port* port =
        const_cast<Port*>(&instruction.move(cfIndex).source().port());

    if (dynamic_cast<SpecialRegisterPort*>(port) != NULL) {
        ///TODO: double check or replace with some more meaningfull
        /// solution. This one doesn't add edges from ra->jump.1 and
        /// trusts that edge to Exit node will be added later by addEntryExit
        /// from node that has no outgoing edges.
        return;
    }
    // POM can contain scheduled and unscheduled code so each jump
    // must test how many delay slots it uses from machine
    FunctionUnit& unit = const_cast<FunctionUnit&>(
        instruction.move(cfIndex).destination().functionUnit());
    ControlUnit& control = dynamic_cast<ControlUnit&>(unit);
    delaySlots = control.delaySlots();


    ControlFlowEdge::CFGEdgePredicate edgePredicate =
        ControlFlowEdge::CFLOW_EDGE_NORMAL;
    ControlFlowEdge::CFGEdgePredicate fallPredicate =
        ControlFlowEdge::CFLOW_EDGE_NORMAL;
    if (instruction.move(cfIndex).isUnconditional() == false) {
        if (instruction.move(cfIndex).guard().isInverted()) {
            edgePredicate = ControlFlowEdge::CFLOW_EDGE_FALSE;
            fallPredicate = ControlFlowEdge::CFLOW_EDGE_TRUE;
        } else {
            edgePredicate = ControlFlowEdge::CFLOW_EDGE_TRUE;
            fallPredicate = ControlFlowEdge::CFLOW_EDGE_FALSE;
        }
    }
    if (procedure.hasNextInstruction(instruction)) {
        Instruction* iNext = const_cast<Instruction*>(&instruction);
        for (int i = 0; i < delaySlots + 1; i++) {
            if (procedure.hasNextInstruction(*iNext)) {
                iNext = &procedure.nextInstruction(*iNext);
            }
            if (iNext->hasControlFlowMove() &&
                i < delaySlots) {
                //test if there  is no another CF operation in delay
                // slots
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format(
                    "Control flow operation in delay slot in %d!")
                    % iNext->address().location()).str());
            }
        }

        if (instruction.move(cfIndex).isUnconditional() == false) {
            if (!MapTools::containsValue(leaders, iNext)){
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    "Fall through basic block is missing!");
            }
            createControlFlowEdge(
                    *leaders[leaderAddr], *iNext,fallPredicate);
        }
    }
    while (dataCodeIterator != dataCodeRellocations.end()) {
    // Target of jump is reachable also from it's predecessor
    // block, in case there is no unconditional jump at the end.
    // If the end of previous BB is conditional jump it will be
    // added elsewhere.
    // Target is limited to present procedure, no interprocedural
    // indirect jump for now.
        createControlFlowEdge(
            *leaders[leaderAddr], *(*dataCodeIterator).second,
            edgePredicate);
        dataCodeIterator++;
    }
}

/**
 * Adds artificial block named 'Exit' to the graph
 */
void
ControlFlowGraph::addExit() {
    BasicBlockNode* exit = new BasicBlockNode(0, 0, false, true);
    addNode(*exit);
    for (int i = 0; i < nodeCount(); i++) {
        BasicBlockNode& block(static_cast<BasicBlockNode&>(node(i)));
        if (outDegree(block) == 0 && exit != &block) {
            ControlFlowEdge* edge = new ControlFlowEdge(edgeCount());
            connectNodes(block, *exit, *edge);
        }
    }
}

/**
 * Creates reversed underlying graph.
 *
 * Control Flow Graph has member graph_ which stores actual data.
 * This function returns reversed graph_ (edge directions changed).
 *
 * @return Reversed underlying graph.
 */
ControlFlowGraph::ReversedGraph&
ControlFlowGraph::reversedGraph() const {
    ReversedGraph* R = new ReversedGraph(graph_);
    return *R;
}

/**
 * Return the entry node of the graph.
 *
 * @return The entry node of the graph.
 * @exception InstanceNotFound if the graph does not have a entry node.
 * @exception InvalidData if the graph has multiple nodes that are
 *            recognised as entry nodes.
 */
BasicBlockNode&
ControlFlowGraph::entryNode() {
    BasicBlockNode* result = NULL;
    bool found = false;
    bool unlinkedEntryNode = false;
    for (int i = 0; i < nodeCount(); i++) {
        if (inDegree(node(i)) == 0) {
            // sanity check
            if (!static_cast<BasicBlockNode&>(node(i)).isEntryBB()) {
                // probably the entry node is not present
                // or there are more nodes which are not reachable from
                // entry nodes... likely caused by frontend not doing
                // any of -O{1,2} optimizations (in case of gcc)
                unlinkedEntryNode = true;
                continue;
            }
            if (found == true) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    "Corrupted graph. Found multiple entry nodes.");
            }
            result = dynamic_cast<BasicBlockNode*>(&node(i));
            found = true;
        }
    }
    if (found == false || result == NULL) {
        string errorMsg("Graph does not have entry node.");
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
    return *result;
}


/**
 * Return the stop/exit node of the graph.
 *
 *
 * @return The stop node of the graph.
 * @exception InstanceNotFound if the graph does not have a stop node.
 * @exception InvalidData if the graph has multiple nodes that are
 *            recognised as stop nodes.
 */
BasicBlockNode&
ControlFlowGraph::exitNode() {

    BasicBlockNode* result = NULL;
    bool found = false;
    bool unlinkedExitNode = false;

    for (int i = 0; i < nodeCount(); i++) {
        if (outDegree(node(i)) == 0) {
            // sanity check
            if (!static_cast<BasicBlockNode&>(node(i)).isExitBB()) {
                // probably the stop node is not present
                unlinkedExitNode = true;
                continue;
            }
            if (found == true) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    "Corrupted graph. Found multiple exit nodes.");
            }
            result = dynamic_cast<BasicBlockNode*>(&node(i));
            found = true;
        }
    }
    if (found == false || result == NULL || unlinkedExitNode == true) {
        string errorMsg("Graph does not have exit node.");
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
    return *result;
}

/**
 * Create a "false" edge between Entry and Exit. Replaces edges from
 * Entry to graph with "true" edges.
 * This is not strictly part of Control Flow Graph, it is used
 * during construction of control dependencies.
 *
 * The entry node is connected to exit node
 */
void
ControlFlowGraph::addEntryExitEdge() {
    // edge from Entry to first "real" node of CFG needs to be true
    // artificial edge to Exit node needs to be false
    BasicBlockNode& entry = entryNode();
    std::vector<std::pair<BasicBlockNode*, int> > fromEntry;
    for (int i = 0; i < outDegree(entry); i++) {
        fromEntry.push_back(
            std::pair<BasicBlockNode*, int>(
                &headNode(outEdge(entry,i)), outEdge(entry,i).edgeID()));
    }
    for (unsigned int i = 0; i < fromEntry.size(); i++) {
        disconnectNodes(entry, *(fromEntry[i].first));
        ControlFlowEdge* edge = new ControlFlowEdge(
            fromEntry[i].second, ControlFlowEdge::CFLOW_EDGE_TRUE);
        connectNodes(entry, *(fromEntry[i].first), *edge);
    }
    ControlFlowEdge* edge = new ControlFlowEdge(
        edgeCount(), ControlFlowEdge::CFLOW_EDGE_FALSE);
    connectNodes(entryNode(), exitNode(), *edge);
}

/**
 * Remove a "false" edge between Entry and Exit after control
 * dependencies are created.
 *
 * The entry node is connected to exit
 */
void
ControlFlowGraph::removeEntryExitEdge() {
    // Edge from Entry to Exit node of CFG needs to be removed
    // it is not really control flow edge
    BasicBlockNode& entry = entryNode();
    std::vector<std::pair<BasicBlockNode*, int> > fromEntry;
    for (int i = 0; i < outDegree(entry); i++) {
        fromEntry.push_back(
            std::pair<BasicBlockNode*, int>(
                &headNode(outEdge(entry,i)), outEdge(entry,i).edgeID()));
    }
    for (unsigned int i = 0; i < fromEntry.size(); i++) {
        disconnectNodes(entry, *(fromEntry[i].first));
        ControlFlowEdge* edge = new ControlFlowEdge(fromEntry[i].second);
        connectNodes(entry, *(fromEntry[i].first), *edge);
    }
    disconnectNodes(entryNode(), exitNode());
}

/**
 * Returns a name of procedure the graph represents taken from original POM
 * object.
 *
 * @return The name of procedure as a string
 */
std::string
ControlFlowGraph::procedureName() const {
    return procedureName_;
}

/**
 * Returns alignment value copied from original POM procedure
 *
 * @return The alignment of procedure.
 */
int
ControlFlowGraph::alignment() const {
    return alignment_;
}

/**
 * Returns a pointer to the POM Program object procedure was part
 * of in POM.
 *
 * @return The pointer to POM Program
 */
TTAProgram::Program*
ControlFlowGraph::program() const {
    return program_;
}

/**
 * Helper function to find target address of a jump in case source of jump
 * is immediate register or general purpose register.
 *
 * @param leaders Starting instructions of all basic blocks
 * @param leaderAddr Address of a first instruction of present Basic Block
 * @param dataCodeRellocations Read from POM, the possible targets of indirect
 * jumps are in data code rellocation information
 * @param instruction Currect instruction containing jump
 * @param procedure The reference to current procedure
 * @param moveIndex Index of move with jump in current instruction
 * @note Abort when the source of jump is immediateregister and no write to
 * such register is found in same basic block.
 */
void
ControlFlowGraph::createJumps(
    InstructionAddressMap& leaders,
    const InstructionAddress& leaderAddr,
    InstructionAddressMap& dataCodeRellocations,
    const TTAProgram::Instruction& instruction,
    const TTAProgram::Procedure& procedure,
    int moveIndex) {

    if (instruction.move(moveIndex).source().isInstructionAddress()) {
        Move* tmp = &instruction.move(moveIndex);
        directJump(
            leaders, leaderAddr, instruction,
            tmp->source().instructionReference().instruction(),
            procedure);
         return;
    }
    if (instruction.move(moveIndex).source().isImmediateRegister() ||
        instruction.move(moveIndex).source().isGPR()) {
        const Instruction* iPrev = &instruction;
        TTAProgram::TerminalRegister* sourceTerm =
            dynamic_cast<TTAProgram::TerminalRegister*>(
            &instruction.move(moveIndex).source());
        while (iPrev->address().location() > leaderAddr) {
            iPrev = &procedure.previousInstruction(*iPrev);
            const TTAProgram::TerminalRegister* destTerm = NULL;
            if (sourceTerm->isImmediateRegister()) {
                for (int j = 0; j < iPrev->immediateCount(); j++){
                    destTerm =
                        dynamic_cast<const TTAProgram::TerminalRegister*>(
                            &iPrev->immediate(j).destination());
                    TTAProgram::Immediate* tmpImm = &iPrev->immediate(j);
                    if (sourceTerm->equals(*destTerm)) {
                        directJump(
                            leaders, leaderAddr, instruction,
                        tmpImm->value().instructionReference().instruction(),
                            procedure);
                        return;
                    }
                }
            }
            if (sourceTerm->isGPR()) {
                for (int j = 0; j < iPrev->moveCount(); j++){
                    destTerm =
                        dynamic_cast<const TTAProgram::TerminalRegister*>(
                            &iPrev->move(j).destination());
                    if (destTerm == NULL) {
                        continue;
                    }
                    TTAProgram::Terminal* tmpTerm = &iPrev->move(j).source();
                    if (sourceTerm->equals(*destTerm)) {
                        if (tmpTerm->isInstructionAddress()){
                            directJump(
                                leaders, leaderAddr, instruction,
                                tmpTerm->instructionReference().instruction(),
                                procedure);
                            return;
                        }
                        if (tmpTerm->isGPR() ||
                            tmpTerm->isImmediateRegister()) {
                            sourceTerm =
                                dynamic_cast<TTAProgram::TerminalRegister*>(
                                tmpTerm);
                            break;
                        }
                        if (tmpTerm->isFUPort()) {
                            indirectJump(
                                leaders, leaderAddr,
                                dataCodeRellocations, instruction,
                                procedure);
                            return;
                        }
                    }
                }
            }
        }
    } else {
        if (instruction.move(moveIndex).source().isImmediateRegister()) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Source of immediate write not found!");
        }
        indirectJump(
            leaders, leaderAddr, dataCodeRellocations,
            instruction, procedure);
            return;
    }
}

/**
 * Returns basic statistics about control flow graph as a string.
 *
 * @return String with basic statistics about control flow graph.
 */
std::string
ControlFlowGraph::printStatistics() {
    const CFGStatistics& stats = statistics();
    
    std::string result = "";
    result += (boost::format("Procedure '%s' has %d moves, %d immediate"
        " writes, %d instructions and %d bypassed moves in %d basic blocks.")
        %procedureName() % stats.moveCount() % stats.immediateCount()
        % stats.instructionCount() % stats.bypassedCount() 
        % stats.normalBBCount()).str();
    result += (boost::format("\n\tLargest basic block has %d moves, %d"
        " immediate writes, %d instructions and %d bypassed moves.")
        % stats.maxMoveCount() % stats.maxImmediateCount() 
        % stats.maxInstructionCount() % stats.maxBypassedCount()).str();
    return result;
}

/**
 * Returns basic statistics about control flow graph in statistic object.
 *
 * @return Object with basic statistics about control flow graph.
 */
const CFGStatistics&
ControlFlowGraph::statistics() {  
    
    CFGStatistics* result = new CFGStatistics;
    int moveCount = 0;
    int immediateCount = 0;
    int instructionCount = 0;
    int bypassCount = 0;
    int normalBBCount = 0;
    int maxMoveCount = 0;
    int immediateCountInMax = 0;
    int instructionCountInMax = 0;
    int bypassCountInMax = 0;
    for (int i = 0; i < nodeCount(); i++) {
        if (node(i).isNormalBB()) {
            const BasicBlockStatistics& stats = node(i).statistics();
            moveCount += stats.moveCount();
            immediateCount += stats.immediateCount();
            instructionCount += stats.instructionCount();
            bypassCount += stats.bypassedCount();
            normalBBCount++;
            if (stats.moveCount() > maxMoveCount) {
                maxMoveCount = stats.moveCount();
                immediateCountInMax = stats.immediateCount();
                instructionCountInMax = stats.instructionCount();
                bypassCountInMax = stats.bypassedCount();
            }
        }
    }

    result->setMoveCount(moveCount);
    result->setImmediateCount(immediateCount);
    result->setInstructionCount(instructionCount);
    result->setBypassedCount(bypassCount);
    result->setNormalBBCount(normalBBCount);
    result->setMaxMoveCount(maxMoveCount);
    result->setMaxInstructionCount(instructionCountInMax);
    result->setMaxImmediateCount(immediateCountInMax);
    result->setMaxBypassedCount(bypassCountInMax);
    return *result;
}
