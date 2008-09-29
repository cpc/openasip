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
 * @file LinearScanAllocator.cc
 *
 * Implmentation of LinearScanAllocator class
 *
 * Linear Scan register allocator.
 * This version Spills variables after allocation, not before.
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <set>
#include <list>

#include <boost/graph/strong_components.hpp>

#include "AssocTools.hh"
#include "StackFrameData.hh"
#include "Terminal.hh"
#include "TerminalRegister.hh"
#include "Procedure.hh"
#include "Instruction.hh" 
#include "Move.hh"
#include "RegisterFile.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "ControlUnit.hh"
#include "Program.hh"
#include "UniversalMachine.hh"
#include "POMDisassembler.hh"

#include "DataDependenceGraph.hh"
#include "DataDependenceEdge.hh"
#include "DataDependenceGraphBuilder.hh"
#include "ControlFlowGraph.hh"
#include "ProgramOperation.hh"
#include "Operation.hh"
#include "MoveNodeSet.hh"

#include "StackFrameCodeCreator.hh"
#include "StackFrameData.hh"
#include "StackCodeRemover.hh"

#include "RegisterMap.hh"

#include "LinearScanAllocatorCore.hh"
#include "ProcedurePass.hh"

using namespace TTAProgram;
using namespace TTAMachine;

using std::string;
using std::set;

LinearScanAllocatorCore::LinearScanAllocatorCore() :
    ddg_(NULL), cfg_(NULL), regMap_(NULL), stackFrameCodeCreator_(NULL),
    stackFrameData_(NULL) {}

LinearScanAllocatorCore::~LinearScanAllocatorCore() {
    if(ddg_ != NULL) {
        delete ddg_; 
        ddg_ = NULL;
    }
    if(cfg_ != NULL) {
        delete cfg_; 
        cfg_ = NULL;
    }
    if(regMap_ != NULL) {
        delete regMap_; 
        regMap_ = NULL;
    }

    if (stackFrameCodeCreator_ != NULL) {
        delete stackFrameCodeCreator_;
        stackFrameCodeCreator_ = NULL;
    }
}
    
void 
LinearScanAllocatorCore::allocateProcedure(TTAProgram::Procedure& procedure) {
    
    initProcedure(procedure);
    
    collectVariables();
    calculateLiveRanges();
    sortLiveRanges(); // sort according to birth and death order
    allocateStatic();
    allocate();
    
    selectSpills();
    
    // allocate space on stack    
    stackFrameData_->setSpilledIntegerCount(intSpillCount_);
    stackFrameData_->setSpilledFloatCount(floatSpillCount_);

    updateSavedRegs();
    
    // updates moves to contain new ??
    updateMoves();

    // updates sp-related addresses
    updateStackReferences();
    
    
    // create context-save-code
    createCSCode();
    
}

void
LinearScanAllocatorCore::initialize(
    const Machine& machine, Program& program, InterPassData& ipd) {
 
    if(regMap_ != NULL ) {
        delete regMap_;
        regMap_ = NULL;
    }

    regMap_ = new RegisterMap(machine, ipd);

    if(stackFrameCodeCreator_ != NULL ) {
        delete stackFrameCodeCreator_;
        stackFrameCodeCreator_ = NULL;
    }
    
    // assume that is uvMachine
    um_ = dynamic_cast<const UniversalMachine*>
        (&(program.targetProcessor()));
    
    assert(um_!=NULL);

    // statically allocate SP
    stackFrameCodeCreator_ = 
        new StackFrameCodeCreator(machine, *um_, regMap_->terminal(1));

    calleeSaveGprStartScheduled_ = regMap_->gprCount()/2;
    if (calleeSaveGprStartScheduled_ < 3) {
        throw IllegalMachine(__FILE__,__LINE__,"","Too few integer regs");
    }
    if (calleeSaveGprStartScheduled_ < 6) {
        calleeSaveGprStartScheduled_ = 6;
    }
    
    if (regMap_->fprCount() > 10) {
        calleeSaveFprStartScheduled_ = regMap_->fprCount() / 2;
    } else {
        if (regMap_->fprCount() >= 7 ) {
            calleeSaveFprStartScheduled_ = 5;
        } else {
            // we really cannot run FP code without fp param regs
            calleeSaveFprStartScheduled_ = 0;
        }
    }
    calleeSaveFprStartScheduled_ +=FPR_OFFSET;
}

/**
 * Initializes internal data structures for specified procedure
 */
void LinearScanAllocatorCore::initProcedure(Procedure& proc) {

    if( stackFrameData_ ) {
        delete stackFrameData_;
        stackFrameData_ = NULL;
    }

    stackFrameData_ = scRemover_.removeStackFrameCode(proc);
    stackFrameCodeCreator_->setParameters(*stackFrameData_);

    // delete DDG first as deleting CFG deletes stuff DDG refers to
    if( ddg_ != NULL ) {
        delete ddg_;
        ddg_ = NULL;
    }

    if( cfg_ != NULL ) {
        delete cfg_;
        cfg_ = NULL;
    }
    cfg_ = new ControlFlowGraph(proc);

    DataDependenceGraphBuilder ddgb;
    ddg_ = ddgb.build(*cfg_,um_);

    variablesByBirth_.clear();
    variablesByDeath_.clear(); 

    AssocTools::deleteAllValues(variables_);
    AssocTools::deleteAllValues(regAllocations_);

    regValuesByAllocatedIndex_.clear();
    regAllocatedIndecesByValue_.clear();

    allocatedUseCount_.clear();
    regNeedsContextSave_.clear();

    proc_ = &proc; 

    noSaveGprCount_ = GLOBAL_GPR_COUNT;
    noSaveFprCount_ = GLOBAL_FPR_COUNT;
    
    calleeSaveGprCount_ = 0;
    calleeSaveFprCount_ = 0;
}

