/**
 * @file RegisterCopyAdder.cc
 *
 * Definition of RegisterCopyAdder class.
 *
 * @todo rename the file to match the class name
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 16-17 January 2008 by pj, pk, ml, hk
 */

#include "Machine.hh"
#include "ControlUnit.hh"
#include "RegisterCopyAdder.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceGraphBuilder.hh"
#include "ProgramOperation.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "MachineConnectivityCheck.hh"
#include "BasicBlock.hh"
#include "POMDisassembler.hh"
#include "Instruction.hh"
#include "AssocTools.hh"
#include "SpecialRegisterPort.hh"
#include "ProgramAnnotation.hh"
#include "InterPassDatum.hh"
#include "InterPassData.hh"
#include "Exception.hh"
#include "BusBroker.hh"

/**
 * Constructor.
 *
 * @param data The inter-pass data.
 * @param rm The resource manager used to check for availability of resources.
 */
RegisterCopyAdder::RegisterCopyAdder(
    InterPassData& data, SimpleResourceManager& rm) :
    interPassData_(data), rm_(rm) {
}

/**
 * Destructor.
 */
RegisterCopyAdder::~RegisterCopyAdder() {
}

/**
 * Counts the temporary register copies required for each FU in case the
 * given operation was assigned to them.
 *
 * @param targetMachine The machine to use.
 * @return An index with temporary register move counts. 
 */
RegisterCopyAdder::RegisterCopyCountIndex 
RegisterCopyAdder::requiredRegisterCopiesForEachFU(
    const TTAMachine::Machine& targetMachine,
    ProgramOperation& programOperation) {

    RegisterCopyCountIndex registerCopiesRequired;           

    // analyze each FU assignment alternative for the operation
    TTAMachine::Machine::FunctionUnitNavigator FUs = 
        targetMachine.functionUnitNavigator();

    for (int i = 0; i <= FUs.count(); i++) {

        // include control unit in the traversed FUs
        const TTAMachine::FunctionUnit& unit = 
            (i == FUs.count())?
            (*targetMachine.controlUnit()):(*FUs.item(i));

        std::string operationName = programOperation.operation().name();
        if (unit.hasOperation(operationName)) {
            registerCopiesRequired[&unit] = 
                addRegisterCopies(programOperation, unit);
        }
    }

    return registerCopiesRequired;
}

/**
 * Adds minimum register copies required for the given operation.
 *
 * Adds the register copies to the given DDG along with required new edges
 * between the copy moves and annotates the moves with a candidate set
 * for the FU binding.
 *
 * @param programOperation The operation execution.
 * @param targetMachine The target machine.
 * @param ddg If NULL, no register copies are really added, only
 * the required copies are counted, otherwise register copies are added and
 * the DDG is updated.
 * @return The count of register copies added.
 * @exception In case there was no such FU that could be connected with 
 *            a temp register chain of maximal length of 2 copies.
 */
int
RegisterCopyAdder::addMinimumRegisterCopies(
    ProgramOperation& programOperation,
    const TTAMachine::Machine& targetMachine,
    DataDependenceGraph& ddg) {

    RegisterCopyCountIndex 
        registerCopiesRequired = 
        requiredRegisterCopiesForEachFU(targetMachine, programOperation);

    // find the FU which requires least register copies
    int min = INT_MAX;            
    const TTAMachine::FunctionUnit* unit = NULL;
    for (RegisterCopyAdder::RegisterCopyCountIndex::const_iterator
             i = registerCopiesRequired.begin(); 
         i != registerCopiesRequired.end(); ++i) {

        const TTAMachine::FunctionUnit* u = (*i).first;
        int copies = (*i).second;
        if (copies < min) {
            min = copies;
            unit = u;
            if (copies == 0) {
                break;
            }
        }
    }
        
    if (min == 0) {
        // no temp reg copies required for some FU, annotate the moves in case
        // some FUs require the copies and thus the FU selection should be
        // restricted to those which do not require any copies
        addCandidateSetAnnotations(programOperation, targetMachine);
        return 0;
    } else if (min < INT_MAX) {
        // add register copies as if the operation was assigned to that FU
        addRegisterCopies(programOperation, *unit, &ddg);
        
        // create the FU candidate set now that we have added the register
        // copies
        addCandidateSetAnnotations(programOperation, targetMachine); 
        return min;
    } else {
        throw IllegalMachine(
            __FILE__, __LINE__, __func__, 
            std::string("Cannot schedule '") + 
            programOperation.toString() +
            "' ensure that at least one FU supports the operation and is "
            "connected to some register file.");
    } 
}

/**
 * Adds or counts register copies required for the given operation to be 
 * assigned without connectivity problems to the given FU.
 *
 * @param programOperation The operation execution.
 * @param fu The function unit the operation should be able to be assigned to.
 * @param ddg If NULL, no register copies are really added, only
 * the required copies are counted, otherwise register copies are added and
 * the DDG is updated.
 * @return Returns the count of register copies required if the given operation
 * execution was assigned to the given FU.
 */
