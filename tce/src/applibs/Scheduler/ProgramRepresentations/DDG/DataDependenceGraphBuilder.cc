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
 * @file DataDependenceGraphBuilder.cc
 *
 * Implementation of data dependence graph builder.
 *
 * DDG's can be built only from unscheduled code. Registers can
 * however have been allocated.
 *
 * @author Heikki Kultala 2006-2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include <llvm/CodeGen/MachineInstr.h>
#include <llvm/CodeGen/MachineMemOperand.h>

#include "AssocTools.hh"
#include "ContainerTools.hh"
#include "TCEString.hh"
#include "SequenceTools.hh"

#include "Program.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "Operation.hh"
#include "SpecialRegisterPort.hh"
#include "Move.hh"
#include "ProgramOperation.hh"
#include "RegisterFile.hh"
#include "Machine.hh"
#include "UniversalMachine.hh"
#include "Exception.hh"
#include "UnboundedRegisterFile.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "MoveNodeSet.hh"
#include "Operand.hh"
#include "POMDisassembler.hh"
#include "DisassemblyRegister.hh"
#include "Move.hh"
#include "ControlFlowGraph.hh"
#include "ControlFlowEdge.hh"
#include "BasicBlockNode.hh"
#include "BasicBlock.hh"
#include "DataDependenceGraphBuilder.hh"
#include "DataDependenceEdge.hh"
#include "MemoryAliasAnalyzer.hh"

#include "TerminalRegister.hh"
#include "TerminalFUPort.hh"

#include "ConstantAliasAnalyzer.hh"
#include "FalseAliasAnalyzer.hh"
#include "LLVMAliasAnalyzer.hh"
#include "LLVMTCECmdLineOptions.hh"
//#include "StackAliasAnalyzer.hh"
//#include "OffsetAliasAnalyzer.hh"
//#include "TrivialAliasAnalyzer.hh"

#include "InterPassData.hh"
#include "InterPassDatum.hh"
//#include "SchedulerCmdLineOptions.hh"

#include "MachineInfo.hh"

using namespace TTAProgram;
using namespace TTAMachine;

using std::list;

static const int REG_SP = 1;
static const int REG_RV = 0;
static const int REG_IPARAM = 2;
static const int REG_RV_HIGH = 6;
static const int REG_FP = 7;

POP_COMPILER_DIAGS

//#define USE_FALSE_AA

/**
 * Constructor of Data Dependence graph builder.
 *
 * This constructor does not take special registers from 
 * interpass data, so it must analyze them from the
 * code annotations. Used with old frontend.
 */
DataDependenceGraphBuilder::DataDependenceGraphBuilder() :
    interPassData_(NULL), cfg_(NULL), rvIsParamReg_(false) {

    /// constant alias AA check aa between global variables.
    addAliasAnalyzer(new ConstantAliasAnalyzer);
    
#ifdef USE_FALSE_AA
    /// defining USE_FALSE_AA results in faster but
    /// broken code. just for testing theoretical benefits.
    addAliasAnalyzer(new FalseAliasAnalyzer);
#endif
    LLVMTCECmdLineOptions* llvmOptions =
        dynamic_cast<LLVMTCECmdLineOptions*>(
                Application::cmdLineOptions());    
    if (llvmOptions != NULL && llvmOptions->disableLLVMAA() == false) {
        addAliasAnalyzer(new LLVMAliasAnalyzer);
    }
}

/**
 * Constructor of Data Dependence graph builder.
 *
 * This constructor takes special registers from 
 * interpass data.
 */
DataDependenceGraphBuilder::DataDependenceGraphBuilder(InterPassData& ipd) :
    // TODO: when param reg thing works, rvIsParamReg becomes true here
    interPassData_(&ipd), cfg_(NULL), rvIsParamReg_(true) {

    // Need to store data about special registers which have semantics
    // between function calls and exits. These are stack pointer,
    // return value register and the secondary "hi bits"
    // return value register.

    static const TCEString SP_DATUM = "STACK_POINTER";
    static const TCEString FP_DATUM = "FRAME_POINTER";
    static const TCEString RV_DATUM = "RV_REGISTER";
    // high part of 64-bit return values.
    static const TCEString RV_HIGH_DATUM = "RV_HIGH_REGISTER";

#ifdef ENABLE_BETTER_AA // todo: until backport some more from trunk
    SchedulerCmdLineOptions* options =
        dynamic_cast<SchedulerCmdLineOptions*>(
                Application::cmdLineOptions());

#endif

#ifdef USE_RESTRICTED_POINTER_AA
    /** seems to worsen restric_aa test results, enable when reason found.
       One reason is lowering 32-bit memory operations into 4 8-bit
       memory operations by llvm. These 4 have same index, which makes
       trivialAA to think they alias.
       This may be fixed on llvm side some day, but also, the trivialAA should
       check the sizes of the memory operations.
       But even when this is fixed, it seems the trivialAA still has some
       problems; offsets seem to be 0 sometimes when they should not
    */
    if (ipd.hasDatum("RESTRICTED_POINTERS_FOUND")) {
        addAliasAnalyzer(new
                TrivialAliasAnalyzer(
                    ipd.hasDatum("MULTIPLE_ADDRESS_SPACES_FOUND")));
    }
#endif

    if (ipd.hasDatum(SP_DATUM)) {
        RegDatum& sp = dynamic_cast<RegDatum&>(ipd.datum(SP_DATUM));
        TCEString spName =  sp.first + '.' + Conversion::toString(sp.second);
        specialRegisters_[REG_SP] = spName;

#ifdef ENABLE_BETTER_AA // todo: until backport some more from trunk
        // create stack alias analyzer if enabled.
        if ((options != NULL && options->enableStackAA())) {
            addAliasAnalyzer(new StackAliasAnalyzer(spName));
        }

        if (options != NULL && options->enableOffsetAA()) {
            addAliasAnalyzer(new OffsetAliasAnalyzer(spName));
        }
#endif
    } else {
        if (Application::verboseLevel() > 
            Application::VERBOSE_LEVEL_DEFAULT) {
            Application::logStream() 
                << "Warning: Stack pointer datum not found "
                << "in interpassdata given to ddg builder. "
                << "May generate invalid code if stack used."
                << std::endl;
        }
    }

    if (ipd.hasDatum(RV_DATUM)) {
        RegDatum& rv = dynamic_cast<RegDatum&>(ipd.datum(RV_DATUM));
        TCEString reg = rv.first + '.' + Conversion::toString(rv.second);
        specialRegisters_[REG_RV] = reg;
        if (rvIsParamReg_) {
            allParamRegs_.insert(reg);
        }

    } else {
        if (Application::verboseLevel() > 
            Application::VERBOSE_LEVEL_DEFAULT) {
            Application::logStream() 
                << "Warning: Return value register datum not found "
                << "in interpassdata given to ddg builder. "
                << "May generate invalid code if return values used."
                << std::endl;
        }
    }

    if (ipd.hasDatum(FP_DATUM)) {
        RegDatum& fp = dynamic_cast<RegDatum&>(ipd.datum(FP_DATUM));
        TCEString reg = fp.first + '.' + Conversion::toString(fp.second);
        specialRegisters_[REG_FP] = reg;
    } else {
        if (Application::verboseLevel() >
            Application::VERBOSE_LEVEL_DEFAULT) {
            Application::logStream()
                << "Warning: Frame Pointer Register datum not found "
                << "in interpassdata given to ddg builder. "
                << "May generate invalid code."
                << std::endl;
        }
    }

    if (ipd.hasDatum(RV_HIGH_DATUM)) {
        RegDatum& rvh = dynamic_cast<RegDatum&>(ipd.datum(RV_HIGH_DATUM));
        specialRegisters_[REG_RV_HIGH] =
            rvh.first + '.' + Conversion::toString(rvh.second);
    } else {
        if (Application::verboseLevel() > 
            Application::VERBOSE_LEVEL_DEFAULT) {
            Application::logStream() 
                << "Warning: Return value hi register datum not "
                << "found in interpassdata given to ddg builder. "
                << "May generate invalid code if "
                << "64-bit (struct) return values used."
                << std::endl;
        }
    }

    // constant alias AA check aa between global variables.
    addAliasAnalyzer(new ConstantAliasAnalyzer);

    LLVMTCECmdLineOptions* llvmOptions =
        dynamic_cast<LLVMTCECmdLineOptions*>(
                Application::cmdLineOptions());    
    if (llvmOptions != NULL && llvmOptions->disableLLVMAA() == false) {
        addAliasAnalyzer(new LLVMAliasAnalyzer);
    }
#ifdef USE_FALSE_AA
    /// defining USE_FALSE_AA results in faster but
    /// broken code. just for testing theoretical benefits.
    addAliasAnalyzer(new FalseAliasAnalyzer);
#endif
}

/**
 * Destructor of DataDependenceGraphBuilder
 */
DataDependenceGraphBuilder::~DataDependenceGraphBuilder() {
    SequenceTools::deleteAllItems(aliasAnalyzers_);
}

/**
 * Adds a memory alias analyzer to the DDG builder.
 *
 * @param analyzer object which will analyze memory accesses.
 */
void
DataDependenceGraphBuilder::addAliasAnalyzer(MemoryAliasAnalyzer* analyzer) {
    aliasAnalyzers_.push_back(analyzer);
}

/**
 * Tries to find annotations which tell the static registers
 * from a program.
 *
 * Used only with the old gcc frontend.
 *
 * @param cs codesnippet where to search the annotations.
 */
void
DataDependenceGraphBuilder::findStaticRegisters(
    TTAProgram::CodeSnippet& cs,
    std::map<int,TCEString>& registers) {
    for (int i = 0; i < cs.instructionCount(); i++) {
        Instruction& ins = cs.instructionAtIndex(i);
        findStaticRegisters(ins, registers);
    }
}

/**
 * Tries to find annotations which tell the static registers
 * from a program.
 *
 * Used only with the old gcc frontend.
 *
 * @param cfg cfg where to search the annotations.
 */
void
DataDependenceGraphBuilder::findStaticRegisters(
    ControlFlowGraph& cfg,
    std::map<int,TCEString>& registers) {
    for (int i = 0; i < cfg.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg.node(i);
        if (bbn.isNormalBB()) {
            findStaticRegisters(bbn.basicBlock(), registers);
        }
    }
}

/**
 * Tries to find annotations which tell the static registers
 * from a program.
 *
 * Used only with the old gcc frontend.
 *
 * @param ins instruction where to search the annotations.
 */
void
DataDependenceGraphBuilder::findStaticRegisters(
    TTAProgram::Instruction& ins,
    std::map<int,TCEString>& registers) {

    try {
        for (int i = 0; i < ins.moveCount(); i++) {
            Move& move = ins.move(i);
            for (int j = 0; j < move.annotationCount(); j++) {
                ProgramAnnotation anno = move.annotation(j);
                switch (anno.id()) {
                    case ProgramAnnotation::ANN_REGISTER_RV_READ: {
                        registers[REG_RV] = DisassemblyRegister::registerName(
                            move.source());
                        break;
                    }
                    case ProgramAnnotation::ANN_REGISTER_RV_SAVE: {
                        registers[REG_RV] = DisassemblyRegister::registerName(
                            move.destination());
                        break;
                    }
                    case ProgramAnnotation::ANN_REGISTER_SP_READ: {
                        registers[REG_SP] = DisassemblyRegister::registerName(
                            move.source());
                        break;
                    }
                    case ProgramAnnotation::ANN_REGISTER_SP_SAVE: {
                        registers[REG_SP] = DisassemblyRegister::registerName(
                            move.destination());
                        break;
                    }
                    case ProgramAnnotation::ANN_REGISTER_IPARAM_READ: {
/* this fixes one unit test silent breakage but another will then happen - unit test
   tpef's seem to contain a bit broken code
                        Terminal& src = move.source();
                        if (!src.isGPR()) {
                            break;
                        }
*/
                        TCEString reg = 
                            DisassemblyRegister::registerName(move.source());
                        registers[
                            REG_IPARAM+Conversion::toInt(anno.stringValue())] =
                            reg;
                        allParamRegs_.insert(reg);
                        break;
                    }
                    case ProgramAnnotation::ANN_REGISTER_IPARAM_SAVE: {
                        TCEString reg =
                            DisassemblyRegister::registerName(move.destination());
                        registers[
                            REG_IPARAM+Conversion::toInt(anno.stringValue())] =
                            reg;
                        allParamRegs_.insert(reg);
                        break;
                    }
                    default:
                        //TODO: frame pointer, not yet implemented
                        break;
                }
            }
        }
    } catch (std::bad_cast& e) {
        throw IllegalProgram(__FILE__,__LINE__, __func__, "Illegal annotation");
    }
}

