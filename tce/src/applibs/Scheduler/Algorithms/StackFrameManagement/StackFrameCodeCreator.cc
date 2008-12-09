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
 * Implementation of StackFramCodeCreator class.
 * 
 * Class which creates stack-related code.
 */

#include <iostream>

#include "CodeSnippet.hh"
#include "UniversalMachine.hh"
#include "Terminal.hh"
#include "TerminalRegister.hh"
#include "TerminalImmediate.hh"
#include "Procedure.hh"
#include "Program.hh"
#include "TerminalFUPort.hh"
#include "Move.hh"
#include "SpecialRegisterPort.hh"
#include "ControlUnit.hh"

#include "InstructionReferenceManager.hh"
#include "MoveNode.hh"
#include "ProgramOperation.hh"

#include "StackFrameCodeCreator.hh"

using namespace TTAMachine;
using namespace TTAProgram;

void StackFrameCodeCreator::setParameters(StackFrameData &sfd) {
    sfd_ = &sfd;
}

StackFrameCodeCreator::StackFrameCodeCreator(
    const TTAMachine::Machine& machine, const UniversalMachine& uMach, 
    const TTAProgram::Terminal& stackPointer) 
    : stackPointer_(stackPointer.copy()), machine_(machine) {
    Machine::FunctionUnitNavigator funav = 
        uMach.functionUnitNavigator();

    TTAMachine::FunctionUnit* uvmFU = funav.item(0); // TODO: which is first?
    ControlUnit* cu = uMach.controlUnit();

    SpecialRegisterPort* srp = cu->returnAddressPort();
    RATerminal_ = new TerminalFUPort(*srp);
    
    Machine::BusNavigator busnav = uMach.busNavigator();
    uvmBus_ = busnav.item(0); // TODO: kludge

    storeHWOp_ = uvmFU->operation("stw");
    loadHWOp_ = uvmFU->operation("ldw");
    addHWOp_ = uvmFU->operation("add");

    assert( storeHWOp_ != NULL);
    assert( loadHWOp_ != NULL);
    assert( addHWOp_ != NULL);
}

StackFrameCodeCreator::~StackFrameCodeCreator() {}

void
StackFrameCodeCreator::createAndUpdate(TTAProgram::Procedure& proc) 
    throw (IllegalMachine, IllegalProgram) {
    updateStackReferences(proc);
    createStackFrameCode(proc);
}


/**
 * Creates and inserts new Stack frame construction and deconstruction code
 * into the procedure being processed.
 * Return jump is not included in this code.
 */
void 
StackFrameCodeCreator::createStackFrameCode(TTAProgram::Procedure& proc) 
    throw (IllegalMachine,IllegalProgram) {

    InstructionReferenceManager& irm = 
        proc.parent().instructionReferenceManager();

    CodeSnippet* constrCode = NULL;
    CodeSnippet* deconsCode = NULL;
    int constrCodeSize = 0;
    int deconstrCodeSize = 0;

    // not crt0
    if (sfd_->stackInitAddr() == 0 ) {
        constrCode = createConstructionCode();
        constrCodeSize = constrCode->instructionCount();
        deconsCode = createDeconstructionCode();
        deconstrCodeSize = deconsCode->instructionCount();
    } else {
        constrCode = createCRT0Code();
        constrCodeSize = constrCode->instructionCount();
    }

    if (proc.instructionCount() == 0) {
        throw IllegalProgram(
            __FILE__,__LINE__,"","Empty procedure");
    }                 
    Instruction& firstIns = proc.firstInstruction();
    Instruction& lastIns = proc.lastInstruction();
    
    proc.insertBefore(firstIns, constrCode);
    if (deconsCode != NULL ) {
        proc.insertBefore(lastIns, deconsCode);
    }

    // update all refs to old first to point new first
    if (constrCodeSize != 0 && irm.hasReference(firstIns)) {
        irm.replace(firstIns, proc.firstInstruction());
    }
    
    // update all refs to old ret to point to stack frame decons
    if (deconstrCodeSize != 0 && irm.hasReference(lastIns)) {
        irm.replace(lastIns, proc.instructionAtIndex(
                        proc.instructionCount()-1-deconstrCodeSize));
    }

}

/**
 * Creates stack offset for outgoing function parameter.
 *
 * @param index offset from out parameter area of stack.
 * @return stack offset from top of the stack
 */