int
RegisterCopyAdder::addRegisterCopies(
    ProgramOperation& programOperation,
    const TTAMachine::FunctionUnit& fu,
    DataDependenceGraph* ddg) {
    
    const bool countOnly = (ddg == NULL);
    int registerCopies = 0;
    for (int input = 0; input < programOperation.inputMoveCount(); ++input) {
        MoveNode& m = programOperation.inputMove(input);                
        const int copies = addConnectionRegisterCopies(m, fu, ddg);
        if (copies == INT_MAX)
            if (countOnly)
                return INT_MAX;
            else
                assert(false && "Temp moves not possible for the FU.");
        
        registerCopies += copies;
    }

    for (int output = 0; output < programOperation.outputMoveCount(); 
         ++output) {
        MoveNode& m = programOperation.outputMove(output);
        const int copies = addConnectionRegisterCopies(m, fu, ddg);
        if (copies == INT_MAX)
            if (countOnly)
                return INT_MAX;
            else
                assert(false && "Temp moves not possible for the FU.");
        registerCopies += copies;        
    }
    return registerCopies;
}

/**
 * Adds or counts register copies required for a move between the given ports.
 *
 * Returns 0 in case there is a connection already.
 *
 * @param originalMove The move that might not be unschedulable due to missing
 * connectivity. Will be modified to read from the temporary reg instead in
 * case connectivity is missing.
 * @param sourcePort The source port.
 * @param destinationPort The destination port.
 * @param ddg If NULL, no register copies are really added, only the 
 *        required copies are counted, otherwise register copies are added 
 *        and the DDG is updated.
 * @return Returns the count of register copies required.
 * @exception Exception Throws in case the machine does not have
 *            enough connectivity even when 2 register copies are used.
 */