/** 
 * Runs linear scan for one procedure.
 * Maps pre-allocated variables into linear-scan-allocated indeces.
 * These indeces are then mapper into registers later.
 */
void 
LinearScanAllocatorCore::allocate() {
    typedef std::vector<Variable*>::iterator vari;

    vari bIter = variablesByBirth_.begin();
    vari dIter = variablesByDeath_.begin();
    while(bIter != variablesByBirth_.end() &&
          dIter != variablesByDeath_.end()) {
        // beginning of a new variable
        // TODO: end of list
        if( dIter == variablesByDeath_.end() || 
            (*bIter)->birthTime() < (*dIter)->deathTime() ) {
            if(isReservedReg((*bIter)->originalIndex())) {
//                cerr << "ERROR: is reserved shen should reserve" << endl;
            } else {
                if ( (*bIter)->type() == Variable::VAR_INT ) {
                    unsigned int index = reserveGPR((*bIter)->originalIndex());
                    Variable* v = *bIter;
                    v->setRegister(index);
                    if( index >= calleeSaveGprStartScheduled_  ) {
//                        regNeedsContextSave_.insert(index);
                    }
                    allocatedUseCount_[index]+=(v->readCount()+
                                                v->writeCount());
                } else {
                    if ( (*bIter)->type() == Variable::VAR_FP ) {
                        unsigned int index = reserveFPR((*bIter)->originalIndex());
                        Variable* v = *bIter;
                        v->setRegister(index);
                        if( index >= calleeSaveGprStartScheduled_  ) {
//                            regNeedsContextSave_.insert(index);
                        }
                        allocatedUseCount_[index]+=(v->readCount()+
                                                    v->writeCount());
                    } else {
                        throw Exception(
                            __FILE__,__LINE__,__func__,
                            "Other variable types not yet supported");
                    }
                }
            }
            bIter++;
        } else {
            // end of a variable
            unsigned int index = (*dIter)->originalIndex();
            if((index >= GLOBAL_GPR_COUNT && index < FPR_OFFSET) ||
               (index >= GLOBAL_FPR_COUNT+ FPR_OFFSET)){
                assert(isReservedReg(index));

                freeReg(index);

            } else {
                // release of static reg not!
            }
        dIter++;
        }
    }
    
    while( dIter != variablesByDeath_.end()) { 
        if ( (*dIter)->type() == Variable::VAR_INT||
                 (*dIter)->type() == Variable::VAR_FP ) {
            if(isReservedReg((*dIter)->originalIndex())) {
                freeReg((*dIter)->originalIndex());
            }
        } else {
            throw Exception(
                __FILE__,__LINE__,__func__,
                "Other variable types not yet supported");
        }
        dIter++;
    }
}

/**
 * Allocates global variables( Stack pointer, function parameters ) 
 */
void
LinearScanAllocatorCore::allocateStatic() {
    for (unsigned int i = 0; i < GLOBAL_GPR_COUNT; i++) {
        regValuesByAllocatedIndex_[i] = i;
        regAllocatedIndecesByValue_[i] = i;
        if(!AssocTools::containsKey(variables_,i)) {
            variables_[i] = 
                new Variable(i, Variable::VAR_INT, *ddg_, *cfg_);
        }
            allocatedUseCount_[i] = 1;        
    }
    
    // Only support FP if we have enough FP regs available
    if (regMap_->fprCount() >= 7) {
        for (unsigned int i = 0; i < GLOBAL_FPR_COUNT; i++) {
            regValuesByAllocatedIndex_[i+FPR_OFFSET] = i+FPR_OFFSET;
            regAllocatedIndecesByValue_[i+FPR_OFFSET] = i+FPR_OFFSET;
            
            if(!AssocTools::containsKey(variables_,i+FPR_OFFSET)) {
                variables_[i+FPR_OFFSET] = 
                    new Variable(
                        i+FPR_OFFSET, Variable::VAR_FP, *ddg_, *cfg_);
            }
            allocatedUseCount_[i+FPR_OFFSET] = 1;
        }
    }
}
    
/**
 * Releases index from bookkeeping.
 * @param var Variable index of register being freed.
 */
void 
LinearScanAllocatorCore::freeReg( unsigned int  var) {
    int index = regAllocatedIndecesByValue_[var];
//    cout << " F:" << var << "<->" << index << " ";
    std::map<int,int>::iterator iter = regValuesByAllocatedIndex_.find(index);
    regValuesByAllocatedIndex_.erase(iter);
}

/**
 * Reserves an allocated index for an integer variable
 * @param var Variable being allocated
 * @return index of allocated register
 */
