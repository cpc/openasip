/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * Implementation of data dependence graph builder
 *
 * @author Heikki Kultala 2006-2008 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include <list>

#include "AssocTools.hh"
#include "ContainerTools.hh"
#include "StringTools.hh"

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

#include "ControlFlowGraph.hh"
#include "ControlFlowEdge.hh"
#include "BasicBlockNode.hh"

#include "DataDependenceGraphBuilder.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceEdge.hh"
#include "MemoryAliasAnalyzer.hh"
#include "POMDisassembler.hh"

#include "ConstantAliasAnalyzer.hh"
#include "FalseAliasAnalyzer.hh"
#include "TCEString.hh"
#include "InterPassData.hh"
#include "InterPassDatum.hh"

using namespace TTAProgram;
using namespace TTAMachine;

using std::list;

static const int REG_SP = 1;
static const int REG_RV = 0;
static const int REG_IPARAM = 2;
static const int REG_RV_HIGH = 6;

class DataDependenceGraph;
class BasicBlockNode;

/**
 * Constructor of Data Dependence graph builder
 */
DataDependenceGraphBuilder::DataDependenceGraphBuilder() :
    interPassData_(NULL), cfg_(NULL) {

    // alias analyzers.
    
    // constant alias AA check aa between global variables.
    addAliasAnalyzer(new ConstantAliasAnalyzer);

    // uncommenting the following line results in faster but
    // broken code. just for testing theoretical benefits.
//     addAliasAnalyzer(new FalseAliasAnalyzer); 

}

/**
 * Constructor of Data Dependence graph builder
 */
