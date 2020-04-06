/*
    Copyright (c) 2002-2015 Tampere University.

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
 * @file ControlFlowGraph.cc
 *
 * Implementation of prototype control flow graph of TTA program
 * representation.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @author Heikki Kultala 2011 (heikki.kultala-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011,2015
 * @note rating: red
 */

// LLVM_CPPFLAGS might disable debugging
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <vector>
#include <algorithm>
#include <functional>

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wunused-local-typedef")
#include <boost/graph/depth_first_search.hpp>
POP_CLANG_DIAGS
#include <llvm/CodeGen/MachineFunction.h>
#include <llvm/Target/TargetMachine.h>
#include "tce_config.h"
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetInstrInfo.h>
#else
#include <llvm/CodeGen/TargetInstrInfo.h>
#endif

#if defined(LLVM_3_2)
#include <llvm/Function.h>
#include <llvm/Module.h>
#else
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#if (!(defined(LLVM_3_3)) && !(defined(LLVM_3_4)) && !(defined(LLVM_3_5)))
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetSubtargetInfo.h>
#else
#include <llvm/CodeGen/TargetSubtargetInfo.h>
#endif
#endif
#endif
#include <llvm/MC/MCContext.h>
#pragma GCC diagnostic warning "-Wunused-parameter"

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
#include "TerminalRegister.hh"
#include "TerminalImmediate.hh"
#include "BasicBlock.hh"
#include "CFGStatistics.hh"
#include "Conversion.hh"
#include "InterPassData.hh"
#include "InterPassDatum.hh"
#include "TerminalInstructionReference.hh"
#include "TerminalProgramOperation.hh"
#include "CodeGenerator.hh"
#include "UniversalMachine.hh"
#include "Guard.hh"
#include "DataDependenceGraph.hh"
#include "TerminalFUPort.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "LiveRangeData.hh"

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
using TTAProgram::ProgramAnnotation;
using TTAProgram::CodeGenerator;
using TTAMachine::SpecialRegisterPort;
using TTAMachine::Port;
using TTAMachine::FunctionUnit;
using TTAMachine::ControlUnit;

//#define DEBUG_BB_OPTIMIZER

/**
 * Removes nodes and edge from the graph.
 *
 * Removal of nodes automatically frees also the edges.
 *
 * @todo: this routine is O(n�)
 */
ControlFlowGraph::~ControlFlowGraph() {
    while (nodeCount() > 0) {
        BasicBlockNode* b = &node(0);
        removeNode(*b);
        delete b;
    }
}

/**
 * Create empty CFG with given name
 *
 */
ControlFlowGraph::ControlFlowGraph(
    const TCEString name,
    TTAProgram::Program* program) :
    BoostGraph<BasicBlockNode, ControlFlowEdge>(name),
    program_(program),
    startAddress_(TTAProgram::NullAddress::instance()),
    endAddress_(TTAProgram::NullAddress::instance()),
    passData_(NULL) {
    procedureName_ = name;
}

/**
 * Read a procedure of TTA program and build a control flow graph
 * out of it.
 *
 * Create a control flow graph using the procedure of POM. Procedure must be
 * registered in Program to get access to relocations.
 */
ControlFlowGraph::ControlFlowGraph(
    const TTAProgram::Procedure& procedure) :
    BoostGraph<BasicBlockNode, ControlFlowEdge>(procedure.name()),
    program_(NULL),
    procedure_(&procedure),
    startAddress_(TTAProgram::NullAddress::instance()),
    endAddress_(TTAProgram::NullAddress::instance()),
    passData_(NULL) {
    buildFrom(procedure);
}

/**
 * Read a procedure of TTA program and build a control flow graph
 * out of it.
 *
 * A version that allows adding inter pass data with additional data
 * to add to the CFG (currently inner loop BB trip counts).
 *
 * Create a control flow graph using the procedure of POM. Procedure must be
 * registered in Program to get access to relocations.
 */
ControlFlowGraph::ControlFlowGraph(
    const TTAProgram::Procedure& procedure,
    InterPassData& passData) :
    BoostGraph<BasicBlockNode, ControlFlowEdge>(procedure.name()),
    program_(NULL),
    procedure_(&procedure),
    startAddress_(TTAProgram::NullAddress::instance()),
    endAddress_(TTAProgram::NullAddress::instance()),
    passData_(&passData) {
    buildFrom(procedure);
}

/**
 * Constructs the CFG from the given procedure.
 */
void
ControlFlowGraph::buildFrom(const TTAProgram::Procedure& procedure) {
    procedure_ = &procedure;
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
    /// While finding jump successors, also test if there is indirect jump
    /// in procedure, in such case also rellocations are used to find
    /// basic block starts.
    if (computeLeadersFromJumpSuccessors(leaders, procedure)) {
        computeLeadersFromRelocations(
            leaders, dataCodeRellocations, procedure);
    }

    createAllBlocks(leaders, procedure);

    // Creates edges between basic blocks
    createBBEdges(procedure, leaders, dataCodeRellocations);
    detectBackEdges();
    addExit();
}

/**
 * Creates edges between basic blocks.
 *
 * @param procedure Procedure that holds the instructions.
 * @param leaders Leader instructions, first instructions in basic blocks.
 * @param dataCodeRellocations Set of dataCodeRellocations that applies to the
 *                             given procedure.
 */