int
StackFrameCodeCreator::createOutParamOffset(int offset) 
    throw (IllegalProgram) {
    if(sfd_->isLeafProcedure()) {
        throw IllegalProgram(
            __FILE__,__LINE__,"StackFrameCodeCreator::createOutParamOffset",
            "Output parameter on leaf procedure");
    }
    return offset;
}

/**
 * Creates stack offset for local variable
 *
 * @param index offset to a local variable.
 * @return stack offset from top of the stack
 */
int
StackFrameCodeCreator::createLocalVarOffset(int offset) {
    if(!sfd_->isLeafProcedure()) {

        return offset + 4 +(sfd_->gprSaveCount() << 2) + 
            (sfd_->fprSaveCount() << 3) + sfd_->outputParamsSize();

    } else {

        return offset +(sfd_->gprSaveCount() << 2) + 
            (sfd_->fprSaveCount() << 3);
    }

}

/**
 * Creates stack offset for stored RA.
 *
 * This should be only needed for some strange hacks like threading or
 * exception handling.
 * @return stack offset to RA storage area from top of the stack
 */
int
StackFrameCodeCreator::createRAOffset() throw (IllegalProgram) {
    if(sfd_->isLeafProcedure()) {

        throw IllegalProgram(
            __FILE__,__LINE__,"StackFrameCodeCreator::createRAOffset",
            "RA stack usage on leaf procedure");
    }
    return sfd_->outputParamsSize() + (sfd_->gprSaveCount() << 2) + 
        (sfd_->fprSaveCount() << 3);
}

/**
 * Creates stack offset for incoming function parameter.
 *
 * @param index offset from input parameter area of stack.
 * @return stack offset from top of the stack
 */
int
StackFrameCodeCreator::createInParamOffset(int offset) {
    if( !sfd_->isLeafProcedure()) {

        return offset + 4 +(sfd_->gprSaveCount() << 2) + 
            (sfd_->fprSaveCount() << 3 ) + sfd_->totalLocalVarSize() 
            + sfd_->outputParamsSize();
    } else {

        return offset +(sfd_->gprSaveCount() << 2) + 
            (sfd_->fprSaveCount() << 3 ) + sfd_->totalLocalVarSize() ;
    }
}

/**
 * Creates stack offset from annotation.
 *
 * @return offset from bottom of the stack.
 */
int StackFrameCodeCreator::createStackOffset(
    TTAProgram::ProgramAnnotation& anno) throw (IllegalProgram) {

    switch(anno.id()) {
        
    case ProgramAnnotation::ANN_STACKUSE_OUT_PARAM:
        return createOutParamOffset(Conversion::toInt(anno.stringValue()));
        
    case ProgramAnnotation::ANN_STACKUSE_LOCAL_VARIABLE:
        return createLocalVarOffset(Conversion::toInt(anno.stringValue()));

    case ProgramAnnotation::ANN_STACKUSE_RA:
        return createRAOffset();
        
    case ProgramAnnotation::ANN_STACKUSE_IN_PARAM:
        return createInParamOffset(Conversion::toInt(anno.stringValue()));

    case ProgramAnnotation::ANN_STACKUSE_SAVED_GPR:
        // No messing up with saved registers of caller
        throw IllegalProgram(__FILE__,__LINE__,"",
                             "Procedure tried to mess up callers saved regs");
        
    case ProgramAnnotation::ANN_STACKUSE_SAVED_FPR:
        // No messing up with saved registers of caller
        throw IllegalProgram(__FILE__,__LINE__,"",
                             "Procedure tried to mess up callers saved regs");
                
    default:
    throw IllegalProgram(__FILE__,__LINE__,"",
                         "Invalid stack use annotation");
    }
}

/**
 * Checks if move contains stack offset immediate and if it does,
 * Updates it according to the stack frame data.
 *
 * Stack offsets are detected from Annotations; Those moves containing
 * stack use annotations are updated.
 *
 * @param move Move being processed.
 */