unsigned int 
LinearScanAllocatorCore::reserveGPR( unsigned int var) {
    // reserve 7-10 for temp registers that do not need saving
    unsigned int i = (var >= CALLEE_SAVE_GPR_START_SEQUENTIAL) ? 
        calleeSaveGprStartScheduled_ : GLOBAL_GPR_COUNT;
    
    while(true) {
        if( !AssocTools::containsKey( regValuesByAllocatedIndex_, i)) {
            regAllocatedIndecesByValue_[var] = i;
            regValuesByAllocatedIndex_[i] = var;
//            cout << " R:" << var << "<->" << i << " ";
            if( i >= GLOBAL_GPR_COUNT ) {
                if( i-GLOBAL_GPR_COUNT >= calleeSaveGprCount_ ) {
                    calleeSaveGprCount_++; 
                } 
            } else {
                if( i >= noSaveGprCount_ ) {
                    noSaveGprCount_++;
                }
            }
            return i;
        }
        i++;
    }
}

/**
 * Reserves an allocated index for an floating-point variable
 * @param var Variable being allocated
 * @return index of allocated register
 */
unsigned int 
LinearScanAllocatorCore::reserveFPR( unsigned int var) {
    if (regMap_->fprCount() < 6 ) {
        std::string msg = "Machine does not have enough floating-point regs"
            " but the code uses floating point. Should have at least 6 FPRs";
        
        throw IllegalProgram(__FILE__,__LINE__,__func__, msg);
    }

    // reserve 7-10 for temp registers that do not need saving
    unsigned int i = (var >= CALLEE_SAVE_FPR_START_SEQUENTIAL) ? 
        calleeSaveFprStartScheduled_ : GLOBAL_FPR_COUNT+FPR_OFFSET;
    
    while(true) {
        if( !AssocTools::containsKey( regValuesByAllocatedIndex_, i)) {
            regAllocatedIndecesByValue_[var] = i;
            regValuesByAllocatedIndex_[i] = var;
//            cout << " R:" << var << "<->" << i << " ";
            if( i >= GLOBAL_FPR_COUNT+FPR_OFFSET ) {
                if( i-GLOBAL_GPR_COUNT-FPR_OFFSET >= calleeSaveFprCount_ ) {
                    calleeSaveFprCount_++; 
                } 
            } else {
                if( i >= noSaveFprCount_ ) {
                    noSaveFprCount_++;
                }
            }
            return i;
        }
        i++;
    }
}

/** 
 * Returns whether the variable already has register index allocated for it
 */
bool 
LinearScanAllocatorCore::isReservedReg( unsigned int var) {
    return AssocTools::containsKey( regAllocatedIndecesByValue_, var);
}

/**
 * Sorts all variables by their birth and death ranges
 */
void 
LinearScanAllocatorCore::sortLiveRanges() {
    for( map<int, Variable*>::iterator iter =
             variables_.begin();
         iter != variables_.end(); iter++) {
        
        variablesByBirth_.push_back(iter->second);
        variablesByDeath_.push_back(iter->second);
    }

    std::sort(variablesByBirth_.begin(), variablesByBirth_.end(),
              VariableBirthComparator());
    std::sort (variablesByDeath_.begin(), variablesByDeath_.end(),
               VariableDeathComparator());

}

/**
 * Calculates live ranges for all variables
 */
void
LinearScanAllocatorCore::calculateLiveRanges() {

    typedef std::map<int,Variable*>::iterator vari;
    for (vari iter = variables_.begin(); 
        iter != variables_.end(); iter++) {
        iter->second->calculateRange(*proc_);
    }
}

void 
LinearScanAllocatorCore::collectVariables() {
    // TODO: does not handle FPR's

    const int nodeCount = ddg_->nodeCount();

    for (int i = 0; i < nodeCount; i++) {
        MoveNode& mn = ddg_->node(i);

        DataDependenceGraph::EdgeSet outEdges = ddg_->outEdges(mn);
        for (DataDependenceGraph::EdgeSet::iterator edgeIter =
                 outEdges.begin(); edgeIter != outEdges.end(); edgeIter++) {
            DataDependenceEdge& e = **edgeIter;
            if( e.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
                e.dependenceType() == DataDependenceEdge::DEP_RAW ) {
                if (!e.guardUse() ) {
                    MoveNode& writeNode = mn;
                    if( writeNode.isMove()) {
                        Terminal &td = writeNode.move().destination();
                        if (td.isGPR()) {
                            TerminalRegister& tr = 
                                dynamic_cast<TerminalRegister&>(td);
                            unsigned int index = tr.index();
                            if( tr.registerFile().width() == 32 ) {
                                if( !AssocTools::containsKey(
                                        variables_, index)) {
                                    variables_[index] = 
                                        new Variable(index, Variable::VAR_INT,
                                                     *ddg_, *cfg_);
                                }
                            } else {
                                index+=FPR_OFFSET;
                                if( !AssocTools::containsKey(
                                        variables_, index)) {
                                    variables_[index] = 
                                        new Variable(index, Variable::VAR_FP,
                                                     *ddg_, *cfg_);
                                }
                            }
                            variables_[index]->addWrite(writeNode);
                        }
                    } 
                    MoveNode& readNode = ddg_->headNode(e);
                    Terminal& ts = readNode.move().source();
                    if(ts.isGPR()) {
                        
                        TerminalRegister& tr = 
                            dynamic_cast<TerminalRegister&>(ts);
                        unsigned int index = tr.index();
                        if( tr.registerFile().width() == 32 ) {
                            if( !AssocTools::containsKey(variables_, index)) {
                                variables_[index] = 
                                    new Variable(index, Variable::VAR_INT,
                                                 *ddg_, *cfg_);
                            }
                        } else {
                            index+=FPR_OFFSET;
                            if( !AssocTools::containsKey(variables_, index)) {
                                variables_[index] = 
                                    new Variable(index, Variable::VAR_FP,
                                                 *ddg_, *cfg_);
                            }
                            
                        }
                        variables_[index]->addRead(readNode);
                        variables_[index]->addEdge(e,writeNode,readNode);
                    } 
                }
            }
        }
    }
}