int 
RegisterCopyAdder::addConnectionRegisterCopies(
    MoveNode& originalMove,
    const TTAMachine::Port& sourcePort,
    const TTAMachine::Port& destinationPort,
    DataDependenceGraph* ddg) {

    if (MachineConnectivityCheck::isConnected(sourcePort, destinationPort))
        return 0;

    typedef SimpleInterPassDatum<
    std::vector<std::pair<TTAMachine::RegisterFile*, int> > > 
        TempRegData;
    
    if (!interPassData_.hasDatum("SCRATCH_REGISTERS") ||
        (dynamic_cast<TempRegData&>(
            interPassData_.datum("SCRATCH_REGISTERS"))).size() == 0)
        throw IllegalProgram(
            __FILE__, __LINE__, __func__,
            "No scratch registers available for temporary moves.");

    const TempRegData& tempRegs = 
        dynamic_cast<TempRegData&>(interPassData_.datum("SCRATCH_REGISTERS"));

    const bool countOnly = (ddg == NULL);

    const int minRegisterWidth = 
        std::min(sourcePort.width(), destinationPort.width());

    int tempRegisterIndex1 = -1;
    // try to find a RF which is connected both to source and destination
    // ports
    const TTAMachine::RegisterFile* connectionRF = NULL;
    const TTAMachine::RegisterFile* sourceConnectedRF = NULL;
    int correctSizeTempsFound = 0;
    for (std::size_t i = 0; i < tempRegs.size(); ++i) {
        const TTAMachine::RegisterFile& rf = *tempRegs.at(i).first;
        if (rf.width() < minRegisterWidth) {
            continue;
        }
        ++correctSizeTempsFound;
        if (MachineConnectivityCheck::isConnected(sourcePort, rf)) {

            // found a RF that is connected to the source port
            sourceConnectedRF = &rf;
            tempRegisterIndex1 = tempRegs.at(i).second;
            if (MachineConnectivityCheck::isConnected(rf, destinationPort)) {
                /// @todo check that the buses support the guard of the
                /// original move? This is not critical now as we are writing
                /// to a scratch register, we can do that speculatively without
                /// breaking anything.

                // found a RF that is connected both to the src and dst
                connectionRF = &rf;
                break;
            }
        }
    }
    
    if (correctSizeTempsFound == 0) {
        throw IllegalProgram(
            __FILE__, __LINE__, __func__, 
            (boost::format(
                "Register allocator didn't reserve a large enough "
                "connectivity register for routing the move %s.") 
             % originalMove.toString()).str());
    }

    // the RF for the first temp reg copy
    const TTAMachine::RegisterFile* tempRF1 = NULL;
    // the RF for the (optional) second temp reg copy
    const TTAMachine::RegisterFile* tempRF2 = NULL;

    int regsRequired = INT_MAX;

    int tempRegisterIndex2 = -1;
    if (connectionRF != NULL) {
        // found a RF for the temporary register (src -> temp1 -> dst)
        // already, no 2nd route register needed
        tempRF1 = connectionRF;
        regsRequired = 1;
    } else if (sourceConnectedRF != NULL) {
        // need two register copies, first add the temp reg copy to the
        // first RF
        tempRF1 = sourceConnectedRF;
        // find the second temporary RF (temp1 -> temp2 -> dst)
        for (std::size_t i = 0; i < tempRegs.size(); i++) {
            const TTAMachine::RegisterFile& rf = *tempRegs.at(i).first;

            
            if (rf.width() < minRegisterWidth) {
                continue;
            }
            if (MachineConnectivityCheck::isConnected(*tempRF1, rf) &&
                MachineConnectivityCheck::isConnected(rf, destinationPort)) {
                tempRF2 = &rf;
                tempRegisterIndex2 = tempRegs.at(i).second;
                /// @todo check that the buses support the guard of the
                /// original move? This is not critical now as we are writing
                /// to a scratch register.
                regsRequired = 2;
                break;
            }
        }
    } 
    

    if (countOnly) {
        return regsRequired;
    }

    if (regsRequired > 2) {
        throw IllegalMachine(
            __FILE__, __LINE__, __func__, 
            std::string("Cannot schedule ") + 
            originalMove.toString() + 
            " ensure that all FUs are connected to at least one RF and " +
            "all RFs are connected to each other. " + 
            destinationPort.parentUnit()->name() + " cannot be reached.");
    }
    
    BasicBlockNode& bbn = ddg->getBasicBlockNode(originalMove);        

    /* two options:
       1:
       src -> temp1;
       temp1 -> dst;

       2:
       src -> temp1;
       temp1 -> temp2;
       temp2 -> dst;
    */

    // before splitting, annotate the possible return move so we can still
    // detect a procedure return in simulator
    if (originalMove.move().isReturn()) {
        TTAProgram::ProgramAnnotation annotation(
            TTAProgram::ProgramAnnotation::ANN_STACKFRAME_PROCEDURE_RETURN);
        originalMove.move().setAnnotation(annotation);
    }

    // add the register copy moves

    // create the first temporary move 'src -> temp1'
    // find a connected port in the temp reg file
    const TTAMachine::RFPort* dstRFPort = NULL;
    for (int p = 0; p < tempRF1->portCount(); ++p) {
        const TTAMachine::RFPort* RFport = tempRF1->port(p);

        if (MachineConnectivityCheck::isConnected(sourcePort, *RFport)) {
            dstRFPort = RFport;
            break;
        }
    }
    assert(dstRFPort != NULL);

    TTAProgram::TerminalRegister* temp1 =  
        new TTAProgram::TerminalRegister(*dstRFPort, tempRegisterIndex1);

    TTAProgram::Terminal* originalDestination = 
        originalMove.move().destination().copy();

    MoveNode* firstMove = NULL;
    MoveNode* lastMove = NULL;

    TTAProgram::ProgramAnnotation connMoveAnnotation(
        TTAProgram::ProgramAnnotation::ANN_CONNECTIVITY_MOVE);

    /* Make sure that the original move is still the one that should
       be in the ProgramOperation, i.e., an operation move. The original
       move should be either the last of the chain or the first, in case
       it's input or output move, respectively. */

    TTAProgram::Terminal& omDest = originalMove.move().destination();
    // may not catch RA on this one.
    if (omDest.isFUPort() && !omDest.isRA()) {
        // input move
        firstMove = new MoveNode(originalMove.move().copy());
        lastMove = &originalMove;
        ddg->addNode(*firstMove,bbn);
        firstMove->move().addAnnotation(connMoveAnnotation);

    } else if (originalMove.move().source().isFUPort()) {
        // output move
        firstMove = &originalMove;
        lastMove = new MoveNode(originalMove.move().copy());
        ddg->addNode(*lastMove,bbn);
        lastMove->move().addAnnotation(connMoveAnnotation);
    } else {
        abortWithError("Can add temp regs only for operation moves.");
    }
  
    // src -> temp1
    firstMove->move().setDestination(temp1->copy());

    TTAProgram::TerminalRegister* lastMoveSrc = temp1;

    /// in the case 2, add the extra register copy 'temp1 -> temp2'
    MoveNode* regToRegCopy = NULL;
    if (tempRF2 != NULL) {
        regToRegCopy = new MoveNode(originalMove.move().copy());

        // find a connected port in the temp2 reg file
        const TTAMachine::RFPort* dstRFPort2 = NULL;
        for (int p = 0; p < tempRF2->portCount(); ++p) {
            const TTAMachine::RFPort* RFport = tempRF2->port(p);
            if (MachineConnectivityCheck::isConnected(
                    *RFport, destinationPort)) {
                dstRFPort2 = RFport;
                break;
            }
        }
        assert(dstRFPort2 != NULL);
        TTAProgram::TerminalRegister* temp2 =         
            new TTAProgram::TerminalRegister(*dstRFPort2, tempRegisterIndex2);

        // temp1 -> temp2
        regToRegCopy->move().setSource(temp1); // temp1 now owned by the regCopy
        regToRegCopy->move().setDestination(temp2->copy());
        lastMoveSrc = temp2;
        
        ddg->addNode(*regToRegCopy,bbn);
        regToRegCopy->move().addAnnotation(connMoveAnnotation);
    }

    // lastMoveSrc={temp1|temp2} -> dst
    lastMove->move().setSource(lastMoveSrc);
    lastMove->move().setDestination(originalDestination);

    // update the DDG edges
    fixDDGEdgesInTempRegChain(
        *ddg, originalMove, firstMove, regToRegCopy, lastMove, tempRF1,
        tempRF2, tempRegisterIndex1, tempRegisterIndex2);

    return regsRequired;
}

/**
 * Adds register copies required for transporting an immediate to the given
 * port.
 *
 * If there is at least one IU that is connected to the destination and
 * the immediate is going to be converted to a long immediate, does not
 * add any temp registers, but annotates the move with the IU choice so
 * RM can assign it correctly later.
 *
 * @param originalMove The move that cannot be scheduled due to missing
 *        connectivity. Will be modified to read from the temporary reg 
 *        instead.
 * @param destinationPort The destination port.
 * @param ddg If NULL, no register copies are really added, only
 *        the required copies are counted, otherwise register copies are added 
 *        and the DDG is updated.
 * @return Returns the count of register copies required.
 * @exception Exception Throws in case the machine does not have
 *            enough connectivity even when 2 register copies are used or 
 *            no scratch registers to redirect unconnected moves through.
 */
