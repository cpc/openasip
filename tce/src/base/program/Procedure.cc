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
 * @file Procedure.cc
 *
 * Implementation of Procedure class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "Procedure.hh"
#include "Program.hh"
#include "NullProgram.hh"
#include "NullProcedure.hh"
#include "NullInstruction.hh"
#include "InstructionReferenceManager.hh"
#include "GlobalScope.hh"

using std::string;
using namespace TTAMachine;

namespace TTAProgram {

const int Procedure::INSTRUCTION_INDEX_ALIGNMENT = 1;

/**
 * Constructor.
 *
 * Create an empty procedure in given address space. Automatically
 * create an empty procedure scope to store all program symbols that
 * are local to this procedure. The default value zero will be used
 * for the start location. Alignment is 1 MAU.
 *
 * @param name Name of the procedure.
 * @param space The address space of the procedure.
 */
Procedure::Procedure(
    const TCEString& name, const AddressSpace& space):
    CodeSnippet(Address(0, space)) , name_(name) {
}

/**
 * Alternative constructor that takes the start location of the procedure.
 *
 * @param name Name of the procedure.
 * @param space The address space of the procedure.
 * @param startLocation The start location of the procedure.
 */
Procedure::Procedure(
    const TCEString& name, const AddressSpace& space,
    UIntWord startLocation):
    CodeSnippet(Address(startLocation,space)), name_(name) {
}

/**
 * Destructor
 */
Procedure::~Procedure() {
}

/**
 * Returns the alignment.
 *
 * @return The alignment.
 */
int
Procedure::alignment() const {
    return alignment_;
}

/**
 * Returns the address of the given instruction.
 *
 * @return The address of the given instruction.
 * @exception IllegalRegistration if the instruction does not belong to
 *            this procedure.
 */
Address
Procedure::address(const Instruction& ins) const {
    unsigned int i = 0;

    /* this loop is executed very ofter so 
       uses pre-computed size and [] for performance reasons */
    const unsigned int size = instructions_.size();
    while (i < size && instructions_[i] != &ins) {
        i++;
    }

    if (i != instructions_.size()) {
        Address insAddress(
            start_.location() + i, start_.space());
        return insAddress;
    } else {
        throw IllegalRegistration(__FILE__, __LINE__);
    }
}

/**
 * Make a complete copy of the procedure.
 *
 * The copy is identical, except that it is not registered to the
 * program of the original procedure.
 *
 * @return A complete copy of the procedure.
 */
CodeSnippet*
Procedure::copy() const {

    Procedure* newProc = new Procedure(
        name_, start_.space(), start_.location());
    for (int i = 0; i < instructionCount(); i++) {
        newProc->add(instructionAtIndex(i).copy());
    }
    return newProc;
}

/**
 * Adds an instruction as the last instruction in the procedure.
 *
 * Remaining procedures in the parent program will be relocated as
 * needed. The ownership of the instruction will be passed to the
 * procedure.
 *
 * @param ins The instruction to add.
 * @exception IllegalRegistration if the instruction is already registered
 *                                in another procedure.
 */
void
Procedure::add(Instruction* ins) {
    if (!ins->isInProcedure()) {

        if (instructions_.size() == instructions_.capacity()) {
            instructions_.reserve(instructions_.size() * 2);
        }

        ins->setParent(*this);
        instructions_.push_back(ins);

        // if registered to a program, tell to move the procedures
        // after the added instruction
        if (parent_ != NULL && this != &parent_->lastProcedure()) {
            parent_->moveProcedure(
                parent_->nextProcedure(*this), 1);
        }

    } else {
        throw IllegalRegistration(__FILE__, __LINE__);
    }
}

/**
 * Insert the given instruction after the given position.
 *
 * Instructions from pos are relocated to make room for the new instruction.
 *
 * @param pos The position.
 * @param ins The instruction to insert.
 * @exception IllegalRegistration if pos does not belong to the procedure
 *            or ins already belongs to a procedure.
 */
void
Procedure::insertAfter(const Instruction& pos, Instruction* ins) {
    if (!ins->isInProcedure()) {
        for (InsList::iterator iter = instructions_.begin();
             iter != instructions_.end(); iter++) {
            
            if ((*iter) == &pos) {
                iter++;
                if (iter != instructions_.end()) {
                    ins->setParent(*this);
                    instructions_.insert(iter, ins);
                    
                    // if registered to a program, tell to move the procedures
                    // after this procedure 
                    if (parent_ != NULL && this != &parent_->lastProcedure()) {
                        parent_->moveProcedure(
                            parent_->nextProcedure(*this), 1);
                    }
                    return;
                } else { // end of procedure
                    add(ins);
                    return;
                }
            }
        }
        // should not go here in any case
        throw IllegalRegistration(
            __FILE__,__LINE__,__func__,"pos not in this procedure");

    } else {
        throw IllegalRegistration(
            __FILE__, __LINE__, "CodeSnippet::insertInstructionBefore",
            "Instruction already belongs to a procedure.");
    }
}

/**
 * Insert the given instruction before the given position.
 *
 * Instructions from and including pos are relocated to make room for
 * the new instruction.
 *
 * @todo Refactor and share implementation with insertInstructionAfter()
 *
 * @param pos The position.
 * @param ins The instruction to insert.
 * @exception IllegalRegistration if pos does not belong to the procedure
 *            or ins already belongs to a procedure.
 */
void
Procedure::insertBefore(const Instruction& pos, Instruction* ins) {
    if (!ins->isInProcedure()) {

        InsList::iterator iter = instructions_.begin();
        const Instruction& next = pos;
        
        while (iter != instructions_.end()) {
            
            if ((*iter) == &next) {
                
                ins->setParent(*this);
                
                iter = instructions_.insert(iter, ins);
                
                iter++;
                
                // if registered to a program, tell to move the procedures
                // after this procedure 
                if (parent_ != NULL && this != &parent_->lastProcedure()) {
                    parent_->moveProcedure(
                        parent_->nextProcedure(*this), 1);
                }
                return;
            }
            
            iter++;
        }
        
        // should not go here in any case
        assert(false);

    } else {
        throw IllegalRegistration(
            __FILE__, __LINE__, "CodeSnippet::insertInstructionBefore",
            "Instruction already belongs to a procedure.");
    }
}

/**
 * Remove instruction from the procedure.
 *
 * The instruction becomes independent (it is not deleted). All
 * instructions following the removed procedure are relocated
 * appropriately. All code labels attached to the removed instruction
 * are deleted from the Program.
 *
 * @note Possible references to the instruction elsewhere
 * in the program are not automatically fixed! It's a responsibility of
 * the caller to fix the Program back to a consistent state.
 *
 * @param ins Instruction to remove.
 * @exception IllegalRegistration If instruction does not belong to the
 *                                procedure.
 */
void
Procedure::remove(Instruction& ins) {
    bool first = false;
    bool refs = false;

    if( parent_ != NULL ) {
        InstructionReferenceManager& irm = 
            parent_->instructionReferenceManager();
        refs = irm.hasReference(ins);
    }

    if (!ins.isInProcedure() || !(&ins.parent() == this)) {
        string msg = "Instruction doesn't belong to the procedure.";
        throw IllegalRegistration(__FILE__, __LINE__, __func__, msg);
    }

    if (&ins == &firstInstruction()) {
        first = true;
    }

    int insIndex = 0;
    for (InsList::iterator iter = instructions_.begin();
         iter != instructions_.end(); iter++, insIndex++) {
        if ((*iter) == &ins) {

            iter = instructions_.erase(iter);
            const InstructionAddress addr = start_.location() + insIndex;

            // remove code label of first instruction only if empty
            if ((!first && refs) || instructions_.empty()) {
                parent().globalScope().removeCodeLabels(addr);
            }

            ins.setParent(NullProcedure::instance());

            if (parent_ != NULL && this != &parent_->lastProcedure()) {
                parent_->moveProcedure(
                    parent_->nextProcedure(*this), -1);
            }

            return;
        }
    }
}

/**
 * Clears a procedure, updates addresses of following addresses.
 */
void
Procedure::clear() {
    int insCount = instructionCount();
    CodeSnippet::clear();

    if (parent_ != NULL && this != &parent_->lastProcedure()) {
        parent_->moveProcedure(
            parent_->nextProcedure(*this), -insCount);
    }
}

}
