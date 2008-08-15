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