int 
RegisterCopyAdder::addConnectionRegisterCopiesImmediate(
    MoveNode& originalMove,
    const TTAMachine::Port& destinationPort,
    DataDependenceGraph* ddg) {
       
    assert(originalMove.isSourceConstant());

    typedef SimpleInterPassDatum<
    std::vector<std::pair<TTAMachine::RegisterFile*, int> > > 
        TempRegData;
    
    if (!interPassData_.hasDatum("SCRATCH_REGISTERS") ||
        (dynamic_cast<TempRegData&>(
            interPassData_.datum("SCRATCH_REGISTERS"))).size() == 0)
        throw IllegalProgram(
            __FILE__, __LINE__, __func__,
            "No scratch registers available for temporary moves.");

    const TempRegData& tempRegs = 
        dynamic_cast<TempRegData&>(interPassData_.datum("SCRATCH_REGISTERS"));

    const bool countOnly = (ddg == NULL);

    const TTAProgram::TerminalImmediate& immediate =
        dynamic_cast<const TTAProgram::TerminalImmediate&>(
            originalMove.move().source());

    const int immediateBitWidth = 
        MachineConnectivityCheck::requiredImmediateWidth(false, immediate);

    // try to find a RF which is connected to the bus with immediate 
    // capabilities and the destination port
    const TTAMachine::RegisterFile* connectionRF = NULL;
    const TTAMachine::RegisterFile* fallBackRF = NULL;
    const TTAMachine::RegisterFile* sourceConnectedRF = NULL;
    int connectionIndex = -1;
    int sourceConnectedIndex = -1;
    int fallBackIndex = -1;

    int correctSizeTempsFound = 0;
    for (std::size_t i = 0; i < tempRegs.size(); ++i) {
        const TTAMachine::RegisterFile& rf = *tempRegs.at(i).first;
        if (rf.width() < immediateBitWidth) {
            continue;
        }
        ++correctSizeTempsFound;
        if (MachineConnectivityCheck::canTransportImmediate(immediate, rf)) {
            sourceConnectedRF = &rf;
            sourceConnectedIndex = tempRegs.at(i).second;
            if (MachineConnectivityCheck::isConnected(rf, destinationPort)) {
                /// @todo check that the buses support the guard of the
                /// original move
                connectionRF = &rf;
                connectionIndex = tempRegs.at(i).second;
                break;
            }
        } else {
            fallBackRF = &rf;
            fallBackIndex = tempRegs.at(i).second;
        }
    }

    assert(
        correctSizeTempsFound > 0 && 
        "Register allocator didn't reserve a large enough temp reg!");

    /* In case no RF was found, it is possible that there was no bus
       that could transfer the given immediate, thus it does not really
       matter which RF choose here, as the immediate will be transferred
       to the IU first anyways, thus IMM -> dst will be converted to
       IU -> dst, from IU there should be a connection to all RFs, due to
       the TCE connectivity restriction. */
    if (sourceConnectedRF == NULL) {
        sourceConnectedRF = fallBackRF;
        connectionRF = fallBackRF;
        connectionIndex = fallBackIndex;
        sourceConnectedIndex = fallBackIndex;
    }

    // the RF for the first temp reg copy
    const TTAMachine::RegisterFile* tempRF1 = NULL;
    // the RF for the (optional) second temp reg copy
    const TTAMachine::RegisterFile* tempRF2 = NULL;

    int tempRegisterIndex1 = -1;
    int tempRegisterIndex2 = -1;

    if (connectionRF != NULL) {
        // found a RF for the temporary register (src -> temp1 -> dst)
        tempRF1 = connectionRF;
        tempRegisterIndex1 = connectionIndex;
    } else if (sourceConnectedRF != NULL) {
        // need two register copies, first add the temp reg copy to the
        // first RF
        tempRF1 = sourceConnectedRF;
        tempRegisterIndex1 = sourceConnectedIndex;
        // find the second temporary RF (temp1 -> temp2 -> dst)
        for (std::size_t i = 0; i < tempRegs.size(); i++) {
            const TTAMachine::RegisterFile& rf = *tempRegs.at(i).first;
            if (rf.width() < immediateBitWidth) 
                continue;
            if (MachineConnectivityCheck::isConnected(*tempRF1, rf) &&
                MachineConnectivityCheck::isConnected(rf, destinationPort)) {
                tempRF2 = &rf;
                tempRegisterIndex2 = tempRegs.at(i).second;
                break;
            }
        }
    } 

    if (tempRF1 == NULL || (connectionRF == NULL && tempRF2 == NULL)) {
        if (countOnly)
            return INT_MAX;
        else
            throw IllegalMachine(
                __FILE__, __LINE__, __func__, 
                std::string("Cannot schedule ") + originalMove.toString() + 
                " ensure that all FUs are connected to at least one RF and " +
                "all RFs are connected to each other. " + 
                destinationPort.parentUnit()->name() + " cannot be reached.");
    }

    int regsRequired = INT_MAX;

    if (tempRF2 != NULL) {
        regsRequired = 2;
    } else {
        regsRequired = 1;
    }

    if (countOnly) {
        return regsRequired;
    }

    BasicBlockNode& bbn = ddg->getBasicBlockNode(originalMove);

    /* two options:
       1:
       IMM -> temp1;
       temp1 -> dst;

       2:
       IMM -> temp1;
       temp1 -> temp2;
       temp2 -> dst;
    */

    // before splitting, annotate the possible return move so we can still
    // detect a procedure return in simulator
    if (originalMove.move().isReturn()) {
        TTAProgram::ProgramAnnotation annotation(
            TTAProgram::ProgramAnnotation::ANN_STACKFRAME_PROCEDURE_RETURN);
        originalMove.move().setAnnotation(annotation);
    }

    // add the register copy moves

    // create the first temporary move 'src -> temp1'
    // find a connected port in the temp reg file
    const TTAMachine::RFPort* dstRFPort = NULL;
    const TTAMachine::RFPort* fallBackRFPort = NULL;
    for (int p = 0; p < tempRF1->portCount(); ++p) {
        const TTAMachine::RFPort* RFport = tempRF1->port(p);
        if (MachineConnectivityCheck::canTransportImmediate(
                immediate, *RFport)) {
            dstRFPort = RFport;
            break;
        } else {
            fallBackRFPort = RFport;
        }
    }
    // see comment for the "fall back RF" above, same applies here
    if (dstRFPort == NULL) {
        dstRFPort = fallBackRFPort;
    }

    assert(dstRFPort != NULL);

    TTAProgram::TerminalRegister* temp1 =  
        new TTAProgram::TerminalRegister(*dstRFPort, tempRegisterIndex1);

    TTAProgram::Terminal* originalDestination = 
        originalMove.move().destination().copy();

    MoveNode* firstMove = NULL;
    MoveNode* lastMove = NULL;

    TTAProgram::ProgramAnnotation connMoveAnnotation(
        TTAProgram::ProgramAnnotation::ANN_CONNECTIVITY_MOVE);

    /* Make sure that the original move is still the one that should
       be in the ProgramOperation, i.e., an operation move. The original
       move should be the last of the chain (should be an input move only
       as this is an immediate move). */
    if (originalMove.move().destination().isFUPort() ||
        originalMove.move().destination().isGPR()) {
        // input move
        firstMove = new MoveNode(originalMove.move().copy());
        lastMove = &originalMove;
        ddg->addNode(*firstMove,bbn);
        firstMove->move().addAnnotation(connMoveAnnotation);
    } else {
        abortWithError("Can add imm temp regs only for FU or RF moves.");
    }
  
    // src -> temp1
    firstMove->move().setDestination(temp1->copy());

    TTAProgram::TerminalRegister* lastMoveSrc = temp1;

    // in the case 2, add the extra register copy 'temp1 -> temp2'
    MoveNode* regToRegCopy = NULL;
    if (tempRF2 != NULL) {
        regToRegCopy = new MoveNode(originalMove.move().copy());

        // find a connected port in the temp2 reg file
        const TTAMachine::RFPort* dstRFPort2 = NULL;
        for (int p = 0; p < tempRF2->portCount(); ++p) {
            const TTAMachine::RFPort* RFport = tempRF2->port(p);
            if (MachineConnectivityCheck::isConnected(
                    *RFport, destinationPort)) {
                dstRFPort2 = RFport;
                break;
            }
        }
        assert(dstRFPort2 != NULL);
        TTAProgram::TerminalRegister* temp2 =         
            new TTAProgram::TerminalRegister(*dstRFPort2, tempRegisterIndex2);

        // temp1 -> temp2
        regToRegCopy->move().setSource(temp1); // temp1 now owned by the regCopy
        regToRegCopy->move().setDestination(temp2->copy());
        lastMoveSrc = temp2;
        
        ddg->addNode(*regToRegCopy,bbn);
        regToRegCopy->move().addAnnotation(connMoveAnnotation);
    }

    // lastMoveSrc={temp1|temp2} -> dst
    lastMove->move().setSource(lastMoveSrc);
    lastMove->move().setDestination(originalDestination);

    // update the DDG edges
    fixDDGEdgesInTempRegChain(
        *ddg, originalMove, firstMove, regToRegCopy, lastMove, tempRF1,
        tempRF2, tempRegisterIndex1, tempRegisterIndex2);

    return regsRequired;
}

