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
 * @file RegisterCopyAdder.hh
 *
 * Declaration of RegisterCopyAdder class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @author Fabio Garzia 2010 (fabio.garzia-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed 16-17 January 2008 by pj, pk, ml, hk
 */

#ifndef TTA_REGISTER_COPY_ADDER_HH
#define TTA_REGISTER_COPY_ADDER_HH

#include <map>

#include "DataDependenceGraph.hh"
#include "FunctionUnit.hh"

class ProgramOperation;
class MoveNode;
class DataDependenceGraph;
class InterPassData;
class SimpleResourceManager;

namespace TTAProgram {
    class Move;
    class BasicBlock;
}

namespace TTAMachine {
    class Port;
    class RegisterFile;
}

/**
 * Adds register copies through connected register files in case of missing 
 * connectivity in the operand moves of operations.
 *
 * In addition, annotates the moves in the operation with missing connectivity 
 * with the function unit candidates that are connected to all operand
 * moves of the operation. The annotations should be checked and followed 
 * by the resource assignment stage to avoid scheduling failure due to
 * missing connectivity. That is, after executing this pass, the scheduler 
 * (and resource assigner) can assume there is enough connectivity in the 
 * machine to schedule all moves of the operation (given the candidate FU 
 * annotations are adhered to). 
 *
 * Immediates that must be converted to long immediates are annotated with 
 * the required IU which is guaranteed to have connectivity to the target FU. 
 * The RM/scheduler, when converting a short immediate to a long immediate 
 * must take this annotation in account, otherwise scheduling error might 
 * occur. It tries to minimize the added temp register copies for
 * the operation by considering all possible FU bindings to each operation.
 *
 * The pass inputs a DDG with registers allocated but other resources 
 * unassigned in the considered operation and outputs a DDG with register 
 * copy moves inserted due to missing connectivity. Operation moves are
 * annotated with candidate function units and immediate units for operation and
 * immediate assignments. In case all  function units / immediate units are 
 * equal candidates according to connectivity, no annotations are inserted as 
 * the FU/IU can be assigned freely to the operation/immediate. 
 *
 * The candidate annotatons *must be* adhered to in resource assignment, 
 * otherwise the scheduling might fail due to missing connectivity.
 */
class RegisterCopyAdder {
public:
    RegisterCopyAdder(
        InterPassData& data, 
        SimpleResourceManager& rm,
        bool buScheduler = false);

    virtual ~RegisterCopyAdder();

    typedef std::map<const MoveNode*, DataDependenceGraph::NodeSet> 
    AddedRegisterCopyMap;

    struct AddedRegisterCopies {
        AddedRegisterCopies(int count);
        AddedRegisterCopies();
        int count_;
        AddedRegisterCopyMap operandCopies_;
        AddedRegisterCopyMap resultCopies_;
    };

    AddedRegisterCopies addMinimumRegisterCopies(
        ProgramOperation& programOperation,
        const TTAMachine::Machine& targetMachine,
        DataDependenceGraph* ddg);

    void operandsScheduled(
        AddedRegisterCopies& copies,
        DataDependenceGraph& ddg);

    void resultsScheduled(
        AddedRegisterCopies& copies,
        DataDependenceGraph& ddg);

    AddedRegisterCopies addRegisterCopiesToRRMove(
        MoveNode& moveNode, 
        DataDependenceGraph* ddg);

    static void findTempRegisters(
        const TTAMachine::Machine& machine, InterPassData& ipd);

    static void fixDDGEdgesInTempReg(
        DataDependenceGraph& ddg,
        MoveNode& originalMove,
        MoveNode* firstMove,
        MoveNode* lastMove,
        const TTAMachine::RegisterFile* lastRF,
        int lastRegisterIndex,
        BasicBlockNode& currentBBNode,
        bool bottomUpScheduling);

private:
    bool isAllowedUnit(
	const TTAMachine::FunctionUnit& fu,
	const ProgramOperation& po);

    AddedRegisterCopies addRegisterCopies(
        ProgramOperation& programOperation,
        const TTAMachine::FunctionUnit& fu,
        bool countOnly = true,
        DataDependenceGraph* ddg = NULL,
        int neededCopies = 0);

    int addConnectionRegisterCopies(
        MoveNode& originalMove,
        const TTAMachine::Port& sourcePort,
        const TTAMachine::Port& destinationPort,
        bool countOnly = true,
        DataDependenceGraph* ddg = NULL,
        DataDependenceGraph::NodeSet* addedNodes = NULL,
        int neededCopies = 0);

    int addConnectionRegisterCopiesImmediate(
        MoveNode& originalMove,
        const TTAMachine::Port& destinationPort,
        bool countOnly = true,
        DataDependenceGraph* ddg = NULL,
	DataDependenceGraph::NodeSet* addedNodes = NULL);

    int addConnectionRegisterCopies(
        MoveNode& moveNode,
        const TTAMachine::FunctionUnit& fu,
        bool countOnly = true,
        DataDependenceGraph* ddg = NULL,
        DataDependenceGraph::NodeSet* addedNodes = NULL,
        int neededCopies = 0);

    int countAndAddConnectionRegisterCopiesToRR(
        MoveNode& moveNode,
        DataDependenceGraph* ddg = NULL,
        DataDependenceGraph::NodeSet* addedNodes = NULL);

    void addCandidateSetAnnotations(
        ProgramOperation& programOperation,
        const TTAMachine::Machine& machine);

    void fixDDGEdgesInTempRegChain(
        DataDependenceGraph& ddg,
        MoveNode& originalMove,
        MoveNode* firstMove,
        std::vector<MoveNode*> intMoves,
        MoveNode* lastMove,
        const TTAMachine::RegisterFile* firstRF,
	std::vector<const TTAMachine::RegisterFile*> intRF,
        const TTAMachine::RegisterFile* lastRF,
        int firstRegisterIndex,
	std::vector<int> intRegisterIndex,
        int lastRegisterIndex,
	int regsRequired,
        BasicBlockNode& currentBBNode);

    static void createAntidepsForReg(
        const MoveNode& defMove, 
        const MoveNode& useMove,
        const MoveNode& originalMove,
        const TTAMachine::RegisterFile& rf, 
        int index,
        DataDependenceGraph& ddg, 
        BasicBlockNode& bbn,
        bool backwards);

  void fixDDGEdgesInTempRegChainImmediate(
    DataDependenceGraph& ddg,
    MoveNode& originalMove,
    MoveNode* firstMove,
    MoveNode* regToRegCopy,
    MoveNode* lastMove,
    const TTAMachine::RegisterFile* tempRF1, 
    const TTAMachine::RegisterFile* tempRF2, 
    int tempRegisterIndex1,
    int tempRegisterIndex2,
    BasicBlockNode& currentBBNode);

    /// container for storing the required register copies if the operation
    /// was bound to the given FU
    typedef std::map<
        const TTAMachine::FunctionUnit*, int, 
        TTAMachine::FunctionUnit::Comparator> 
    RegisterCopyCountIndex;

    RegisterCopyCountIndex requiredRegisterCopiesForEachFU(
        const TTAMachine::Machine& targetMachine,
        ProgramOperation& programOperation);

    /// the inter pass data from which to fetch the scratch register list
    InterPassData& interPassData_;
    /// the resource manager to check for machine resources in heuristics
    // SimpleResourceManager provides many methods not in the base interface
    // ResourceManager, so we cannot use it.
    SimpleResourceManager& rm_;

    /// Indicate that register copy adder is called from bottom up scheduler,
    /// this causes search for first scheduled register write instead of last
    /// read.
    bool buScheduler_;
};
#endif