void StackFrameCodeCreator::updateStackReference(TTAProgram::Move& move){

    for (int i = 0; i < move.annotationCount(); i++) {
        ProgramAnnotation anno = move.annotation(i);
        if (anno.id() >= ProgramAnnotation::ANN_STACKUSE_BEGIN &&
            anno.id() <= ProgramAnnotation::ANN_STACKUSE_END&&
            anno.id() != ProgramAnnotation::ANN_STACKUSE_SPILL) {
            int offset = createStackOffset(anno);
            move.setSource(new TerminalImmediate(SimValue(offset,32)));
        }
    }
}

/**
 * Goes thru all stack indeces in procedure and adjusts the 
 * stack pointer offsets to the correct offset
 * according to the information in the current stack frame data.
 *
 * Stack offsets are detected from Annotations; Those moves containing
 * stack use annotations are updated.
 *
 * @param The procedure being updated.
 */
void StackFrameCodeCreator::updateStackReferences(TTAProgram::Procedure& proc){
    for (int i = 0; i < proc.instructionCount(); i++) {
        Instruction& ins = proc.instructionAtIndex(i);
        
        for (int j = 0; j < ins.moveCount(); j++ ) {
            updateStackReference(ins.move(j));
        }
    }
}

/**
 * Creates stack frame construction code for crt0 routine.
 *
 * @return CodeSnippet containing the creates stack frame construction code.
 */
CodeSnippet*
StackFrameCodeCreator::createCRT0Code() throw (IllegalMachine) {
    // TODO: annotations

    CodeSnippet * cs = new CodeSnippet;
    cs->add(new Instruction);
    
    int lastAddress = 0;
    AddressSpace* instructionAS = machine_.controlUnit()->addressSpace();
    const Machine::AddressSpaceNavigator& asNav = 
        machine_.addressSpaceNavigator();
    for (int i = 0; i < asNav.count(); i++) {
        const AddressSpace* as = asNav.item(i);
        if (as != instructionAS) {
            lastAddress = as->end()&0xFFFFFFF8;
        }
    }
    if (lastAddress == 0) {
        throw IllegalMachine(
            __FILE__,__LINE__,__func__, "No Data address space in machine");
    }

    Move* move = new Move(
        new TerminalImmediate(SimValue(lastAddress,32)),
        stackPointer_->copy(), *uvmBus_);
    move->addAnnotation(ProgramAnnotation(
                            ProgramAnnotation::ANN_REGISTER_SP_SAVE));

    move->addAnnotation(ProgramAnnotation(
                            ProgramAnnotation::ANN_STACKFRAME_STACK_INIT,
                            Conversion::toString(lastAddress)));

    cs->instructionAtIndex(0).addMove(move);

    return cs;
}

/**
 * Creates stack frame construction code.
 *
 * @return CodeSnippet containing the creates stack frame construction code.
 */
CodeSnippet*
StackFrameCodeCreator::createConstructionCode() {

    CodeSnippet* constrCode = new CodeSnippet;
    
    int lvSize = sfd_->totalLocalVarSize();
    
    if(!sfd_->isLeafProcedure()) {
        lvSize += 4; // RA Storage
    }
    
    if( lvSize ) {

        ProgramAnnotation::Id annoID = sfd_->isLeafProcedure() ? 
            ProgramAnnotation::ANN_STACKFRAME_LVAR_ALLOC :
            ProgramAnnotation::ANN_STACKFRAME_LVAR_RA_ALLOC;
        

        constrCode->append(createSPUpdate(-lvSize,annoID));

    } 

    if(!sfd_->isLeafProcedure()) {
        constrCode->append(
            createStackStore(
                *RATerminal_, ProgramAnnotation::ANN_STACKFRAME_RA_SAVE));
    }

    for (int i = 0; i < sfd_->registerSaveCount(); i++ ) {
        constrCode->append(createPush(sfd_->getSavedRegister(i)));
    }
    
    if (sfd_->outputParamsSize()) {
        constrCode->append(
            createSPUpdate(
                -sfd_->outputParamsSize(),
                ProgramAnnotation::ANN_STACKFRAME_OUT_PARAM_ALLOC));
    }
    return constrCode;
    
}

/**
 * Creates stack frame deconstruction code. return jump not included.
 *
 * @return CodeSnippet containing the creates stack frame deconstruction code.
 */