/**
 * Selects which variables are to be spilled to memory.
 * This works after the linear scan allocateion routine,
 * so it may spill multiple "Original variables" at once.
 */
void 
LinearScanAllocatorCore::selectSpills() {
    // TODO: FP support..

/*
    cout << "\tSelecting values to be spilled.." << endl;

    cout <<"\t\tuse counts: " ;

    for (std::map<unsigned int,unsigned int>::iterator iter = 
             allocatedUseCount_.begin();
        iter != allocatedUseCount_.end(); iter++ ) {
        cout << iter->first << ":" << iter->second << " ";
    }
*/

    intSpillCount_ = 0;
    floatSpillCount_ = 0;

    // TODO: BUG: this clears list so same cals used for temporaries

    unsigned int allocatedIndex = calleeSaveGprStartScheduled_;
    unsigned int maxAllocations = regMap_->gprCount();

    // leave space for spill temporaries
/*
    cout << "max allocations:" << maxAllocations << endl;
    cout << "callee save gpr count:" << calleeSaveGprCount_ << endl;
    cout << "callee save gpr count2:" << calleeSaveGprCount_ +
         calleeSaveGprStartScheduled_ << endl;
*/
    if( calleeSaveGprCount_+calleeSaveGprStartScheduled_
        > maxAllocations ) {

        // use caller-save as spill tmp reg?
        
        if( !AssocTools::containsKey(allocatedUseCount_,
                                     calleeSaveGprStartScheduled_-1)) {
            if( !AssocTools::containsKey(allocatedUseCount_,
                                         calleeSaveGprStartScheduled_-2)) {
//                cout << "\t\t\tboth tmp cals from caller save" << endl;
                intTmp2_ = &regMap_->terminal(calleeSaveGprStartScheduled_-2);
            } else {
//                cout << "\t\t\t1 tmp val from caller save" << endl;
                intTmp2_ = &regMap_->terminal(regMap_->gprCount()-1);
                maxAllocations-=1;
                regNeedsContextSave_.insert(maxAllocations);
            }
            intTmp1_ = &regMap_->terminal(calleeSaveGprStartScheduled_-1);
        } else {
//            cout << "\t\t\tno tmp vals from callee save" << endl;
            intTmp1_ = &regMap_->terminal(regMap_->gprCount()-2);
            intTmp2_ = &regMap_->terminal(regMap_->gprCount()-1);

            maxAllocations-=2;
            regNeedsContextSave_.insert(maxAllocations);
            regNeedsContextSave_.insert(maxAllocations+1);
        }
//        cout << "seems to need spilling" << endl;
    } else {
//        cout << "should not spill" << endl;
    }
//    cout << "max allocations:" << maxAllocations << endl;

    // allocate callee-save without spilling
    while(true) {
        std::map<unsigned int, unsigned int>::iterator iter = 
            allocatedUseCount_.begin();

        if( iter != allocatedUseCount_.end() 
            && iter->first < calleeSaveGprStartScheduled_ ) {
//            cout << " CSR: " << iter->first;
            regAllocations_[iter->first] = 
                new RegisterVariable(regMap_->terminal(iter->first));
            allocatedUseCount_.erase(iter);
        } else {
            break;
        }
    }

    while (allocatedUseCount_.size()) {
        std::map<unsigned int,unsigned int>::iterator mostUsesIter = 
            allocatedUseCount_.begin();
        unsigned int mostUsesCount = 0;
        if (mostUsesIter->first >= (int)FPR_OFFSET)
            break;

        if( allocatedIndex < maxAllocations ) {
            for (std::map<unsigned int, unsigned int>::iterator iter = 
                     allocatedUseCount_.begin();
                 iter != allocatedUseCount_.end() && 
                     iter->first < FPR_OFFSET; iter++ ) {
                if( iter->second > mostUsesCount ) {
                    mostUsesIter = iter;
                    mostUsesCount = iter->second;
                }
            }

            regAllocations_[mostUsesIter->first] = 
                new RegisterVariable(
                    regMap_->terminal(allocatedIndex));

            regNeedsContextSave_.insert(allocatedIndex++);
            allocatedUseCount_.erase(mostUsesIter);
        }
        else {
//            cout << " Spill:" << mostUsesIter->first;
            regAllocations_[mostUsesIter->first] = 
            new SpilledVariable(*stackFrameCodeCreator_,intSpillCount_++);
            allocatedUseCount_.erase(mostUsesIter);
        }
    }

    allocatedIndex = FPR_OFFSET;
    maxAllocations = regMap_->fprCount()+FPR_OFFSET;
    if( calleeSaveFprCount_+calleeSaveFprStartScheduled_
        > maxAllocations ) {
        maxAllocations-=2;
        regNeedsContextSave_.insert(maxAllocations);
        regNeedsContextSave_.insert(maxAllocations+1);
//        cout << "\nFP vals seems to need spilling" << endl;
    } else {
//        cout << "\nshould not spill FP" << endl;
    }
//    cout << "max FP allocations:" << maxAllocations << endl;

    // allocate static FP vals
    while(true) {
        std::map<unsigned int, unsigned int>::iterator iter = 
            allocatedUseCount_.begin();

        if( iter != allocatedUseCount_.end() 
            && iter->first < calleeSaveFprStartScheduled_ ) {
//            cout << " CSR: " << iter->first;
            regAllocations_[iter->first] = 
                new RegisterVariable(
                    regMap_->terminal(iter->first));
            allocatedUseCount_.erase(iter);
        } else {
            break;
        }
    }
    
    // select spills of rest FP vals
    while(allocatedUseCount_.size()) {
        std::map<unsigned int,unsigned int>::iterator mostUsesIter = 
            allocatedUseCount_.begin();
        unsigned int mostUsesCount = 0;
        
        if( allocatedIndex < maxAllocations ) {
            for (std::map<unsigned int, unsigned int>::iterator iter = 
                     allocatedUseCount_.begin();
                 iter != allocatedUseCount_.end() ; iter++ ) {
                if( iter->second > mostUsesCount ) {
                    mostUsesIter = iter;
                    mostUsesCount = iter->second;
                }
            }
/*
            cout << "\tR:" << mostUsesIter->first << ":"
                 << mostUsesCount << " A:" << allocatedIndex << endl;
*/          
            regAllocations_[mostUsesIter->first] = 
                new RegisterVariable(
                    regMap_->terminal(allocatedIndex));
/*
            const TerminalRegister& tr = regMap_->terminal(allocatedIndex);

            cout << " " << tr.registerFile().name() 
                 << "." << tr.index() << endl;
*/
            regNeedsContextSave_.insert(allocatedIndex++);
            allocatedUseCount_.erase(mostUsesIter);
        }
        else {
//            cout << " Spill:" << mostUsesIter->first;
            regAllocations_[mostUsesIter->first] = 
                new SpilledVariable(*stackFrameCodeCreator_,
                                    (floatSpillCount_++)*2);
            allocatedUseCount_.erase(mostUsesIter);
        }
    }

/*
    cout << "\n\tSelected values to be spilled, count: " 
         << intSpillCount_ << " " << floatSpillCount_ << endl;
*/
}