void
ControlFlowGraph::createBBEdges(
    const TTAProgram::Procedure& procedure,
    InstructionAddressMap& leaders,
    InstructionAddressMap& dataCodeRellocations) {

    InstructionAddress leaderAddr;
    bool hasCFMove = false;

    InstructionAddress exitAddr = 0;
    bool hasExit = false;
    if (procedure.isInProgram()) {
        /// Look for __exit procedure, if it is found, calls to it will not
        /// have fall through edges in CFG to avoid possible endless loops
        /// and create possible unreachable basic blocks

        if (program_->hasProcedure("__exit")) {
            exitAddr =
                program_->procedure("__exit").firstInstruction().address().
                location();
            hasExit = true; 
        } else {
            /// The __exit procedure was not found, we do not detect calls to
            /// __exit in calls
            hasExit = false;
        }
    }

    const int iCount = procedure.instructionCount();
    for (int insIndex = 0; insIndex < iCount; insIndex++) {
        Instruction* instruction = &procedure.instructionAtIndex(insIndex);
        InstructionAddress addr = 
            procedure.startAddress().location() + insIndex;

        if (MapTools::containsKey(leaders, addr)) {
            leaderAddr = addr;
            hasCFMove = false;
        }
        // We only deal with one JUMP or CALL per instruction,
        // there is restriction that there can be no control flow
        // operations in delay slots of previous operation
        for (int i = 0, count = instruction->moveCount(); i < count; i++) {
            if (instruction->move(i).isCall()) {
                // There is some CF move in a basic block
                hasCFMove = true;
                int nextIndex = findNextIndex(procedure, insIndex, i);
                if (iCount > nextIndex) {
                    /// Do not create fall through edge after call to __exit
                    if (hasExit &&
                        instruction->move(i).source().isInstructionAddress()) {
                        TTAProgram::TerminalInstructionReference* address =
                        dynamic_cast<TTAProgram::TerminalInstructionReference*>
                            (&instruction->move(i).source());
                        Instruction& destination =
                            address->instructionReference().instruction();
                        if (destination.address().location() == exitAddr) {
                           break;
                        }
                    }
                    Instruction& iNext = 
                        procedure.instructionAtIndex(nextIndex);
                    createControlFlowEdge(
                        *leaders[leaderAddr], iNext,
                        ControlFlowEdge::CFLOW_EDGE_NORMAL,
                        ControlFlowEdge::CFLOW_EDGE_CALL);

                }
                break;
            }
            if (instruction->move(i).isJump()) {
                // There is some CF move in basic block
                hasCFMove = true;
                createJumps(
                    leaders, leaderAddr, dataCodeRellocations,
                    procedure, insIndex, i);
                continue;
            }
        }
        if (iCount > insIndex+1) {
            Instruction& nextInstruction =
                procedure.instructionAtIndex(insIndex+1);
            // Look if next instruction is beginning of basic block
            // and if there was no CF move in current basic block
            // add fall true edge in such case
            int nextInsAddr = procedure.startAddress().location() + insIndex+1;
            if (hasCFMove == false &&
                MapTools::containsKey(leaders, nextInsAddr)) {
                BasicBlockNode& blockSource(*blocks_[leaderAddr]);
                BasicBlockNode& blockTarget(
                    *blocks_[nextInsAddr]);
                if (!hasEdge(blockSource, blockTarget)) {
                    createControlFlowEdge(
                        *leaders[leaderAddr],nextInstruction,
                        ControlFlowEdge::CFLOW_EDGE_NORMAL,
                        ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
                }
            }
        } else {
            break;
        }
    }
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

    // this can get slow if there are zillion instructionreferences?
    for (InstructionReferenceManager::Iterator i = refManager.begin();
         i != refManager.end(); ++i) {
        Instruction& instruction = i->instruction();
        InstructionAddress insAddr = instruction.address().location();
        if (insAddr > startAddress_.location() &&
            insAddr < endAddress_.location()) {
            assert(&instruction.parent() == &procedure);
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
 * Also detect if any of jumps is indirect, which will require data-code
 * rellocation information for creating jumps as well.
 *
 * @param leaders Set of leader instructions to update.
 * @param procedure The procedure to analyse.
 * @return true indicates there is indirect jump in procedure
 */
bool
ControlFlowGraph::computeLeadersFromJumpSuccessors(
    InstructionAddressMap& leaders,
    const Procedure& procedure) {
    
    bool indirectJump = false;
    // record target instructions of jumps, because they are
    // leaders of basic block too
    InstructionAddressMap targets;
    // The procedure start point is always a block leader.

    const unsigned int iCount = procedure.instructionCount();
    for (unsigned int insIndex = 0;insIndex < iCount;) {
        Instruction* instruction = &procedure.instructionAtIndex(insIndex);
        // Only one control flow operation per cycle
        bool increase = true;
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
                if (m.isJump() && 
                     (m.source().isGPR() || 
                      m.source().isImmediateRegister())) {
                    indirectJump = true;
                }
                increase = false;
                unsigned int nextIndex = findNextIndex(procedure, insIndex, i);
                if (iCount > nextIndex) {
                    insIndex = nextIndex;
                    instruction = &procedure.instructionAtIndex(insIndex);
                    leaders[instruction->address().location()] = instruction;
                } else {
                    return indirectJump; // end of procedure.
                }
                break;
            }
        }
        if (increase) {
            insIndex++;
        }
    }
    return indirectJump;
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
    Instruction& leader,
    const Instruction& endBlock) {

    InstructionAddress blockStart = leader.address().location();
    InstructionAddress blockEnd = endBlock.address().location();

    CodeSnippet& proc = leader.parent();

    unsigned int blockStartIndex = blockStart - proc.startAddress().location();
    unsigned int blockEndIndex = blockEnd - proc.startAddress().location();

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

    for (unsigned int i = blockStartIndex; i <= blockEndIndex; i++) {
        Instruction *newInstruction = proc.instructionAtIndex(i).copy();
        node->basicBlock().add(newInstruction);
    }

    addNode(*node);
    // Create entry node and add edge from it into current BB
    // if it's start is also start of procedure
    if (leader.address().location() == startAddress_.location()){
        BasicBlockNode* entry = new BasicBlockNode(0, 0, true);
        addNode(*entry);
        ControlFlowEdge* edge = new ControlFlowEdge;//(edgeCount());
        connectNodes(*entry, *node, *edge);
    }

    if (leader.hasAnnotations(TTAProgram::ProgramAnnotation::ANN_LOOP_INNER)) {
        node->basicBlock().setInInnerLoop();
        node->basicBlock().setTripCount(0); // 0 indicates unknown trip count
        
        if (leader.hasAnnotations(
                TTAProgram::ProgramAnnotation::ANN_LOOP_TRIP_COUNT)) {

            unsigned tripCount =
                static_cast<unsigned>(
                    leader.annotation(
                        0, TTAProgram::ProgramAnnotation::ANN_LOOP_TRIP_COUNT).
                    intValue());
            if (tripCount > 0) {
                node->basicBlock().setTripCount(tripCount);
            }
        }
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
 * @param edgeType Defines if edge represents jump or call or fall-through,
 * default jump
 * @return The created control flow edge.
 */
ControlFlowEdge&
ControlFlowGraph::createControlFlowEdge(
    const Instruction& iTail,
    const Instruction& iHead,
    ControlFlowEdge::CFGEdgePredicate edgePredicate,
    ControlFlowEdge::CFGEdgeType edgeType) {

    InstructionAddress sourceAddr = iTail.address().location();
    InstructionAddress targetAddr = iHead.address().location();
    if (!MapTools::containsKey(blocks_, sourceAddr)) {
        if (Application::verboseLevel() >= 1) {
            TCEString msg = (boost::format(
                "Source instruction %d:%s\nDestination instruction %d:%s\n")
                % Conversion::toString(sourceAddr)
                % POMDisassembler::disassemble(iTail)
                % Conversion::toString(targetAddr)
                % POMDisassembler::disassemble(iHead)).str();
            Application::logStream() << msg;
        }
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Source basic block is missing!");
    }
    if (!MapTools::containsKey(blocks_, targetAddr)) {
        if (Application::verboseLevel() >= 1) {
            TCEString msg =(boost::format(
                "Source instruction %d:%s\nDestination instruction %d:%s\n")
                % Conversion::toString(sourceAddr)
                % POMDisassembler::disassemble(iTail)
                % Conversion::toString(targetAddr)
                % POMDisassembler::disassemble(iHead)).str();
            Application::logStream() << msg;
        }
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Destination basic block is missing!");
    }

    BasicBlockNode& blockSource(*blocks_[sourceAddr]);
    BasicBlockNode& blockTarget(*blocks_[targetAddr]);

    ControlFlowEdge* theEdge;
    theEdge = new ControlFlowEdge(edgePredicate, edgeType);

    if (edgeType == ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH) {
        assert(blockSource.originalEndAddress() +1 ==
               blockTarget.originalStartAddress());
    }

    connectNodes(blockSource, blockTarget, *theEdge);
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
    int insIndex, int cfMoveIndex,
    const TTAProgram::Instruction& instructionTarget,
    const TTAProgram::Procedure& procedure) {

    Instruction& instruction = procedure.instructionAtIndex(insIndex);
    // find other jumps from same ins.
    bool hasAnotherJump = hasInstructionAnotherJump(instruction, cfMoveIndex);
    TTAProgram::Move& move = instruction.move(cfMoveIndex);
    InstructionAddress targetAddr = instructionTarget.address().location();
    if (!MapTools::containsKey(leaders, targetAddr)) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Target basic block of jump is missing!");
    }

    if (!move.isUnconditional()) {
        // if jump is conditional we consider guard
        // if we add also fall-through edge to next block,
        // for inverted value of guard
        // no other jump in same BB.

        Instruction* iNext = NULL;
        if (!hasAnotherJump) {
            int nextIndex = findNextIndex(procedure, insIndex, cfMoveIndex);
            if (nextIndex >= procedure.instructionCount()) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format(
                         "Fall-through of jump missing:"
                         "Address: %d jump: %s\n") 
                     % (procedure.startAddress().location() + insIndex)
                     % POMDisassembler::disassemble(instruction)).str());
            }
            iNext = &procedure.instructionAtIndex(nextIndex);
            InstructionAddress nextAddr = 
                procedure.startAddress().location() + nextIndex;
            if (!MapTools::containsKey(leaders, nextAddr)) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    "Fall through basic block is missing!");
            }
        }
        if (move.guard().isInverted()) {
            // jumps on !bool, fall-through on bool
            createControlFlowEdge(
                *leaders[leaderAddr], instructionTarget,
                ControlFlowEdge::CFLOW_EDGE_FALSE);
            if (iNext != NULL) {
                createControlFlowEdge(
                    *leaders[leaderAddr], *iNext,
                    ControlFlowEdge::CFLOW_EDGE_TRUE,
                    ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
            }
        } else {
            createControlFlowEdge(
                *leaders[leaderAddr], instructionTarget,
                ControlFlowEdge::CFLOW_EDGE_TRUE);
            if (iNext != NULL) {
                createControlFlowEdge(
                    *leaders[leaderAddr], *iNext,
                    ControlFlowEdge::CFLOW_EDGE_FALSE,
                    ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
            }
        }
    } else {
        createControlFlowEdge(*leaders[leaderAddr], instructionTarget);
    }
}

/**
 * Tells whether an instruction has another jump in addition to the given.
 *
 * @param ins instruction to check
 * @param moveIndex index of the move triggering the known jump.
 */
bool 
ControlFlowGraph::hasInstructionAnotherJump(
    const Instruction& ins, int moveIndex) {
    for (int i = 0; i < ins.moveCount(); i++) {
        if (i != moveIndex && ins.move(i).isJump()) {
            return true;
        }
    }
    return false;
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
    int insIndex, int cfMoveIndex,
    const TTAProgram::Procedure& procedure) {

    Instruction& instruction = procedure.instructionAtIndex(insIndex);
    InstructionAddressMap::iterator dataCodeIterator =
        dataCodeRellocations.begin();
    bool hasAnotherJump = hasInstructionAnotherJump(instruction, cfMoveIndex);
    TTAProgram::Move& move = instruction.move(cfMoveIndex);

    if (move.hasAnnotations(ProgramAnnotation::ANN_JUMP_TO_NEXT)) {
        int nextIndex = findNextIndex(procedure, insIndex, cfMoveIndex);
        if (nextIndex < procedure.instructionCount()) {
            const Instruction& iNext = procedure.instructionAtIndex(nextIndex);
            createControlFlowEdge(
                *leaders[leaderAddr], iNext,
                ControlFlowEdge::CFLOW_EDGE_FAKE);
        }
        else {
            throw IllegalProgram(
                __FILE__,__LINE__,__func__,
                "Jump to next annotation without next instruction");
        }
        return;
    }

    ControlFlowEdge::CFGEdgePredicate edgePredicate =
        ControlFlowEdge::CFLOW_EDGE_NORMAL;
    ControlFlowEdge::CFGEdgePredicate fallPredicate =
        ControlFlowEdge::CFLOW_EDGE_NORMAL;
    if (instruction.move(cfMoveIndex).isUnconditional() == false) {
        if (instruction.move(cfMoveIndex).guard().isInverted()) {
            edgePredicate = ControlFlowEdge::CFLOW_EDGE_FALSE;
            fallPredicate = ControlFlowEdge::CFLOW_EDGE_TRUE;
        } else {
            edgePredicate = ControlFlowEdge::CFLOW_EDGE_TRUE;
            fallPredicate = ControlFlowEdge::CFLOW_EDGE_FALSE;
        }
    }

    if (!instruction.move(cfMoveIndex).isUnconditional() &&
        !hasAnotherJump) {
        int nextIndex = findNextIndex(procedure, insIndex, cfMoveIndex);
        InstructionAddress nextAddr = 
            procedure.startAddress().location() + nextIndex;
        if (nextIndex >= procedure.instructionCount() ||
            !MapTools::containsKey(leaders, nextAddr)) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Fall through basic block is missing!");
        }
        Instruction& iNext = procedure.instructionAtIndex(nextIndex);
        createControlFlowEdge(
                    *leaders[leaderAddr], iNext,fallPredicate,
                    ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
    }

    // Check if this jump is a return. 
    const Port* port =
        &instruction.move(cfMoveIndex).source().port();

    if (dynamic_cast<const SpecialRegisterPort*>(port) != NULL || 
        move.hasAnnotations(
            ProgramAnnotation::ANN_STACKFRAME_PROCEDURE_RETURN)) {
        returnSources_.insert(
            ReturnSource(leaderAddr, edgePredicate));
        return;
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
 * Finds the index of the next instruction after jump and delay slots
 * in the procedure. 
 *
 * Also checks for control flow moves in delay slots, and throws if found,
 * or if the procedure ends before all delays slots.
 *
 * @param procedure procedure we are processing
 * @param jumpInsIndex index of the jump inside in the procedure
 * @param index of the jump move in the jump instruction.
 */
unsigned int
ControlFlowGraph::findNextIndex(
    const TTAProgram::Procedure& proc, int jumpInsIndex, int jumpMoveIndex) {
    Instruction& instruction = proc.instructionAtIndex(jumpInsIndex);
    
    // POM allows mixed scheduled an unscheduled code, so each jump
    // must check from machine how many delay slots it needs
    FunctionUnit& unit = const_cast<FunctionUnit&>(
            instruction.move(jumpMoveIndex).destination().functionUnit());
    ControlUnit* control = dynamic_cast<ControlUnit*>(&unit);
    if (control == NULL) {
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, (boost::format(
            "Control flow move '%s' has destination unit %s, "
            "not global control unit!")
            % POMDisassembler::disassemble(instruction.move(jumpMoveIndex))
            % unit.name()).str());
    }
    int delaySlots = control->delaySlots();
    int nextIndex = jumpInsIndex + delaySlots + 1;
    if (nextIndex > proc.instructionCount()) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Procedure ends before all delay slot instructions");
    }
    // Then check for control flow instructions inside delay slots.
    for (int i = jumpInsIndex + 1; i < nextIndex; i++) {
        Instruction &dsIns = proc.instructionAtIndex(i);
        if (dsIns.hasControlFlowMove()) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                (boost::format(
                     "Control flow operation in delay slot"
                     " in %d! Instruction:\n%s")
                 % dsIns.address().location()
                 % POMDisassembler::disassemble(dsIns)
                    ).str());
        }
    }
    return nextIndex;
}

