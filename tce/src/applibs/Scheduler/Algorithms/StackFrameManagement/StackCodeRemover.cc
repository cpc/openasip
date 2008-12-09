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
 * @file StackCodeRemover.cc
 *
 * Implementation of StackCodeRemover class
 *
 * Removes old stack frame code from procedures and collects the data in them.
 *
 * @return data about the stack frame. User has to delete this struct.
 *
 */

#include "Procedure.hh"
#include "Program.hh"
#include "ProgramAnnotation.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "InstructionReferenceManager.hh"

#include "POMDisassembler.hh"

#include "StackCodeRemover.hh"

#include "StackFrameData.hh"

using namespace TTAProgram;

StackFrameData*
StackCodeRemover::removeStackFrameCode(TTAProgram::Procedure& proc) {

    InstructionReferenceManager& irm = 
        proc.parent().instructionReferenceManager();
    
    StackFrameData* sfd = new StackFrameData(proc.name() == "__crt0");

    for (int i = 0; i < proc.instructionCount();) {
        Instruction& ins = proc.instructionAtIndex(i);

        // not leaf function if contains calls
        if (ins.hasCall()) {
            sfd->setLeafProcedure(false);
        }

        if( hasStackFrameAnnotation(ins)) {
            ProgramAnnotation sfAnn = getStackFrameAnnotation(ins);

            switch(sfAnn.id()) {
            case ProgramAnnotation::ANN_STACKFRAME_LVAR_ALLOC:
                sfd->setLocalVarSize(Conversion::toInt(sfAnn.stringValue()));
                break;
            case ProgramAnnotation::ANN_STACKFRAME_RA_ALLOC:
                break;
            case ProgramAnnotation::ANN_STACKFRAME_GPR_SAVE_REG: {
                Move& move = ins.move(0);
                sfd->addRegisterSave(move.destination());
                break;
            }
            case ProgramAnnotation::ANN_STACKFRAME_FPR_SAVE_REG: {
                Move& move = ins.move(0);
                sfd->addRegisterSave(move.destination());

                break;
            }
            case ProgramAnnotation::ANN_STACKFRAME_OUT_PARAM_ALLOC:
                sfd->setOutputParamsSize(
                    Conversion::toInt(sfAnn.stringValue()));
                break;
            case ProgramAnnotation::ANN_STACKFRAME_STACK_INIT:
                sfd->setStackInitAddr(
                    Conversion::toInt(sfAnn.stringValue()));
            default:
                break;
            }

            if (proc.instructionCount() > i+1 ) {
                if (irm.hasReference(proc.instructionAtIndex(i+1))) {
                    // just clear the instr, ie leave a nop
                    for (int j = 0; i < ins.moveCount(); j++ ) {
                        ins.removeMove(ins.move(j));
                    }
                    continue;
                }
            }
            if (irm.hasReference(ins)) {
                if (proc.instructionCount() > i+1 ) {
                    irm.replace(ins, proc.instructionAtIndex(i+1));
                } else {
                    throw IllegalProgram(__FILE__,__LINE__,"",
                                         "cannot update ref");
                }
            }

            proc.remove(ins); 
            delete &ins;

        } else {
            i++;
        }
        
    }
    if( sfd->outputParamsSize() != 0 && sfd->isLeafProcedure()) {
        throw IllegalProgram(
            __FILE__,__LINE__,"StackCodeRemover::removeStackFrameCode",
            "Output parametes on leaf procedure");
    }
    return sfd;
}

/**
 * Returns whether an instruction contains stack frame generation code.
 */
bool StackCodeRemover::hasStackFrameAnnotation(TTAProgram::Instruction& ins) {
    for( int i = 0; i < ins.moveCount(); i++ ) {
        Move& move = ins.move(i);
        for( int j = 0; j < move.annotationCount(); j++ ) {
            const ProgramAnnotation& ann = move.annotation(j);
            if( ann.id() >= ProgramAnnotation::ANN_STACKFRAME_BEGIN &&
                ann.id() <= ProgramAnnotation::ANN_STACKFRAME_END )
                return true;
        }
    }
    return false;
}

/**
 * Returns annotation which means that this instruction contains
 * Stack frame generation code.
 *
 * If the instruction does not contain such annotation, returns NULL.
 *
 * @return Pointer to stack-frame-related annotation. The object is owned
 * by POM, the user must NOT delete it.
 */
const ProgramAnnotation
StackCodeRemover::getStackFrameAnnotation(TTAProgram::Instruction& ins) 
    throw (NotAvailable) {
    for( int i = 0; i < ins.moveCount(); i++ ) {
        Move& move = ins.move(i);
        for( int j = 0; j < move.annotationCount(); j++ ) {
            const ProgramAnnotation& ann = move.annotation(j);
            if( ann.id() >= ProgramAnnotation::ANN_STACKFRAME_BEGIN &&
                ann.id() <= ProgramAnnotation::ANN_STACKFRAME_END )
                return ann;
        }
    }
    throw NotAvailable(__FILE__,__LINE__,"","SF Annotation not found");
}