/**
 * Updates move source.
 * Sets move source to be either allocated register or generates 
 * spill load code.
 */
void 
LinearScanAllocatorCore::updateMoveSrc(MoveNode& mn) {
    Move& move = mn.move();
    Terminal &src = move.source();
    if(src.isGPR()) {
        TerminalRegister& tr = dynamic_cast<TerminalRegister&>(src);
        if( tr.registerFile().width() == 32 ) {
            int allocatedIndex = regAllocatedIndecesByValue_[tr.index()];
            AllocatedVariable* av = regAllocations_[allocatedIndex];
            if( mn.nodeID()&1) {
                av->convertUse(mn, *intTmp1_);
            } else {
                av->convertUse(mn, *intTmp2_);
            }
        } else {
            if (tr.registerFile().width() == 1 ) {
                // bool reads not supported
            } else { // FP
                // seems that we can get this far without fp regs
                // but fp um reg refs if
                // they are really not used, just idling. so check also here.
                if (regMap_->fprCount() <6 ) {
                    std::string msg = 
                        "Machine does not have enough floating-point regs"
                        " but the code uses floating point. "
                        "Should have at least 6 FPRs";
                    throw IllegalProgram(
                        __FILE__,__LINE__,__func__, msg);
                }
                int allocatedIndex = 
                    regAllocatedIndecesByValue_[tr.index()+FPR_OFFSET];
                AllocatedVariable* av = 
                    regAllocations_[allocatedIndex];
                if( mn.nodeID()&1) {
                    av->convertUse(mn,
                                   regMap_->terminal(regMap_->fprCount()
                                                     -2+FPR_OFFSET));
                } else {
                    av->convertUse(mn,
                                   regMap_->terminal(regMap_->fprCount()
                                                     -1+FPR_OFFSET));
                }
            }
        }
    }
}

/**
 * Updates move destination.
 * Sets move destination to be either allocated register or generates 
 * spill save code. If the result is dead, don't allocate register but
 * return false.
 *
 * @param mn MoveNode containing the move being updated.
 *
 * @return whether the result was alive and was really updated.
 */
bool 
LinearScanAllocatorCore::updateMoveDest(MoveNode& mn) {
    Move& move = mn.move();
    Terminal& dst = move.destination();
    if(dst.isGPR()) {
        
        // TODO: Fp as idnex regs???
        TerminalRegister& tr = dynamic_cast<TerminalRegister&>(dst);
        if( tr.registerFile().width() == 32) {

            AllocationIndexMap::iterator iter = 
                regAllocatedIndecesByValue_.find(tr.index());
            if (iter == regAllocatedIndecesByValue_.end()) {
                return false;
            }
                
            int allocatedIndex = iter->second;

            AllocatedVariable* av = regAllocations_[allocatedIndex];
            if( mn.nodeID()&1) {
                av->convertSave(mn, *intTmp2_, *intTmp1_);
            } else {
                av->convertSave(mn, *intTmp1_, *intTmp2_);
            }

        } else {
            // guard allocation is not done here
            if (tr.registerFile().width() != 1 ) {
                // FP
                int allocatedIndex = 
                    regAllocatedIndecesByValue_[tr.index()+FPR_OFFSET];
                AllocatedVariable* av = 
                    regAllocations_[allocatedIndex];
                
//                std::cerr << "converting save from index: " << tr.index();
//                std::cerr << " which is allocated to: " << allocatedIndex
//                          << std::endl;
                
                if( mn.nodeID()&1) {
                    av->convertSave(mn,regMap_->terminal(regMap_->fprCount()
                                                         +FPR_OFFSET-1),
                                    regMap_->terminal(regMap_->fprCount()
                                                      +FPR_OFFSET-2));
                } else {
                    av->convertSave(mn,regMap_->terminal(regMap_->fprCount()
                                                         +FPR_OFFSET-2),
                                    regMap_->terminal(regMap_->fprCount()
                                                      +FPR_OFFSET-1));
                }
            }
        }
    }
    return true;
}