CodeSnippet* 
StackFrameCodeCreator::createDeconstructionCode() {
    CodeSnippet* deconsCode = new CodeSnippet;

    int lvSize = sfd_->totalLocalVarSize(); 
    
    if(!sfd_->isLeafProcedure()) {
        lvSize += 4; // RA Storage
    }

    if (sfd_->outputParamsSize()) {
        deconsCode->append(
            createSPUpdate(
                sfd_->outputParamsSize(),
                ProgramAnnotation::ANN_STACKFRAME_OUT_PARAM_DEALLOC));
    }

    for( int i = sfd_->registerSaveCount()-1; i>=0 ; i-- ) {
        deconsCode->append( createPop(sfd_->getSavedRegister(i)));
    }

    if(!sfd_->isLeafProcedure()) {
        deconsCode->append(
            createStackLoad(
                *RATerminal_, ProgramAnnotation::ANN_STACKFRAME_RA_RESTORE));
        
    }
    
    if( lvSize ) {

        ProgramAnnotation::Id annoID = sfd_->isLeafProcedure() ? 
            ProgramAnnotation::ANN_STACKFRAME_LVAR_DEALLOC :
            ProgramAnnotation::ANN_STACKFRAME_LVAR_RA_DEALLOC;

        deconsCode->append(createSPUpdate(lvSize, annoID));
    }
    return deconsCode;
}

/**
 * Creates a load code.
 *
 * @param destination Terminal where to put the loaded data.
 * @address Terminal containing the memory address.
 * @return CodeSnippet containing the load code.
 */
CodeSnippet*
StackFrameCodeCreator::createLoad (
    const TTAProgram::Terminal& destination, 
    const TTAProgram::Terminal& address,
    ProgramAnnotation::Id firstAnnotationID,
    ProgramAnnotation::Id secondAnnotationID) {
    
    CodeSnippet* cs = new CodeSnippet;
    for( int i = 0; i < 2; i++ ) {
        Instruction* ins = new Instruction;
        cs->add(ins);
    }

    cs->instructionAtIndex(0).addMove(
        new Move(address.copy(),
                 new TerminalFUPort(*loadHWOp_,1), *uvmBus_));

    cs->instructionAtIndex(1).addMove(
        new Move(new TerminalFUPort(*loadHWOp_,2), 
                 destination.copy(), *uvmBus_ ));
    
    if( firstAnnotationID != 0 ) {
        cs->instructionAtIndex(0).move(0).addAnnotation(
            ProgramAnnotation(firstAnnotationID,""));

        cs->instructionAtIndex(1).move(0).addAnnotation(
            ProgramAnnotation(secondAnnotationID,""));
    }

    return cs;
}

/**
 * Creates a load code.
 *
 * @param destination Terminal where to put the loaded data.
 * @address Terminal containing the memory address.
 * @return CodeSnippet containing the load code.
 */
CodeSnippet*
StackFrameCodeCreator::createLoad (
    const TTAProgram::Terminal& destination, 
    const TTAProgram::Terminal& address) {

    CodeSnippet* cs = new CodeSnippet;
    for( int i = 0; i < 2; i++ ) {
        Instruction* ins = new Instruction;
        cs->add(ins);
    }

    cs->instructionAtIndex(0).addMove(
        new Move(address.copy(),
                 new TerminalFUPort(*loadHWOp_,1), *uvmBus_));

    cs->instructionAtIndex(1).addMove(
        new Move(new TerminalFUPort(*loadHWOp_,2), 
                 destination.copy(), *uvmBus_ ));

    return cs;
}


/**
 * Creates a store code 
 *
 * @param source Terminal where to read the data being saved.
 * @address Terminal containing the memory address.
 * @return CodeSnippet containing the store code.
 */
CodeSnippet*
StackFrameCodeCreator::createStore (
    const Terminal& source, const Terminal& address ) {

    CodeSnippet* cs = new CodeSnippet;
    for (int i = 0; i < 2; i++) {
        Instruction* ins = new Instruction;
        cs->add(ins);
    }

    cs->instructionAtIndex(0).addMove(
        new Move(address.copy(),
                 new TerminalFUPort(*storeHWOp_,1), *uvmBus_));
    
    cs->instructionAtIndex(1).addMove(
        new Move(source.copy(),
                 new TerminalFUPort(*storeHWOp_,2), *uvmBus_));

    return cs;
}