/**
 * Initializes the static register table from register from
 * UniversalMachine.
 *
 * Needed for analysis of data dependencies of parameter registers,
 * SP, RV etc.
 *
 * @param um UniversalMachine
 * @param registers map where to store those registers.
 */
void
DataDependenceGraphBuilder::findStaticRegisters(
    const UniversalMachine& um, std::map<int,TCEString>& registers) {
    RegisterFile& rf = um.integerRegisterFile();

    for (int i = 0; i < 6; i++) {
        TerminalRegister tr(*rf.port(0), i);
        TCEString reg = DisassemblyRegister::registerName(tr); 
        registers[i] = reg;
        if (i > REG_SP) {
            allParamRegs_.insert(reg);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// End of initializations
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Single-BB DDG construction
///////////////////////////////////////////////////////////////////////////////

/**
 * Creates new Data Dependence Graph for the given basic block.
 *
 * Client has to delete the graph when it is not anymore used.
 *
 * @param bb BasicBlockNode whose data dependence graph to build.
 * @param registerAntidependenceLevel which reg antidependencies to create
 * @param createMemAndFUDeps whether to create also memory and
 *        fu state(side effect) dependencies or only register deps.
 * @return new DataDependence Graph.
 *
 */
DataDependenceGraph*
DataDependenceGraphBuilder::build(
    BasicBlock& bb, 
    DataDependenceGraph::AntidependenceLevel registerAntidependenceLevel,
    const TTAMachine::Machine& mach,
    const TCEString& ddgName, 
    const UniversalMachine* um, 
    bool createMemAndFUDeps,
    llvm::AliasAnalysis* AA) {

    mach_ = &mach;
    if (AA) {
        for (unsigned int i = 0; i < aliasAnalyzers_.size(); i++) {
            LLVMAliasAnalyzer* llvmaa = 
                dynamic_cast<LLVMAliasAnalyzer*>(aliasAnalyzers_[i]);
            if (llvmaa != NULL) {
                llvmaa->setLLVMAA(AA);
            }
        }
    }
    if (bb.liveRangeData_ == NULL) {
        bb.liveRangeData_ = new LiveRangeData;
    }

    currentBB_ = new BasicBlockNode(bb);
    currentDDG_ = new DataDependenceGraph(
        allParamRegs_, ddgName, registerAntidependenceLevel, currentBB_,
        false,true);
    // GRR, start and end addresses are lost..

    currentData_ = new BBData(*currentBB_);

    if (um != NULL) {
        findStaticRegisters(*um, specialRegisters_);
    } else {
        findStaticRegisters(bb,specialRegisters_);
    }
    try {
        // first phase. registers , ops and PO's.
        constructIndividualBB(
            REGISTERS_AND_PROGRAM_OPERATIONS);
        if (createMemAndFUDeps) {
            //second phase. mem and fu state deps
            constructIndividualBB(
                MEMORY_AND_SIDE_EFFECTS);
        }
    } catch (Exception&) {
        delete currentDDG_; currentDDG_ = NULL;
        delete currentData_; currentData_ = NULL;
        delete currentBB_; currentBB_ = NULL;
        throw;
    }

    clearUnneededBookkeeping(bb, false);

    delete currentData_;
    currentData_ = NULL;
    currentDDG_->setMachine(mach);
    return currentDDG_;
}

/**
 * Constructs a Data Dependence Graph for a single basic block.
 *
 * Goes thru all moves in the basic block and analyzes their dependencies,
 * creates their ProgramOperations, MoveNodes and Edges,
 * and adds the nodes and edges to the graph.
 * Also used inside implementation of multi-BB-DDG-code.
 * BB being analyzed has to be already set in member variable currentBB_,
 * and the graph created and set into member variable currentBB_.
 *
 * @param bbd basic block to constructs.
 * @param phase whether to handle register& operation deps or
 *        memory and side-effect dependencies.
 */
void
DataDependenceGraphBuilder::constructIndividualBB(
    BBData& bbd, ConstructionPhase phase) {
    currentData_ = &bbd;
    currentBB_ = bbd.bblock_;
    constructIndividualBB(phase);
}

/**
 * Constructs a Data Dependence Graph for a single basic block.
 *
 * Goes thru all moves in the basic block and analyzes their dependencies,
 * creates their ProgramOperations, MoveNodes and Edges,
 * and adds the nodes and edges to the graph.
 * Also used inside implementation of multi-BB-DDG-code.
 * BB being analyzed has to be already set in member variable currentBB_,
 * and the graph created and set into member variable currentBB_.
 *
 * @param phase whether to handle register& operation deps or
 *        memory and side-effect dependencies.
 */
void
DataDependenceGraphBuilder::constructIndividualBB(
    ConstructionPhase phase) {

    for (int ia = 0; ia < currentBB_->basicBlock().instructionCount(); ia++) {
        Instruction& ins = currentBB_->basicBlock().instructionAtIndex(ia);
        for (int i = 0; i < ins.moveCount(); i++) {
            auto movePtr = ins.movePtr(i);
            auto& move = *movePtr;

            MoveNode* moveNode = NULL;

            // if phase is 0, create the movenode, and handle guard.
            if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
                /* In case using the LLVMTCEIRBuilder, the POs have been built already
                   and set to corresponding TerminalFUPorts. Use those MoveNodes and
                   ProgramOperations instead of creating new ones here. NOTE: the
                   ownership of the MoveNodes is transferred to the DDG.
                */
                if (move.destination().isFUPort() && 
                    dynamic_cast<TerminalFUPort&>(move.destination()).
                    hasProgramOperation()) {
                    ProgramOperationPtr po = 
                        dynamic_cast<TerminalFUPort&>(move.destination()).
                        programOperation();
                    if (po->hasMoveNodeForMove(move)) {
                        moveNode = &po->moveNode(move);
                    } else {
                        // the po might be corrupted and point to an old POM's Moves
                        moveNode = new MoveNode(movePtr);
                    }
                } else if (move.source().isFUPort() && 
                           dynamic_cast<TerminalFUPort&>(move.source()).
                           hasProgramOperation()) {
                    ProgramOperationPtr po = 
                        dynamic_cast<TerminalFUPort&>(move.source()).
                        programOperation();
                    if (po->hasMoveNodeForMove(move)) {
                        moveNode = &po->moveNode(move);
                    } else {
                        // the po might be corrupted and point to an old POM's Moves
                        moveNode = new MoveNode(movePtr);
                    }
                } else {
                    moveNode = new MoveNode(movePtr);
                }
                currentDDG_->addNode(*moveNode, *currentBB_);

                if (!move.isUnconditional()) {
                    processGuard(*moveNode);
                }

                processSource(*moveNode);

            } else {
                // on phase 2, just find the already created movenode.
                moveNode = &currentDDG_->nodeOfMove(move);
            }

            // destinaition need to be processed in both phases 0 and 1.
            processDestination(*moveNode, phase);
        }
    }

    // these are needed no more.
    currentBB_->basicBlock().liveRangeData_->potentialRegKills_.clear();

    // Checks if we have some unready program operations at the end
    // of a basic block.
    if (currentData_->destPending_ != NULL ||
        currentData_->readPending_ != NULL) {

        TCEString msg = TCEString("Basic block ")
            + Conversion::toString(currentBB_->originalStartAddress())
            + TCEString(" - ")
            + Conversion::toString(currentBB_->originalEndAddress())
            + TCEString(", size : ")
            + Conversion::toString(
                currentBB_->basicBlock().instructionCount())
            + TCEString(" handled but we have unready PO at: ")
            + currentDDG_->name()
            + TCEString(", probably an operation without result move?");

        if (currentData_->readPending_ != NULL) {
            msg += "\n\tmissing read: " +
                TCEString(currentData_->readPending_->operation().name());
        }

        if (currentData_->destPending_ != NULL) {
            msg += "\n\tmissing dest: " +
                TCEString(currentData_->destPending_->operation().name());
        }

        if (cfg_ != NULL) {
            cfg_->writeToDotFile("constructBBbroken_cfg.dot");
        }

        throw IllegalProgram(__FILE__,__LINE__,__func__, msg);
    }
}


/**
 * Analyzes dependencies related to guard usage.
 *
 * Finds the guard register used for the guard and the move
 * Which writes the guard register, and creates a guard egde
 * between them.
 *
 * @param moveNode MNData of move containing guarded move.
 */
void
DataDependenceGraphBuilder::processGuard(MoveNode& moveNode) {

    // new code
    const Guard& g = moveNode.move().guard().guard();
    const RegisterGuard* rg = dynamic_cast<const RegisterGuard*>(&g);
    if (rg != NULL) {
        TCEString regName = rg->registerFile()->name() + '.' +
            Conversion::toString(rg->registerIndex());
        processRegUse(MoveNodeUse(moveNode, true),regName);
    } else {
        throw IllegalProgram(
            __FILE__,__LINE__,__func__,
            "Analysis for port guards not supported! used in: "
            + moveNode.toString());
    }
}

/**
 * Analysis a source of a move and processes it's dependencies,
 * and if it's a result read then also participates in ProgramOperation
 * creation.
 *
 * @param moveNode Movenode being analyzed.
 */
void
DataDependenceGraphBuilder::processSource(MoveNode& moveNode) {
    Terminal& source = moveNode.move().source();

    // is this result move of an operation?
    if (source.isFUPort()) {
        if (!(dynamic_cast<const SpecialRegisterPort*>(&source.port()))) {
            processResultRead(moveNode);
        } else {
            // handle read from RA.
            processRegUse(MoveNodeUse(moveNode, false, true), RA_NAME);

            if (moveNode.move().isReturn()) {
                processReturn(moveNode);
            }
        }
    } else {
        if (source.isGPR()) {
            TerminalRegister& tr = dynamic_cast<TerminalRegister&>(source);
            TCEString regName = DisassemblyRegister::registerName(tr);
            processRegUse(MoveNodeUse(moveNode), regName);
        }
    }
}


bool DataDependenceGraphBuilder::isTriggering(const MoveNode& mn) {
    int destIndex = mn.move().destination().operationIndex();
    const Operation& op = mn.destinationOperation().operation();
    int triggerIndex = MachineInfo::triggerIndex(*mach_, op);
    switch (triggerIndex) {
    case -1: {
        TCEString msg = "Trigger index ambiguous for operation: ";
        msg << op.name() << " in the machine.";
        throw IllegalMachine(__FILE__,__LINE__,__func__, msg);
        break; 
    }
    case 0: {
        TCEString msg = "Operation: ";
        msg << op.name() << " Not found from the machine";
        throw CompileError(__FILE__,__LINE__,__func__, msg);
        break;
    }
    default:
        return triggerIndex == destIndex;
    }
}

/**
 * Analyzes destination of a move.
 * Updates bookkeeping and handles WaW and WaR dependencies of the move.
 *
 * Checks whether destination is operation or register and calls other
 * functions to do the actual dependence checks etc.
 *
 * @param moveNode MoveNode whose destination is being processed.
 * @param phase whether to handle register& operation deps or
 *        memory and side-effect dependencies.
 */
void
DataDependenceGraphBuilder::processDestination(
    MoveNode& moveNode, ConstructionPhase phase) {
    Terminal& dest = moveNode.move().destination();

    // is this a operand to an operation?
    if (dest.isFUPort()) {
        if (!(dynamic_cast<const SpecialRegisterPort*>(&dest.port()))) {
            TerminalFUPort& tfpd = dynamic_cast<TerminalFUPort&>(dest);
            Operation &dop = tfpd.hintOperation();

            if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
                if (tfpd.isOpcodeSetting()) {
                    processTriggerRegistersAndOperations(
                        moveNode, dop);
                } else {
                    processOperand(moveNode, dop);
                }
            } else { // memory and fu state deps
                if (isTriggering(moveNode)) {
                    processTriggerMemoryAndFUStates(moveNode, dop);
                }
            }
        } else {  // RA write
            if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
                processRegWrite(MoveNodeUse(moveNode,false,true), RA_NAME);
            }
        }
    } else {
        if (dest.isGPR()) {
            // we do not care about register reads in second phase
            if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
                TerminalRegister& tr = dynamic_cast<TerminalRegister&>(dest);
                TCEString regName = DisassemblyRegister::registerName(tr);
                processRegWrite(MoveNodeUse(moveNode), regName);
            }
        } else { // something else
            throw IllegalProgram(__FILE__,__LINE__,__func__,
                                 "Move has illegal destination" +
                                 moveNode.toString());
        }
    }
}