/**
 * Update source and destination of a move.
 *
 * @param MoveNode movenode pointing to a move being updated
 *
 * @return false if the move should be dropped due dead result
 */
bool 
LinearScanAllocatorCore::updateMove(MoveNode& mn) {
    updateMoveSrc(mn);
    return updateMoveDest(mn);
}

void LinearScanAllocatorCore::updateMoves() {
    
    for (int i = 0; i < ddg_->nodeCount(); i++) {
        MoveNode& mn = ddg_->node(i);
        if (mn.isMove()) {
            if (!updateMove(mn)) {
                std::list<MoveNode*> movesToRemove;

                movesToRemove.push_back(&mn);
                if( mn.isSourceOperation()) {
                    
                    ProgramOperation& po = mn.sourceOperation();
                    Operation& op = po.operation();
                    if( op.hasSideEffects()) {
                        throw Exception(
                            __FILE__,__LINE__,__func__,
                            "Dead result from op which has side effects");
                    } else {
                        for (int j = 1; j <= op.numberOfInputs(); j++ ) {
                            MoveNodeSet& inputNodes = po.inputNode(j);
                            for (int k = 0; k < inputNodes.count(); k++) {
                                movesToRemove.push_back(
                                    &inputNodes.at(k));
                            }
                        }
                    }
                }
                
                for (std::list<MoveNode*>::iterator iter = 
                         movesToRemove.begin(); 
                     iter != movesToRemove.end(); iter++ ) {

                    Move* move = &(*iter)->move();                    
                    Instruction& ins = move->parent();

                    ddg_->removeNode(**iter);
                    ins.removeMove(*move);

                    delete *iter; // until we have MNM ready
                    delete move;
                    i--;
                }
            }
        }
	}

    ProcedurePass::copyCfgToProcedure(*proc_,*cfg_);


    InstructionReferenceManager& irm = 
        proc_->parent().instructionReferenceManager();
    // delete NOPs
    for (int i = 0; i < proc_->instructionCount(); i++) {
        Instruction& ins = proc_->instructionAtIndex(i);
        if (ins.moveCount() == 0 && ins.immediateCount() == 0) {
            if (irm.hasReference(ins)) {
                if (!proc_->hasNextInstruction(ins)) {
                    throw IllegalProgram(__FILE__, __LINE__, __func__,
                                         "Illegal reference at end of proc");
                }
                irm.replace( ins, proc_->nextInstruction(ins));
            }

            proc_->remove(ins);
            delete &ins;
            i--;
        }
    }
}

void
LinearScanAllocatorCore::updateSavedRegs() {

    unsigned int savedRegs = 0;

    // clear original CS code
//    stackManager_->clearRegisterSaves();

    for (std::set<unsigned int>::iterator iter = regNeedsContextSave_.begin();
         iter != regNeedsContextSave_.end(); iter++ ) {
        if( *iter >= calleeSaveGprStartScheduled_ 
            && *iter < regMap_->gprCount()) {
            stackFrameData_->addRegisterSave(regMap_->terminal(*iter));
            savedRegs++;
        }
    }
}

void 
LinearScanAllocatorCore::createCSCode() {
    stackFrameCodeCreator_->createStackFrameCode(*proc_);
}

/**
 * Updates all stack-based memory addresses to be correct after spilling 
 * and changed context-save code.
 */
void 
LinearScanAllocatorCore::updateStackReferences() {

    stackFrameCodeCreator_->updateStackReferences(*proc_);
}

// TODO: do not context-save temp registers


/**
 * Here starts code of member class RegisterVariable
 */

/* Converts register use into  */
void 
LinearScanAllocatorCore::RegisterVariable::convertUse(
    class MoveNode& movenode, const TerminalRegister&) const {
    TerminalRegister& tr = 
        dynamic_cast<TerminalRegister&>(movenode.move().source());
    int orgIndex = tr.index();
    int registerWidth = tr.registerFile().width();

    movenode.move().setSource(allocatedRegister_.copy());
    
    if (registerWidth == 32) {
        // annotate use
        switch (orgIndex) {
        case 0:
            movenode.move().addAnnotation(
                ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_RV_READ,""));
            break;
        case 1:
            movenode.move().addAnnotation(
                ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_READ,""));
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            movenode.move().addAnnotation(
                ProgramAnnotation(
                    ProgramAnnotation::ANN_REGISTER_IPARAM_READ,
                    Conversion::toString(orgIndex-2)));
        default:
            break;
        }
    }
    // TODO: FP parameter regs
}