CodeSnippet*
StackFrameCodeCreator::createOffsetCalculation(
    const TTAProgram::Terminal& destination, int offset) {
  
    CodeSnippet* cs = new CodeSnippet;
    for (int i = 0; i < 3; i++ ) {
        Instruction* ins = new Instruction;
        cs->add(ins);
    }
    
    TerminalFUPort* tfup = new TerminalFUPort(*addHWOp_,1);
    Terminal* sp2 = stackPointer_->copy();

    cs->instructionAtIndex(0).addMove(
        new Move(sp2, tfup, *uvmBus_));

    cs->instructionAtIndex(0).move(0).addAnnotation(
        ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_READ));
    
    cs->instructionAtIndex(1).addMove(
        new Move(new TerminalImmediate(SimValue(offset,32)),
                 new TerminalFUPort(*addHWOp_,2), *uvmBus_));
    
    cs->instructionAtIndex(2).addMove(
        new Move(new TerminalFUPort(*addHWOp_,3),
                 destination.copy(), *uvmBus_));
    /*
    cs.instructionAtIndex(0).move(0).addAnnotation(
        ProgramAnnotation(firstAnnotationId,Conversion::toString(offset)));

    cs.instructionAtIndex(1).move(0).addAnnotation(
        ProgramAnnotation(annotationId,Conversion::toString(offset)));

    cs.instructionAtIndex(2).move(0).addAnnotation(
        ProgramAnnotation(annotationId,Conversion::toString(offset)));
    */
    return cs;
}

/**
 * Creates stack pointer update code.
 * 
 * @param offset how much to change SP value.
 * @return CodeSnippet containing the SP update code.
 * @param annotationID Annotation to put into the created code.
 * @TODO: check that offset gets into annotations.
 */
CodeSnippet*
StackFrameCodeCreator::createSPUpdate (
    int offset, ProgramAnnotation::Id annotationID) {

    CodeSnippet* cs = createOffsetCalculation(
        *stackPointer_, offset); 

    cs->instructionAtIndex(2).move(0).addAnnotation(
        ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_SAVE));

    for( int i = 0; i < 3; i++ ) {
        cs->instructionAtIndex(i).move(0).addAnnotation(
            ProgramAnnotation(annotationID,Conversion::toString(offset)));
    }
    return cs;
}

/**
 * Creates stack pointer update code.
 * 
 * @param offset how much to change SP value.
 * @return CodeSnippet containing the SP update code.
 * @param firstAnnotationID Annotation to put into the first move of the 
 * created code.
 * @param annotationID Annotation to put into the other moves of the 
 * created code.
 * @TODO: check that offset gets into annotations.
 */
CodeSnippet*
StackFrameCodeCreator::createSPUpdate (
    int offset, ProgramAnnotation::Id firstAnnotationID,
    ProgramAnnotation::Id annotationID) {
    CodeSnippet* cs = createOffsetCalculation(
        *stackPointer_, offset);

    cs->instructionAtIndex(0).move(0).addAnnotation(
        ProgramAnnotation(firstAnnotationID,Conversion::toString(offset)));

    cs->instructionAtIndex(1).move(0).addAnnotation(
        ProgramAnnotation(annotationID,Conversion::toString(offset)));

    cs->instructionAtIndex(2).move(0).addAnnotation(
        ProgramAnnotation(annotationID,Conversion::toString(offset)));

    cs->instructionAtIndex(2).move(0).addAnnotation(
        ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_SAVE));

    return cs;
}


/**
 * Creates store to the top of the stack.
 * 
 * @param source Source of the store.
 * @return CodeSnippet containing the store code.
 */
CodeSnippet*
StackFrameCodeCreator::createStackStore (
    const Terminal& source, ProgramAnnotation::Id firstAnnotationID,
    ProgramAnnotation::Id secondAnnotationID) {
    CodeSnippet* cs = createStore(
        source, *stackPointer_);

    cs->instructionAtIndex(0).move(0).addAnnotation(
        ProgramAnnotation(firstAnnotationID,""));

    cs->instructionAtIndex(1).move(0).addAnnotation(
        ProgramAnnotation(secondAnnotationID,""));
    
    cs->instructionAtIndex(0).move(0).addAnnotation(
        ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_READ));

    return cs;
}

/**
 * Creates store to the top of the stack.
 * 
 * @param source Source of the store.
 * @return CodeSnippet containing the store code.
 */ 
