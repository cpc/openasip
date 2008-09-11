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
    const std::string& name, const AddressSpace& space):
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
    const std::string& name, const AddressSpace& space,
    UIntWord startLocation):
    CodeSnippet(Address(startLocation,space)), name_(name) {
}

/**
 * Destructor
 */
Procedure::~Procedure() {
}

/**
 * Returns the name of the procedure.
 *
 * @return The name of the procedure.
 */
string
Procedure::name() const {
    return name_;
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
Procedure::address(const Instruction& ins) const
    throw (IllegalRegistration) {

    unsigned int i = 0;

    /* this loop is executed very ofter so 
       uses pre-computer size and [] for performance reasons */
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
    if (instructionCount() > 0) {
        Instruction* ins = &firstInstruction();
        while (ins != &NullInstruction::instance()) {
            newProc->add(ins->copy());
            ins = &nextInstruction(*ins);
        }
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
Procedure::add(Instruction* ins) 
    throw (IllegalRegistration) {

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
Procedure::insertAfter(const Instruction& pos, Instruction* ins)
    throw (IllegalRegistration) {

    if (!ins->isInProcedure()) {

        if (hasNextInstruction(pos)) {

            InsList::iterator iter = instructions_.begin();
            Instruction& next = nextInstruction(pos);

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
            add(ins);
        }

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
Procedure::insertBefore(const Instruction& pos, Instruction* ins)
    throw (IllegalRegistration) {

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
Procedure::remove(Instruction& ins) throw (IllegalRegistration) {

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

    InsList::iterator iter = instructions_.begin();
    const InstructionAddress addr = ins.address().location();

    for (; iter != instructions_.end(); iter++) {
        if ((*iter) == &ins) {

            iter = instructions_.erase(iter);

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



}