void 
LinearScanAllocatorCore::RegisterVariable::convertSave(
    MoveNode& movenode, const TerminalRegister&,
    const TerminalRegister&) const {

    TerminalRegister& tr = 
        dynamic_cast<TerminalRegister&>(movenode.move().destination());
    int orgIndex = tr.index();
    int registerWidth = tr.registerFile().width();
    movenode.move().setDestination(allocatedRegister_.copy());

    if (registerWidth == 32) {
        // annotate save
        switch (orgIndex) {
        case 0:
            movenode.move().addAnnotation(
                ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_RV_SAVE,""));
            break;
        case 1:
            movenode.move().addAnnotation(
                ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_SAVE,""));
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            movenode.move().addAnnotation(
                ProgramAnnotation(
                    ProgramAnnotation::ANN_REGISTER_IPARAM_SAVE,
                    Conversion::toString(orgIndex-2)));
        default:
            break;
        }
    }
}

LinearScanAllocatorCore::RegisterVariable::RegisterVariable(
    const TTAProgram::TerminalRegister& tr) :
    allocatedRegister_(tr) {
}

/**
 * Here starts code of member class Spilled Variable
 */

LinearScanAllocatorCore::SpilledVariable::SpilledVariable(
    class StackFrameCodeCreator& sfcc, int index) :
    spillIndex_(index),sfcc_(sfcc) {
}

void
LinearScanAllocatorCore::SpilledVariable::convertUse(
    class MoveNode& moveNode, const TerminalRegister& temp) const {
    
    sfcc_.createSpillLoad(
        moveNode, temp, spillIndex_<<2);

}

void 
LinearScanAllocatorCore::SpilledVariable::convertSave(
    class MoveNode& moveNode, const TerminalRegister& temp1,
    const TerminalRegister& temp2) const {

    sfcc_.createSpillStore(
        moveNode.move(), temp1, temp2,spillIndex_<<2);

}

bool 
LinearScanAllocatorCore::SpilledVariable::isSpill() const { 
    return true; 
}


bool
LinearScanAllocatorCore::VariableDeathComparator::operator() (
    const LinearScanAllocatorCore::Variable* var1, 
    const LinearScanAllocatorCore::Variable* var2) {
            return var1->deathTime() < var2->deathTime();
}

bool
LinearScanAllocatorCore::VariableBirthComparator::operator() (
    const LinearScanAllocatorCore::Variable* var1, 
    const LinearScanAllocatorCore::Variable* var2) {
    return var1->birthTime() < var2->birthTime();
}


/** These fucntions belong to subclass Variable */


LinearScanAllocatorCore::Variable::Variable(
    int index, LinearScanAllocatorCore::Variable::VarType type,
    DataDependenceGraph& ddg, ControlFlowGraph& cfg) : 
    umRegIndex_(index), type_(type), ddg_(ddg), cfg_(cfg) {}

void 
LinearScanAllocatorCore::Variable::addRead(MoveNode& readNode) {
    reads_.push_back(&readNode);
}

void
LinearScanAllocatorCore::Variable::addWrite(MoveNode& writeNode) {
    writes_.push_back(&writeNode);
}

int 
LinearScanAllocatorCore::Variable::readCount() const { 
    return reads_.size(); 
}

int 
LinearScanAllocatorCore::Variable::writeCount() const { 
    return writes_.size(); 
}

int 
LinearScanAllocatorCore::Variable::edgeCount() const { 
    return edges_.size(); 
}

int
LinearScanAllocatorCore::Variable::birthTime() const {
    return bIndex_;
}

int
LinearScanAllocatorCore::Variable::deathTime() const {
    return dIndex_;
}

unsigned int
LinearScanAllocatorCore::Variable::originalIndex() const {
    return umRegIndex_;
}

LinearScanAllocatorCore::Variable::VarType 
LinearScanAllocatorCore::Variable::type() const {
    return type_;
}

/**
 * Terribly slow and ugly function 
 */
int
LinearScanAllocatorCore::Variable::moveAddress(MoveNode& mn) {
    if (!mn.isMove()) {
        return -1;
    }
    BasicBlockNode& bbn = ddg_.getBasicBlockNode(mn);
    BasicBlock& bb = bbn.basicBlock();
    for (int i = 0; i < bb.instructionCount(); i++) {
        Instruction& ins = bb.instructionAtIndex(i);
        for (int j = 0; j < ins.moveCount(); j++) {
            Move& move = ins.move(j);
            if (&mn.move() == &move) {
                return bbn.originalStartAddress() + i;
            }
        }
    }
    throw Exception(__FILE__,__LINE__,__func__,"Move not in BB");
}


/**
 * Calculates live range for the variable.
 * This functions is still quite stupid.
 * 
 * @param proc Procedure in which this variable belongs.
 */