CodeSnippet*
StackFrameCodeCreator::createStackStore (
    const Terminal& source, ProgramAnnotation::Id annotationID) {

    CodeSnippet* cs = createStore(source, *stackPointer_);

    cs->instructionAtIndex(0).move(0).addAnnotation(
        ProgramAnnotation(annotationID,""));

    cs->instructionAtIndex(1).move(0).addAnnotation(
        ProgramAnnotation(annotationID,""));

    cs->instructionAtIndex(0).move(0).addAnnotation(
        ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_READ));

    return cs;
}


/**
 * Creates load from the top of the stack.
 * 
 * @param source Destination of the load
 * @return CodeSnippet containing the load code.
 */
CodeSnippet*
StackFrameCodeCreator::createStackLoad (
    const TTAProgram::Terminal& destination, 
    ProgramAnnotation::Id annotationID) {
    CodeSnippet* cs = createLoad(destination, *stackPointer_, 
                                 annotationID, annotationID);

    cs->instructionAtIndex(0).move(0).addAnnotation(
        ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_READ));

    return cs;
}

/**
 * Creates load from the top of the stack.
 * 
 * @param source Destination of the load
 * @return CodeSnippet containing the load code.
 */
CodeSnippet*
StackFrameCodeCreator::createStackLoad (
    const TTAProgram::Terminal& destination, 
    ProgramAnnotation::Id firstAnnotationID,
    ProgramAnnotation::Id annotationID) {
    CodeSnippet* cs = createLoad(destination, *stackPointer_, 
                                 firstAnnotationID, annotationID);

    cs->instructionAtIndex(0).move(0).addAnnotation(
        ProgramAnnotation(ProgramAnnotation::ANN_REGISTER_SP_READ));

    return cs;
}

/**
 * Creates a stack push code.
 *
 * Push consist of stack update and storing data into top of the stack.
 *
 * @param terminal Data to push into stack.
 * @return CodeSnippet containing the push code.
 */
CodeSnippet*
StackFrameCodeCreator::createPush (
    const TTAProgram::Terminal& terminal) {
    
    CodeSnippet* stackUpdate = NULL;
    CodeSnippet* save = NULL;

    const TerminalRegister& tr = 
        dynamic_cast <const TerminalRegister&>(terminal);
    const RegisterFile& rf = tr.registerFile();

    if( rf.width() == 64 ) {
        stackUpdate = createSPUpdate(
            -8, ProgramAnnotation::ANN_STACKFRAME_FPR_SAVE_BEGIN, 
            ProgramAnnotation::ANN_STACKFRAME_FPR_SAVE);

        save = createStackStore(
            terminal, ProgramAnnotation::ANN_STACKFRAME_FPR_SAVE,
            ProgramAnnotation::ANN_STACKFRAME_FPR_SAVE_REG);
        
    } else {
        stackUpdate = createSPUpdate(
            -4, ProgramAnnotation::ANN_STACKFRAME_GPR_SAVE_BEGIN, 
            ProgramAnnotation::ANN_STACKFRAME_GPR_SAVE);

        save = createStackStore(
            terminal, ProgramAnnotation::ANN_STACKFRAME_FPR_SAVE,
            ProgramAnnotation::ANN_STACKFRAME_FPR_SAVE_REG);
    }
        
    stackUpdate->append(save);
    return stackUpdate;
}

/**
 * Creates a stack pop code.
 *
 * Push consist of loading data from stack and a stack update.
 *
 * @param terminal Place where to put the data popped from the stack.
 * @return CodeSnippet containing the pop code.
 */
CodeSnippet*
StackFrameCodeCreator::createPop (
    const TTAProgram::Terminal& terminal) {

    const TerminalRegister& tr = 
        dynamic_cast <const TerminalRegister&>(terminal);
    const RegisterFile& rf = tr.registerFile();
    CodeSnippet* load;
    CodeSnippet* stackUpdate;

    if (rf.width() == 64 ) {

        load = createStackLoad(
            terminal, ProgramAnnotation::ANN_STACKFRAME_FPR_RESTORE,
            ProgramAnnotation::ANN_STACKFRAME_FPR_RESTORE_REG);

        stackUpdate = createSPUpdate(
            8, ProgramAnnotation::ANN_STACKFRAME_FPR_RESTORE);
    } else {
        load = createStackLoad(
            terminal, ProgramAnnotation::ANN_STACKFRAME_GPR_RESTORE,
            ProgramAnnotation::ANN_STACKFRAME_GPR_RESTORE_REG);

        stackUpdate = createSPUpdate(
            4, ProgramAnnotation::ANN_STACKFRAME_GPR_RESTORE);
    }

    load->append(stackUpdate);
    return load;
}