/**
 * Clears bookkeeping which is only needed during ddg construction.
 *
 * @param BB containing basic blocks which contain the bookkeeping.
 * @param interBBInformation needed whether information about inter-bb-
 *        dependencies need to be left intact.
 */
void
DataDependenceGraphBuilder::clearUnneededBookkeeping(
    BasicBlock& bb, bool interBBInformationNeeded) {

    if (!interBBInformationNeeded) {
        //used by regcopyadder.
        bb.liveRangeData_->regFirstDefines_.clear();
        bb.liveRangeData_->regLastUses_.clear();

        // used by both
        bb.liveRangeData_->regDefines_.clear();

        // these are neede for live range things
        bb.liveRangeData_->regDefReaches_.clear();
        bb.liveRangeData_->registersUsedAfter_.clear();
        bb.liveRangeData_->regFirstUses_.clear();
    }
    bb.liveRangeData_->regUseReaches_.clear();
    bb.liveRangeData_->regLastKills_.clear();

    bb.liveRangeData_->regDefAfter_.clear();
    bb.liveRangeData_->regUseAfter_.clear();

    bb.liveRangeData_->memDefines_.clear();
    bb.liveRangeData_->memLastUses_.clear();

    bb.liveRangeData_->memFirstUses_.clear();
    bb.liveRangeData_->memFirstDefines_.clear();

    bb.liveRangeData_->memDefReaches_.clear();
    bb.liveRangeData_->memUseReaches_.clear();

    bb.liveRangeData_->fuDepReaches_.clear();
    bb.liveRangeData_->fuDeps_.clear();
    bb.liveRangeData_->fuDepAfter_.clear();

    bb.liveRangeData_->registersUsedInOrAfter_.clear();
}

///////////////////////////////////////////////////////////////////////////////
// ProgramOperation and Operation edges.
///////////////////////////////////////////////////////////////////////////////

/**
 * Handles ProgramOperation creation for a triggering move.
 *
 * @param moveNode triggering movenode.
 * @param dop operation which is being triggered by the movenode.
 */
void
DataDependenceGraphBuilder::processTriggerPO(
    MoveNode& moveNode, Operation& dop) {
    if (currentData_->destPending_ != NULL) {
        ProgramOperationPtr po = currentData_->destPending_;
        
        if (&dop != &po->operation()) {
            std::cerr << "pending po: " << po->toString() << std::endl;
            std::cerr << "current dop: " << dop.name() << std::endl;
            currentDDG_->writeToDotFile("build_fail_po.dot");
        }
        assert(&dop == &po->operation());
        if (!po->isComplete()) {
            po->addInputNode(moveNode);
            moveNode.addDestinationOperationPtr(po);
        }
        if (po->isReady()) {
            currentData_->destPending_ = ProgramOperationPtr();
            if (dop.numberOfOutputs() > 0) {
                assert(currentData_->readPending_ == NULL);
                currentData_->readPending_ = po;
		currentData_->poReadsHandled_ = 0;
            } else {
                currentDDG_->addProgramOperation(po);
            }
        } else {
            throw IllegalProgram(
                __FILE__, __LINE__, __func__, 
                "Trigger before all operands.");
        }
        return;
    }
    // only one triggering input?
    if (dop.numberOfInputs() == 1) {
        TerminalFUPort& tfpd = 
            dynamic_cast<TerminalFUPort&>(moveNode.move().destination());
        ProgramOperationPtr po;
        if (tfpd.hasProgramOperation()) {
            po = tfpd.programOperation();
        } else {
            po = ProgramOperationPtr(new ProgramOperation(dop));
            moveNode.addDestinationOperationPtr(po);
            po->addInputNode(moveNode);
        }
        if (dop.numberOfOutputs()) {
            assert(currentData_->readPending_ == NULL);
            currentData_->readPending_ = po;
        } else {
            currentDDG_->addProgramOperation(po);
        }
    } else { // trigger came too early
        const TCEString moveDisasm =
            POMDisassembler::disassemble(moveNode.move());
        throw IllegalProgram(
            __FILE__,__LINE__, __func__,
            TCEString("Trigger without operand in ") + moveDisasm);
    }
}

/**
 * Analyze write to a trigger of an operation.
 *
 * Participates in ProgramOperation building. Calls 
 * createTriggerDependencies(moveNode, dop) to create the register and
 * operation dependence egdes of the operation. Checks if operation is 
 * call and if it is, processes the call-related register dependencies.
 *
 * @param moveNode mnData related to a move which triggers an operation
 * @param dop Operation being triggered
 */
void
DataDependenceGraphBuilder::processTriggerRegistersAndOperations(
    MoveNode& moveNode, Operation& dop) {

    processTriggerPO(moveNode, dop);
    
    if (moveNode.move().isCall()) {
        processCall(moveNode);
    }
}

/**
 * Analyze write to a trigger of an operation.
 *
 * Participates in ProgramOperation building. Calls 
 * createTriggerDependencies(moveNode, dop) to create the memory and
 * fu state dependence egdes of the operation. Checks if operation is 
 * call and if it is, processes the call-related memory dependencies.
 *
 * @param moveNode mnData related to a move which triggers an operation
 * @param dop Operation being triggered
 */
void
DataDependenceGraphBuilder::processTriggerMemoryAndFUStates(
    MoveNode& moveNode, Operation &dop) {

    createTriggerDependencies(moveNode, dop);
    
    // handle call mem deps
    if (moveNode.move().isCall()) {
        // no guard, is not ra, is pseudo.
        MoveNodeUse mnd2(moveNode, false, false, true);
        processMemWrite(mnd2);
    }
}

/**
 * Analyzes operand writes.
 *
 * Part of ProgramOperation creation.
 *
 * @param moveNode mnData related to a move which writes a parameter.
 * @param dop Operation whose parameter is being written.
 */
void
DataDependenceGraphBuilder::processOperand(
    MoveNode& moveNode, Operation &dop) {

    // first operands already analyzed for PO? 
    // then update existing.
    if (currentData_->destPending_ != NULL) {
        ProgramOperationPtr po = currentData_->destPending_;

        assert(&dop == &po->operation());

        if (!po->isComplete()) {
            po->addInputNode(moveNode);
            moveNode.addDestinationOperationPtr(po);
        } else {
            // The MoveNode and the PO has been created before entering DDG
            // building (in LLVMTCEBuilder.cc).
        }
        return;
    }

    // create a new ProgramOperation
    TerminalFUPort& tfpd = 
        dynamic_cast<TerminalFUPort&>(moveNode.move().destination());
    ProgramOperationPtr po;
    if (tfpd.hasProgramOperation()) {
        po = tfpd.programOperation();
    } else {
        po = ProgramOperationPtr(new ProgramOperation(dop));
        moveNode.addDestinationOperationPtr(po);
        po->addInputNode(moveNode);
    }
    currentData_->destPending_ = po;
}

/**
 * Analyzes a source of a result read. 
 *
 * Handles program operation creation and operation dependence creation.
 *
 * @param moveNode MoveNode of the move being analyzed.
 */
void
DataDependenceGraphBuilder::processResultRead(MoveNode& moveNode) {

    // Goes thru all programoperations lacking result read.
    // There should be only one if well-behaving
    // universalmachine code.
    if (currentData_->readPending_ != NULL) {
        ProgramOperationPtr po = currentData_->readPending_;
        currentData_->poReadsHandled_++;
        
        if (!po->isComplete()) {
            po->addOutputNode(moveNode);
            moveNode.setSourceOperationPtr(po);
        }

        // if this PO is ready, remove from list of incomplete ones
        if (currentData_->poReadsHandled_ >= 
            po->operation().numberOfOutputs()) {
            createOperationEdges(po);
            currentData_->readPending_ = ProgramOperationPtr();
            currentDDG_->addProgramOperation(po);
            currentData_->poReadsHandled_ = 0;
        }
        return;
    }
    throw IllegalProgram(
        __FILE__, __LINE__, __func__,
        (boost::format("Result move '%s' without operands") %
         moveNode.move().toString()).str());
}

/**
 * Creates operand edges between input and output moves of a
 * programoperation.
 *
 * @param po ProgramOperation whose egdes we are creating.
 */