void 
LinearScanAllocatorCore::Variable::calculateRange(Procedure& proc) {
//  std::cout << "\t\tchecking variable range:" <<originalIndex() << std::endl;

    std::list<DataDependenceEdge*> remoteEdges;
    std::list<MoveNode*> rEdgeTails;
    std::list<MoveNode*> rEdgeHeads;

    if (writeCount() && readCount()) {
        bool allBBLocal = true;
        MoveNode* firstWrite = writes_.at(writeCount()-1);
        MoveNode* lastRead = reads_.at(0);
        
        int eCount = edgeCount();
        for (int i = 0; i < eCount; i++) {
            DataDependenceEdge& edge = *edges_.at(i);
            MoveNode& headNode = *heads_.at(i);
            MoveNode& tailNode = *tails_.at(i);

            class BasicBlockNode& bb1 = ddg_.getBasicBlockNode(tailNode);
            class BasicBlockNode& bb2 = ddg_.getBasicBlockNode(headNode);
            if (&bb1 != &bb2) {
                remoteEdges.push_back(&edge);
                rEdgeTails.push_back(&tailNode);
                rEdgeHeads.push_back(&headNode);
                allBBLocal = false;
            } else {

                if (moveAddress(headNode) > moveAddress(*lastRead)) {
                    lastRead = &headNode;
                }
                if (moveAddress(tailNode) < moveAddress(*firstWrite)) {
                    firstWrite = &tailNode;
                }
            }
        }

        dIndex_ = moveAddress(*lastRead);
        if (firstWrite->isMove()) {
            bIndex_ = moveAddress(*firstWrite);
        } else {
            bIndex_ = proc.startAddress().location();
        }
        if (allBBLocal) {
            /*
            std::cout << "\t\t\tOnly local live ranges on variable: " 
                      << originalIndex() << ", range: " 
                      << bIndex_ << ".." << dIndex_ << std::endl;
            */
            return;
        }        
    
        bool simplePath = true;
        
        while(!remoteEdges.empty()) {
            std::list<DataDependenceEdge*>::iterator rei = remoteEdges.begin();
            std::list<MoveNode*>::iterator tailiter = rEdgeTails.begin();
            std::list<MoveNode*>::iterator headiter = rEdgeHeads.begin();

            MoveNode& headNode = **headiter;
            MoveNode& tailNode = **tailiter;
            
            class BasicBlockNode* bb1 = &ddg_.getBasicBlockNode(tailNode);
            class BasicBlockNode& bb2 = ddg_.getBasicBlockNode(headNode);
            
            int tna;
            if (tailNode.isMove()) {
                tna = moveAddress(tailNode);
            } else {
                tna = proc.startAddress().location();
            }
            int hna = moveAddress(headNode);

            if(tna < bIndex_ ) {
                bIndex_ = tna;
            }
            
            if(hna > dIndex_ ) {
                dIndex_ = hna;
            }
            
            while(true) {
                if( cfg_.outDegree(*bb1) != 1 || cfg_.inDegree(*bb1) != 1 ) {
                    simplePath = false;
//                std::cout << "\t\t\tedge between complex path" << std::endl;
                    remoteEdges.erase(rei);
                    rEdgeTails.erase(tailiter);
                    rEdgeHeads.erase(headiter);
                    break;
                }
                ControlFlowEdge& controlEdge = cfg_.outEdge(*bb1,0);
                if( &cfg_.headNode(controlEdge) == &bb2) {
//                std::cout << "\t\t\tedge between simple path" << std::endl;
                    remoteEdges.erase(rei);
                    rEdgeTails.erase(tailiter);
                    rEdgeHeads.erase(headiter);
                    break;
                } else {
                    bb1 = &cfg_.headNode(controlEdge);
                    
                    if (bb1->isNormalBB()) {
                        int bb1Size = 
                            bb1->basicBlock().instructionCount();
                        if (bb1Size > 0) {
                            if((int)bb1->originalStartAddress() < bIndex_ ) {
                                bIndex_ = bb1->originalStartAddress();
                            }
                            int lastAddrOfBB = bb1->originalStartAddress() + 
                                bb1->basicBlock().instructionCount();
                            if (lastAddrOfBB > dIndex_) {
                                dIndex_ = lastAddrOfBB;
                            }
                        }
                    }
                }
            }
        }
        if (simplePath) {
//            std::cout << "\t\trange: " <<bIndex_<<".."<<dIndex_<<std::endl;
            return;
        }
    }
//    std::cout << "multi-BB live range on variable: " 
//              << originalIndex()<<std::endl;

    // this is stupid but this works
    dIndex_ = proc.endAddress().location()-1;

//    if( writeCount() == 1 ) {
//        bIndex_ = writes_[0]->move().parent().address().location();//nodeID();
//    } else { // complicated algorithm not yet implemented
        bIndex_ = proc.startAddress().location();
//    }
}

/**
 * Searches first write to this variable.
 * 
 * @return MoveNode containing first write to this variable
 */
MoveNode& 
LinearScanAllocatorCore::Variable::searchFirstWrite() {
    MoveNode* firstWrite = writes_.at(0);
    for (unsigned int i = 1; i < writes_.size(); i++) {
        if (writes_.at(i)->nodeID() < firstWrite->nodeID()) {
            firstWrite = writes_.at(i);
        }
    }
    return *firstWrite;
}

/** 
 * Searches last read from this variable.
 * 
 * @return MoveNode containing last read from this variable
 */
MoveNode& 
LinearScanAllocatorCore::Variable::searchLastRead() {
    MoveNode* lastRead = reads_.at(0);

    for (unsigned int i = 1; i < reads_.size(); i++) {
        if (reads_.at(i)->nodeID() > lastRead->nodeID()) {
            lastRead = reads_.at(i);
        }
    }
    return *lastRead;
}

void 
LinearScanAllocatorCore::Variable::addEdge(
    DataDependenceEdge& edge, MoveNode& tail, MoveNode& head) {
    edges_.push_back(&edge);
    tails_.push_back(&tail);
    heads_.push_back(&head);
}