/**
 * Creates a register spill load code.
 *

*/
CodeSnippet*
StackFrameCodeCreator::createSpillLoadCode(
    const TTAProgram::Terminal& destination, 
    const TTAProgram::TerminalRegister& temp, int spillIndex) {
    
    int offset = createLocalVarOffset(spillIndex) + sfd_->localVarSize();
    
    CodeSnippet* cs =  createOffsetCalculation(temp, offset);
    cs->append(createLoad(destination, temp));
    
    for( int i = 0; i < 5; i++ ) {
        cs->instructionAtIndex(i).move(0).addAnnotation(
            ProgramAnnotation(
                ProgramAnnotation::ANN_STACKUSE_SPILL,
                Conversion::toString(spillIndex+sfd_->localVarSize())));
    }

    return cs;
}

/**
 * Creates a register spill store code.
 *

*/
CodeSnippet*
StackFrameCodeCreator::createSpillStoreCode(
    const TTAProgram::Terminal& source, 
    const TTAProgram::TerminalRegister& temp1, 
    const TerminalRegister& temp2, int spillIndex) {
    
    int offset = createLocalVarOffset(spillIndex) + sfd_->localVarSize();
    
    CodeSnippet* cs = NULL;
    if (source.isGPR()) {
        cs  = createOffsetCalculation(temp1, offset);
        cs->append(createStore(source, temp1));
        
        for( int i = 0; i < 5; i++ ) {
            cs->instructionAtIndex(i).move(0).addAnnotation(
                ProgramAnnotation(
                    ProgramAnnotation::ANN_STACKUSE_SPILL,
                    Conversion::toString(spillIndex+sfd_->localVarSize())));
        }
    } else {
        cs = createOffsetCalculation(temp1, offset);
        cs->append(createStore(temp2, temp1));

        for( int i = 0; i < 5; i++ ) {
            cs->instructionAtIndex(i).move(0).addAnnotation(
                ProgramAnnotation(
                    ProgramAnnotation::ANN_STACKUSE_SPILL,
                    Conversion::toString(spillIndex+sfd_->localVarSize())));
        }
    }

    return cs;
}

/**
 * Replaces one instruction containing register store move with a spill code.
 */
void StackFrameCodeCreator::createSpillStore(
    Move& move, const TTAProgram::TerminalRegister& temp1, 
    const TerminalRegister& temp2, int spillIndex) {

    // TODO: sub-optimal solution, one stupid register-register-move
    // and many unnnecessary temp registers

    Instruction& ins = move.parent();
    CodeSnippet& parent = ins.parent();

    CodeSnippet* cs = createSpillStoreCode( 
        move.source(), temp1, temp2, spillIndex);

    parent.insertAfter(ins, cs);

    move.setDestination(temp2.copy());
}

/**
 * Replaces one instruction containing a resuster load move with a spill code.
 */

void StackFrameCodeCreator::createSpillLoad(

    // TODO: does NOT handle guarded stuff.
    
    MoveNode& moveNode, const TTAProgram::TerminalRegister& temp,
    int spillIndex) {

    Move& move = moveNode.move();
//    Terminal& destination = move.destination();
    Instruction& ins = move.parent();
    CodeSnippet& parent = ins.parent();

    if (moveNode.isDestinationOperation()) {
        CodeSnippet *cs = createSpillLoadCode(temp, temp, spillIndex);

        moveNode.move().setSource(temp.copy());

        ProgramOperation& po = moveNode.destinationOperation();
        Instruction& firstInsOfOp =
            po.inputNode(1).at(0).move().parent();

        parent.insertBefore(firstInsOfOp, cs);

    } else {
        Terminal& destination = move.destination();

        // TODO: remembver to delete
        CodeSnippet *cs = createSpillLoadCode(destination, temp, spillIndex);

        Instruction &last = cs->instructionAtIndex(4);
        Terminal* term = last.move(0).source().copy();
        cs->remove(last);
        delete &last;

        parent.insertBefore(ins, cs);
        move.setSource(term);
    }
}