/**
 * Adds artificial block named 'Exit' to the graph
 */
void
ControlFlowGraph::addExit() {
    BasicBlockNode* exit = new BasicBlockNode(0, 0, false, true);
    addNode(*exit);

    // the actual code which inserts the exit on normal case.
    for (ReturnSourceSet::iterator i = returnSources_.begin();
         i != returnSources_.end(); i++) {

        InstructionAddress sourceAddr = i->first;
        if (!MapTools::containsKey(blocks_, sourceAddr)) {
            if (Application::verboseLevel() >= 1) {
                TCEString msg = (
                    boost::format(
                        "Source instr %d:%s\nDestination instr %d:%s\n")
                    % Conversion::toString(sourceAddr)
                    ).str();
                Application::logStream() << msg;
            }
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Source basic block of edge to exit is missing!");
        }
        BasicBlockNode& blockSource(*blocks_[sourceAddr]);
        ControlFlowEdge* theEdge = 
            new ControlFlowEdge(i->second, ControlFlowEdge::CFLOW_EDGE_JUMP);
        connectNodes(blockSource, *exit, *theEdge);
    } 

    addExitFromSinkNodes(exit);

}

void 
ControlFlowGraph::addExitFromSinkNodes(BasicBlockNode* exitNode) {
    
    // kludge needed for start and exit methods which do nto have ret inst.
    for (int i = 0; i < nodeCount(); i++) {
        BasicBlockNode& block(node(i));
        if (outDegree(block) == 0 && exitNode != &block) {
            ControlFlowEdge* edge = new ControlFlowEdge(
                ControlFlowEdge::CFLOW_EDGE_NORMAL,
                ControlFlowEdge::CFLOW_EDGE_CALL);
            connectNodes(block, *exitNode, *edge);
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
ControlFlowGraph::entryNode() const {
    BasicBlockNode* result = NULL;
    bool found = false;
    for (int i = 0; i < nodeCount(); i++) {
        if (inDegree(node(i)) == 0) {
            // sanity check
            if (!static_cast<BasicBlockNode&>(node(i)).isEntryBB()) {
                // probably the entry node is not present
                // or there are more nodes which are not reachable from
                // entry nodes... likely caused by frontend not doing
                // any of -O{1,2} optimizations (in case of gcc)
                continue;
            }
            if (found == true) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    "Corrupted graph. Found multiple entry nodes.");
            }
            result = &node(i);
            found = true;
        }
    }
    if (found == false || result == NULL) {
        TCEString errorMsg("Graph does not have entry node.");
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
    return *result;
}

BasicBlockNode& 
ControlFlowGraph::firstNormalNode() const {
    ControlFlowGraph::NodeSet entrySucc = successors(entryNode());
    if (entrySucc.size() != 1) {
	throw InvalidData(__FILE__,__LINE__,__func__,
			  "Entry node has not exactly one successor");
    }
    BasicBlockNode* firstNode = *entrySucc.begin();
    if (!firstNode->isNormalBB()) {
	throw InvalidData(__FILE__,__LINE__,__func__,
			  "Successor of entry node is not normal bb");
    }
    return *firstNode;
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
ControlFlowGraph::exitNode() const {

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
            result = &node(i);
            found = true;
        }
    }
    if (found == false || result == NULL || unlinkedExitNode == true) {
        TCEString errorMsg("Graph does not have exit node.");
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
            ControlFlowEdge::CFLOW_EDGE_TRUE);
        connectNodes(entry, *(fromEntry[i].first), *edge);
    }
    ControlFlowEdge* edge = new ControlFlowEdge(
        ControlFlowEdge::CFLOW_EDGE_FALSE);
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
        ControlFlowEdge* edge = new ControlFlowEdge; //(fromEntry[i].second);
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
TCEString
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
    const TTAProgram::Procedure& procedure,
    int insIndex,
    int moveIndex) {

    const Instruction& instruction = procedure.instructionAtIndex(insIndex);
    if (instruction.move(moveIndex).source().isInstructionAddress()) {
        Move* tmp = &instruction.move(moveIndex);
        directJump(
            leaders, leaderAddr, insIndex, moveIndex,
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
                            leaders, leaderAddr, insIndex, moveIndex,
                            tmpImm->value().instructionReference().
                            instruction(),
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
                                leaders, leaderAddr, insIndex, moveIndex,
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
                                dataCodeRellocations, insIndex, moveIndex,
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
            insIndex, moveIndex, procedure);
            return;
    }
}

/**
 * Returns basic statistics about control flow graph as a string.
 *
 * @return String with basic statistics about control flow graph.
 */