/**
 * Fixes edges in DDG after creating the temporary register chain.
 *
 * @todo currently leaves some inter-bb-antidependencies out,
 * these are caught later when doing delay slot filling in order 
 * to get it working.
 *
 * @param ddg The DDG to fix.
 * @param originalMove The move which got the temp reg chain added.
 * @param firstMove First move in the chain.
 * @param regToRegCopy A register to register copy in case of a chain of
 *        length 2 (NULL otherwise).
 * @param lastMove The last move in the chain.
 * @param tempRF1 The RF used for the 1st temp move.
 * @param tempRF2 The RF used for the 2nd temp move (optional).
 * @param tempRegisterIndex1 The index of the 1st temp register.
 * @param tempRegisterIndex2 The index of the 2nd temp register.
 *
 */
void
RegisterCopyAdder::fixDDGEdgesInTempRegChain(
    DataDependenceGraph& ddg,
    MoveNode& originalMove,
    MoveNode* firstMove,
    MoveNode* regToRegCopy,
    MoveNode* lastMove,
    const TTAMachine::RegisterFile* tempRF1, 
    const TTAMachine::RegisterFile* tempRF2, 
    int tempRegisterIndex1,
    int tempRegisterIndex2) {

    // the guard edge needs to be copied to all new nodes
    DataDependenceEdge* guardEdge = NULL;
    MoveNode* guardDef = NULL;
    
    // move the possible WAW/WAR edge(s) pointing to the first move to point to
    // the last move in the chain (in case of output move)
    DataDependenceGraph::EdgeSet inEdges = ddg.inEdges(originalMove);
    for (DataDependenceGraph::EdgeSet::iterator i = inEdges.begin(); 
         i != inEdges.end(); ++i) {
        DataDependenceEdge& edge = **i;
        if (edge.dependenceType() == DataDependenceEdge::DEP_WAR ||
            edge.dependenceType() == DataDependenceEdge::DEP_WAW) {

            MoveNode& source = ddg.tailNode(edge);
            DataDependenceEdge* edgeCopy = new DataDependenceEdge(edge);
            ddg.removeEdge(edge);
            ddg.connectNodes(source, *lastMove, *edgeCopy);
        } else if (edge.guardUse()) {
            assert(guardEdge == NULL && "Multiple guard edges not supported.");
            // save the guard edge for later
            guardEdge = new DataDependenceEdge(edge);
            guardDef = &ddg.tailNode(edge);
            // remove it for now, and add it back later
            ddg.removeEdge(edge);
        }
    }    

    // move the possible WAW/WAR edge(s) going out from the original move 
    // to point to the first move in the chain except M_waw, it should still
    // stay between the store operand moves
    DataDependenceGraph::EdgeSet outEdges = ddg.outEdges(originalMove);
    for (DataDependenceGraph::EdgeSet::iterator i = outEdges.begin(); 
         i != outEdges.end(); ++i) {
        DataDependenceEdge& edge = **i;
        MoveNode& dest = ddg.headNode(edge);
        if (edge.dependenceType() == DataDependenceEdge::DEP_WAR ||
            edge.dependenceType() == DataDependenceEdge::DEP_WAW) {
            // do not touch memory edges, they should still be going out from
            // original moves
            if (edge.edgeReason() == DataDependenceEdge::EDGE_MEMORY) {
                continue;
            }

            DataDependenceEdge* edgeCopy = new DataDependenceEdge(edge);
            ddg.removeEdge(edge);
            ddg.connectNodes(*firstMove, dest, *edgeCopy);
        }        
    }    


    // move the rest of the edges, incoming to the first node, outgoing to
    // the last
    ddg.moveInEdges(originalMove, *firstMove);
    ddg.moveOutEdges(originalMove, *lastMove);

    // special case: the RV edge should still point to the original in
    // case of a jump, also M_raw should point to the mem operation operand
    // move instead of the operand temp move
    inEdges = ddg.inEdges(*firstMove);
    for (DataDependenceGraph::EdgeSet::iterator i = inEdges.begin(); 
         i != inEdges.end(); ++i) {
        DataDependenceEdge& edge = **i;
        MoveNode& source = ddg.tailNode(edge);
        if (edge.dependenceType() == DataDependenceEdge::DEP_RAW &&
            (originalMove.move().isJump() && 
             !source.move().destination().equals(
                 firstMove->move().source()) && 
             !source.move().destination().isFUPort()) ||
            edge.edgeReason() == DataDependenceEdge::EDGE_MEMORY) {
            DataDependenceEdge* edgeCopy = new DataDependenceEdge(edge);
            ddg.removeEdge(edge);
            ddg.connectNodes(source, originalMove, *edgeCopy);
        }
    }    

    // add the new edge(s)
    if (regToRegCopy != NULL) {
        DataDependenceEdge* edge1 = 
            new DataDependenceEdge(
                DataDependenceEdge::EDGE_REGISTER, 
                DataDependenceEdge::DEP_RAW);
        ddg.connectNodes(*firstMove, *regToRegCopy, *edge1);

        DataDependenceEdge* edge2 = 
            new DataDependenceEdge(
                DataDependenceEdge::EDGE_REGISTER, 
                DataDependenceEdge::DEP_RAW);
        ddg.connectNodes(*regToRegCopy, *lastMove, *edge2);

        MoveNode* lastUse =
            ddg.lastScheduledRegisterRead(*tempRF2, tempRegisterIndex2);

        if (lastUse != NULL) {
            // the WAR edges from the last (scheduled) use of the temporary
            // registers
            DataDependenceEdge* edge3 = 
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER, 
                    DataDependenceEdge::DEP_WAR);

            ddg.connectNodes(*lastUse, *regToRegCopy, *edge3);
        } 
    } else {
        DataDependenceEdge* edge1 = 
            new DataDependenceEdge(
                DataDependenceEdge::EDGE_REGISTER, 
                DataDependenceEdge::DEP_RAW);
        ddg.connectNodes(*firstMove, *lastMove, *edge1);
    } 

    MoveNode* lastUse =
        ddg.lastScheduledRegisterRead(*tempRF1, tempRegisterIndex1);

    if (lastUse != NULL) {
        DataDependenceEdge* war = 
            new DataDependenceEdge(
                DataDependenceEdge::EDGE_REGISTER, 
                DataDependenceEdge::DEP_WAR);

        ddg.connectNodes(*lastUse, *firstMove, *war);
    }

    if (!originalMove.move().isUnconditional()) {
        // copy the guard RAW edge to all nodes
        // TODO: breaks if guard comes outside of the BB.
        if (guardEdge != NULL) {
            ddg.connectNodes(*guardDef, *firstMove, *guardEdge);
            ddg.connectNodes(
                *guardDef, *lastMove, *(new DataDependenceEdge(*guardEdge)));
            if (regToRegCopy != NULL) {
                ddg.connectNodes(
                    *guardDef, *regToRegCopy, 
                    *(new DataDependenceEdge(*guardEdge)));
            }
        }
    }
}