DataDependenceGraphBuilder::DataDependenceGraphBuilder(InterPassData& ipd) :
    interPassData_(&ipd) {

    static const TCEString SP_DATUM = "STACK_POINTER";
    static const TCEString RV_DATUM = "RV_REGISTER";
    // high part of 64-bit return values.
    static const TCEString RV_HIGH_DATUM = "RV_HIGH_REGISTER";

    if (ipd.hasDatum(SP_DATUM)) {
        RegDatum& sp = dynamic_cast<RegDatum&>(ipd.datum(SP_DATUM));
        specialRegisters_[REG_SP] = 
            sp.first + '.' + Conversion::toString(sp.second);
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
        specialRegisters_[REG_RV] =
            rv.first + '.' + Conversion::toString(rv.second);
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
    
    // alias analyzers.
    
    // constant alias AA check aa between global variables.
    addAliasAnalyzer(new ConstantAliasAnalyzer);

    // uncommenting the following line results in faster but
    // broken code. just for testing theoretical benefits.
//     addAliasAnalyzer(new FalseAliasAnalyzer); 
}

/**
 * Destructor of DataDependenceGraphBuilder
 */
DataDependenceGraphBuilder::~DataDependenceGraphBuilder() {
    for (unsigned int i = 0; i < aliasAnalyzers_.size(); i++) {
        delete(aliasAnalyzers_.at(i));
    }
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
 * Changes state of a basic block in processing.
 * Move BBData into a diffefent list and changes the state data in BBData.
 * 
 * @param bbd BBData of basic block whose state is being changed
 * @param newState the new state of the basic block.
*/
void
DataDependenceGraphBuilder::changeState(
    BBData& bbd, BBState newState) {

    BBState oldState = bbd.state_;
    if (newState != oldState) {
        ContainerTools::removeValueIfExists(blocksByState_[oldState], &bbd);
        bbd.state_ = newState;
        blocksByState_[newState].push_back(&bbd);
    }
}

/**
 * Creates new Data Dependence Graph for the given basic block.
 *
 * Client has to delete the graph when it is not anymore used.
 *
 * @param bb BasicBlockNode whose data dependence graph to build.
 * @return new DataDependence Graph.
 *
 */
DataDependenceGraph*
DataDependenceGraphBuilder::build(
    BasicBlock& bb, 
    DataDependenceGraph::AntidependenceLevel registerAntidependenceLevel,
    const TCEString& ddgName, const UniversalMachine* um,
    bool createMemAndFUDeps) {

    currentDDG_ = new DataDependenceGraph(
        ddgName, registerAntidependenceLevel);
    // GRR, start and end addresses are lost..
    currentBB_ = new BasicBlockNode(bb); 
    currentData_ = new BBData(*currentBB_);

    if (um != NULL) {
        getStaticRegisters(*um, specialRegisters_);
    } else {
        getStaticRegisters(bb,specialRegisters_);
    }

    try {
        constructIndividualBB(REGISTERS_AND_PROGRAM_OPERATIONS);
        if (createMemAndFUDeps) {
            constructIndividualBB(MEMORY_AND_SIDE_EFFECTS);
        }
    } catch (Exception&) {
        delete currentDDG_; currentDDG_ = NULL;
        delete currentData_; currentData_ = NULL;
        delete currentBB_; currentBB_ = NULL;
        throw;
    }

    delete currentData_;
    currentData_ = NULL;
    return currentDDG_;
}

/**
   Builds a DDG from a CFG.
*/

DataDependenceGraph*
DataDependenceGraphBuilder::build(
    ControlFlowGraph& cfg, 
    DataDependenceGraph::AntidependenceLevel antidependenceLevel,
    const UniversalMachine* um,
    bool createMemAndFUDeps) {

    cfg_ = &cfg;

    // @TODO: when CFG subgraphs are in use, 2nd param not always true
    DataDependenceGraph* ddg = new DataDependenceGraph(
        cfg.procedureName(), antidependenceLevel, true);

    try {
        if (um != NULL) {
            getStaticRegisters(*um, specialRegisters_);
        } else {
            getStaticRegisters(cfg, specialRegisters_);
        }
        
        currentDDG_ = ddg;

        createRegisterDeps();
        if (createMemAndFUDeps) {
            createMemAndFUstateDeps();
        }
    } catch (...) {
        delete ddg;
        // free bb data
        AssocTools::deleteAllValues(bbData_);
        throw;
    }
    return ddg;
}

void DataDependenceGraphBuilder::iterateBBs(ConstructionPhase phase) {

    while (!blocksByState_[BB_QUEUED].empty()) {
        std::list<BBData*>::iterator bbIter = 
            blocksByState_[BB_QUEUED].begin();
        BBData& bbd = **bbIter;

        // construct or update BB
        if (bbd.constructed_) {
            updateBB(bbd, phase);
        } else {
            constructIndividualBB(bbd,phase);
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
 * This appends the data from one RegisterUseMapSet to another.
 * 
 * it traverses the map, and for every string, set pair it 
 * finds or creates the corresponging set in the destination and appends
 * the set to that set.
 * This is used for copying alive definitions.
 *
 * @param srcMap source where to copy from
 * @param dstMap destination where to copy to.
 * @return true if destination changed (needs updating)
 */
bool DataDependenceGraphBuilder::appendUseMapSets(
    const RegisterUseMapSet& srcMap, RegisterUseMapSet& dstMap) {
    bool changed = false;
    for (RegisterUseMapSet::const_iterator srcIter = srcMap.begin();
         srcIter != srcMap.end(); srcIter++) {
        std::string reg = srcIter->first;
        const RegisterUseSet& srcSet = srcIter->second;
        RegisterUseSet& dstSet = dstMap[reg];
        // dest set size before appending.
        size_t size = dstSet.size();
        AssocTools::append(srcSet, dstSet);
        // if size has changed, dest is changed.
        if (dstSet.size() > size) {
            changed = true;
        }
    }
    return changed;
}


/**
 * Sets outgoing data from this BB to incoming data of successors.
 * Also queues them to be reprocessed if they are changed.
 *
 * @param bbd BBD whose successors will be updated.
 * @param cfg Control flow graph where to find those successors
 * @param queueAll. If true, queues all successors even if they do not change.
 */
void DataDependenceGraphBuilder::setSucceedingPredeps(
    BBData& bbd, bool queueAll, ConstructionPhase phase) {

    BasicBlockNode& bbn = *bbd.bblock_;
    BBNodeSet successors = cfg_->successors(bbn);
    for (BBNodeSet::iterator succIter = successors.begin(); 
         succIter != successors.end(); succIter++) {
        BasicBlockNode* succ = *succIter;

        BBData& succData = *bbData_[succ];
        bool changed = false;
        if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
            changed |= appendUseMapSets(
                bbd.regDefAfter_, succData.regDefReaches_);
            
            if (currentDDG_->hasAllRegisterAntidependencies() ||
                (&bbn == succ &&
                 currentDDG_->hasSingleBBLoopRegisterAntidependencies())) {
                changed |= appendUseMapSets(
                    bbd.regUseAfter_, succData.regUseReaches_);
            }
        } else {
            // mem deps + fu state deps

            // size at beginning.
            size_t size = succData.memDefReaches_.size() + 
                succData.memUseReaches_.size() + succData.fuDepReaches_.size();
        
            AssocTools::append(
                bbd.memDefAfter_, succData.memDefReaches_);
            AssocTools::append(
                bbd.memUseAfter_, succData.memUseReaches_);
            AssocTools::append(
                bbd.fuDepAfter_, succData.fuDepReaches_);
            
            // if size increased, something is changed.
            if (succData.memDefReaches_.size() + 
                succData.memUseReaches_.size() + 
                succData.fuDepReaches_.size() > size) {
                changed = true;
            }
            // need to queue successor for update?
        }
        if (changed || queueAll) {
            if (succData.state_ != BB_QUEUED) {
                changeState(succData, BB_QUEUED);
            }
        }
    }
}

/**
 * Reprocesses a basic block which has already once been processed.
 * 
 * Checks dependencies to first uses and definitions of registers,
 * does not recreate edges inside the basic block.
 *
 * @param bbd BBData for basic block which is being reprocessed.
  */
void
DataDependenceGraphBuilder::updateBB(BBData& bbd, ConstructionPhase phase) {
    currentData_ = &bbd;
    currentBB_ = bbd.bblock_;

    // register and operation dependencies
    if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {

        //loop all regs having ext deps and create reg edges
        for (RegisterUseMapSet::iterator firstUseIter = 
                 bbd.regFirstUses_.begin();
             firstUseIter != bbd.regFirstUses_.end(); firstUseIter++) {
            std::string reg = firstUseIter->first;
            std::set<MNData2>& firstUseSet = firstUseIter->second;
            for (std::set<MNData2>::iterator iter2 = firstUseSet.begin();
                 iter2 != firstUseSet.end(); iter2++) {
                updateRegUse(*iter2, reg);
            }
        }
        
        if (currentDDG_->hasSingleBBLoopRegisterAntidependencies()) {
            // antidependencies to registers
            for (RegisterUseMapSet::iterator firstDefineIter = 
                     bbd.regFirstDefines_.begin();
                 firstDefineIter != bbd.regFirstDefines_.end(); 
                 firstDefineIter++) {
                std::string reg = firstDefineIter->first;
                std::set<MNData2>& firstDefineSet = firstDefineIter->second;
                for (std::set<MNData2>::iterator iter2 = 
                         firstDefineSet.begin();
                     iter2 != firstDefineSet.end(); iter2++) {
                    updateRegWrite(*iter2, reg);
                }
            }
        }
    } else {
        // phase 1 .. mem deps and fu state/side effect dependencies.
        // then memory deps.. use
        for (RegisterUseSet::iterator firstUseIter = bbd.memFirstUses_.begin();
             firstUseIter != bbd.memFirstUses_.end(); firstUseIter++) {
            updateMemUse(*firstUseIter);
        }
        // and defs for memory antideps
        for (RegisterUseSet::iterator firstDefIter = 
                 bbd.memFirstDefines_.begin();
             firstDefIter != bbd.memFirstDefines_.end(); firstDefIter++) {
            updateMemWrite(*firstDefIter);
        }
        // and fu state deps
        for (RegisterUseSet::iterator iter = bbd.fuDeps_.begin();
             iter != bbd.fuDeps_.end(); iter++) {
            Terminal& dest = iter->mn_->move().destination();
            TerminalFUPort& tfpd = dynamic_cast<TerminalFUPort&>(dest);
            Operation &dop = tfpd.hintOperation();
            createSideEffectEdges(
                currentData_->fuDepReaches_, *iter->mn_, dop);
        }
    }
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
 */
void
DataDependenceGraphBuilder::constructIndividualBB(ConstructionPhase phase) {

    for (int ia = 0; ia < currentBB_->basicBlock().instructionCount(); ia++) {
        Instruction& ins = currentBB_->basicBlock().instructionAtIndex(ia);
        for (int i = 0; i < ins.moveCount(); i++) {
            Move& move = ins.move(i);
            MoveNode* moveNode = NULL;

            // if phase is 0, create the movenode, and handle guard.
            if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
                moveNode = new MoveNode(move);
                currentDDG_->addNode(*moveNode, *currentBB_);

                if (!(move.isUnconditional())) {
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

    // Checks if we have some unready program operations at the end
    // of a basic block.
    if (currentData_->destPending_.size() > 0 ||
        currentData_->readPending_.size() > 0) {
        
        std::string msg = std::string("Basic block ") 
            + Conversion::toString(currentBB_->originalStartAddress()) 
            + std::string(" - ")
            + Conversion::toString(currentBB_->originalEndAddress()) 
            + std::string(", size : ") 
            + Conversion::toString(
                currentBB_->basicBlock().instructionCount())
            + std::string(" handled but we have ")
            + Conversion::toString(currentData_->destPending_.size())
            + std::string(" ")
            + Conversion::toString(currentData_->readPending_.size())
            + std::string(" unready PO's at: ")
            + currentDDG_->name() 
            + std::string(", probably an operation without result move?");

        for (unsigned int i = 0; i < currentData_->readPending_.size(); i++) {
            msg += "\n\tmissing read: " + 
                std::string(currentData_->readPending_[i]->operation().name());
        }

        for (unsigned int i = 0; i < currentData_->destPending_.size(); i++) {
            msg += "\n\tmissing dest: " + 
                std::string(currentData_->destPending_[i]->operation().name());
        }

        throw IllegalProgram(__FILE__,__LINE__,__func__, msg);
    }

}

/**
 * Updates live register lists after a basic block has been processed.
 *
 * Copies use and define definitions from the basic block and
 * it's prodecessors to the after alive data structures.
 *
 * @param bbd BBData for bb being processed
 * @return true if the after alive data structures have been changed
 */
bool DataDependenceGraphBuilder::updateRegistersAliveAfter(BBData& bbd) {
    bool changed = false;

    // copy reg definitions that are alive
    for (RegisterUseMapSet::iterator iter =
             bbd.regDefReaches_.begin(); iter != bbd.regDefReaches_.end(); 
         iter++) {
        std::string reg = iter->first;
        std::set<MNData2>& preDefs = iter->second;
        // todo: clear or not?
        std::set<MNData2>& defAfter = bbd.regDefAfter_[reg];
        size_t size = defAfter.size();
        
        // only copy incomin dep if this has no unconditional writes
        if (bbd.regKills_.find(reg) == bbd.regKills_.end()) {
            for (std::set<MNData2>::iterator i = preDefs.begin();
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
    changed |= appendUseMapSets(bbd.regDefines_, bbd.regDefAfter_);

    if (currentDDG_->hasAllRegisterAntidependencies()) {
        // copy uses that are alive
        for (RegisterUseMapSet::iterator iter =
                 bbd.regUseReaches_.begin(); iter != bbd.regUseReaches_.end(); 
             iter++) {
            std::string reg = iter->first;
            std::set<MNData2>& preUses = iter->second;
            std::set<MNData2>& useAfter = bbd.regUseAfter_[reg];
            size_t size = useAfter.size();
            if (bbd.regKills_.find(reg) == bbd.regKills_.end()) {
                for (std::set<MNData2>::iterator i = preUses.begin();
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
    changed |= appendUseMapSets(bbd.regLastUses_, bbd.regUseAfter_);
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
    bool changed = false;

    // mem deps and fu state deps

    size_t size = bbd.memDefAfter_.size() + bbd.memUseAfter_.size() +
        bbd.fuDepAfter_.size();
    // no killing write? then copy pre-deps.
    if (bbd.memKill_.mn_ == NULL) {
        AssocTools::append(bbd.memDefReaches_, bbd.memDefAfter_);
        AssocTools::append(bbd.memUseReaches_, bbd.memUseAfter_);
    } 

    AssocTools::append(bbd.memDefines_, bbd.memDefAfter_);
    AssocTools::append(bbd.memLastUses_, bbd.memUseAfter_);
    AssocTools::append(bbd.fuDeps_, bbd.fuDepAfter_);
    
    if (bbd.memDefAfter_.size() + bbd.memUseAfter_.size() +
        bbd.fuDepAfter_.size() > size) {
        changed = true;
    }
    return changed;
}

/**
 * Creates operand edges between intput and output moves of a
 * programoperation.
 * 
 * @param po ProgramOperation whose egdes we are creating.
 */
void DataDependenceGraphBuilder::createOperationEdges(
    ProgramOperation& po) {

    Operation& op = po.operation();
    // create operation edges
    // from all input nodes to all
    // output nodes, loops follow
    
    // loop thru all input parameters
    for (int inps = 1;
         inps <= op.numberOfInputs();
         inps++) {
        MoveNodeSet& mnsi = po.inputNode(inps);
        
        // exactly once on unscheduled code,
        // but still nice to not assume anything.
        for (int inp = 0; inp < mnsi.count(); inp++) {
            
            // Loop thru all output parameters
            for (int outps = op.numberOfInputs()+1;
                 outps <=
                     op.numberOfInputs()+op.numberOfOutputs();
                 outps++ ) {
                if (po.hasOutputNode(outps)) {
                    MoveNodeSet& mnso = po.outputNode(outps);
                    // exactly once on unscheduled code
                    // but still nice to not assume anything.
                    for (int outp = 0; outp<mnso.count(); outp++) {
                        // and create operation edges
                        // from all input nodes to all
                        // output nodes.
                        DataDependenceEdge* dde = 
                            new DataDependenceEdge(
                                DataDependenceEdge::EDGE_OPERATION,
                                DataDependenceEdge::DEP_UNKNOWN);
                        
                        currentDDG_->connectOrDeleteEdge(
                            mnsi.at(inp), mnso.at(outp), dde);
                    }
                }
            }
        }
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
    Guard& g = moveNode.move().guard().guard();
    RegisterGuard* rg = dynamic_cast<RegisterGuard*>(&g);
    if (rg != NULL) {
        string regName = rg->registerFile()->name() + '.' +
            Conversion::toString(rg->registerIndex());
        processRegUse(MNData2(moveNode, true),regName);
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
            // new code
            processRegUse(MNData2(moveNode,false,true), RA_NAME);
            
            if (moveNode.move().isReturn()) {
                processReturn(moveNode);
            }
        }
    } else {
        if (source.isGPR()) {
            // new code
            TerminalRegister& tr = dynamic_cast<TerminalRegister&>(source);
            string regName = trName(tr);
                /* tr.registerFile().name() + 
                   Conversion::toString(tr.index());*/
            processRegUse(MNData2(moveNode), regName);
        }
    }
}

/**
 *
 * Analyzes a source of a result read. Handles program operation creation 
 * and operation dependence creation.
 *
 * @param moveNode MoveNode of the move being analyzed.
 */
void
DataDependenceGraphBuilder::processResultRead(MoveNode& moveNode) {
 
    Terminal& source = moveNode.move().source();

    Operation* sop = NULL;
    TerminalFUPort& tfps = dynamic_cast<TerminalFUPort&>(source);
    sop = &tfps.hintOperation();

    for (POLIter poli = currentData_->readPending_.begin();
            poli != currentData_->readPending_.end(); poli++) {
        ProgramOperation* po = *poli;
        if (sop == &po->operation()) {
            po->addOutputNode(moveNode);
            moveNode.setSourceOperation(*po);

            // if this PO is ready, remove from list of uncomplete ones
            if (po->isComplete())   {

                createOperationEdges(*po);

                currentData_->readPending_.erase(poli);
                currentDDG_->addProgramOperation(po);
            } 
            return;
        }
    }
    throw IllegalProgram(
        __FILE__, __LINE__, __func__, "Result move without operands");
}

/**
 * Helper function to create register raw dependence edge
 * 
 * @param source MNData2 containing the source movenode of the edge
 * @param current MNData2 containing the destination movenode of the edge
 */
void
DataDependenceGraphBuilder::createRegRaw(
    const MNData2& source, const MNData2& current) {
    DataDependenceEdge* dde =
        new DataDependenceEdge(
            current.ra_ ? DataDependenceEdge::EDGE_RA :
            DataDependenceEdge::EDGE_REGISTER,
            DataDependenceEdge::DEP_RAW, current.guard_, false, 
            source.pseudo_, current.pseudo_);
    
        // create dependency edge
    currentDDG_->connectOrDeleteEdge(*source.mn_, *current.mn_, dde);
}

/**
 * Helper function to create register raw antidependence edge
 * 
 * @param source MNData2 containing the source movenode of the edge
 * @param current MNData2 containing the destination movenode of the edge
 */
void 
DataDependenceGraphBuilder::createRegWar(
    const MNData2& source, const MNData2& current) {
    DataDependenceEdge* dde =
        new DataDependenceEdge(
            current.ra_ ? DataDependenceEdge::EDGE_RA :
            DataDependenceEdge::EDGE_REGISTER,
            DataDependenceEdge::DEP_WAR, source.guard_, false, 
            source.pseudo_, current.pseudo_);
    
        // create dependency edge
    currentDDG_->connectOrDeleteEdge(*source.mn_, *current.mn_, dde);
}

/**
 * Helper function to create register waw output dependence edge
 *
 * Does not create WaW's from the entry node.
 * 
 * @param source MNData2 containing the source movenode of the edge
 * @param current MNData2 containing the destination movenode of the edge
 */
void 
DataDependenceGraphBuilder::createRegWaw(
    const MNData2& source, const MNData2& current) {
    if (source.mn_->isMove()) {
        DataDependenceEdge* dde =
            new DataDependenceEdge(
            current.ra_ ? DataDependenceEdge::EDGE_RA :
            DataDependenceEdge::EDGE_REGISTER,
                DataDependenceEdge::DEP_WAW, false, false, 
                source.pseudo_, current.pseudo_);
        
        // create dependency edge
        currentDDG_->connectOrDeleteEdge(
            *source.mn_, *current.mn_, dde);
    }
}

/** 
 * Processes the pseudo deps from entry node.
 * This procedure must be called when currentBB is the first real
 * BB of the procedure.
 */
void DataDependenceGraphBuilder::processEntryNode(MoveNode& mn) {
    
    // initializes RA
    currentData_->regDefReaches_[RA_NAME].insert(mn);

    // sp 
    MNData2 mnd2(mn);
    std::string sp = specialRegisters_[REG_SP];
    if (sp != "") {
        currentData_->regDefReaches_[sp].insert(mnd2);
    } 

    std::string rv = specialRegisters_[REG_RV];
    if (rv != "") {
        currentData_->regDefReaches_[rv].insert(mnd2);
    } 

    // params
    for (int i = 0; i < 4;i++) {
        std::string paramReg = specialRegisters_[REG_IPARAM+i];
        if(paramReg != "") {
            currentData_->regDefReaches_[paramReg].insert(mnd2);
        }
    }
}


/**
 * Creates dependencies from incoming definitions to a reg use.
 * 
 * @param mnd 
 */
void DataDependenceGraphBuilder::updateRegUse(
    MNData2 mnd, const std::string& reg) {

    // create RAW's from definitions in previous BBs.
    std::set<MNData2>& defReaches = currentData_->regDefReaches_[reg];
    for (std::set<MNData2>::iterator i = defReaches.begin();
         i != defReaches.end(); i++) {
        createRegRaw(*i,mnd);
    }
}

void DataDependenceGraphBuilder::processRegUse(
    MNData2 mnd, const std::string& reg) {

    // writes in previous BB's killed or not?
    if (currentData_->regKills_.find(reg) == currentData_->regKills_.end()) {
        currentData_->regFirstUses_[reg].insert(mnd);
        // deps from previous BB's
        updateRegUse(mnd, reg);
    } 
    // can be multiple if some write predicated
    std::set<MNData2>& defines_ = currentData_->regDefines_[reg];

    for (std::set<MNData2>::iterator i = defines_.begin();
         i != defines_.end(); i++) {
        createRegRaw(*i,mnd);
    }
    // add this read to list of uses
    currentData_->regLastUses_[reg].insert(mnd);
}

/**
 * Creates dependencies to MN's in other BBs
 */
void
DataDependenceGraphBuilder::updateRegWrite(
    MNData2 mnd, const std::string& reg) {

    // WaWs
    std::set<MNData2>& defReaches_ = currentData_->regDefReaches_[reg];
    for (std::set<MNData2>::iterator i = defReaches_.begin();
         i != defReaches_.end(); i++) {
        createRegWaw(*i,mnd);
    }
    
    // WaRs
    std::set<MNData2>& useReaches_ = currentData_->regUseReaches_[reg];
    for (std::set<MNData2>::iterator i = useReaches_.begin();
         i != useReaches_.end(); i++) {
        createRegWar(*i,mnd);
    }
}


/* 
 * Checks whether two movenodes have exclusive guard, ie
 * same guard but inverted on one of them.
 *
 * If not sure returns false
 * 
 * @param mn1 first movenode to check.
 * @param mn2 second movenode to check.
 * @return true if they have same guard inverted, false otherwise.
 */
bool
DataDependenceGraphBuilder::exclusingGuards(
    const MoveNode& mn1, const MoveNode& mn2) {
    if (!mn1.isMove() || !mn2.isMove()) {
        return false;
    }
    const Move& move1 = mn1.move();
    const Move& move2 = mn2.move();
    if (!move1.isUnconditional() && !move2.isUnconditional()) {
        MoveGuard& mg1 = move1.guard();
        MoveGuard& mg2 = move2.guard();
        if (mg1.isInverted() != mg2.isInverted()) {

            DataDependenceEdge* guardEdge1 = 
                currentDDG_->onlyIncomingGuard(mn1);
            DataDependenceEdge* guardEdge2 = 
                currentDDG_->onlyIncomingGuard(mn2);

            if (guardEdge1 != NULL && guardEdge2 != NULL) {
                if (&currentDDG_->tailNode(*guardEdge1) ==
                    &currentDDG_->tailNode(*guardEdge2)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Analyzes a write to a register.
 * 
 * Creates dependence edges and updates bookkeeping.
 * @param mnd MNData2 containing MoveNode that writes a register
 * @param reg register being written by the given movenode.
 */
void
DataDependenceGraphBuilder::processRegWrite(
    MNData2 mnd, const std::string& reg) {

    std::set<MNData2>& lastUses_ = currentData_->regLastUses_[reg];
    std::set<MNData2>& defines_ = currentData_->regDefines_[reg];

    // if no kills to this reg this one kills it.
    if (currentData_->regKills_.find(reg) == currentData_->regKills_.end()) {

        // is this one a barrier.
        if (mnd.mn_->move().isUnconditional()) {
            currentData_->regKills_[reg] = mnd; 
        }
        // may have incoming WaW's / WaRs to this
        currentData_->regFirstDefines_[reg].insert(mnd);

        // deps from other BB.
        updateRegWrite(mnd, reg);
    } 

    // create WaW to another in own bb
    // Can be multiple if predication used.
    for (std::set<MNData2>::iterator i = defines_.begin();
         i != defines_.end(); i++) {
        // Do not create mem WaW if writes have excluding guards(select op)
        if (!exclusingGuards(*(i->mn_), *(mnd.mn_))) {
            createRegWaw(*i,mnd);
        }
    }
    
    // create WaR to reads in same bb
    for (std::set<MNData2>::iterator i = lastUses_.begin();
         i != lastUses_.end(); i++) {
        createRegWar(*i,mnd);
    }

    // if unconditional , this kills previous deps.
    if (mnd.mn_->move().isUnconditional()) {
        currentData_->regDefines_[reg].clear();
        currentData_->regLastKills_[reg] = mnd;

        // clear reads to given reg.
        lastUses_.clear();
    }

    currentData_->regDefines_[reg].insert(mnd);
}

/**
 * Analyzes destination of a move. 
 * Updates bookkeeping and handles WaW and WaR dependencies of the move.
 * 
 * Checks whether destination is operation or register and calls other
 * functions to do the actual dependence checks etc.
 *
 * @param moveNode MoveNode whose destination is being processed.
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

            // is this a trigger?
            if (tfpd.isOpcodeSetting()) {
                if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
                    processTriggerRegistersAndOperations(
                        moveNode, dop);
                } else {
                    processTriggerMemoryAndFUStates(moveNode, dop);
                }
            } else { // not trigger
                // we don't care about operands in second phase.
                if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
                    processOperand(moveNode, dop);
                }
            }
        } else {  // RA write
            if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
                processRegWrite(MNData2(moveNode,false,true), RA_NAME);
            }
        }
    } else {
        if (dest.isGPR()) {
            // we do not care about register reads in second phase
            if (phase == REGISTERS_AND_PROGRAM_OPERATIONS) {
                TerminalRegister& tr = dynamic_cast<TerminalRegister&>(dest);
                TCEString regName = trName(tr);
                processRegWrite(MNData2(moveNode), regName);
            }
        } else { // something else
            throw IllegalProgram(__FILE__,__LINE__,__func__,
                                 "Move has illegal destination" +
                                 moveNode.toString());
        }
    }
}

/**
 * Method for getting name of a register to be used in bookkeeping.
 * There propably is already a method for this in somewhere,
 * so this could maybe be replaced with usafe of that function.
 *
 * @param tr TerminalRegister of register whose name we are querying
 * @return name of a register.
 */
std::string
DataDependenceGraphBuilder::trName(TerminalRegister& tr) {
    return tr.registerFile().name() + '.' +
        Conversion::toString(tr.index());
}


/**
 * Gets the address-writing move of a move which is a trigger or operand
 * to a memory operation.
 *
 * If none found, return null
 *
 * @param mn moveNode whose address write move is being searched.
 * @return MoveNode which writes address to a mem operation or NULL.
 */
MoveNode* DataDependenceGraphBuilder::addressMove(const MoveNode& mn) {
    if (mn.isDestinationOperation()) {
        return addressOperandMove(mn.destinationOperation());
    }
    return NULL;
}

/**
 * Gets the address-writing move of a ProgramOperation which is a memory
 * operation.
 *
 * If none found, return null
 *
 * @param po programOperation whose address write move is being searched.
 * @return MoveNode which writes address to a mem operation or NULL.
 */

MoveNode* DataDependenceGraphBuilder::addressOperandMove(
    ProgramOperation&po) {
    std::string opName = StringTools::stringToUpper(po.operation().name());
    if (opName == "STW" || opName == "STH" || opName == "STHU" || 
        opName == "STQ" || opName == "STHU" || opName == "STQU" ||
        opName == "LDW" || opName == "LDH" || opName == "LDHU" ||
        opName == "LDQ" || opName == "LDQU") {
        MoveNodeSet& mns = po.inputNode(1);
        if (mns.count() > 1) {
            return NULL;
        } else {
            return &mns.at(0);
        }
    }
    return NULL;
}


/**
 * Compares a memory op against one previous memory ops and
 * creates dependence if may alias.
 *
 * @param prev Previous Memory write movenode
 * @param mn Current memory write movenode
 * @param pseudo create pseudo flag in the DD edge.
 * @return true if true alias.
 */
bool
DataDependenceGraphBuilder::checkAndCreateMemDep(
    MNData2 prev, MNData2 mnd, DataDependenceEdge::DependenceType depType) {
        
    // TODO: this should be done for addresses always.
    MemoryAliasAnalyzer::AliasingResult aliasResult = 
        MemoryAliasAnalyzer::ALIAS_UNKNOWN;

    // only for true stores and loads
    if (!prev.pseudo_ && !mnd.pseudo_) {
        MoveNode* currentAddress = addressMove(*mnd.mn_);
        MoveNode* prevAddress = addressMove(*prev.mn_);
        
        if (currentAddress != NULL && prevAddress != NULL) {
            aliasResult = analyzeMemoryAlias(prevAddress, currentAddress);
        }
    }
        
    if (aliasResult != MemoryAliasAnalyzer::ALIAS_FALSE) {
        bool trueAlias = (aliasResult == MemoryAliasAnalyzer::ALIAS_TRUE);
        ProgramOperation& prevPo = prev.mn_->destinationOperation();
        for (int i = 0; i < prevPo.inputMoveCount(); i++) {
            DataDependenceEdge* dde2 =
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_MEMORY, depType, false, 
                    trueAlias, prev.pseudo_, mnd.pseudo_);
            currentDDG_->connectOrDeleteEdge(
                prevPo.inputMove(i), *mnd.mn_, dde2);
        }
        return trueAlias;
    }
    return false;
}

/**
 * Checks memory write against uses and defs in incoming basic blocks.
 * Creates the needed dependence edges.
 *
 * @param mnd MNData2 of movenode being processed.
*/
void
DataDependenceGraphBuilder::updateMemWrite(MNData2 mnd) {

    for (RegisterUseSet::iterator iter = currentData_->memDefReaches_.begin();
         iter != currentData_->memDefReaches_.end(); iter++) {
        checkAndCreateMemDep(*iter, mnd, DataDependenceEdge::DEP_WAW);
    }

    for (RegisterUseSet::iterator iter = currentData_->memUseReaches_.begin();
         iter != currentData_->memUseReaches_.end(); iter++) {
        checkAndCreateMemDep(*iter, mnd, DataDependenceEdge::DEP_WAR);
    }
}

/**
 * Updates memory operation bookkeeping and creates WaR and WaW 
 * memory dependencies.
 *
 * @param moveNode MNData2 related to Move whose memory write to are
 * processing.
 */
void
DataDependenceGraphBuilder::processMemWrite(
    MNData2 mnd) {
    
    // no kills to this one.
    if (currentData_->memKill_.mn_ == NULL) {
        // is this a kill?
        if (mnd.mn_->move().isUnconditional() &&
            !addressTraceable(addressMove(*mnd.mn_))) {
            currentData_->memKill_ = mnd;
        }
        // may have incoming WaW's / WaRs to this
        currentData_->memFirstDefines_.insert(mnd);
        updateMemWrite(mnd);
    }
    // create WaW to another in own bb
    for (RegisterUseSet::iterator iter = currentData_->memDefines_.begin();
         iter != currentData_->memDefines_.end();) {
        if (checkAndCreateMemDep(*iter, mnd, DataDependenceEdge::DEP_WAW)
            && mnd.mn_->move().isUnconditional()) {
            // remove current element and update iterator to next.
            currentData_->memDefines_.erase(iter++);
        } else { // just take next from the set
            iter++;
        }
    }

    // create WaR to reads in same bb
    for (RegisterUseSet::iterator iter = currentData_->memLastUses_.begin();
         iter != currentData_->memLastUses_.end();) {
        if (checkAndCreateMemDep(*iter, mnd, DataDependenceEdge::DEP_WAR)
            && mnd.mn_->move().isUnconditional()) {
            // remove current element and update iterator to next.
            currentData_->memLastUses_.erase(iter++);
        } else { // just take next from the set
            iter++;
        }
    }

    // does this kill previous deps?
    if (mnd.mn_->move().isUnconditional() &&
        !addressTraceable(addressMove(*mnd.mn_))) {
        currentData_->memLastKill_ = mnd;
        currentData_->memDefines_.clear();
        currentData_->memLastUses_.clear();
    }

    currentData_->memDefines_.insert(mnd);
}


/**
 * Handles ProgramOperation creation for a triggering mvoe.
 */
void DataDependenceGraphBuilder::processTriggerPO(
    MoveNode& moveNode, Operation &dop) throw (IllegalProgram) {
    for (POLIter poli = currentData_->destPending_.begin();
         poli != currentData_->destPending_.end();
         poli++) {
        ProgramOperation* po = *poli;

        if (&dop == &po->operation()) {
            po->addInputNode(moveNode);
            moveNode.setDestinationOperation(*po);
            if (po->isReady()) {
                currentData_->destPending_.erase(poli);
                if (dop.numberOfOutputs()) {
                    currentData_->readPending_.push_back(po);
                } else {
                    currentDDG_->addProgramOperation(po);
                }
            } else {
                throw IllegalProgram(
                    __FILE__, __LINE__, __func__, "Trigger too early");
            }
            return;
        }
    }
    // only one triggering input?
    if (dop.numberOfInputs() == 1) {
        ProgramOperation *po = new ProgramOperation(dop);
        moveNode.setDestinationOperation(*po);
        po->addInputNode(moveNode);
        if (dop.numberOfOutputs()) {
            currentData_->readPending_.push_back(po);
        } else {
            currentDDG_->addProgramOperation(po);
        }
    } else { // trigger came too early
        const std::string moveDisasm = 
            POMDisassembler::disassemble(moveNode.move());
        throw IllegalProgram(
            __FILE__,__LINE__, __func__, 
            std::string("Trigger without operand in ") + moveDisasm);
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
 * Calls createTriggerDependencies(moveNode, dop) to create 
 * the dependence egdes of the operation.
 * Checks if operation is call and if it is, processes the call-related
 * register dependencies.
 * 
 * @param moveNode mnData related to a move which triggers an operation
 * @param dop Operation being triggered
 */
void
DataDependenceGraphBuilder::processTriggerMemoryAndFUStates(
    MoveNode& moveNode, Operation &dop) {

    createTriggerDependencies(moveNode, dop);

    if (moveNode.move().isCall()) {
        // no guard, is not ra, is pseudo.
        MNData2 mnd2(moveNode, false, false, true);
        processMemWrite(mnd2);
    }
}

/** 
 * Processes a return from a function.
 * Creates pseudo-read-deps to SP and RV registers.
 * 
 * @param moveNode moveNode containg the return move.
 */
void DataDependenceGraphBuilder::processReturn(MoveNode& moveNode) {
    std::string sp = specialRegisters_[REG_SP];
    if (sp != "") {
        processRegUse(MNData2(moveNode,false,false,true),sp);
    }
    std::string rv = specialRegisters_[REG_RV];
    if (rv != "") {
        processRegUse(MNData2(moveNode,false,false,true),rv);
    }

    // return is also considered as read of RV high(for 64-bit RV's)
    TCEString rvh = specialRegisters_[REG_RV_HIGH];
    if (rvh != "") {
        processRegUse(MNData2(moveNode,false,false,true),rvh);
    }
}

/**
 * Processes a call of a function.
 * Pseudo-reads from parameter registers and SP, writes to RV and RA.
 *
 * @param mn MoveNode containg the function call move.
 */
void DataDependenceGraphBuilder::processCall(MoveNode& mn) {

    // calls mess up RA. But immediately, not after delay slots?
    processRegWrite(MNData2(mn,false,true, false), RA_NAME);

    // sp and rv
    MNData2 mnd2(mn, false,false, true);
    std::string sp = specialRegisters_[REG_SP];
    if (sp != "") {
       processRegUse(mnd2,sp);
    } 

    std::string rv = specialRegisters_[REG_RV];
    if (rv != "") {
        processRegUse(mnd2,rv);
        processRegWrite(mnd2,rv);
    }

    // call is considered as write of RV high (64-bit return values)
    TCEString rvh = specialRegisters_[REG_RV_HIGH];
    if (rvh != "") {
        processRegWrite(mnd2, rvh);
    }

    // params
    for (int i = 0; i < 4;i++) {
        std::string paramReg = specialRegisters_[REG_IPARAM+i];
        if(paramReg != "") {
            processRegUse(mnd2, paramReg);
        }
    }
}

/**
 * Checks memory read against uses and defs in incoming basic blocks.
 * Creates the needed dependence edges.
 *
 * @param mnd MNData2 of movenode being processed.
*/
void DataDependenceGraphBuilder::updateMemUse(MNData2 mnd) {
    
    for (RegisterUseSet::iterator iter = currentData_->memDefReaches_.begin();
         iter != currentData_->memDefReaches_.end(); iter++) {
        checkAndCreateMemDep(*iter, mnd,DataDependenceEdge::DEP_RAW);
    }
}

/**
 * Processes a memory read.
 *
 * Creates dependence edges and updates bookkeeping.
 * 
 * @param mnd MNData2 of MoveNode being processed.
 */
void DataDependenceGraphBuilder::processMemUse(MNData2 mnd) {
    
    // no kills to this one.
    if (currentData_->memKill_.mn_ == NULL) {
        currentData_->memFirstUses_.insert(mnd);
        // so create deps from previous BB's
        updateMemUse(mnd);
    }
    
    // create deps from writes in this BB.
    for (RegisterUseSet::iterator iter = currentData_->memDefines_.begin();
         iter != currentData_->memDefines_.end(); iter++) {
        checkAndCreateMemDep(*iter, mnd,DataDependenceEdge::DEP_RAW);
    }
    // update bookkeeping.
    currentData_->memLastUses_.insert(mnd);
}

/**
 * Analyzes operation of a trigger write.
 * If memory read, callls processMemRead to manage memory read dependencies.
 * Manages FU State dependencies between operations.
 *
 * @param moveNode mnData related to a move which triggers an operation
 * @param dop Operation being triggered
 *
 */
void DataDependenceGraphBuilder::createTriggerDependencies(
    MoveNode& moveNode, Operation& dop) {

    // TODO: move memory write handling here.
    // currently causes a slight problem with subgraphs, 
    // so not yet moved here.


    if (dop.writesMemory()) {
        processMemWrite(MNData2(moveNode));
    }
    else {
        if (dop.readsMemory()) {
            processMemUse(MNData2(moveNode));
        }
    }
    // new code
    createSideEffectEdges(currentData_->fuDeps_, moveNode, dop);
    createSideEffectEdges(currentData_->fuDepReaches_, moveNode, dop);

    if (dop.hasSideEffects() || dop.affectsCount() != 0 || 
        dop.affectedByCount() != 0) {

        // remove old same op from bookkeeping. 
        // this should prevent exponential explosion or edge count.
        if (dop.hasSideEffects() && moveNode.move().isUnconditional()) {
            for (RegisterUseSet::iterator iter = currentData_->fuDeps_.begin();
                 iter != currentData_->fuDeps_.end(); iter++) {
                
                Operation& o = iter->mn_->destinationOperation().operation();
                if (&o == &dop) {
                    currentData_->fuDeps_.erase(iter);
                    break;
                }
            }
        }
        // add the new one to bookkeeping
        currentData_->fuDeps_.insert(MNData2(moveNode));
    }
}

/**
 * Analyzes operand write.
 * 
 * Part of ProgramOperation creation.
 *
 * @param moveNode mnData related to a move which writes a parameter.
 * @param dop Operation whose parameter is being written.
 */
void
DataDependenceGraphBuilder::processOperand(
    MoveNode& moveNode, Operation &dop) {

    for (POLIter poli = currentData_->destPending_.begin();
         poli != currentData_->destPending_.end(); poli++) {

        ProgramOperation& po = **poli;

        if (&dop == &po.operation()) {
            // add current move to operation

            // check that no other move has same input num
            // not be needed for universalmachine code?

            // does not yet exist

            // TBD: check for inconsistent input code
            po.addInputNode(moveNode);
            moveNode.setDestinationOperation(po);
            return;
        }
    }
    // create new ProgramOperation
    ProgramOperation* po = new ProgramOperation(dop);
    moveNode.setDestinationOperation(*po);
    po->addInputNode(moveNode);
    currentData_->destPending_.push_back(po);
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
void DataDependenceGraphBuilder::createSideEffectEdges(
    RegisterUseSet& prevMoves, const MoveNode& mn, Operation& dop) {
    
    const int affectCount = dop.affectedByCount() + dop.affectsCount();
    if (affectCount != 0 || dop.hasSideEffects()) {
        for (RegisterUseSet::iterator i = prevMoves.begin();
             i != prevMoves.end(); i++) {
            Operation& o = i->mn_->destinationOperation().operation();
            
            // mem writes are handled by memory deps so exclude here
            if ((&dop == &o && o.hasSideEffects()) || 
                dop.dependsOn(o) || o.dependsOn(dop)) {
                
                DataDependenceEdge* dde =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_FUSTATE,
                        DataDependenceEdge::DEP_UNKNOWN);
                currentDDG_->connectOrDeleteEdge(
                    *(i->mn_), mn, dde);
            }
        }
    }
}


/**
 * Delegates call to all registered memory address alias analyzers.
 *
 * Returns the first non-unknown result
 *
 * @return Whether the memory accesses in the given moves alias.
 */
MemoryAliasAnalyzer::AliasingResult
DataDependenceGraphBuilder::analyzeMemoryAlias(
    const MoveNode* mn1, const MoveNode* mn2) {

    MemoryAliasAnalyzer::AliasingResult result = 
        MemoryAliasAnalyzer::ALIAS_UNKNOWN;

    if (mn1 != NULL && mn2 != NULL) {
        for (unsigned int i = 0; i < aliasAnalyzers_.size(); i++) {
            result = aliasAnalyzers_[i]->analyze(*currentDDG_, *mn1, *mn2);
            if (result != MemoryAliasAnalyzer::ALIAS_UNKNOWN) {
                return result;
            }
        }
    }
    return result;
}

void
DataDependenceGraphBuilder::getStaticRegisters(
    TTAProgram::Program& prog, 
    std::map<int,std::string>& registers) {
    for (int i = 0; i < prog.procedureCount(); i++) {
        getStaticRegisters(prog.procedure(i), registers);
    }
}

void
DataDependenceGraphBuilder::getStaticRegisters(
    TTAProgram::CodeSnippet& cs, 
    std::map<int,std::string>& registers) {
    for (int i = 0; i < cs.instructionCount(); i++) {
        Instruction& ins = cs.instructionAtIndex(i);
        getStaticRegisters(ins, registers);
    }
}

void
DataDependenceGraphBuilder::getStaticRegisters(
    ControlFlowGraph& cfg,
    std::map<int,std::string>& registers) {
    for (int i = 0; i < cfg.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg.node(i);
        if (bbn.isNormalBB()) {
            getStaticRegisters(bbn.basicBlock(), registers);
        }
    }
}

void
DataDependenceGraphBuilder::getStaticRegisters(
    TTAProgram::Instruction& ins, 
    std::map<int,std::string>& registers) {
    try {
        for (int i = 0; i < ins.moveCount(); i++) {
            Move& move = ins.move(i);
            for (int j = 0; j < move.annotationCount(); j++) {
                ProgramAnnotation anno = move.annotation(j);
                switch (anno.id()) {
                case ProgramAnnotation::ANN_REGISTER_RV_READ: {
                    TerminalRegister& tr = 
                        dynamic_cast<TerminalRegister&>(move.source());
                    registers[REG_RV] = trName(tr);
                    break;
                }
                case ProgramAnnotation::ANN_REGISTER_RV_SAVE: {
                    TerminalRegister& tr = 
                        dynamic_cast<TerminalRegister&>(move.destination());
                    registers[REG_RV] = trName(tr);
                    break;
                }
                case ProgramAnnotation::ANN_REGISTER_SP_READ: {
                    TerminalRegister& tr = 
                        dynamic_cast<TerminalRegister&>(move.source());
                    registers[REG_SP] = trName(tr);
                    break;
                }
                case ProgramAnnotation::ANN_REGISTER_SP_SAVE: {
                    TerminalRegister& tr = 
                        dynamic_cast<TerminalRegister&>(move.destination());
                    registers[REG_SP] = trName(tr);
                    break;
                }
                case ProgramAnnotation::ANN_REGISTER_IPARAM_READ: {
                    TerminalRegister& tr = 
                        dynamic_cast<TerminalRegister&>(move.source());
                    registers[
                        REG_IPARAM+Conversion::toInt(anno.stringValue())] = 
                        trName(tr);
                    break;
                }
                case ProgramAnnotation::ANN_REGISTER_IPARAM_SAVE: {
                    TerminalRegister& tr = 
                        dynamic_cast<TerminalRegister&>(move.destination());
                    registers[
                        REG_IPARAM+Conversion::toInt(anno.stringValue())] = 
                        trName(tr);
                }
                default:
                    //TODO: frame pointer, not yet implemented
                    break;
                }
            }
        }
    } catch (std::bad_cast& e) {
        throw IllegalProgram(
            __FILE__,__LINE__, __func__, "Illegal annotation");
    }
}

/**
 * Initializes the static register table from register from
 * UniversalMachine. Needed for analysis of data dependencies
 * of parameter registers, SP, RV etc. 
 *
 * @param um UniversalMachine
 * @param registers map where to store those registers.
 */
void
DataDependenceGraphBuilder::getStaticRegisters(
    const UniversalMachine& um, std::map<int,std::string>& registers) {
    RegisterFile& rf = um.integerRegisterFile();
    
    for (int i = 0; i < 6; i++) {
        TerminalRegister tr(*rf.port(0), i);
        registers[i] = trName(tr);
    }
}

/**
 * Can some analyzer can say something about this address?
 *
 * @param mn Movenode containing memory address write.
 * @return true if some alias analyzer knows something about the address.
 */
bool 
DataDependenceGraphBuilder::addressTraceable(const MoveNode* mn) {

    if (mn == NULL) {
        return false;
    }

    for (unsigned int i = 0; i < aliasAnalyzers_.size(); i++) {
        if (aliasAnalyzers_.at(i)->addressTraceable(*mn)) {
            return true;
        }
    }
    return false;
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
        BBData* bbd = new BBData(bbn);
        bbData_[&bbn] = bbd;
        // in the beginning all are unreached
        if (bbn.isNormalBB()) {
            blocksByState_[BB_UNREACHED].push_back(bbd);
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
    BBNodeSet firstBBs = cfg_->successors(cfg_->entryNode());
    assert(firstBBs.size() == 1);
    BasicBlockNode* firstBB = *firstBBs.begin();
    changeState(*(bbData_[firstBB]), BB_QUEUED);
    return firstBB;
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
    currentData_ = bbData_[firstBB];

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

//////////////////////////////////////////////////////////////////////////////
// Static data members
//////////////////////////////////////////////////////////////////////////////

/**
 * Internal constant for the name of the return address port 
 */
const std::string DataDependenceGraphBuilder::RA_NAME = "RA";

//////////////////////////////////////////////////////////////////////////////
// BBData
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor
 */
DataDependenceGraphBuilder::BBData::BBData(BasicBlockNode& bb) :
    state_(BB_UNREACHED), constructed_(false), bblock_(&bb) , memLastKill_(),
    memKill_() {
}

/**
 * Destructor. 
 */
DataDependenceGraphBuilder::BBData::~BBData() {
//    clear();
}
