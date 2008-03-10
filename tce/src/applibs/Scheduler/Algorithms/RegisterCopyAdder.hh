/**
 * @file RegisterCopyAdder.hh
 *
 * Declaration of RegisterCopyAdder class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 16-17 January 2008 by pj, pk, ml, hk
 */

#ifndef TTA_REGISTER_COPY_ADDER_HH
#define TTA_REGISTER_COPY_ADDER_HH

#include <map>

#include "SimpleResourceManager.hh"

class ProgramOperation;
class MoveNode;
class BasicBlock;
class DataDependenceGraph;
class InterPassData;

namespace TTAProgram {
    class Move;
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
    RegisterCopyAdder(InterPassData& data, SimpleResourceManager& rm);
    virtual ~RegisterCopyAdder();

    int addMinimumRegisterCopies(
        ProgramOperation& programOperation,
        const TTAMachine::Machine& targetMachine,
        DataDependenceGraph& ddg);

private:
    int addRegisterCopies(
        ProgramOperation& programOperation,
        const TTAMachine::FunctionUnit& fu,
        DataDependenceGraph* ddg = NULL);

    int addConnectionRegisterCopies(
        MoveNode& originalMove,
        const TTAMachine::Port& sourcePort,
        const TTAMachine::Port& destinationPort,
        DataDependenceGraph* ddg = NULL);

    int addConnectionRegisterCopiesImmediate(
        MoveNode& originalMove,
        const TTAMachine::Port& destinationPort,
        DataDependenceGraph* ddg = NULL);

    int addConnectionRegisterCopies(
        MoveNode& moveNode,
        const TTAMachine::FunctionUnit& fu,
        DataDependenceGraph* ddg = NULL);

    void addCandidateSetAnnotations(
        ProgramOperation& programOperation,
        const TTAMachine::Machine& machine);

    void fixDDGEdgesInTempRegChain(
        DataDependenceGraph& ddg,
        MoveNode& originalMove,
        MoveNode* firstMove,
        MoveNode* regToRegCopy,
        MoveNode* lastMove,
        const TTAMachine::RegisterFile* tempRF1,
        const TTAMachine::RegisterFile* tempRF2,
        int tempRegisterIndex1,
        int tempRegisterIndex2);

    /// container for storing the required register copies if the operation
    /// was bound to the given FU
    typedef std::map<const TTAMachine::FunctionUnit*, int> 
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
};
#endif