/**
 * Adds register copies required for the given transport.
 *
 * Returns 0 in case there is a connection already.
 *
 * @param moveNode The transport.
 * @param fu The assumed function unit assigned to the operation of the move.
 * @param ddg If NULL, no register copies are really added, only
 *        the required copies are counted, otherwise register copies are added 
 *        and the DDG is updated.
 * @return Returns the count of register copies required.
 */
int 
RegisterCopyAdder::addConnectionRegisterCopies(
    MoveNode& moveNode,
    const TTAMachine::FunctionUnit& fu,
    DataDependenceGraph* ddg) {
    
    const bool countOnly = (ddg == NULL);

    // collect the set of possible candidate destination ports (in case of 
    // RFs, there can be multiple ports)
    TTAProgram::Terminal& dest = moveNode.move().destination();
    std::set<const TTAMachine::Port*> dstPorts;
    if (dest.isGPR()) {
        // add all write ports of the RF to the dstPorts
        const TTAMachine::RegisterFile& rf = dest.registerFile();
        const int ports = rf.portCount();
        for (int p = 0; p < ports; ++p) {
            const TTAMachine::RFPort* port = rf.port(p);
            if (port->isInput())
                dstPorts.insert(port);
        }
    } else if (dest.isFUPort()) {
        if (dynamic_cast<const TTAMachine::SpecialRegisterPort*>(
                &dest.port())) {
            // the return address port
            dstPorts.insert(fu.machine()->controlUnit()->returnAddressPort());
        } else {
            // regular FU port
            dstPorts.insert(
                fu.operation(
                    dest.hintOperation().name())->port(dest.operationIndex()));
        }
    } else {
        abortWithError(
            "Unsupported move destination type in move '" + 
            moveNode.toString() + "'.");
    } 

    // collect the set of possible candidate source ports (in case of 
    // RFs, there can be multiple)
    std::set<const TTAMachine::Port*> srcPorts;
    TTAProgram::Terminal& source = moveNode.move().source();
    if (source.isGPR()) {
        // add all read ports of the RF to the srcPorts
        const TTAMachine::RegisterFile& rf = source.registerFile();
        const int ports = rf.portCount();
        for (int p = 0; p < ports; ++p) {
            const TTAMachine::RFPort* port = rf.port(p);
            if (port->isOutput()) {
                srcPorts.insert(port);
            }
        }
    } else if (source.isFUPort()) {
        
        if (dynamic_cast<const TTAMachine::SpecialRegisterPort*>(
                &source.port())) {
            // source is the return address port, it might be still pointing
            // to the universal_bus, so we'll check where the RA port is
            // connected in the target machine
            srcPorts.insert(fu.machine()->controlUnit()->returnAddressPort());
        } else {
            // a normal FU port
            srcPorts.insert(
                fu.operation(
                    source.hintOperation().name())->port(
                        source.operationIndex()));
        }
    } else if (source.isImmediate() && dest.isFUPort()) {
        /** Check that there is a bus with wide enough immediate slot
            that is connected to the target, if not, convert the "constant to
            an operand move" to "a constant to a register move". 

            This way we ensure connectivity after IU assignment. */

        const TTAMachine::Port* destPort = NULL;
        if (dynamic_cast<const TTAMachine::SpecialRegisterPort*>(
                &dest.port())) {
            // the return address port
            destPort = fu.machine()->controlUnit()->returnAddressPort();
        } else {
            destPort = 
                fu.operation(
                    dest.hintOperation().name())->port(dest.operationIndex());
        }

        if (destPort == NULL) {
            throw IllegalProgram(
                __FILE__, __LINE__, __func__,
                (boost::format(
                    "Could not find destination FU port candidate for "
                    "move '%s'. Is the operand-port binding missing?")
                 % moveNode.toString()).str());
        }
        const bool connectionFound = 
            MachineConnectivityCheck::canTransportImmediate(
                dynamic_cast<const TTAProgram::TerminalImmediate&>(
                    moveNode.move().source()), *destPort);

        // no temp move required, there's at least one bus that can transport 
        // the IMM to the target FU directly
        if (connectionFound) {
            return 0;
        }
                    
        // is there a bus (at all) with a wide enough immediate field?
        const bool busFound = rm_.canTransportImmediate(moveNode);

        /**
           Convert:
           
           IMM -> FU

           to

           IMM -> RF
           RF  -> FU

           In case IMM does not fit in any bus, after scheduler's LIMM 
           conversion, this should end up being:

           [IMM -> IU] (long immediate transport)
           IU -> RF
           RF -> FU

           This should fix the following cases: 
           1) IMM only fits in a bus that is not directly connected to the FU.
           2) IMM does not fit in any bus, thus should be transported through
              an IU. In that case, ensure connectivity between a IU and FU.
        */
        
        if (busFound && !connectionFound) {
            // case 1)
            // add SIMM -> RF, RF -> FU, (needs a temp register)
        } else if (!busFound && !connectionFound) {
            // case 2)
            /* Ensure there's at least one IU connecting to the target,
               if there is, no temp register is needed as the connectivity
               appears after LIMM conversion (IU is a register file that
               is guaranteed to be connected at least to a RF that is 
               connected to the target, if not directly to the target). */
            TTAMachine::Machine::ImmediateUnitNavigator nav = 
                fu.machine()->immediateUnitNavigator();
            for (int i = 0; i < nav.count(); ++i) {
                const TTAMachine::ImmediateUnit* iu = nav.item(i);
                if (MachineConnectivityCheck::isConnected(*iu, *destPort)) {
                    return 0;
                }
            }
            /* None of the IUs are connected to the target FU,
               need to add one or more temp registers. */
        } else {
            abortWithError("Should be an impossible situation.");
        }
        return addConnectionRegisterCopiesImmediate(
            moveNode, *(*dstPorts.begin()), ddg);
    } else if (source.isImmediate() && dest.isGPR()) {
        // IMM -> REG should always be possible, at least after LIMM
        // conversion (all IUs must be connected to all RFs), so no temp 
        // registers needed
        // register copy adder should not get single immediate moves,
        // only operations anyways
        return 0;                        
    } else {
        abortWithError(
            "Unsupported move source type in move '" + 
            moveNode.toString() + "'.");
    }

    // go through all srcPorts,dstPorts pairs and see which of them require
    // least connection registers and finally select one pair and add
    // connection registers accordingly

    typedef std::set<
    std::pair<const TTAMachine::Port*, const TTAMachine::Port*> >
        CombinationSet;

    CombinationSet pairs = AssocTools::pairs(srcPorts, dstPorts);
    
    const std::pair<const TTAMachine::Port*, const TTAMachine::Port*>*
        bestConnection = NULL;
    int registersRequired = INT_MAX;
    for (CombinationSet::const_iterator i = pairs.begin(); i != pairs.end();
         ++i) {
        const TTAMachine::Port& src = *(*i).first;
        const TTAMachine::Port& dst = *(*i).second;
        if ((&src == NULL) || (&dst == NULL)) {
            throw IllegalMachine(
                __FILE__, __LINE__, __func__,
                "Could not schedule move " + moveNode.toString());
        }
        int regCount = addConnectionRegisterCopies(moveNode, src, dst);

        if (regCount == 0) {
            return 0;
        }

        if (regCount < registersRequired) {
            bestConnection = &(*i);
            registersRequired = regCount;
        }
    }

    if (countOnly) {
        return registersRequired;
    }

    if (bestConnection == NULL) {
        throw IllegalMachine(
            __FILE__, __LINE__, __func__,
            "Could not schedule move '" + moveNode.toString() + 
            "' due to missing connectivity. "
            "Add a connection or a register file that connects "
            "the source and the destination.");
    }

    const TTAMachine::Port& src = *(bestConnection->first);
    const TTAMachine::Port& dst = *(bestConnection->second);

    // actually add the connection now that we have found the best way
    return addConnectionRegisterCopies(moveNode, src, dst, ddg);
}