TCEString
ControlFlowGraph::printStatistics() {
    const CFGStatistics& stats = statistics();

    TCEString result = "";
    result += (boost::format("Procedure '%s' has %d moves, %d immediate"
        " writes, %d instructions and %d bypassed moves in %d basic blocks.")
        % procedureName() % stats.moveCount() % stats.immediateCount()
        % stats.instructionCount() % stats.bypassedCount()
        % stats.normalBBCount()).str();
    result += (boost::format("\n\tLargest basic block has %d moves, %d"
        " immediate writes, %d instructions and %d bypassed moves.\n")
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
            const TTAProgram::BasicBlockStatistics& stats = 
                node(i).statistics();
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


/**
 * Finds a node where control falls thru from the give node.
 *
 * @param bbn basic block node whose successor we are searching
 * @return node where control falls thru from given node or NULL if not exist.
 */
BasicBlockNode*
ControlFlowGraph::fallThruSuccessor(const BasicBlockNode& bbn) {
    if (bbn.isExitBB()) {
        return NULL;
    }

    EdgeSet oEdges = outEdges(bbn);
    for (EdgeSet::iterator i = oEdges.begin(); i != oEdges.end(); i++) {
        if ((*i)->isFallThroughEdge() || (*i)->isCallPassEdge()) {
            return &headNode(**i);
        }
    }
    return NULL;
}

/**
 * Returns true if given basic blocks has a predecessor which
 * falls thru to it.
 *
 * @param bbn bbn to check for fall-thru predecessors
 * @return if control can fall-thru to this BB.
 */
bool ControlFlowGraph::hasFallThruPredecessor(const BasicBlockNode& bbn) {
    EdgeSet iEdges = inEdges(bbn);
    for (EdgeSet::iterator i = iEdges.begin(); i != iEdges.end(); i++) {
        if ((*i)->isFallThroughEdge() || (*i)->isCallPassEdge()) {
            return true;
        }
    }
    return false;
}

/**
 * Does a breadth first search to find all reachable nodes.
 */
ControlFlowGraph::NodeSet
ControlFlowGraph::findReachableNodes() {
    NodeSet queuedBBs;
    NodeSet processedBBs;

    ControlFlowGraph::NodeSet firstBBs = successors(entryNode());
    AssocTools::append(firstBBs,queuedBBs);

    while (queuedBBs.size() != 0) {
        BasicBlockNode& current = **queuedBBs.begin();
        if (current.isNormalBB()) {
            processedBBs.insert(&current);
            NodeSet succs = successors(current);
            for (NodeSet::iterator i = succs.begin(); i != succs.end(); i++) {
                if (!AssocTools::containsKey(processedBBs,*i)) {
                    queuedBBs.insert(*i);
                }
            }
            processedBBs.insert(&current);
        }
        queuedBBs.erase(&current);
    }
    return processedBBs;
}

/**
 * Copies the CFG into a procedure. 
 *
 * Clears the procedure and replaces all instructions in it with ones 
 * in CFG. Tries to get rid of some unnecessary jumps.
 *
 * @param proc procedure where the copy the cfg.
 */
void
ControlFlowGraph::copyToProcedure(
    TTAProgram::Procedure& proc, InstructionReferenceManager* irm) {

    // todo: make sure not indeterministic.
    // two-way maps between copied and in cfg instructions.
    typedef std::map<TTAProgram::Instruction*,TTAProgram::Instruction*>
        InsMap;
    InsMap copiedInsFromCFG;

    std::vector<Instruction*> oldInstructions;

    int jumpsRemoved = 0;
    ControlFlowGraph::NodeSet firstBBs = successors(entryNode());
    assert(firstBBs.size() == 1);
    BasicBlockNode* firstBBN = *firstBBs.begin();
    BasicBlockNode* currentBBN = firstBBN;

    // fix refs to old first to point to first in cfg - later fixed to
    // first in program
    if (irm == NULL) {
        irm = &program_->instructionReferenceManager();
        assert(irm != NULL);
    }
    if (!firstBBN->isNormalBB()) {
        std::cerr << "First Basic block is not normal basic block. "
                  << "This is propably due function that is completely empty,"
                  <<  " not containg even return jump. The cause of this "
                  << "might be LLVM optimizing away code it considers dead."
                  << std::endl
                  << "Control flow graph written to empty_fn.dot" 
                  << std::endl;
        writeToDotFile("empty_fn.dot");
    }        
    assert(firstBBN->isNormalBB());

    // procedure should not have any references.
    for (int i = 0; i < proc.instructionCount(); i++) {
        assert(!irm->hasReference(proc.instructionAtIndex(i)));
    }

    proc.clear();

    // find and queue reachable nodes
    NodeSet queuedNodes = findReachableNodes();
    NodeSet unreachableNodes = findUnreachableNodes(queuedNodes);

    // then loop as long as we have BBs which have not been written to
    // the procedure.
    while (currentBBN != NULL) {
        BasicBlockNode* nextNode = NULL;
        TTAProgram::BasicBlock& bb = currentBBN->basicBlock();
        // todo: if refs to skipped instructions, breaks?

        for (int i = 0; i < bb.skippedFirstInstructions(); i++) {
            Instruction& ins = bb.instructionAtIndex(i);
            if (irm->hasReference(ins)) {
                std::cerr << "\tSkipped inst has refs, proc: " << proc.name()
                          << " index: " << i << std::endl;
                writeToDotFile("skipped_has_ref.dot");
                PRINT_VAR(bb.toString());
            }
            assert(!irm->hasReference(ins));
        }

        // copy instructions of a BB to procedure.
        for (int i = bb.skippedFirstInstructions();
             i < bb.instructionCount(); i++) {
            Instruction* ins = &bb.instructionAtIndex(i);
            Instruction* copiedIns = ins->copy();
            copiedInsFromCFG[ins] = copiedIns;

            // CodeSnippet:: is a speed optimization here.
            // only later fix the addresses of followind functions.
            proc.CodeSnippet::add(copiedIns);
        }

        queuedNodes.erase(currentBBN);

        // then start searching for the next node.

        // if has fall-turu-successor, select it so no need to add
        // extra jump
        BasicBlockNode* ftNode = fallThruSuccessor(*currentBBN);
        if (ftNode != NULL && ftNode->isNormalBB()) {

            if (queuedNodes.find(ftNode) == queuedNodes.end()) {
                std::cerr << "not-queued fall-thru: " << ftNode->toString()
                          << " current: " << currentBBN->toString() << 
                    std::endl;
                writeToDotFile("copyToProcedureFallThruBBNotQueued.dot");
            }
            // must not be already processed.
            assert(queuedNodes.find(ftNode) != queuedNodes.end());
            currentBBN = ftNode;
            continue;
        }

        // Select some node, preferably successors without ft-preds
        // The jump can then be removed.
        EdgeSet oEdges = outEdges(*currentBBN);
        for (EdgeSet::iterator i = oEdges.begin(); i != oEdges.end(); i++) {
            ControlFlowEdge& e = **i;
            BasicBlockNode& head = headNode(e);
            if (!hasFallThruPredecessor(head) && head.isNormalBB() &&
                queuedNodes.find(&head) != queuedNodes.end()) {
                // try to remove the jump as it's jump to the next BB.
                RemovedJumpData rjd = removeJumpToTarget(
                    proc, head.basicBlock().firstInstruction(),
                    proc.instructionCount() - 
                    (bb.instructionCount() - bb.skippedFirstInstructions()));
                if (rjd != JUMP_NOT_REMOVED) {
                    jumpsRemoved++;
                    // if BB got empty,
                    // move refs to beginning of the next BB.
                    if (rjd == LAST_ELEMENT_REMOVED) {
                        Instruction& ins = bb.instructionAtIndex(0);
                        if (irm->hasReference(ins)) {
                            irm->replace(
                                ins, head.basicBlock().instructionAtIndex(
                                    head.basicBlock().
                                    skippedFirstInstructions()));
                        }
                    }
                    // we removed a jump so convert the jump edge into
                    // fall-through edge.
                    ControlFlowEdge* ftEdge = new ControlFlowEdge(
                        e.edgePredicate(), 
                        ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
                    removeEdge(e);
                    connectNodes(*currentBBN, head, *ftEdge);
                    nextNode = &head;
                    break;
                }
            }
        }

        // need to select SOME node as successor.
        // first without ft-predecessor usually is a good candidate.
        // smarter heuristic does not seem to help at all.
        // try to select
        if (nextNode == NULL) {
            bool ftPred = false;
            for (NodeSet::iterator i = queuedNodes.begin();
                 i != queuedNodes.end(); i++) {
                if (!hasFallThruPredecessor(**i)) {
                    nextNode = *i;
                    break;
                } else {
                    ftPred = true;
                }
            }
            
            // unreachable node having ft may have prevented us from
            // managing some node whose fall-thru succs prevent
            // futher nodes. try to select some unreached node.
            if (nextNode == NULL && ftPred) {
                for (NodeSet::iterator i = unreachableNodes.begin();
                     i != unreachableNodes.end(); i++) {
                    if (fallThruSuccessor(**i) != NULL) {
                        nextNode = *i;
                        unreachableNodes.erase(*i);
                        break;
                    }
                }
            }

            // did not help. we cannot select node which has
            // fall-thru predecessor.
            if (nextNode == NULL && ftPred) {
                writeToDotFile(
                    "CopyToProcedure_multiple_fall_thorough_nodes.dot");
                assert(0 && "CFG may have multiple fall-thorough nodes!");
            }
        }
        currentBBN = nextNode;
    }

    // now all instructions are copied.

    // this can happen in indeterministic order.
    // but it should not cause any indeterministicity
    // effects on the schedule.

    // Update refs from cfg into final program
    // only works for refs
    for (InsMap::iterator i = copiedInsFromCFG.begin();
         i != copiedInsFromCFG.end(); i++) {
        std::pair<Instruction*,Instruction*> insPair = *i;
        if (irm->hasReference(*insPair.first)) {
            irm->replace(*insPair.first, *insPair.second);
        }
    }

    // move the following procedures to correct place
    if (proc.instructionCount() != 0 && proc.isInProgram()) {
        if (!(&proc == &proc.parent().lastProcedure())) {
            proc.parent().moveProcedure(
                proc.parent().nextProcedure(proc),
                proc.instructionCount());
        }
    }

    // make sure no refs to dead code?
/*
    for (NodeSet::iterator i = unreachableNodes.begin();
         i != unreachableNodes.end(); i++) {
        BasicBlockNode& bbn = **i;
        if (bbn.isNormalBB()) {
            BasicBlock& bb = bbn.basicBlock();
            for (int i = 0; i < bb.instructionCount();i++) {
                Instruction& ins = bb.instructionAtIndex(i);
                assert(!irm.hasReference(ins));
            }
        }
    }
*/
}


/**
 * Copies the CFG into an LLVM MachineFunction.
 *
 * Assumes an operation triggered target and that all scheduler restrictions
 * to produce valid code for such an target have been enabled in ADF while
 * producing the schedule.
 *
 * @param mf The MachineFunction where to copy the cfg.
 * @param irm InstructionReferenceManager for resolving instruction refs.
 */

void 
ControlFlowGraph::copyToLLVMMachineFunction(
    llvm::MachineFunction& mf,
    TTAProgram::InstructionReferenceManager* irm) {

    // todo: make sure not indeterministic.
    // two-way maps between copied and in cfg instructions.
    typedef std::map<TTAProgram::Instruction*,TTAProgram::Instruction*>
        InsMap;
    InsMap copiedInsFromCFG;

    std::vector<Instruction*> oldInstructions;

    ControlFlowGraph::NodeSet firstBBs = successors(entryNode());
    assert(firstBBs.size() == 1);
    BasicBlockNode* firstBBN = *firstBBs.begin();
    BasicBlockNode* currentBBN = firstBBN;

    // fix refs to old first to point to first in cfg - later fixed to
    // first in program
    if (irm == NULL) {
        irm = &program_->instructionReferenceManager();
        assert(irm != NULL);
    }
    assert(firstBBN->isNormalBB());

#if 0
    // procedure should not have any references.
    for (int i = 0; i < proc.instructionCount(); i++) {
        assert(!irm->hasReference(proc.instructionAtIndex(i)));
    }
#endif
        
    while (!mf.empty())
        mf.erase(mf.begin());

    // find and queue reachable nodes
    NodeSet queuedNodes = findReachableNodes();
    NodeSet unreachableNodes;

    // find dead nodes
    for (int i = 0; i < nodeCount(); i++) {
        BasicBlockNode& n = node(i);
        if (!AssocTools::containsKey(queuedNodes,&n) &&
            n.isNormalBB()) {
            unreachableNodes.insert(&n);
        }
    }

    /// This loop now only creates empty basic blocks in same order as they were
    /// transfered from LLVM to POM previously.
    /// Actuall copying of the content is done afterwords.
    while (currentBBN != NULL) {
        BasicBlockNode* nextNode = NULL;
        TTAProgram::BasicBlock& bb = currentBBN->basicBlock();

        /// This will create MachineBasicblock corresponding to BB if it does
        /// not exists already.
        getMBB(mf, bb);

        queuedNodes.erase(currentBBN);

        // then start searching for the next node.

        // if has fall-thru-successor, select it so no need to add
        // extra jump
        BasicBlockNode* ftNode = fallThruSuccessor(*currentBBN);
        if (ftNode != NULL && ftNode->isNormalBB()) {

            if (queuedNodes.find(ftNode) == queuedNodes.end()) {
                std::cerr << "not-queued fall-thru: " << ftNode->toString()
                          << " current: " << currentBBN->toString() << 
                    std::endl;
                writeToDotFile("copyToProcedureFallThruBBNotQueued.dot");
            }
            // must not be already processed.
            assert(queuedNodes.find(ftNode) != queuedNodes.end());
            currentBBN = ftNode;
            continue;
        }

        // Select some node, preferably successors without ft-preds
        // The jump can then be removed.
        EdgeSet oEdges = outEdges(*currentBBN);

        // need to select SOME node as successor.
        // first without ft-predecessor usually is a good candidate.
        // smarter heuristic does not seem to help at all.
        // try to select
        if (nextNode == NULL) {
            bool ftPred = false;
            for (NodeSet::iterator i = queuedNodes.begin();
                 i != queuedNodes.end(); i++) {
                if (!hasFallThruPredecessor(**i)) {
                    nextNode = *i;
                    break;
                } else {
                    ftPred = true;
                }
            }
            
            // unreachable node having ft may have prevented us from
            // managing some node whose fall-thru succs prevent
            // futher nodes. try to select some unreached node.
            if (nextNode == NULL && ftPred) {
                for (NodeSet::iterator i = unreachableNodes.begin();
                     i != unreachableNodes.end(); i++) {
                    if (fallThruSuccessor(**i) != NULL) {
                        nextNode = *i;
                        unreachableNodes.erase(*i);
                        break;
                    }
                }
            }

            // did not help. we cannot select node which has
            // fall-thru predecessor.
            if (nextNode == NULL && ftPred) {
                writeToDotFile(
                    "CopyToProcedure_multiple_fall_thorough_nodes.dot");
                assert(0 && "CFG may have multiple fall-thorough nodes!");
            }
        }
        currentBBN = nextNode;
    }

    // now all instructions are copied.

    // this can happen in indeterministic order.
    // but it should not cause any indeterministicity
    // effects on the schedule.

    // Update refs from cfg into final program
    // only works for refs
    // TODO: Is this really necessary or usefull here?
    for (InsMap::iterator i = copiedInsFromCFG.begin();
         i != copiedInsFromCFG.end(); i++) {
        std::pair<Instruction*,Instruction*> insPair = *i;
        if (irm->hasReference(*insPair.first)) {
            irm->replace(*insPair.first, *insPair.second);
        }
    }

    /// Fill in created machine basic blocks with machine instructions
    /// based on corresponding basic blocks.
    unsigned int nCount = nodeCount();
    for (unsigned int j = 0; j < nCount; j++) {
        TTAProgram::BasicBlock& bb = node(j).basicBlock();
        llvm::MachineBasicBlock* mbb = &getMBB(mf, bb);
        buildMBBFromBB(*mbb, bb);  
    }

    /// Add the dummy instructions denoting labels to instructions
    /// that are not basic block starts. This is only for the SPU's 
    /// branch hint instructions at the moment. It instantiates
    /// an LLVM/SPU-backend-specific dummy instruction HBR_LABEL at 
    /// the moment.
    for (std::set<std::pair<ProgramOperationPtr, llvm::MCSymbol*> >::const_iterator i = 
             tpos_.begin(); i != tpos_.end(); ++i) {
        ProgramOperationPtr po = (*i).first;
        llvm::MCSymbol* symbol = (*i).second;
        assert(programOperationToMIMap_.find(po.get()) != programOperationToMIMap_.end());
        llvm::MachineInstr* mi = programOperationToMIMap_[po.get()];
        assert(mi != NULL);
#if (defined(LLVM_3_2) || defined(LLVM_3_3) || defined(LLVM_3_4) || defined(LLVM_3_5))
        const llvm::TargetInstrInfo& tii = *mf.getTarget().getInstrInfo();
#elif (defined (LLVM_3_6))
        const llvm::TargetInstrInfo& tii = 
            *mf.getTarget().getSubtargetImpl()->getInstrInfo();
#elif LLVM_OLDER_THAN_6_0
        const llvm::TargetInstrInfo& tii = 
            *mf.getTarget().getSubtargetImpl(*mf.getFunction())->getInstrInfo();
#else
        const llvm::TargetInstrInfo& tii =
            *mf.getTarget().getSubtargetImpl(mf.getFunction())->getInstrInfo();
#endif
        const llvm::MCInstrDesc& tid =
            findLLVMTargetInstrDesc("HBR_LABEL", tii);
        llvm::MachineInstr* labelInstruction = 
            mf.CreateMachineInstr(tid, llvm::DebugLoc());
        labelInstruction->addOperand(
            llvm::MachineOperand::CreateMCSymbol(symbol));
        mi->getParent()->insert(
            llvm::MachineBasicBlock::instr_iterator (mi), labelInstruction);        
    }
    tpos_.clear();
    programOperationToMIMap_.clear();
    /// Based on CFG edges, add successor information to the generated
    /// machine function.
    unsigned int eCount = edgeCount();
    for (unsigned int i = 0; i < eCount; i++) {
        ControlFlowEdge& testEdge = edge(i);
        if (!headNode(testEdge).isNormalBB() ||
            !tailNode(testEdge).isNormalBB())
            continue;
        
        llvm::MachineBasicBlock& hNode =
            getMBB(mf, headNode(testEdge).basicBlock());
        llvm::MachineBasicBlock& tNode =
            getMBB(mf, tailNode(testEdge).basicBlock());
        if (hNode.isSuccessor(&tNode))
            continue;
        tNode.addSuccessor(&hNode);
    }

}

//#define DEBUG_POM_TO_MI

/**
 * Finds the TargetInstrDesc for the given LLVM instruction name.
 */
const llvm::MCInstrDesc& 
ControlFlowGraph::findLLVMTargetInstrDesc(
    TCEString name, 
    const llvm::MCInstrInfo& tii) const {
    for (unsigned opc = 0; opc < tii.getNumOpcodes(); ++opc) {
#if LLVM_OLDER_THAN_4_0
        if (name.ciEqual(tii.getName(opc))) {
#else
        if (name.ciEqual(tii.getName(opc).str())) {
#endif
            return tii.get(opc);
        }
    }
    abortWithError(TCEString("Could not find ") << name << " in the TII.");
}

void
ControlFlowGraph::buildMBBFromBB(
    llvm::MachineBasicBlock& mbb,
    const TTAProgram::BasicBlock& bb) const {

#ifdef DEBUG_POM_TO_MI
    Application::logStream()
        << "TTA instructions:" << std::endl 
        << bb.toString() << std::endl << std::endl
        << "OTA instructions:" << std::endl;
#endif

    /* Find the target machine from an instruction link. Ugly,
       should probably pass it as a parameter instead. */
    const TTAMachine::Machine* mach = NULL;
    for (int i = bb.skippedFirstInstructions(); i < bb.instructionCount(); 
         ++i) {
        const TTAProgram::Instruction& instr = 
            bb.instructionAtIndex(i);
        if (!instr.isNOP()) {
            mach = instr.move(0).bus().machine();
            break;
        }
    }
    if (mach == NULL)
        return; // The BB has only NOPs. Empty MBB is correct already.

    // the order of function unit operations in the instruction bundle
    typedef std::vector<const TTAMachine::FunctionUnit*> BundleOrderIndex;
    BundleOrderIndex bundleOrder;

    // Currently the bundle order is hard coded to the order of appearance
    // in the ADF file.
    for (int fuc = 0; fuc < mach->functionUnitNavigator().count(); ++fuc) {
        TTAMachine::FunctionUnit* fu = mach->functionUnitNavigator().item(fuc);
        bundleOrder.push_back(fu);
    }

    for (int i =  bb.skippedFirstInstructions(); i < bb.instructionCount(); 
         ++i) {
        const TTAProgram::Instruction& instr = 
            bb.instructionAtIndex(i);
        // First collect all started operations at this cycle
        // on each FU. 
        typedef std::map<const TTAMachine::FunctionUnit*, 
                         const TTAMachine::HWOperation*> OpsMap;
        OpsMap startedOps;
        typedef std::map<const TTAMachine::FunctionUnit*, 
                         ProgramOperationPtr> POMap;
        POMap startedPOs;
        for (int m = 0; m < instr.moveCount(); ++m) {
            const TTAProgram::Move& move = instr.move(m);
            if (move.isTriggering()) {
                TTAProgram::TerminalFUPort& tfup =
                    dynamic_cast<TTAProgram::TerminalFUPort&>(
                        move.destination());
                startedOps[&move.destination().functionUnit()] =
                    tfup.hwOperation();
                startedPOs[&move.destination().functionUnit()] =
                    tfup.programOperation();
            }
        }

        // in OTAs with data hazard detection, we do not need to emit
        // completely empty instruction bundles at all
        if (startedOps.size() == 0)
            continue; 
        
        typedef std::map<const TTAMachine::HWOperation*,
                         std::vector<TTAProgram::Terminal*> > OperandMap;
        OperandMap operands;
        // On a second pass through the moves we now should know the operand 
        // numbers of all the moves. The result moves should be at an 
        // instruction at the operation latency.
        OperationPool operations;

        for (OpsMap::const_iterator opsi = startedOps.begin(); 
            opsi != startedOps.end(); ++opsi) {
            const TTAMachine::HWOperation* hwOp = (*opsi).second;
            const Operation& operation = 
                operations.operation(hwOp->name().c_str());
            // first find the outputs
            for (int out = 0; out < operation.numberOfOutputs(); ++out) {
                const TTAProgram::Instruction& resultInstr = 
                    bb.instructionAtIndex(i + hwOp->latency());
                for (int m = 0; m < resultInstr.moveCount(); ++m) {
                    const TTAProgram::Move& move = resultInstr.move(m);
                    // assume it's a register write, the potential (implicit) 
                    // bypass move is ignored
                    if (move.source().isFUPort() && 
                        &move.source().functionUnit() ==
                        hwOp->parentUnit() &&
                        (move.destination().isGPR() ||
                         move.destination().isRA())) {
                        operands[hwOp].push_back(&move.destination());
                    }
                }
            }
            if ((std::size_t)operation.numberOfOutputs() != 
                operands[hwOp].size()) {
                PRINT_VAR(operation.name());
                PRINT_VAR(operands[hwOp].size());
                PRINT_VAR(operation.numberOfOutputs());
                assert((std::size_t)operation.numberOfOutputs() == 
                       operands[hwOp].size());
                abort();
            }

            // then the inputs
            for (int input = 0; input < operation.numberOfInputs();
                 ++input) {
                for (int m = 0; m < instr.moveCount(); ++m) {
                    const TTAProgram::Move& move = instr.move(m);
                    if (move.destination().isFUPort() &&
                        &move.destination().functionUnit() ==
                        hwOp->parentUnit() &&
                        dynamic_cast<const TTAMachine::Port*>(
                            hwOp->port(input + 1)) == 
                        &move.destination().port()) {
                        // if the result is forwarded (bypass), find the
                        // result move 
                        if (move.source().isFUPort()) {
                            for (int mm = 0; mm < instr.moveCount(); ++mm) {
                                const TTAProgram::Move& move2 = 
                                    instr.move(mm);
                                if (move2.destination().isGPR() &&
                                    move2.source().isFUPort() && 
                                    &move2.source().port() ==
                                    &move.source().port()) {
                                    operands[hwOp].push_back(&move2.destination());
                                }
                            }
                        } else {
                            // otherwise assume it's not bypassed but
                            // read from the RF
                            operands[hwOp].push_back(&move.source());
                        }
                    }
                }
            }

            if ((std::size_t)operation.numberOfInputs() + 
                operation.numberOfOutputs() !=
                operands[hwOp].size()) {
                PRINT_VAR(operation.name());
                PRINT_VAR(operands[hwOp].size());
                PRINT_VAR(operation.numberOfInputs());
                PRINT_VAR(operation.numberOfOutputs());
                assert(
                    operation.numberOfInputs() + operation.numberOfOutputs() ==
                    (int)operands[hwOp].size());
            }
        }

        for (BundleOrderIndex::const_iterator boi = bundleOrder.begin();
             boi != bundleOrder.end(); ++boi) {
            llvm::MachineInstr* mi = NULL;
#if LLVM_OLDER_THAN_3_6
            const llvm::TargetInstrInfo& tii = 
                *mbb.getParent()->getTarget().getInstrInfo();
#elif LLVM_OLDER_THAN_6_0
            const llvm::TargetInstrInfo& tii = 
                *mbb.getParent()->getTarget().getSubtargetImpl(
                    *mbb.getParent()->getFunction())->getInstrInfo();
#else
            const llvm::TargetInstrInfo& tii =
                *mbb.getParent()->getTarget().getSubtargetImpl(
                    mbb.getParent()->getFunction())->getInstrInfo();
#endif
            if (startedOps.find(*boi) == startedOps.end()) {
#if 0
                // TODO: figure out a generic way to find the NOP opcode for 
                // the current "lane", it's SPU::ENOP and SPU::LNOP for SPU.                
                // Could call the TargetInstrInfo::insertNoop() if it was 
                // implemented for SPU.
                // Just omit NOP instructions for now and assume the NOP inserter
                // pass takes care of it.
                mi = mbb.getParent()->CreateMachineInstr(
                    findLLVMTargetInstrDesc("nop", tii),
                    llvm::DebugLoc());
#endif

#ifdef DEBUG_POM_TO_MI
                Application::logStream() << "nop";
#endif
            } else {
                const TTAMachine::HWOperation* hwop = 
                    (*startedOps.find(*boi)).second;
                assert(hwop->name() != "");
#ifdef DEBUG_POM_TO_MI
                Application::logStream() << "hwop: '" << hwop->name() << "' " << std::endl;
#endif

                const llvm::MCInstrDesc& tid =
                    findLLVMTargetInstrDesc(hwop->name(), tii);
                mi = mbb.getParent()->CreateMachineInstr(
                    tid, llvm::DebugLoc());

#ifdef DEBUG_POM_TO_MI
                Application::logStream() << "MI: "; 
                //mi->dump();
#endif

                
                std::vector<TTAProgram::Terminal*>& opr = operands[hwop];

                unsigned counter = 0;
                // add the MachineOperands to the instruction via
                // POM Terminal --> MachineOperand conversion
                for (std::vector<TTAProgram::Terminal*>::const_iterator opri =
                         opr.begin(); opri != opr.end() && 
                         (counter < tid.getNumOperands() || mi->getDesc().isReturn()); 
                     ++opri, ++counter) {
                    TTAProgram::Terminal* terminal = *opri;
                    if (terminal->isCodeSymbolReference()) {
                        // has to be a global variable at this point?
                        // Constant pool indeces are converted to
                        // dummy references when LLVM->POM conversion
                        // in the form of ".CP_INDEX_OFFSET"
                        if (terminal->toString().startsWith(".CP_")) {
                            std::vector<TCEString> refs = 
                                terminal->toString().split("_");
                            unsigned index = Conversion::toInt(refs.at(1));
                            unsigned offset = Conversion::toInt(refs.at(2));
                            mi->addOperand(
                                llvm::MachineOperand::CreateCPI(index, offset));     
                        } else if (terminal->toString().startsWith(".JTI_")) {
                            TCEString ref = terminal->toString().substr(5);
                            unsigned index = Conversion::toInt(ref);
                            mi->addOperand(    
                                llvm::MachineOperand::CreateJTI(index, 0));     
                        } else {       
                            mi->addOperand(
                                llvm::MachineOperand::CreateES(
                                    terminal->toString().c_str()));
                        }
                    } else if (terminal->isBasicBlockReference()) {
                        llvm::MachineBasicBlock& mbb2 =
                            getMBB(*mbb.getParent(), terminal->basicBlock());                          
                        mi->addOperand(
                            llvm::MachineOperand::CreateMBB(&mbb2)); 
                        mbb.addSuccessor(&mbb2);
                    } else if (terminal->isProgramOperationReference()) {
                        const TTAProgram::TerminalProgramOperation& tpo =
                            dynamic_cast<
                            const TTAProgram::TerminalProgramOperation&>(
                                *terminal);
#ifdef LLVM_OLDER_THAN_3_7
                        llvm::MCSymbol* symbol = 
                            mbb.getParent()->getContext().GetOrCreateSymbol(
                                llvm::StringRef(tpo.label()));
#else
                        llvm::MCSymbol* symbol = 
                            mbb.getParent()->getContext().getOrCreateSymbol(
                                llvm::StringRef(tpo.label()));
#endif
                        mi->addOperand(llvm::MachineOperand::CreateMCSymbol(symbol));
                        // need to keep book of the TPOs in order to recreate the
                        // label instructions
                        tpos_.insert(std::make_pair(tpo.programOperation(), symbol));
                    } else if (terminal->isImmediate()) {
                        if (!mi->getDesc().isReturn()) {
                            mi->addOperand(
                                 llvm::MachineOperand::CreateImm(
                                 terminal->value().intValue()));
                        } 
                    } else if (terminal->isGPR()) {
                        // in case it's an output, it's a def, the outputs are always the
                        // first operands in the LLVM instruction
                        bool isDef = counter < tid.getNumDefs();
                        bool isImp = false;
                        // RET on spu seems to have implicit operand
                        // TODO: implement real implicit property to OSAL
                        // operands.
                        if (mi->getDesc().isReturn()) {
                            isImp = true;
                        }

                        // LLVM register index starts from 1, 
                        // we count register from 0
                        // thus add 1 to get correct data to the LLVM
                        if (!mi->getDesc().isReturn()) {
                            mi->addOperand(
                                llvm::MachineOperand::CreateReg(
                                    terminal->index() + 1, isDef, isImp));                            
                        }

                    } else {
                        abortWithError(
                            "Unsupported Terminal -> MachineOperand conversion attempted.");
                    }
#ifdef DEBUG_POM_TO_MI
                    if (counter > 0)
                        Application::logStream() << ", ";
                    Application::logStream() << terminal->toString();
#endif
                }
            }
            if (mi != NULL) {
                mbb.push_back(mi);
                assert(startedPOs.find(*boi) != startedPOs.end());
                ProgramOperationPtr po = (*startedPOs.find(*boi)).second;
                assert(po.get() != NULL);
                if (po.get() != NULL) {
                    programOperationToMIMap_[po.get()] = mi;
                } else {                    
                    //assert(po.get() != NULL);
                }
            }
#ifdef DEBUG_POM_TO_MI
            Application::logStream() << "\t# " << (*boi)->name() << std::endl;
#endif
        }
#ifdef DEBUG_POM_TO_MI
        Application::logStream() << std::endl;
#endif
    }

#ifdef DEBUG_POM_TO_MI
    Application::logStream() << std::endl << std::endl;
#endif
}


/**
 * Updates instruction references from procedure to cfg
 * Which is constructed from the procedure.
 *
 */
void
ControlFlowGraph::updateReferencesFromProcToCfg() {

    // make all refs point to the new copied instructions.
    for (int i = 0; i < nodeCount(); i++) {
        BasicBlockNode& bbn = node(i);
        bbn.updateReferencesFromProcToCfg(*program_);
    }

#if 0 // TODO: why does this irm claim to be unuser variable??
    InstructionReferenceManager& irm = program_->instructionReferenceManager();
    // procedure should not have any references.
    for (int i = 0; i < procedure_->instructionCount(); i++) {
        assert(!irm.hasReference(procedure_->instructionAtIndex(i)));
    }
#endif
}


/**
 * Return an incoming fall-thru edge to a node.
 * Jump and entry is also considered fall-thru
 *
 * @param bbn the node
 * @return the edge or null, if none found.
 */
ControlFlowEdge*
ControlFlowGraph::incomingFTEdge(const BasicBlockNode& bbn) const {

    auto edges = boost::in_edges(descriptor(bbn), graph_);
    for (auto i = edges.first; i != edges.second; ++i) {
        auto edge = graph_[(*i)];
        if (!edge->isJumpEdge()) {
            edgeDescriptors_[edge] = *i;
            return edge;
        }
    }
    return nullptr;
}

/**
 * Tells whether a node has incoming jumps that are not from
 * a single-basic block loop, ie source is not the same node.
 *
 * @param bbn the node
 */
bool
ControlFlowGraph::hasIncomingExternalJumps(const BasicBlockNode& bbn) const {
    ControlFlowGraph::EdgeSet jumpEdges = incomingJumpEdges(bbn);
    
    for (auto e: jumpEdges) {
        if (&tailNode(*e) != &bbn) {
            return true;
        }
    }
    return false;
}

ControlFlowGraph::EdgeSet
    ControlFlowGraph::incomingJumpEdges(const BasicBlockNode& bbn) const {

    auto edges = boost::in_edges(descriptor(bbn), graph_);
    EdgeSet result;
    for (auto i = edges.first; i != edges.second; ++i) {
        auto edge = graph_[(*i)];
        if (edge->isJumpEdge()) {
            edgeDescriptors_[edge] = *i;
            result.insert(edge);
        }
    }
    return result;
}

/**
 * Finds a jump that jumps to target from a codesnippet and removes the
 * jump.
 *
 * @param cs CodeSnippet where to remove the jump from.
 * @param target jump target instruction.
 * @param idx index which after to check for existing moves and immeds
 * @return whther not removed, removed and some moves last after idx,
 *         or if removed and  no moves/immeds left after idx.
 * @TODO should be able to handle also jumps where address is LIMM
 */
ControlFlowGraph::RemovedJumpData
ControlFlowGraph::removeJumpToTarget(
    CodeSnippet& cs, const Instruction& target, int idx,
    DataDependenceGraph* ddg) {
    int index = cs.instructionCount() -1; // - delaySlots;

    Move* lastJump = NULL;
    for (;index >= idx ; index--) {
        Instruction& ins = cs.instructionAtIndex(index);
        for (int j = 0; j < ins.moveCount(); j++) {
            Move& move = ins.move(j);
            if (!move.isJump()) {
                continue;
            }

            TTAProgram::Terminal& term = move.source();
            if (term.isInstructionAddress()) {
                TTAProgram::TerminalInstructionReference& tia =
                    dynamic_cast<TTAProgram::TerminalInstructionReference&>(
                        term);
                TTAProgram::InstructionReference& ir =
                    tia.instructionReference();

                // found jump to target? remove this?
                if (&ir.instruction() == &target) {
                    if (lastJump != NULL) {
                        // TODO: should also check that no other moves
                        // as the lastJump after the delay slots of
                        // the move.
                        if (lastJump->isUnconditional()) {
                            // if removing conditional jump,
                            // make the other jump have opposite guard
                            if (!move.isUnconditional()) {

                                TTAProgram::MoveGuard* invG = NULL;
                                // if already scheduled,
                                // guard must be in same bus
                                if (!lastJump->bus().machine()->
                                    isUniversalMachine()) {
                                    invG = CodeGenerator::createInverseGuard(
                                        move.guard(), &lastJump->bus());
                                } else {
                                    invG = CodeGenerator::createInverseGuard(
                                        move.guard());
                                }
                                if (invG == NULL) {
                                    return JUMP_NOT_REMOVED;
                                }
                                lastJump->setGuard(invG);
                            }
                            
                            if (ddg != NULL) {
#ifdef DEBUG_BB_OPTIMIZER
                                std::cerr << "removing jump node from ddg."
                                          << std::endl;
#endif
                                MoveNode* mn = &ddg->nodeOfMove(move);
                                ddg->removeNode(*mn);
                                delete mn;
                            }

                            ins.removeMove(move);
                            return JUMP_REMOVED;
                        } else {
                            // two conditional jumps? nasty. no can do
                            return JUMP_NOT_REMOVED;
                        }
                    } else {
                        if (ddg != NULL) {
#ifdef DEBUG_BB_OPTIMIZER
                            std::cerr << "removing jump node from ddg(2)."
                                      << std::endl;
#endif
                            MoveNode* mn = &ddg->nodeOfMove(move);
                            ddg->removeNode(*mn);
                            delete mn;
                        }

                        ins.removeMove(move);
                        // check if there are moves/immeds left.
                        // if not, update refs.
                        for (; idx < cs.instructionCount(); idx++) {
                            Instruction& ins2 = cs.instructionAtIndex(idx);
                            if (ins2.moveCount() > 0 ||
                                ins2.immediateCount() > 0) {
                                return JUMP_REMOVED;
                            }
                        }
                        return LAST_ELEMENT_REMOVED;
                    }
                }
            }
            lastJump = &move;
        }
    }
    return JUMP_NOT_REMOVED;
}

/**
 * Removes and deletes a basic block node from the grpahs and
 * updates all references that point to it to point elsewhere.
 *
 * @param node basic block node to be removed and deleted.
 */
void
ControlFlowGraph::deleteNodeAndRefs(BasicBlockNode& node) {
    removeNode(node);
    delete &node;
}

TTAProgram::InstructionReferenceManager&
ControlFlowGraph::instructionReferenceManager() {
    if (program_ == NULL) {
        return *irm_;
        throw NotAvailable(__FILE__,__LINE__,__func__,
            "cfg does not have program");
    }
    return program_->instructionReferenceManager();
}

BasicBlockNode*
ControlFlowGraph::jumpSuccessor(BasicBlockNode& bbn) {
    for (int i = 0; i < outDegree(bbn); i++) {
        Edge& e = outEdge(bbn,i);
        if (e.isJumpEdge()) {
            return &headNode(e);
        }
    }
    return NULL;
}

/**
 * Tests created control flow graph using depth first search algorithm
 * of boost graph library and mark back edges.
 */
void
ControlFlowGraph::detectBackEdges() {
    DFSBackEdgeVisitor vis;
    /// Default starting vertex is vertex(g).first, which is actually
    /// first basic block created. Entry is created as second and
    /// there is connection added from entry to first BB.
    /// Using default parameter for root_vertex is therefore sufficient
    boost::depth_first_search(graph_, visitor(vis));
}


void ControlFlowGraph::convertBBRefsToInstRefs() {

    for (int i = 0; i < nodeCount(); i++) {
        BasicBlockNode& bbn = node(i);

        if (bbn.isNormalBB()) {
            TTAProgram::BasicBlock& bb = bbn.basicBlock();

            for (int j = 0; j < bb.instructionCount(); j++) {
                TTAProgram::Instruction& ins = bb.instructionAtIndex(j);

                for (int k = 0; k < ins.moveCount(); k++) {
                    TTAProgram::Move& move = ins.move(k);
                    TTAProgram::Terminal& src = move.source();

                    if (src.isBasicBlockReference()) {
                        const TTAProgram::BasicBlock& target = 
                            src.basicBlock();
                        assert(target.instructionCount() > 0);
                        move.setSource(
                            new TTAProgram::TerminalInstructionReference(
                                instructionReferenceManager().createReference(
                                    target.firstInstruction())));
                    }
                }

                for (int k = 0; k < ins.immediateCount(); k++) {
                    TTAProgram::Immediate& imm = ins.immediate(k);
                    TTAProgram::Terminal& immVal = imm.value();

                    if (immVal.isBasicBlockReference()) {
                        const TTAProgram::BasicBlock& target = 
                            immVal.basicBlock();
                        assert(target.instructionCount() > 0);
                        imm.setValue(
                            new TTAProgram::TerminalInstructionReference(
                                instructionReferenceManager().createReference(
                                    target.firstInstruction())));
                    }
                }
            }
        }
    }
}

/**
 * Reverses predicate of outgoing edges.
 *
 */
void
ControlFlowGraph::reverseGuardOnOutEdges(const BasicBlockNode& bbn) {
    for (int i = 0; i < outDegree(bbn); i++) {
        Edge& e = outEdge(bbn,i);
        if (e.isTrueEdge()) {
            e.setPredicate(ControlFlowEdge::CFLOW_EDGE_FALSE);
        } else if (e.isFalseEdge()) {
            e.setPredicate(ControlFlowEdge::CFLOW_EDGE_TRUE);
        } else {
            std::cerr << "node in question: " << bbn.toString() <<
                " edge: " << e.toString() << std::endl;
            writeToDotFile("invalid_predicate.dot");
            assert(false && "Can only reverse predicate true or false");
        }
    }
}

ControlFlowGraph::NodeSet
ControlFlowGraph::findUnreachableNodes(
    const ControlFlowGraph::NodeSet& reachableNodes) {
    NodeSet unreachableNodes;
    // find dead nodes
    for (int i = 0; i < nodeCount(); i++) {
        BasicBlockNode& n = node(i);
        if (!AssocTools::containsKey(reachableNodes,&n) &&
            n.isNormalBB()) {
            unreachableNodes.insert(&n);
        }
    }
    return unreachableNodes;
}

/** 
 * The algorithm is same as in CopyToProcedure, but without the copying.
 * Still removes jump, and also does BB mergeing.
 */
void 
ControlFlowGraph::optimizeBBOrdering(
    bool removeDeadCode, InstructionReferenceManager& irm,
    DataDependenceGraph* ddg) {

    ControlFlowGraph::NodeSet firstBBs = successors(entryNode());
    assert(firstBBs.size() == 1);
    BasicBlockNode* firstBBN = *firstBBs.begin();
    BasicBlockNode* currentBBN = firstBBN;
    entryNode().link(firstBBN);

    // find and queue reachable nodes
    NodeSet queuedNodes = findReachableNodes();
    NodeSet unreachableNodes = findUnreachableNodes(queuedNodes);

    if (removeDeadCode) {
        removeUnreachableNodes(unreachableNodes, ddg);
    }

    // then loop as long as we have BBs which have not been written to
    // the procedure.
    while (currentBBN != NULL) {

#ifdef DEBUG_BB_OPTIMIZER
            std::cerr << "current node: " << currentBBN->toString() << 
                std::endl;
#endif
        BasicBlockNode* nextNode = NULL;
        TTAProgram::BasicBlock& bb = currentBBN->basicBlock();
        queuedNodes.erase(currentBBN);

        // if has a fall-through node, it has to be the next node
        BasicBlockNode* ftNode = fallThruSuccessor(*currentBBN);
        if (ftNode != NULL && ftNode->isNormalBB()) {
            if (queuedNodes.find(ftNode) == queuedNodes.end()) {
                std::cerr << "not-queued fall-thru: " << ftNode->toString()
                          << " current: " << currentBBN->toString() << 
                    std::endl;
                writeToDotFile("optimizeCFGFallThruBBNotQueued.dot");
            }
            // must not be already processed.
            assert(queuedNodes.find(ftNode) != queuedNodes.end());

#ifdef DEBUG_BB_OPTIMIZER
            std::cerr << "\tfound FT node: " << ftNode->toString() << std::endl;
#endif
            const ControlFlowEdge& cfe = 
                **connectingEdges(*currentBBN, *ftNode).begin();

            // if fall-through node has no other predecessors, merge.
            if (inDegree(*ftNode) == 1 && outDegree(*currentBBN) == 1 &&
                !cfe.isCallPassEdge()) {
#ifdef DEBUG_BB_OPTIMIZER
                std::cerr << "Merging: " << currentBBN->toString()
                          << " with: " << ftNode->toString() << std::endl;
                writeToDotFile("before_merge.dot");
                if (cfe.isBackEdge()) {
                    std::cerr << "Warning: merging over back edge." << 
                        std::endl;
                }
#endif
                queuedNodes.erase(ftNode);
                mergeNodes(*currentBBN, *ftNode, ddg);
#ifdef DEBUG_BB_OPTIMIZER
                writeToDotFile("after_merge.dot");
                std::cerr << "Merged with ft node." << std::endl;
#endif
            } else {
                currentBBN->link(ftNode);
#ifdef DEBUG_BB_OPTIMIZER
                writeToDotFile("linked.dot");
#endif
                currentBBN = ftNode;
            }
            continue;
        }

        // Select some node, preferably successrs without ft-preds
        // The jump can then be removed.
        EdgeSet oEdges = outEdges(*currentBBN);
        for (EdgeSet::iterator i = oEdges.begin(); i != oEdges.end(); i++) {
            ControlFlowEdge& e = **i;
            BasicBlockNode& head = headNode(e);
            if (!hasFallThruPredecessor(head) && head.isNormalBB() &&
                queuedNodes.find(&head) != queuedNodes.end()) {
                // try to remove the jump as it's jump to the next BB.
                RemovedJumpData rjd = removeJumpToTarget(
                    bb, head.basicBlock().firstInstruction(), 0, ddg);
                if (rjd != JUMP_NOT_REMOVED) {
                    // if BB got empty,
                    // move refs to beginning of the next BB.
                    if (rjd == LAST_ELEMENT_REMOVED) {
                        Instruction& ins = bb.instructionAtIndex(0);
                        if (irm.hasReference(ins)) {
                            irm.replace(
                                ins, head.basicBlock().instructionAtIndex(
                                    head.basicBlock().
                                    skippedFirstInstructions()));
                        }
                        queuedNodes.erase(&head);
                        mergeNodes(*currentBBN, head, ddg);
#ifdef DEBUG_BB_OPTIMIZER
                        std::cerr << "Merged with after jump removal(1)" <<
                            std::endl;
#endif
                        nextNode = currentBBN;
                        break;
                    }
                    // we removed a jump so convert the jump edge into
                    // fall-through edge, OR merge BBs.

                    if (inDegree(head) == 1) {
                        queuedNodes.erase(&head);
                        mergeNodes(*currentBBN, head, ddg);
                        nextNode = currentBBN;
#ifdef DEBUG_BB_OPTIMIZER
                        std::cerr << "Merged with after jump removal(2)" <<
                            std::endl;
#endif
                    } else {
                        ControlFlowEdge* ftEdge = new ControlFlowEdge(
                            e.edgePredicate(), 
                            ControlFlowEdge::CFLOW_EDGE_FALLTHROUGH);
                        removeEdge(e);
                        connectNodes(*currentBBN, head, *ftEdge);
                        nextNode = &head;
                    }
                    // if we did remove a back edge, we need to scan the cfg
                    // again for back edges.
                    // TODO: should we also then mark ddg edges that
                    // do over this cfg edge?
                    if (e.isBackEdge()) {
                        detectBackEdges();
                    }
                    break; // continue outer;
                }
            }
        }
        if (nextNode != NULL) {
            continue;
        }

        // need to select SOME node as successor.
        // first without ft-predecessor usually is a good candidate.
        // smarter heuristic does not seem to help at all.
        // try to select
        bool ftPred = false;
        for (NodeSet::iterator i = queuedNodes.begin();
             i != queuedNodes.end(); i++) {
            if (!hasFallThruPredecessor(**i)) {
                nextNode = *i;
                break;
            } else {
                ftPred = true;
            }
        }

        if (!removeDeadCode) {
            // unreachable node having ft may have prevented us from
            // managing some node whose fall-thru succs prevent
            // futher nodes. try to select some unreached node.
            if (nextNode == NULL && ftPred) {
                for (NodeSet::iterator i = unreachableNodes.begin();
                     i != unreachableNodes.end(); i++) {
                    if (fallThruSuccessor(**i) != NULL) {
                        nextNode = *i;
                        unreachableNodes.erase(*i);
                        break;
                    }
                }
            }
        }
        if (nextNode == NULL) {
            currentBBN->link(&exitNode());
            break;
        }
        else {
            currentBBN->link(nextNode);
            currentBBN = nextNode;
        }
    }
}

/**
 * TODO: what to do with exit node?
 */
void
ControlFlowGraph::removeUnreachableNodes(
    const NodeSet& nodes, DataDependenceGraph* ddg) {
    for (NodeSet::iterator i = nodes.begin(); i != nodes.end(); i++) {
        BasicBlockNode* bbn = *i;
        removeNode(*bbn);
        if (ddg != NULL) {
            TTAProgram::BasicBlock& bb = bbn->basicBlock();
            for (int j = 0; j < bb.instructionCount(); j++) {
                Instruction& ins = bb.instructionAtIndex(j);
                for (int k = 0; k < ins.moveCount(); k++) {
                    Move& move = ins.move(k);
                    MoveNode* mn = &ddg->nodeOfMove(move);
                    ddg->removeNode(*mn);
                    delete mn;
                }
            }
            delete bbn;
        }
    }
}

void
ControlFlowGraph::mergeNodes(
    BasicBlockNode& node1, BasicBlockNode& node2, DataDependenceGraph* ddg) {

    if (ddg != NULL && 
        (!ddg->hasAllRegisterAntidependencies() &&
         ddg->hasIntraBBRegisterAntidependencies())) {
        ddg->fixInterBBAntiEdges(node1, node2, false);
    }
    assert(node1.isNormalBB());
    assert(node2.isNormalBB());
    TTAProgram::BasicBlock& bb1 = node1.basicBlock();
    TTAProgram::BasicBlock& bb2 = node2.basicBlock();
    for (int i = bb2.instructionCount() -1; i >= 0; i--) {
        Instruction& ins = bb2.instructionAtIndex(i);
        if (ddg != NULL) {
            for (int k = 0; k < ins.moveCount(); k++) {
                Move& move = ins.move(k);
                MoveNode* mn = &ddg->nodeOfMove(move);
                ddg->setBasicBlockNode(*mn, node1);
            }
        }
    }
    
    if (node1.basicBlock().liveRangeData_ != NULL &&
        node2.basicBlock().liveRangeData_ != NULL) {
        node1.basicBlock().liveRangeData_->merge(
            *node2.basicBlock().liveRangeData_);
    }

    node2.setBBOwnership(false); // append deletes bb2.
    bb1.append(&bb2);

    EdgeSet n2in = inEdges(node2);
    for (EdgeSet::iterator i = n2in.begin(); i != n2in.end(); i++) {
        ControlFlowEdge* e = *i;
        const BasicBlockNode& tail = tailNode(*e);
        if (&tail != &node1) {
            moveInEdge(node2, node1, *e);
        }
    }

    EdgeSet n2out = outEdges(node2);
    for (EdgeSet::iterator i = n2out.begin(); i != n2out.end(); i++) {
        ControlFlowEdge* e = *i;
        moveOutEdge(node2, node1, *e);
    }

    removeNode(node2);
    delete &node2;
    // TODO: CFG edges
}

/**
 * Fetch machine basic block corresponding to the BasicBlock passed, if
 * it does not exist create empty one.
 */
llvm::MachineBasicBlock&
ControlFlowGraph::getMBB(
    llvm::MachineFunction& mf,
    const TTAProgram::BasicBlock& bb) const {
  
    if (MapTools::containsKey(bbMap_, &bb)) {
        return *bbMap_[&bb];
    } else {        
        llvm::MachineBasicBlock* mbb = mf.CreateMachineBasicBlock();
        mf.push_back(mbb);    
        bbMap_[&bb] = mbb;
        return *mbb;
    }  
}

/**
 * Checks if the basic blocks have calls in the middle of them and splits
 * them to multiple basic blocks with call edge chains.
 *
 * TCE scheduler assumes there cannot be calls in the middle of basic block.
 */
void
ControlFlowGraph::splitBasicBlocksWithCallsAndRefs() {
    std::set<BasicBlockNode*> bbsToHandle;
    for (int i = 0; i < nodeCount(); ++i) {
        BasicBlockNode& bb = node(i);
        bbsToHandle.insert(&bb);
    }

    while (bbsToHandle.size() > 0) {
        BasicBlockNode* bbn = *bbsToHandle.begin();
        TTAProgram::BasicBlock& bb = bbn->basicBlock();

        for (int ii = 0; ii < bb.instructionCount(); ++ii) {
            TTAProgram::Instruction& instr = bb.instructionAt(ii);
            if (instr.hasCall() && &instr != &bb.lastInstruction()) {
                bbsToHandle.insert(splitBasicBlockAtIndex(*bbn, ii+1));
                break;
            }
            assert (irm_ != NULL);
            if (ii != 0 && irm_->hasReference(instr)) {
                bbsToHandle.insert(splitBasicBlockAtIndex(*bbn, ii));
                break;
            }
        }
        bbsToHandle.erase(bbn);
    }
}

/**
 * Checks if the basic blocks have calls in the middle of them and splits
 * them to multiple basic blocks with call edge chains.
 *
 * TCE scheduler assumes there cannot be calls in the middle of basic block.
 */
void
ControlFlowGraph::splitBasicBlocksWithCalls() {
    std::set<BasicBlockNode*> bbsToHandle;
    for (int i = 0; i < nodeCount(); ++i) {
        BasicBlockNode& bb = node(i);
        bbsToHandle.insert(&bb);
    }

    while (bbsToHandle.size() > 0) {
        BasicBlockNode& bbn = **bbsToHandle.begin();
        TTAProgram::BasicBlock& bb = bbn.basicBlock();

        for (int ii = 0; ii < bb.instructionCount(); ++ii) {
            TTAProgram::Instruction& instr = bb.instructionAt(ii);
            if (instr.hasCall() && &instr != &bb.lastInstruction()) {
                TTAProgram::BasicBlock* newbb = new TTAProgram::BasicBlock();
                BasicBlockNode* newbbn = new BasicBlockNode(*newbb);
                addNode(*newbbn);

                // the BB can contain multiple calls, handle them
                // in the new BB
                bbsToHandle.insert(newbbn);
                moveOutEdges(bbn, *newbbn);                

                // move the instructions after the call in the old BB to
                // the new one
                while (&instr != &bb.lastInstruction()) {
                    TTAProgram::Instruction& next = bb.nextInstruction(instr);
                    bb.remove(next);
                    newbb->add(&next);
                }

                ControlFlowEdge* cfe = new ControlFlowEdge(
                    ControlFlowEdge::CFLOW_EDGE_NORMAL, 
                    ControlFlowEdge::CFLOW_EDGE_CALL);
                connectNodes(bbn, *newbbn, *cfe);

                break;
            }
        }
        bbsToHandle.erase(bbsToHandle.begin());
    }
}

BasicBlockNode*
ControlFlowGraph::splitBasicBlockAtIndex(
    BasicBlockNode& bbn, int index) {
    
    TTAProgram::BasicBlock& bb = bbn.basicBlock();
    TTAProgram::BasicBlock* newbb = new TTAProgram::BasicBlock();
    BasicBlockNode* newbbn = new BasicBlockNode(*newbb);
    addNode(*newbbn);
    
    // the BB can contain multiple calls, handle them
    // in the new BB
    moveOutEdges(bbn, *newbbn);                
    
    // move the instructions after the call in the old BB to
    // the new one.
    // no index update because remove puts to same index
    for (int i = index; i < bb.instructionCount(); ) {
        TTAProgram::Instruction& ins = bb.instructionAtIndex(i);
        bb.remove(ins);
        newbb->add(&ins);
    }

    ControlFlowEdge* cfe = new ControlFlowEdge(
        ControlFlowEdge::CFLOW_EDGE_NORMAL, 
        ControlFlowEdge::CFLOW_EDGE_CALL);
    connectNodes(bbn, *newbbn, *cfe);

    return newbbn;
}

bool ControlFlowGraph::isSingleBBLoop(const BasicBlockNode& node) const {
    for (int i = 0; i < outDegree(node); i++) {
        ControlFlowEdge& e = outEdge(node, i);
        if (e.isJumpEdge() && &headNode(e) == &node) {
            assert(e.isBackEdge());
            return true;
        }
    }
    return false;
}