void
DataDependenceGraphBuilder::createOperationEdges(
    ProgramOperationPtr po) {

    const Operation& op = po->operation();

    // loop over all input nodes
    for (int i = 0; i < po->inputMoveCount(); i++) {
        MoveNode& inputNode = po->inputMove(i);
        // loop over all output nodes.
        for (int j = 0; j < po->outputMoveCount(); j++) {
            MoveNode& outputNode = po->outputMove(j);

            // and create operation edges
            // from all input nodes to all
            // output nodes.
            DataDependenceEdge* dde =
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_OPERATION,
                    DataDependenceEdge::DEP_UNKNOWN,
                    op.name());

            currentDDG_->connectOrDeleteEdge(
                inputNode, outputNode, dde);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Register edge creation.
///////////////////////////////////////////////////////////////////////////////

/**
 * Checks whether there is a previous alive write with same guard than
 * given node. 
 * 
 * The origin of the guard value is tracked from DDG, not only plain guard
 * is done.
 *
 * @param mnd MoveNode containing the guard.
 * @param defines set of earlier writes which to check.
 */
bool 
DataDependenceGraphBuilder::hasEarlierWriteWithSameGuard(
    MoveNodeUse& mnd, std::set<MoveNodeUse>& defines) {

    // first just check if there is earlier write to this reg with same guard..
    for (std::set<MoveNodeUse>::iterator i = defines.begin();
         i != defines.end(); i++) {
        // if earlier write to this reg with same guard..
        if (!mnd.guard() &&
            !i->mn()->move().isUnconditional() &&
            currentDDG_->sameGuards(*(i->mn()), *(mnd.mn()))) {
            return true;
        }
    }
    return false;
}

/**
 * Handles a usage of a register value.
 *
 * The usage can be either register read or guard usage.
 * Creates the incoming edges and handles bookkeping related to the
 * register read.
 *
 * @param mnd Data about the register use
 * @param reg Register containing the value being used.
 */
void 
DataDependenceGraphBuilder::processRegUse(
    MoveNodeUse mnd, const TCEString& reg) {

    // We may have multiple definitions to a register alive 
    // (statically) at same time if some of the writes were guarded,
    // so we don't know which of them were actually executed,
    // so we have a set instead of single value.
    std::set<MoveNodeUse>& defines =
        currentBB_->basicBlock().liveRangeData_->regDefines_[reg];

    // find if we have a earlier write with same guard. In this case
    // no need to draw dependencies over it.
    bool guardedKillFound = hasEarlierWriteWithSameGuard(mnd, defines);

    // then create the edges. if no guarded kill found,
    // all non-exclusive. if guarded kill found, not to uncond move.
    for (std::set<MoveNodeUse>::iterator i = defines.begin();
         i != defines.end(); i++) {
        // If we do not have a guarded kill, draw edges from all defines.
        // If we have a guarded kill, only draw edges from 
        // unconditional moves, as the guarded kill overshadows the 
        // inconditional writes.
        if (!guardedKillFound || !i->mn()->move().isUnconditional()) {
            if (!currentDDG_->exclusingGuards(*(i->mn()), *(mnd.mn()))) {
                DataDependenceEdge* dde =
                    new DataDependenceEdge(
                        mnd.ra() ? DataDependenceEdge::EDGE_RA :
                        DataDependenceEdge::EDGE_REGISTER,
                        DataDependenceEdge::DEP_RAW, reg, mnd.guard(), false,
                        i->pseudo(), mnd.pseudo(), i->loop());

                currentDDG_->connectOrDeleteEdge(*i->mn(), *mnd.mn(), dde);
            }
        }
    }

    // writes in previous BB's killed or not?
    // if not(this bb has a kill), has to check deps from incoming BB's.
    if (currentBB_->basicBlock().liveRangeData_->regKills_.find(reg) ==
        currentBB_->basicBlock().liveRangeData_->regKills_.end()) {
        currentBB_->basicBlock().liveRangeData_->regFirstUses_[reg].insert(mnd);
        if (!guardedKillFound) {
            // process dependencies from previous BB's
            currentDDG_->updateRegUse(mnd, reg, currentBB_->basicBlock());
        }
    }
    currentBB_->basicBlock().liveRangeData_->regLastUses_[reg].insert(mnd);

    // Two writes to opposite guard may create a combined kill-pair.
    // But if this is a read between them, it has to be marked in order
    // to save bookkeeping about this move when the another write occurs.
    // So mark here that we have a read if we have one guarded write
    // in our bookkeeping as potential half of a kill pair.
    std::map<TCEString, std::pair<MoveNodeUse, bool> >::iterator iter =
        currentBB_->basicBlock().liveRangeData_->potentialRegKills_.find(reg);
    if (iter != currentBB_->basicBlock().liveRangeData_->potentialRegKills_.end()) {
        iter->second.second = true;
    }
}

/**
 * Creates register antidependencies from set of movenodeuses to one movenode.
 *
 * @param mnd Movenode which to creat those dependencies
 * @param predecessorNodes Nodes where to create dependencies from 
 * @param depType whether to create WAR or WAW antidependencies
 * @param guardedKillFound if there is a write with same guard to the reg.
 */
void 
DataDependenceGraphBuilder::createRegisterAntideps(
    const TCEString& reg, MoveNodeUse& mnd, 
    MoveNodeUseSet& predecessorNodes, 
    DataDependenceEdge::DependenceType depType,
    bool guardedKillFound) {

    // create dep to another in own bb
    for (std::set<MoveNodeUse>::iterator i = predecessorNodes.begin();
         i != predecessorNodes.end();) {
        
        if (depType == DataDependenceEdge::DEP_WAW) {
            // If we do not have a guarded kill, draw edges from all defines.
            // If we have a guarded kill, only draw edges from 
            // unconditional moves, as the guarded kill overshadows the 
            // inconditional writes.
            if (guardedKillFound && i->mn()->move().isUnconditional()) {
                i++;
                continue;
            }
        } else {
            assert(depType == DataDependenceEdge::DEP_WAR);
            // skip if war to itself, ie move foo.1 -> foo.1
            if (i->mn() == mnd.mn()) {
                i++;
                continue;
            }
        }
                    
        // Do not create antideps if have excluding guards
        // But always create antidep if this writes to a reg which is
        // used as guard for the previous move.
        if (!currentDDG_->exclusingGuards(*(i->mn()), *(mnd.mn())) ||
            i->guard()) {
            
            // create dependency edge
            DataDependenceEdge* dde =
                new DataDependenceEdge(
                    mnd.ra() ? DataDependenceEdge::EDGE_RA :
                    DataDependenceEdge::EDGE_REGISTER,
                    depType, reg, i->guard(), false, 
                    i->pseudo(), mnd.pseudo(), i->loop());
            
            // and connect
            currentDDG_->connectOrDeleteEdge(*i->mn(), *mnd.mn(), dde);
            
            // if have same guards, remove the old from bookkeeping
            // this completely overshadows it
            if (currentDDG_->sameGuards(*(i->mn()), *(mnd.mn()))) {
                predecessorNodes.erase(i++);
                continue;
            }
        }
        i++;
    }
}


/**
 * Analyzes a write to a register.
 *
 * Creates dependence edges and updates bookkeeping.
 *
 * @param mnd MoveNodeUse containing MoveNode that writes a register
 * @param reg register being written by the given movenode.
 */
void
DataDependenceGraphBuilder::processRegWrite(
    MoveNodeUse mnd, const TCEString& reg) {

    // We may have multiple definitions to a register alive 
    // (statically) at same time if some of the writes were guarded,
    // so we don't know which of them were actually executed,
    // so we have a set instead of single value.
    std::set<MoveNodeUse>& defines =
        currentBB_->basicBlock().liveRangeData_->regDefines_[reg];

    // Set of register reads which after last kill.
    std::set<MoveNodeUse>& lastUses =
        currentBB_->basicBlock().liveRangeData_->regLastUses_[reg];

    // find if we have a earlier write with same guard. In this case
    // no need to draw dependencies over it.
    bool guardedKillFound = hasEarlierWriteWithSameGuard(mnd, defines);

    // if no kills to this reg in this BB, this one kills it.
    if (currentBB_->basicBlock().liveRangeData_->regKills_.find(reg) ==
        currentBB_->basicBlock().liveRangeData_->regKills_.end()) {

        // is this alone a kill?
        if (mnd.mn()->move().isUnconditional()) {
            currentBB_->basicBlock().liveRangeData_->regKills_[reg].first = mnd;
            currentBB_->basicBlock().liveRangeData_->regKills_[reg].second = MoveNodeUse();
        } else {
            // two guarded moves with opposite guards together may be a kill.
            // Check if we have such previous guarded write with opposite
            // guard.
            std::map<TCEString, std::pair<MoveNodeUse, bool> >::iterator
                iter =
                currentBB_->basicBlock().liveRangeData_->potentialRegKills_.find(reg);
            if (iter != currentBB_->basicBlock().liveRangeData_->potentialRegKills_.end() &&
                currentDDG_->exclusingGuards(
                    *(iter->second.first.mn()), *(mnd.mn()))) {
                currentBB_->basicBlock().liveRangeData_->regKills_[reg].first =
                    iter->second.first;
                currentBB_->basicBlock().liveRangeData_->regKills_[reg].second = mnd;
            }
        }
        if (!guardedKillFound) {
            // may have incoming WaW's / WaRs to this
            // insert to bookkeeping for further analysis.
            currentBB_->basicBlock().liveRangeData_->regFirstDefines_[reg].insert(mnd);

            // do we need to create some inter-bb-antideps?
            if (currentDDG_->hasSingleBBLoopRegisterAntidependencies()) {
                // deps from other BB.LIVERANGEDATA_->
                currentDDG_->updateRegWrite(
                    mnd, reg, currentBB_->basicBlock());
            }
        }
    }

    // Create antideps to defines and uses in this same BB.LIVERANGEDATA_->
    if (currentDDG_->hasIntraBBRegisterAntidependencies()) {
        createRegisterAntideps(
            reg, mnd, defines, DataDependenceEdge::DEP_WAW, guardedKillFound);

        createRegisterAntideps(
            reg, mnd, lastUses, DataDependenceEdge::DEP_WAR, 
            guardedKillFound);
    }

    // if unconditional, this kills previous deps.
    if (mnd.mn()->move().isUnconditional()) {
        defines.clear();

        currentBB_->basicBlock().liveRangeData_->regLastKills_[reg].first = mnd;
        currentBB_->basicBlock().liveRangeData_->regLastKills_[reg].second = MoveNodeUse();

        // clear reads to given reg.
        lastUses.clear();
        currentBB_->basicBlock().liveRangeData_->potentialRegKills_.erase(reg);
    } else {
        // two guarded moves with opposite guards together may be a kill.
        // Check if we have such previous guarded write with opposite
        // guard.
        std::map<TCEString, std::pair<MoveNodeUse, bool> >::iterator iter =
            currentBB_->basicBlock().liveRangeData_->potentialRegKills_.find(reg);
        if (iter != currentBB_->basicBlock().liveRangeData_->potentialRegKills_.end() &&
            currentDDG_->exclusingGuards(
                *(iter->second.first.mn()), *(mnd.mn()))) {

            // found earlier write which is exclusive with this one.
            // mark that these two together are a kill.
            currentBB_->basicBlock().liveRangeData_->regLastKills_[reg].first =
                iter->second.first;
            currentBB_->basicBlock().liveRangeData_->regLastKills_[reg].second = mnd;

            // If we have no usage of the register between these two
            // writes forming the kill pair, we can clear our bookkeeping.

            // only leave the other part of the kill to defines.
            defines.clear();
            defines.insert(iter->second.first);

            if (!iter->second.second) {
                // clear reads to given reg.
                lastUses.clear();
            }
        }
        currentBB_->basicBlock().liveRangeData_->potentialRegKills_[reg] =
            std::pair<MoveNodeUse, bool>(mnd, false);
    }
    defines.insert(mnd);
}

/**
 * Processes a return from a function.
 *
 * Creates pseudo-read-deps to SP and RV registers.
 *
 * @param moveNode moveNode containg the return move.
 */
void 
DataDependenceGraphBuilder::processReturn(MoveNode& moveNode) {
    TCEString sp = specialRegisters_[REG_SP];

    // return is considered as read of sp; 
    // sp must be correct at the end of the procedure.
    if (sp != "") {
        processRegUse(MoveNodeUse(moveNode,false,false,true),sp);
    }

    // return is considered as read of RV.
    TCEString rv = specialRegisters_[REG_RV];
    if (rv != "") {
        processRegUse(MoveNodeUse(moveNode,false,false,true),rv);
    }

    // return is also considered as read of RV high(for 64-bit RV's)
    TCEString rvh = specialRegisters_[REG_RV_HIGH];
    if (rvh != "") {
        processRegUse(MoveNodeUse(moveNode,false,false,true),rvh);
    }

    TCEString fp = specialRegisters_[REG_FP];
    if (fp != "") {
        processRegUse(MoveNodeUse(moveNode,false,false,true),fp);
    }
}

/**
 * Processes a call of a function.
 *
 * Pseudo-reads from parameter registers and SP, writes to RV and RA.
 *
 * @param mn MoveNode containg the function call move.
 */
void 
DataDependenceGraphBuilder::processCall(MoveNode& mn) {

    // calls mess up RA. But immediately, not after delay slots?
    processRegWrite(
        MoveNodeUse(mn, false, true, false), RA_NAME);

    // MoveNodeUse for sp and rv(not guard, not ra, is pseudo)
    MoveNodeUse mnd2(mn, false,false, true);

    // call is considered read of sp
    TCEString sp = specialRegisters_[REG_SP];
    if (sp != "") {
        processRegUse(mnd2,sp);
    }

    // call is considered as write of RV
    TCEString rv = specialRegisters_[REG_RV];
    if (rv != "") {
        if (rvIsParamReg_) {
            processRegUse(mnd2,rv);
        }
        processRegWrite(mnd2,rv);
    }

    // call is considered as write of RV high (64-bit return values)
    TCEString rvh = specialRegisters_[REG_RV_HIGH];
    if (rvh != "") {
        processRegWrite(mnd2, rvh);
    }

    // params
    for (int i = 0; i < 4;i++) {
        TCEString paramReg = specialRegisters_[REG_IPARAM+i];
        if (paramReg != "") {
            processRegUse(mnd2, paramReg);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Side-Effects of triggers.
///////////////////////////////////////////////////////////////////////////////

/**
 * Analyzes operation of a trigger write.
 *
 * If memory read, calls processMemRead to manage memory read dependencies.
 * Manages FU State dependencies between operations.
 *
 * @param moveNode mnData related to a move which triggers an operation
 * @param dop Operation being triggered
 *
 */
void 
DataDependenceGraphBuilder::createTriggerDependencies(
    MoveNode& moveNode, Operation& dop) {

    if (dop.readsMemory()) {
        processMemUse(MoveNodeUse(moveNode));
    } //TODO: avoid drawing antidep to itself

    if (dop.writesMemory()) {
        processMemWrite(MoveNodeUse(moveNode));
    } 

    createSideEffectEdges(currentBB_->basicBlock().liveRangeData_->fuDeps_, moveNode, dop);
    createSideEffectEdges(
        currentBB_->basicBlock().liveRangeData_->fuDepReaches_, moveNode, dop);

    OperationPool pool;
    if (dop.hasSideEffects() || pool.sharesState(dop)) {

        // remove old same op from bookkeeping.
        // this should prevent exponential explosion of edge count.
        if (dop.hasSideEffects() && moveNode.move().isUnconditional()) {
            for (MoveNodeUseSet::iterator iter =
                     currentBB_->basicBlock().liveRangeData_->fuDeps_.begin();
                 iter != currentBB_->basicBlock().liveRangeData_->fuDeps_.end(); iter++) {

                const Operation& o = 
                    iter->mn()->destinationOperation().operation();
                if (&o == &dop) {
                    currentBB_->basicBlock().liveRangeData_->fuDeps_.erase(iter);
                    break;
                }
            }
        }
        // add the new one to bookkeeping
        currentBB_->basicBlock().liveRangeData_->fuDeps_.insert(MoveNodeUse(moveNode));
    }
}

/*
 * Creates operation side effect.
 *
 * Checks the given MoveNode against list of possible side effect
 * dependence sources, and creates side effect edges if there is
 * a side effect/fu state dependence.
 *
 * @param prevMoves moves to check side effects against.
 * @param mn moveNode that is the destination of the dependencies.
 * @param dop Operation that mn triggers.
 */
void 
DataDependenceGraphBuilder::createSideEffectEdges(
    MoveNodeUseSet& prevMoves, const MoveNode& mn, Operation& dop) {

    OperationPool pool;
    if (pool.sharesState(dop) || dop.hasSideEffects()) {
        for (MoveNodeUseSet::iterator i = prevMoves.begin();
             i != prevMoves.end(); i++) {
            const Operation& o = i->mn()->destinationOperation().operation();

            // mem writes are handled by memory deps so exclude here
            if ((&dop == &o && o.hasSideEffects()) ||
                dop.dependsOn(o) || o.dependsOn(dop)) {

                if (!currentDDG_->exclusingGuards(*(i->mn()), mn)) {
                    DataDependenceEdge* dde =
                        new DataDependenceEdge(
                            DataDependenceEdge::EDGE_FUSTATE,
                            DataDependenceEdge::DEP_UNKNOWN, false,false,false,
                            false, static_cast<int>(i->loop()));
                    currentDDG_->connectOrDeleteEdge(
                        *(i->mn()), mn, dde);
                }
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Memory edge creation.
///////////////////////////////////////////////////////////////////////////////

/**
 * Checks if there is an earlier write to same address or with same guard.
 *
 * @param mnd the current node dictating guard and mem address to check.
 * @param defines set of earlier writes to check for the write.
 */
bool
DataDependenceGraphBuilder::hasEarlierMemWriteToSameAddressWithSameGuard(
    MoveNodeUse& mnd, std::set<MoveNodeUse>& defines) {
    // first just check if there is earlier write to this mem address
    // with same guard.
    for (std::set<MoveNodeUse>::iterator i = defines.begin();
         i != defines.end(); i++) {
        // if earlier write to this reg with same guard..
        if (currentDDG_->sameGuards(*(i->mn()), *(mnd.mn()))) {
            ProgramOperation& curPop = mnd.mn()->destinationOperation();
            ProgramOperation& prevPop = (i->mn())->destinationOperation();
//            MoveNode* currentAddress = addressMove(*mnd.mn());
//            MoveNode* prevAddress = addressMove(*(i->mn()));
            if (!isAddressTraceable(prevPop) ||
                analyzeMemoryAlias(prevPop, curPop) ==
                MemoryAliasAnalyzer::ALIAS_TRUE) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Creates memory dependencies from set of nodes to given nodes.
 *
 * Does not create if gaurds of aliasing dictate edge not needed.
 * If both guard and aliasing indicate fully transitive case for some 
 * prev nodes, then remove these previous nodes from the bookkeeping.
 */
void
DataDependenceGraphBuilder::checkAndCreateMemAntideps(
    MoveNodeUse& mnd, std::set<MoveNodeUse>& prevNodes, 
    DataDependenceEdge::DependenceType depType,
    bool traceable) {
    // create WaW to another in own bb
    for (MoveNodeUseSet::iterator iter =
             prevNodes.begin(); iter != prevNodes.end();) {
        if ((checkAndCreateMemDep(*iter, mnd, depType) || !traceable) &&
            (mnd.mn()->move().isUnconditional() ||
             currentDDG_->sameGuards(*(iter->mn()), *(mnd.mn())))) {
            // remove current element and update iterator to next.
            prevNodes.erase(iter++);
        } else { // just take next from the set
            iter++;
        }
    }
}

/**
 * Updates memory operation bookkeeping and creates WaR and WaW
 * memory dependencies.
 *
 * @param moveNode MoveNodeUse related to Move whose memory write to are
 * processing.
 */
void
DataDependenceGraphBuilder::processMemWrite(MoveNodeUse mnd) {

    TCEString category = memoryCategory(mnd);

    std::set<MoveNodeUse>& defines =
        currentBB_->basicBlock().liveRangeData_->memDefines_[category];

    std::set<MoveNodeUse>& lastUses =
        currentBB_->basicBlock().liveRangeData_->memLastUses_[category];

    // check if no earlier barriers/kills to this one in this bb?
    if (currentBB_->basicBlock().liveRangeData_->memKills_[category].mn() == NULL) {

        // is this a kill?
        if (mnd.mn()->move().isUnconditional() &&
            !isAddressTraceable(mnd.mn()->destinationOperation())) {
            currentBB_->basicBlock().liveRangeData_->memKills_[category] = mnd;
        }

        // check if there is "guarded kill" to this mem address
        bool guardedKillFound = 
            hasEarlierMemWriteToSameAddressWithSameGuard(mnd, defines);
        
        if (!guardedKillFound) {
            // may have incoming WaW's / WaRs to this
            currentBB_->basicBlock().liveRangeData_->memFirstDefines_[category].insert(mnd);
            updateMemWrite(mnd, category);
        }
    }

    bool traceable = isAddressTraceable(mnd.mn()->destinationOperation());

    checkAndCreateMemAntideps(
        mnd, defines, DataDependenceEdge::DEP_WAW, traceable);

    checkAndCreateMemAntideps(
        mnd, lastUses, DataDependenceEdge::DEP_WAR, traceable);

    // does this kill previous deps?
    if (mnd.mn()->move().isUnconditional() && !traceable) {
        currentBB_->basicBlock().liveRangeData_->memLastKill_[category] = mnd;
        defines.clear();
        lastUses.clear();
    }

    defines.insert(mnd);
}

/**
 * Processes a memory read.
 *
 * Creates dependence edges and updates bookkeeping.
 *
 * @param mnd MoveNodeUse of MoveNode being processed.
 */
void 
DataDependenceGraphBuilder::processMemUse(MoveNodeUse mnd) {

    TCEString category = memoryCategory(mnd);

    // can be multiple if some write predicated
    std::set<MoveNodeUse>& defines =
        currentBB_->basicBlock().liveRangeData_->memDefines_[category];

    // no kills/barriers to this one in this basic block.
    if (currentBB_->basicBlock().liveRangeData_->memKills_[category].mn() == NULL) {

        // check if there is "guarded kill" to this mem address
        bool guardedKillFound = 
            hasEarlierMemWriteToSameAddressWithSameGuard(mnd, defines);

        if (!guardedKillFound) {
            currentBB_->basicBlock().liveRangeData_->memFirstUses_[category].insert(mnd);
            // so create deps from previous BB's
            updateMemUse(mnd, category);
        }
    }

    // create deps from writes in this BB.LIVERANGEDATA_->
    for (MoveNodeUseSet::iterator iter =
             defines.begin(); iter != defines.end(); iter++) {
        checkAndCreateMemDep(*iter, mnd, DataDependenceEdge::DEP_RAW);
    }
    // update bookkeeping.
    currentBB_->basicBlock().liveRangeData_->memLastUses_[category].insert(mnd);
}

/**
 * Compares a memory op against one previous memory ops and
 * creates dependence if may alias.
 *
 * @param prev Previous Memory write movenode
 * @param mn Current memory write movenode
 * @param depType dependence type which to create.
 * @return true if true alias.
 */
bool
DataDependenceGraphBuilder::checkAndCreateMemDep(
    MoveNodeUse prev, MoveNodeUse mnd,
    DataDependenceEdge::DependenceType depType) {

    // cannot be dep if opposite guards.
    if (currentDDG_->exclusingGuards(*(prev.mn()), *(mnd.mn()))) {
        return false;
    }

    MemoryAliasAnalyzer::AliasingResult aliasResult =
        MemoryAliasAnalyzer::ALIAS_UNKNOWN;

    // only for true stores and loads; we cannot analyze aliasing
    // of pseudo dependencies caused by call/return.
    if (!prev.pseudo() && !mnd.pseudo()) {
        ProgramOperation& currPop = mnd.mn()->destinationOperation();
        ProgramOperation& prevPop = prev.mn()->destinationOperation();

        const llvm::MachineInstr* instr1 = currPop.machineInstr();
        const llvm::MachineInstr* instr2 = prevPop.machineInstr();

        // The LLVM MachineInstructions are not connected to
        // all memory operands, at least not to those in inline
        // assembly blocks (from custom operation calls).
        if (instr1 != NULL && instr2 != NULL) {            
            llvm::MachineInstr::mmo_iterator begin1 =
                instr1->memoperands_begin();
            // Machine instruction could in theory have several memory operands.
            // In practice it is usually just one.
            while (begin1 != instr1->memoperands_end()) {
                llvm::MachineInstr::mmo_iterator begin2 =
                    instr2->memoperands_begin();

                while (begin2 != instr2->memoperands_end()) {
                    // Force program ordering between volatile mem accesses.
                    if ((*begin1)->isVolatile() && (*begin2)->isVolatile()) {
#if 0
                        Application::logStream() << "MemDep >> volatile \n";
                        PRINT_VAR(currPop.toString());
                        PRINT_VAR(prevPop.toString());
                        (*begin1)->getValue()->dump();
                        (*begin2)->getValue()->dump();
#endif
                        aliasResult = MemoryAliasAnalyzer::ALIAS_PARTIAL;
                    }
                    begin2++;
                }
                begin1++;
            }
        }
        if (aliasResult == MemoryAliasAnalyzer::ALIAS_UNKNOWN)
            aliasResult = analyzeMemoryAlias(prevPop, currPop);
    }

    if (aliasResult != MemoryAliasAnalyzer::ALIAS_FALSE) {
        // if not alias false , have to create mem edge.
        bool trueAlias = (aliasResult == MemoryAliasAnalyzer::ALIAS_TRUE);
        ProgramOperation& prevPo = prev.mn()->destinationOperation();
        for (int i = 0; i < prevPo.inputMoveCount(); i++) {
            if (prev.loop() || &prevPo.inputMove(i) != mnd.mn()) {
                DataDependenceEdge* dde2 =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_MEMORY, depType, false,
                        trueAlias, prev.pseudo(), mnd.pseudo(),
                        static_cast<int>(prev.loop()));
                currentDDG_->connectOrDeleteEdge(
                    prevPo.inputMove(i), *mnd.mn(), dde2);
            }
        }
        return trueAlias;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Alias analysis - related things.
///////////////////////////////////////////////////////////////////////////////

/**
 * Gets the address-writing move of a move which is a trigger or operand
 * to a memory operation.
 *
 * If none found, return null
 *
 * @param mn moveNode whose address write move is being searched.
 * @return MoveNode which writes address to a mem operation or NULL.
 
MoveNode*
DataDependenceGraphBuilder::addressMove(const MoveNode& mn) {
    if (mn.isDestinationOperation()) {
        return addressOperandMove(mn.destinationOperation());
    }
    return NULL;
}
*/

/**
 * Delegates the call to all registered memory address alias analyzers.
 *
 * Returns the first non-unknown result.
 * If no alias analyzer can analyze these, returns ALIAS_UNKNOWN
 *
 * @return Whether the memory accesses in the given moves alias.
 */
MemoryAliasAnalyzer::AliasingResult
DataDependenceGraphBuilder::analyzeMemoryAlias(
    const ProgramOperation& pop1, const ProgramOperation& pop2) {

    for (unsigned int i = 0; i < aliasAnalyzers_.size(); i++) {
        MemoryAliasAnalyzer::AliasingResult res =
            aliasAnalyzers_[i]->analyze(
                *currentDDG_, pop1, pop2);
        if (res != MemoryAliasAnalyzer::ALIAS_UNKNOWN) {
            return res;
        }
    }
    return MemoryAliasAnalyzer::ALIAS_UNKNOWN;
}

/**
 * Can some analyzer say something about this address?
 *
 * @param mn Movenode containing memory address write.
 * @return true if some alias analyzer knows something about the address,
 *              ie can return something else than ALIAS_UNKNOWN.
 */
bool
DataDependenceGraphBuilder::isAddressTraceable(const ProgramOperation& pop) {

    for (unsigned int i = 0; i < aliasAnalyzers_.size(); i++) {
        if (aliasAnalyzers_.at(i)->isAddressTraceable(*currentDDG_, pop)) {
            return true;
        }
    }
    return false;
}

/**
 * Checks into which category this memory address belongs.
 *
 * Memory accesses in different categories cannot alias, and there is
 * separate bookkeeping for every category. Current implementation separates
 * spills, different alias spaces, restrict keywords and opencl work items.
 *
 * @param mnd MoveNodeUse which transfers the address of the memory operation.
 * @return string which is then used as key for map.
 *         unique for different categories, empty for the default category.
 *
 * @TODO: create some memorycategorizer interface for this analysis?
 */

TCEString
DataDependenceGraphBuilder::memoryCategory(const MoveNodeUse& mnd) {

    TCEString category;

//    MoveNode* addressNode = addressMove(*mnd.mn());
//    if (addressNode != NULL && addressNode->isMove()) {
    const TTAProgram::Move& move = mnd.mn()->move();//addressNode->move();
    
    // spill annotations are in all operands.
    for (int j = 0; j < move.annotationCount(); j++) {
        TTAProgram::ProgramAnnotation anno = move.annotation(j);
        if (anno.id() ==
            TTAProgram::ProgramAnnotation::ANN_STACKUSE_SPILL) {
            return "_SPILL";
        }
        if (anno.id() ==
            TTAProgram::ProgramAnnotation::ANN_STACKUSE_RA_SAVE) {
            return "_RA";
        }
        if (anno.id() ==
            TTAProgram::ProgramAnnotation::ANN_STACKUSE_FP_SAVE) {
            return "_FP";
        }
    }
    if (!mnd.mn()->isDestinationOperation()) {
        PRINT_VAR(mnd.mn()->toString());
        abortWithError("Not destination operation!");
    }
    ProgramOperation& po = mnd.mn()->destinationOperation();

    // address space
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& mn = po.inputMove(i);
        Move& m = mn.move();
        if (m.hasAnnotations(
                ProgramAnnotation::ANN_POINTER_ADDR_SPACE)) {
            if (m.annotation(
                    0, ProgramAnnotation::ANN_POINTER_ADDR_SPACE).stringValue()
                != "0") {
                category +=
                    "_AS:" +
                    m.annotation(
                        0, ProgramAnnotation::ANN_POINTER_ADDR_SPACE)
                    .stringValue();
                break;
            }
        }
    }

    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& mn = po.inputMove(i);
        Move& m = mn.move();
        TCEString pointerName = "";
        // restrict keyword.
        if (m.hasAnnotations(
                ProgramAnnotation::ANN_POINTER_NAME) &&
            m.hasAnnotations(
                ProgramAnnotation::ANN_POINTER_NOALIAS)) {
            pointerName =
                m.annotation(
                    0, ProgramAnnotation::ANN_POINTER_NAME).stringValue();
            category += "_RESTRICT:" + pointerName;            
            break;
        }
    }

    /* OpenCL work item variable access.
       
    OpenCL kernels enforce memory consistency for local and global
    memory only at explicit barrier() calls within a work group.
    Thus, all memory accesses between work items can be considered
    independent in alias analysis in the regions between barrier
    calls. 
    */ 
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& mn = po.inputMove(i);
        Move& m = mn.move();
        if (m.hasAnnotations(ProgramAnnotation::ANN_OPENCL_WORK_ITEM_ID)) {
            category += 
                "_OPENCL_WI:" + 
                Conversion::toHexString(
                    m.annotation(
                        0, ProgramAnnotation::ANN_OPENCL_WORK_ITEM_ID).
                    intValue(), 8);
            break;
        }
    }
    return category;
}

///////////////////////////////////////////////////////////////////////////////
// Multi-BB DDG construction
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// High-level Multi-BB DDG construction
///////////////////////////////////////////////////////////////////////////////

/**
 * Builds a DDG from a CFG.
 *
 * @param cfg Control flow graph where the ddg is built from.
 * @param antidependenceLevel level of register antidependencies to create.
 * @param um universalmachine used for the code if registers unallocated.
 *        if null, assumed that registers allready allocated.
 * @param createMemAndFUDeps whether to create also memory and
 *        fu state(side effect) dependencies or only register deps.
 * @param createDeathInformation whether to search the last usage
 *        of all liveranges. This is needed for things like register renamer
 *        and threading.
  * @return pointer to the ddg which is created.
 */
DataDependenceGraph*
DataDependenceGraphBuilder::build(
    ControlFlowGraph& cfg, 
    DataDependenceGraph::AntidependenceLevel antidependenceLevel,
    const TTAMachine::Machine& mach,
    const UniversalMachine* um, 
    bool createMemAndFUDeps, bool createDeathInformation,
    llvm::AliasAnalysis* AA) {

    mach_ = &mach;
    if (AA) {
        for (unsigned int i = 0; i < aliasAnalyzers_.size(); i++) {
            LLVMAliasAnalyzer* llvmaa = 
            dynamic_cast<LLVMAliasAnalyzer*>(aliasAnalyzers_[i]);
            if (llvmaa != NULL) {
                llvmaa->setLLVMAA(AA);
            }
        }
    }

    cfg_ = &cfg;

    // @TODO: when CFG subgraphs are in use, 2nd param not always true
    DataDependenceGraph* ddg = new DataDependenceGraph(
        allParamRegs_, cfg.name(), antidependenceLevel, NULL, true, 
        false);
    try {
        // this is just for old frontend code.
        if (um != NULL) {
            findStaticRegisters(*um, specialRegisters_);
        } else {
            findStaticRegisters(cfg, specialRegisters_);
        }

        currentDDG_ = ddg;

        // do the first phase (register dependencies and operation edges)
        createRegisterDeps();

        // then do the second phase - mem and fu deps.
        if (createMemAndFUDeps) {
            createMemAndFUstateDeps();
        }

        // search when registers are used for last time.
        // this live range information is used by register renamer and
        // threading.
        if (createDeathInformation) {
            searchRegisterDeaths();
        }

        // clear bookkeeping which is not needed anymore.
        clearUnneededBookkeeping();
    } catch (const Exception& e) {
        Application::logStream()
            << e.fileName() << ": " << e.lineNum() << ": " << e.errorMessageStack()
            << std::endl;
        delete ddg;
        throw;
    } catch (...) {
        delete ddg;
        throw;
    }
    ddg->setMachine(mach);
    return ddg;
}

/**
 * Initializes states of all BB's to unreached
 *
 */
void
DataDependenceGraphBuilder::initializeBBStates() {

    // initialize state lists
    for (int bbi = 0; bbi < cfg_->nodeCount(); bbi++) {
        BasicBlockNode& bbn = cfg_->node(bbi);
        BasicBlock& bb = bbn.basicBlock();
        if (bb.liveRangeData_ == NULL) {
            bb.liveRangeData_ = new LiveRangeData;
        }
        BBData* bbd = new BBData(bbn);
        bbData_[&bbn] = bbd;
        // in the beginning all are unreached
        if (bbn.isNormalBB()) {
            blocksByState_[BB_UNREACHED].push_back(bbd);
        }
    }
}

/**
 * Changes state of a basic block in processing.
 * Move BBData into a diffefent list and changes the state data in BBData.
 *
 * @param bbd BBData of basic block whose state is being changed
 * @param newState the new state of the basic block.
*/
void
DataDependenceGraphBuilder::changeState(
    BBData& bbd, BBState newState, bool priorize) {

    BBState oldState = bbd.state_;
    if (newState != oldState) {
        ContainerTools::removeValueIfExists(blocksByState_[oldState], &bbd);
        bbd.state_ = newState;
        if (priorize) {
            blocksByState_[newState].push_front(&bbd);
        } else {
            blocksByState_[newState].push_back(&bbd);
        }
    }
}

/**
 * Queues first basic block to be processed.
 *
 * @return the first basic block node
 */
BasicBlockNode*
DataDependenceGraphBuilder::queueFirstBB() {
    // get first BB where to start
    BasicBlockNodeSet firstBBs = cfg_->successors(cfg_->entryNode());
    assert(firstBBs.size() == 1);
    BasicBlockNode* firstBB = *firstBBs.begin();
    changeState(*(bbData_[firstBB]), BB_QUEUED);
    return firstBB;
}

/**
 * Clears bookkeeping which is only needed during ddg construction.
 */
void
DataDependenceGraphBuilder::clearUnneededBookkeeping() {

    for (int i = 0; i < cfg_->nodeCount();i++) {
        BasicBlockNode& bbn = cfg_->node(i);
        if (bbn.isNormalBB()) {
            BasicBlock& bb = bbn.basicBlock();
            clearUnneededBookkeeping(bb, true);
        }
    }
}

/**
 * Does the first phase of ddg construction. handles register deps.
 *
 * @param cfg control flow graph containing the code.
 */
void
DataDependenceGraphBuilder::createRegisterDeps() {

    // initializes states of all BB's to unreached.
    initializeBBStates();

    // queues first bb for processing
    BasicBlockNode* firstBB = queueFirstBB();

    // currentBB need to be set for entry node processing
    currentBB_ = firstBB;

    // set entry deps. ( procedure parameter edges )
    MoveNode* entryNode = new MoveNode();
    currentDDG_->addNode(*entryNode, cfg_->entryNode());

    processEntryNode(*entryNode);

    // iterate over BB's. Loop as long as there are queued BB's.

    iterateBBs(REGISTERS_AND_PROGRAM_OPERATIONS);

    // all should be constructed, but if there are unreachable BB's
    // we handle those also
    while (!blocksByState_[BB_UNREACHED].empty()) {
        if (Application::verboseLevel() > Application::VERBOSE_LEVEL_DEFAULT) {
            Application::logStream() << "Warning: Unreachable Basic Block!"
                                     << std::endl;
            Application::logStream() << "In procedure: " << cfg_->name() <<
                std::endl;
//            cfg.writeToDotFile("unreachable_bb.dot");
        }
        changeState(**blocksByState_[BB_UNREACHED].begin(), BB_QUEUED);
        iterateBBs(REGISTERS_AND_PROGRAM_OPERATIONS);
    }
    // free bb data
    AssocTools::deleteAllValues(bbData_);
}

/**
 * Does the second phase of ddg construction. 
 *
 * handles mem deps and fu state deps.
 */
void
DataDependenceGraphBuilder::createMemAndFUstateDeps() {

    // initializes states of all BB's to unreached.
    initializeBBStates();

    // queues first bb for processing
    queueFirstBB();

    // then iterates over all basic blocks.
    iterateBBs(MEMORY_AND_SIDE_EFFECTS);

    // all should be constructed, but if there are unreachable BB's
    // we might want to handle those also
    while (!blocksByState_[BB_UNREACHED].empty()) {
        changeState(**blocksByState_[BB_UNREACHED].begin(), BB_QUEUED);
        iterateBBs(MEMORY_AND_SIDE_EFFECTS);
    }
    // free bb data
    AssocTools::deleteAllValues(bbData_);
}


/**
 * Iterates over basic blocks as long as there is some BB to process.
 *
 * Handles a BB, updates the live value lists of its successors.
 * If incoming live values of a BB change, it's scheduled for
 * reprocessing.
 *
 * @param phase whether to handle register& operation deps or
 *        memory and side-effect dependencies.
 */
void 
DataDependenceGraphBuilder::iterateBBs(
    ConstructionPhase phase) {

    while (!blocksByState_[BB_QUEUED].empty()) {
        std::list<BBData*>::iterator bbIter =
            blocksByState_[BB_QUEUED].begin();
        BBData& bbd = **bbIter;

        // construct or update BB
        if (bbd.constructed_) {
            updateBB(bbd, phase);
        } else {
            constructIndividualBB(bbd, phase);
        }
        // mark as ready
        changeState(bbd, BB_READY);

        // create deps after and update that to succeeding BBs.
        // succeeding BB's are also queued to be scheduled here.
        // queue succeeding BB's in case either
        // * their input data has changed
        // * current BB was processed for the first time
        if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
            if (updateRegistersAliveAfter(bbd) || !bbd.constructed_) {
                setSucceedingPredeps(bbd, !bbd.constructed_, phase);
            }
        } else {
            if (updateMemAndFuAliveAfter(bbd) || !bbd.constructed_) {
                setSucceedingPredeps(bbd, !bbd.constructed_, phase);
            }
        }
        bbd.constructed_ = true;
    }
}

/**
 * Updates bookkeeping used for calculating register deaths.
 *
 * Marks registers used at or after some BB to it's predecessor BB's.
 *
 * @param bbd basic block to process.
 * @param firstTime whether this BB is analyzer for the first time.
 */
void
DataDependenceGraphBuilder::updatePreceedingRegistersUsedAfter(
    BBData& bbd, bool firstTime) {
    BasicBlockNode& bbn = *bbd.bblock_;
    BasicBlock& bb = bbn.basicBlock();
    BasicBlockNodeSet predecessors = cfg_->predecessors(bbn);
    for (BasicBlockNodeSet::iterator predIter = predecessors.begin();
         predIter != predecessors.end(); predIter++) {
        BasicBlockNode* pred = *predIter;
        BasicBlock& predBB = pred->basicBlock();
        BBData& predData = *bbData_[pred];
        size_t size = predBB.liveRangeData_->registersUsedAfter_.size();
        AssocTools::append(
            bb.liveRangeData_->registersUsedInOrAfter_, predBB.liveRangeData_->registersUsedAfter_);

        // if updated, need to be handled again.
        if (predBB.liveRangeData_->registersUsedAfter_.size() > size || firstTime) {
            if (predData.state_ != BB_QUEUED) {
                changeState(predData, BB_QUEUED);
            }
        }
    }
}

/**
 * Sets outgoing data from this BB to incoming data of successors.
 *
 * Also queues them to be reprocessed if they are changed.
 *
 * @param bbd BBD whose successors will be updated.
 * @param queueAll If true, queues all successors even if they do not change.
 * @param phase whether to handle register& operation deps or
 *        memory and side-effect dependencies.
 */
void
DataDependenceGraphBuilder::setSucceedingPredeps(
    BBData& bbd, bool queueAll, 
    ConstructionPhase phase) {
    BasicBlockNode& bbn = *bbd.bblock_;
    BasicBlock& bb = bbn.basicBlock();

    BasicBlockNodeSet forwardSuccessors = cfg_->successors(bbn, true);
    for (BasicBlockNodeSet::iterator succIter = forwardSuccessors.begin();
         succIter != forwardSuccessors.end(); succIter++) {
        setSucceedingPredepsForBB(
            bb, **succIter, queueAll, false, phase);
    }

    // successors over loop edges.
    BasicBlockNodeSet backwardSuccessors = cfg_->successors(bbn, false, true);
    for (BasicBlockNodeSet::iterator succIter = backwardSuccessors.begin();
         succIter != backwardSuccessors.end(); succIter++) {
        setSucceedingPredepsForBB(
            bb, **succIter, queueAll, true, phase);
    }
}

/**
 * Sets outgoing data from this BB to incoming data of one successor.
 *
 * Also queues them to be reprocessed if they are changed.
 *
 * @param bb BB whose successor will be updated.
 * @param successor successor BB whose incoming data is being updated.
 * @param queueAll If true, queues all successors even if they do not change.
 * @param loop whether to add loop property to the copied
 *        bookkeeping, ie create edges with loop property.
 * @param phase whether to handle register& operation deps or
 *        memory and side-effect dependencies.
 */
void
DataDependenceGraphBuilder::setSucceedingPredepsForBB(
    BasicBlock& bb, BasicBlockNode& successor, bool queueAll, bool loop,
    ConstructionPhase phase) {

    BasicBlock& succBB = successor.basicBlock();
    BBData& succData = *bbData_[&successor];
    bool changed = false;

    if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
        changed |= LiveRangeData::appendUseMapSets(
            bb.liveRangeData_->regDefAfter_, 
            succBB.liveRangeData_->regDefReaches_, loop);

        if (currentDDG_->hasAllRegisterAntidependencies() || 
            (&bb == &succBB && 
             currentDDG_->hasSingleBBLoopRegisterAntidependencies())) {
            changed |= LiveRangeData::appendUseMapSets(
                bb.liveRangeData_->regUseAfter_, 
                succBB.liveRangeData_->regUseReaches_, loop);
        }
    } else {
        // mem deps + fu state deps

        changed |= LiveRangeData::appendUseMapSets(
            bb.liveRangeData_->memDefAfter_, 
            succBB.liveRangeData_->memDefReaches_, loop);

        changed |= LiveRangeData::appendUseMapSets(
            bb.liveRangeData_->memUseAfter_, 
            succBB.liveRangeData_->memUseReaches_, loop);

        size_t size = succBB.liveRangeData_->fuDepReaches_.size();
        LiveRangeData::appendMoveNodeUse(
            bb.liveRangeData_->fuDepAfter_, 
            succBB.liveRangeData_->fuDepReaches_, loop);
        if (succBB.liveRangeData_->fuDepReaches_.size() > size) {
            changed = true;
        }
    }

    // need to queue successor for update?
    if (changed || queueAll) {
        changeState(succData, BB_QUEUED, loop);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Low-level of Multi-BB DDG construction
///////////////////////////////////////////////////////////////////////////////

/**
 * Updates live register lists after a basic block has been processed.
 *
 * Copies use and define definitions from the basic block and
 * it's prodecessors to the after alive data structures.
 *
 * @param bbd BBData for bb being processed
 * @return true if the after alive data structures have been changed
 */
bool DataDependenceGraphBuilder::updateRegistersAliveAfter(
    BBData& bbd) {
    BasicBlock& bb = bbd.bblock_->basicBlock();
    bool changed = false;

    // copy reg definitions that are alive
    for (MoveNodeUseMapSet::iterator iter = 
             bb.liveRangeData_->regDefReaches_.begin();
         iter != bb.liveRangeData_->regDefReaches_.end(); iter++) {
        
        TCEString reg = iter->first;
        std::set<MoveNodeUse>& preDefs = iter->second;
        // todo: clear or not?
        std::set<MoveNodeUse>& defAfter = bb.liveRangeData_->regDefAfter_[reg];
        size_t size = defAfter.size();

        // only copy incomin dep if this has no unconditional writes
        if (bb.liveRangeData_->regKills_.find(reg) == bb.liveRangeData_->regKills_.end()) {
            for (std::set<MoveNodeUse>::iterator i = preDefs.begin();
                 i != preDefs.end(); i++ ) {
                defAfter.insert(*i);
            }
        }
        // if size increased, the data has changed
        if (size < defAfter.size()) {
            changed = true;
        }
    }
    // own deps. need to do only once but now does after every.
    changed |= LiveRangeData::appendUseMapSets(
        bb.liveRangeData_->regDefines_, 
        bb.liveRangeData_->regDefAfter_, false);
    
    if (currentDDG_->hasAllRegisterAntidependencies()) {
        // copy uses that are alive
        for (MoveNodeUseMapSet::iterator iter =
                 bb.liveRangeData_->regUseReaches_.begin(); iter != bb.liveRangeData_->regUseReaches_.end();
             iter++) {
            TCEString reg = iter->first;
            std::set<MoveNodeUse>& preUses = iter->second;
            std::set<MoveNodeUse>& useAfter = bb.liveRangeData_->regUseAfter_[reg];
            size_t size = useAfter.size();
            if (bb.liveRangeData_->regKills_.find(reg) == bb.liveRangeData_->regKills_.end()) {
                for (std::set<MoveNodeUse>::iterator i = preUses.begin();
                     i != preUses.end(); i++ ) {
                    useAfter.insert(*i);
                }
            }
            // if size increased, the data has changed
            if (size < useAfter.size()) {
                changed = true;
            }
        }
    }
    
    // own deps. need to do only once but now does after every.
    changed |= LiveRangeData::appendUseMapSets(
        bb.liveRangeData_->regLastUses_,
        bb.liveRangeData_->regUseAfter_, false);
    return changed;
}


/**
 * Updates live mem access lists and fu state usages after a basic block
 * has been processed.
 *
 * Copies use and write definitions from the basic block and
 * it's prodecessors to the after alive data structures.
 *
 * @param bbd BBData for bb being processed
 * @return true if the after alive data structures have been changed
 */
bool DataDependenceGraphBuilder::updateMemAndFuAliveAfter(BBData& bbd) {
    BasicBlock& bb = bbd.bblock_->basicBlock();
    bool changed = false;

    // copy mem definitions that are alive
    for (MoveNodeUseMapSet::iterator iter = bb.liveRangeData_->memDefReaches_.begin();
         iter != bb.liveRangeData_->memDefReaches_.end(); iter++) {

        TCEString category = iter->first;
        std::set<MoveNodeUse>& preDefs = iter->second;
        std::set<MoveNodeUse>& defAfter = bb.liveRangeData_->memDefAfter_[category];
        std::set<MoveNodeUse>& ownDefines = bb.liveRangeData_->memDefines_[category];
        size_t size = defAfter.size();

        // only copy incomin dep if this has no unconditional writes,
        if (bb.liveRangeData_->memKills_.find(category) == bb.liveRangeData_->memKills_.end()) {
            for (std::set<MoveNodeUse>::iterator i = preDefs.begin();
                 i != preDefs.end(); i++ ) {
//                MoveNode* preAddress = addressMove(*(i->mn()));
                ProgramOperation& prevPop = i->mn()->destinationOperation();

                bool overWritten = false;
                for (std::set<MoveNodeUse>::iterator j = ownDefines.begin();
                     j != ownDefines.end(); j++) {
                    if (j->mn()->move().isUnconditional()) {
//                        MoveNode* ownAddress = addressMove(*(j->mn()));
                        ProgramOperation& ownPop = j->mn()->destinationOperation();
                        if (analyzeMemoryAlias(prevPop, ownPop) ==
                            MemoryAliasAnalyzer::ALIAS_TRUE) {
                            overWritten = true;
                            break;
                        }
                    }
                }
                if (!overWritten) {
                    defAfter.insert(*i);
                }
            }
        }
        // if size increased, the data has changed
        if (size < defAfter.size()) {
            changed = true;
        }
    }
    // own deps. need to do only once but now does after every.
    changed |= LiveRangeData::appendUseMapSets(
        bb.liveRangeData_->memDefines_,
        bb.liveRangeData_->memDefAfter_, false);

    // copy uses that are alive
    for (MoveNodeUseMapSet::iterator iter = bb.liveRangeData_->memUseReaches_.begin();
         iter != bb.liveRangeData_->memUseReaches_.end(); iter++) {

        TCEString category = iter->first;
        std::set<MoveNodeUse>& preUses = iter->second;
        std::set<MoveNodeUse>& useAfter = bb.liveRangeData_->memUseAfter_[category];
        std::set<MoveNodeUse>& ownDefines = bb.liveRangeData_->memDefines_[category];

        size_t size = useAfter.size();
        if (bb.liveRangeData_->memKills_.find(category) == bb.liveRangeData_->memKills_.end()) {
            for (std::set<MoveNodeUse>::iterator i = preUses.begin();
                 i != preUses.end(); i++ ) {
//                MoveNode* preAddress = addressMove(*(i->mn()));
                ProgramOperation& prevPop = i->mn()->destinationOperation();
                bool overWritten = false;
                for (std::set<MoveNodeUse>::iterator j =
                         ownDefines.begin(); j != ownDefines.end(); j++) {
                    if (j->mn()->move().isUnconditional()) {
                        ProgramOperation& ownPop = j->mn()->destinationOperation();
                        if (analyzeMemoryAlias(prevPop, ownPop) ==
                            MemoryAliasAnalyzer::ALIAS_TRUE) {
                            overWritten = true;
                            break;
                        }
                    }
                }
                if (!overWritten) {
                    useAfter.insert(*i);
                }
            }
        }
        // if size increased, the data has changed
        if (size < useAfter.size()) {
            changed = true;
        }
    }

    // fu deps
    size_t size = bb.liveRangeData_->fuDepAfter_.size();
    AssocTools::append(bb.liveRangeData_->fuDeps_, bb.liveRangeData_->fuDepAfter_);
    if (bb.liveRangeData_->fuDepAfter_.size() > size) {
        changed = true;
    }
    return changed;
}

/**
 * Processes the pseudo deps from entry node.
 *
 * This procedure must be called when currentBB is the first real
 * BB of the procedure.
 */
void DataDependenceGraphBuilder::processEntryNode(MoveNode& mn) {

    // initializes RA
    currentBB_->basicBlock().liveRangeData_->regDefReaches_[RA_NAME].insert(mn);

    // sp
    MoveNodeUse mnd2(mn);
    TCEString sp = specialRegisters_[REG_SP];
    if (sp != "") {
        currentBB_->basicBlock().liveRangeData_->regDefReaches_[sp].insert(mnd2);
    }

    if (rvIsParamReg_) {
        TCEString rv = specialRegisters_[REG_RV];
        if (rv != "") {
            currentBB_->basicBlock().liveRangeData_->regDefReaches_[rv].insert(
                mnd2);
        }
    }

    // params
    // this is for old frontend generated code.
    for (int i = 0; i < 4;i++) {
        TCEString paramReg = specialRegisters_[REG_IPARAM+i];
        if(paramReg != "") {
            currentBB_->basicBlock().liveRangeData_->regDefReaches_[paramReg].insert(mnd2);
        }
    }

    TCEString fp = specialRegisters_[REG_FP];
    if (fp != "") {
        currentBB_->basicBlock().liveRangeData_->regDefReaches_[fp].insert(
            mnd2);
    }
}

/**
 * Reprocesses a basic block which has already once been processed.
 *
 * Checks dependencies to first uses and definitions of registers,
 * does not recreate edges inside the basic block.
 *
 * @param bbd BBData for basic block which is being reprocessed.
 * @param phase whether to handle register& operation deps or
 *        memory and side-effect dependencies.
 */
void
DataDependenceGraphBuilder::updateBB(
    BBData& bbd, ConstructionPhase phase) {
    currentData_ = &bbd;
    currentBB_ = bbd.bblock_;
    BasicBlock& bb = bbd.bblock_->basicBlock();

    // register and operation dependencies
    if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
        //loop all regs having ext deps and create reg edges
        for (MoveNodeUseMapSet::iterator firstUseIter=bb.liveRangeData_->regFirstUses_.begin();
             firstUseIter != bb.liveRangeData_->regFirstUses_.end(); firstUseIter++) {
            TCEString reg = firstUseIter->first;
            std::set<MoveNodeUse>& firstUseSet = firstUseIter->second;
            for (std::set<MoveNodeUse>::iterator iter2 = firstUseSet.begin();
                 iter2 != firstUseSet.end(); iter2++) {
                currentDDG_->updateRegUse(
                    *iter2, reg, currentBB_->basicBlock());
            }
        }

        if (currentDDG_->hasSingleBBLoopRegisterAntidependencies()) {
            // antidependencies to registers
            for (MoveNodeUseMapSet::iterator firstDefineIter =
                     bb.liveRangeData_->regFirstDefines_.begin();
                 firstDefineIter != bb.liveRangeData_->regFirstDefines_.end(); 
                 firstDefineIter++) {
                TCEString reg = firstDefineIter->first;
                std::set<MoveNodeUse>& firstDefineSet = 
                    firstDefineIter->second;
                for (std::set<MoveNodeUse>::iterator iter2=
                         firstDefineSet.begin();
                     iter2 != firstDefineSet.end(); iter2++) {
                    currentDDG_->updateRegWrite(
                        *iter2, reg, currentBB_->basicBlock());
                }
            }
        }
    } else {
        // phase 1 .. mem deps and fu state/side effect dependencies.

        //loop all regs having ext deps and create reg edges
        for (MoveNodeUseMapSet::iterator firstUseIter=bb.liveRangeData_->memFirstUses_.begin();
             firstUseIter != bb.liveRangeData_->memFirstUses_.end(); firstUseIter++) {
            TCEString category = firstUseIter->first;
            std::set<MoveNodeUse>& firstUseSet = firstUseIter->second;
            for (std::set<MoveNodeUse>::iterator iter2 = firstUseSet.begin();
                 iter2 != firstUseSet.end(); iter2++) {
                updateMemUse(*iter2, category);
            }
        }

        // antidependencies to registers
        for (MoveNodeUseMapSet::iterator firstDefineIter =
                 bb.liveRangeData_->memFirstDefines_.begin();
             firstDefineIter != bb.liveRangeData_->memFirstDefines_.end(); firstDefineIter++) {
            TCEString category = firstDefineIter->first;
            std::set<MoveNodeUse>& firstDefineSet = firstDefineIter->second;
            for (std::set<MoveNodeUse>::iterator iter2=firstDefineSet.begin();
                 iter2 != firstDefineSet.end(); iter2++) {
                updateMemWrite(*iter2, category);
            }
        }

        // and fu state deps
        for (MoveNodeUseSet::iterator iter = bb.liveRangeData_->fuDeps_.begin();
             iter != bb.liveRangeData_->fuDeps_.end(); iter++) {
            Terminal& dest = iter->mn()->move().destination();
            TerminalFUPort& tfpd = dynamic_cast<TerminalFUPort&>(dest);
            Operation &dop = tfpd.hintOperation();
            createSideEffectEdges(
                currentBB_->basicBlock().liveRangeData_->fuDepReaches_, *iter->mn(), dop);
        }
    }
}

/**
 * Checks memory write against uses and defs in incoming basic blocks.
 * Creates the needed dependence edges.
 *
 * @param mnd MoveNodeUse of movenode being processed.
 * @param category which memory category this memory write belongs.
 */
void
DataDependenceGraphBuilder::updateMemWrite(
    MoveNodeUse mnd, const TCEString& category) {

    // create waw edges from all alive writes to this node.
    for (MoveNodeUseSet::iterator iter =
        currentBB_->basicBlock().liveRangeData_->memDefReaches_[category].begin();
        iter != currentBB_->basicBlock().liveRangeData_->memDefReaches_[category].end();) {
        checkAndCreateMemDep(*iter++, mnd, DataDependenceEdge::DEP_WAW);
    }

    // create war edges from all alive reads to this node.
    for (MoveNodeUseSet::iterator iter = currentBB_->basicBlock().liveRangeData_->
            memUseReaches_[category].begin();
        iter != currentBB_->basicBlock().liveRangeData_->memUseReaches_[category].end();) {
        checkAndCreateMemDep(*iter++, mnd, DataDependenceEdge::DEP_WAR);
    }
}


/**
 * Checks memory read against uses and defs in incoming basic blocks.
 * Creates the needed dependence edges.
 *
 * @param mnd MoveNodeUse of movenode being processed.
 * @param category which memory category this memory write belongs.
 */
void DataDependenceGraphBuilder::updateMemUse(
    MoveNodeUse mnd, const TCEString& category) {

    for (MoveNodeUseSet::iterator iter =
             currentBB_->basicBlock().liveRangeData_->memDefReaches_[category].begin();
        iter != currentBB_->basicBlock().liveRangeData_->memDefReaches_[category].end();
        iter++) {
        checkAndCreateMemDep(*iter, mnd,DataDependenceEdge::DEP_RAW);
    }
}


///////////////////////////////////////////////////////////////////////////////
// Searching of ends of liveranges, ie last uses of values
///////////////////////////////////////////////////////////////////////////////

/**
 * Searches the last usages a registers.
 *
 * This information is used for checking whether given register contains
 * live value at given cycle.
 */
void
DataDependenceGraphBuilder::searchRegisterDeaths() {

    // initializes states of all BB's to unreached.
    initializeBBStates();

    // start from end of cfg. (sink nodes), queue them
    ControlFlowGraph::NodeSet lastBBs = cfg_->sinkNodes();
    for (ControlFlowGraph::NodeSet::iterator iter =
             lastBBs.begin(); iter != lastBBs.end(); iter++) {
        changeState(*(bbData_[*iter]), BB_QUEUED);
    }

    // then iterate over all BB's, going from BB to it's
    // predecessors.
    iterateRegisterDeaths();

    // all should have gone thru, but if there are 4ever loop causing
    // BB's not reachable from the end.
    // we might want to handle those also.
    while (!blocksByState_[BB_UNREACHED].empty()) {
        if (Application::verboseLevel() > Application::VERBOSE_LEVEL_DEFAULT) {
            Application::logStream() << "Warning: BB in 4ever loop!"
                                     << std::endl;
            Application::logStream() << "In procedure: " << cfg_->name() <<
                std::endl;
//            cfg.writeToDotFile("4everloop_bb.dot");
        }
        changeState(**blocksByState_[BB_UNREACHED].begin(), BB_QUEUED);

        iterateRegisterDeaths();
    }

    // free bb data
    AssocTools::deleteAllValues(bbData_);
}

/**
 * Iterates thru all queued BB's and find register deaths from them.
 *
 * Loops as long as something keeps changing.
 */
void
DataDependenceGraphBuilder::iterateRegisterDeaths() {

    // loop as long as we have unprocessed/changed BB's
    while (!blocksByState_[BB_QUEUED].empty()) {
        std::list<BBData*>::iterator bbIter =
            blocksByState_[BB_QUEUED].begin();
        BBData& bbd = **bbIter;

        // mark as ready
        changeState(bbd, BB_READY);

        if (updateRegistersUsedInOrAfter(bbd) || (!bbd.constructed_)) {
            // if there asre more registers read after start of this BB,
            // have to update this information to preceedign BBs,
            // and check if they need to be reprocessed.
            updatePreceedingRegistersUsedAfter(bbd, !bbd.constructed_);
        }
        bbd.constructed_ = true;
    }
}

/**
 * Updates bookkeeping about registers used in this or later BBs.
 *
 * This is a helper function used by searchRegisterDeaths() method.
 *
 * @param bbd Data about one basicblock.
 * @return whether the bookkeeping was changed,
           ie the predecessors of this BB need to be reprocessed.
 */
bool
DataDependenceGraphBuilder::updateRegistersUsedInOrAfter(
    BBData& bbd) {
    BasicBlock& bb = bbd.bblock_->basicBlock();
    size_t size = bb.liveRangeData_->registersUsedInOrAfter_.size();

    // if definition not here, it's earlier - copy.
    // if definition here, not alive unless read here.
    for (std::set<TCEString>::iterator i = bb.liveRangeData_->registersUsedAfter_.begin();
         i != bb.liveRangeData_->registersUsedAfter_.end(); i++) {
        // if not written in this, written earlier.
        if (bb.liveRangeData_->regKills_.find(*i) == bb.liveRangeData_->regKills_.end()) {
            bb.liveRangeData_->registersUsedInOrAfter_.insert(*i);
        }
    }

    // reads in this BB.liveRangeData_-> Only reads whose defining value comes/can come
    // outside this BB.liveRangeData_->
    for (MoveNodeUseMapSet::iterator i = bb.liveRangeData_->regFirstUses_.begin();
         i != bb.liveRangeData_->regFirstUses_.end(); i++) {
        bb.liveRangeData_->registersUsedInOrAfter_.insert(i->first);
    }
    if (bb.liveRangeData_->registersUsedInOrAfter_.size() > size) {
        return true;
    }
    return false;
}

/**
 * Internal constant for the name of the return address port
 */
const TCEString DataDependenceGraphBuilder::RA_NAME = "RA";


///////////////////////////////////////////////////////////////////////////////
// BBData
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
DataDependenceGraphBuilder::BBData::BBData(BasicBlockNode& bb) :
    poReadsHandled_(0),
    state_(BB_UNREACHED),  constructed_(false), bblock_(&bb) {
}

/**
 * Destructor.
 */
DataDependenceGraphBuilder::BBData::~BBData() {
}