/**
 * Adds candidate FU annotations to the operation moves in case there is 
 * a limited set of FUs the operation can be assigned to.
 *
 * @param programOperation The operation of which moves to annotate.
 * @param machine The machine which contains the FUs.
 */
void 
RegisterCopyAdder::addCandidateSetAnnotations(
    ProgramOperation& programOperation,
    const TTAMachine::Machine& machine) {

    RegisterCopyCountIndex 
        registerCopiesRequired = 
        requiredRegisterCopiesForEachFU(machine, programOperation);
    
    std::set<std::string> candidates;
    // check if there's an FU that requires more than 0 copies even now 
    bool allGoodFUCandidates = true;
    // if the long immediate conversion should be performed to ensure
    // connectivity, in that case we add IU candidate sets for the
    // sources of the input moves that have such immediates
    for (std::map<const TTAMachine::FunctionUnit*, int>::const_iterator
             i = registerCopiesRequired.begin(); 
         i != registerCopiesRequired.end(); ++i) {

        const TTAMachine::FunctionUnit* u = (*i).first;
        if (registerCopiesRequired[u] > 0) {
            allGoodFUCandidates = false;
        } else {
            candidates.insert(u->name());
        }
    }

    // no annotations needed if any selection for the FU is equally good
    // in the connectivity point of view
    if (allGoodFUCandidates) {
        return;
    }

    // add annotations to moves of the ProgramOperation that restrict the
    // choice of FU to the ones that are possible to assign with the current
    // register copies
    for (int input = 0; input < programOperation.inputMoveCount(); ++input) {
        MoveNode& m = programOperation.inputMove(input);
        
        for (std::set<std::string>::const_iterator i = candidates.begin();
             i != candidates.end(); ++i) {
            std::string candidateFU = (*i);
            TTAProgram::ProgramAnnotation dstCandidate(
                TTAProgram::ProgramAnnotation::ANN_CANDIDATE_UNIT_DST, 
                candidateFU);
            m.move().addAnnotation(dstCandidate);
        }
    }

    for (int output = 0; output < programOperation.outputMoveCount(); 
         ++output) {
        MoveNode& m = programOperation.outputMove(output);

        for (std::set<std::string>::const_iterator i = candidates.begin();
             i != candidates.end(); ++i) {
            std::string candidateFU = (*i);
            TTAProgram::ProgramAnnotation srcCandidate(
                TTAProgram::ProgramAnnotation::ANN_CANDIDATE_UNIT_SRC, 
                candidateFU);
            m.move().addAnnotation(srcCandidate);
        }
    }
}

