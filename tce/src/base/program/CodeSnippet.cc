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
 * @File CodeSnippet.cc
 *
 * Implementation of CodeSnippet class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "CodeSnippet.hh"
#include "ContainerTools.hh"
#include "POMDisassembler.hh"

#include "NullAddressSpace.hh"
#include "NullInstruction.hh"
#include "InstructionReferenceManager.hh"
#include "NullProgram.hh"
#include "NullProcedure.hh"
#include "Conversion.hh"

using std::string;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// CodeSnippet
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
CodeSnippet::CodeSnippet() : parent_(NULL),
    start_(0,TTAMachine::NullAddressSpace::instance()) {
}

/**
 * Constructor.
 */
CodeSnippet::CodeSnippet(const TTAProgram::Address& start) : 
    parent_(NULL), start_(start) {
}


/**
 * The destructor.
 */
CodeSnippet::~CodeSnippet() {
    for (unsigned int i = 0; i < instructions_.size(); i++) {
        if (instructions_.at(i) != &NullInstruction::instance()) {
            delete instructions_.at(i);
        }
    }
    instructions_.clear();
}

/**
 * Clears the code snippet.
 *
 * Removes all instruction references inside the snippet and deletes
 * the instructions.
 */
void
CodeSnippet::clear() {
    for (unsigned int i = 0; i < instructions_.size(); i++) {
        if (instructions_.at(i) != &NullInstruction::instance()) {
            delete instructions_.at(i);
        }
    }
    instructions_.clear();
}

/**
 * Remove one instruction from the end of the code snippet.
 *
 * Nothing happens if the code snippet is empty.
 */
void
CodeSnippet::removeLastInstruction() {
    if (!instructions_.empty()) {
        instructions_.pop_back();
    }
}

/**
 * Return the parent program that contains the code snippet.
 *
 * @return The parent program that contains the code snippet
 * @exception IllegalRegistration If the code snippet is not registered
 *                                anywhere.
 */
Program&
CodeSnippet::parent() const {
    if (parent_ != NULL) {
        return *parent_;
    } else {
        throw IllegalRegistration(__FILE__, __LINE__,__func__,"no parent");
    }
}

/**
 * Sets the parent program of the code snippet.
 *
 * @note This method does not check that the code snippet actually belongs to
 * the given program. It should, therefore be called only be 
 * Program::addProcedure().
 *
 * @param prog The new parent program.
 */
void
CodeSnippet::setParent(Program& prog) {
    if (&prog == &NullProgram::instance()) {
        parent_ = NULL;
    } else {
        parent_ = &prog;
    }
}

/**
 * Tells whether the code snippet belongs to a program.
 *
 * @return True if the code snippet belongs to a program (it is registered),
 *         false otherwise.
 */
bool
CodeSnippet::isInProgram() const {
    return (parent_ != NULL && parent_ != &NullProgram::instance());
}

/**
 * Returns the address of the given instruction.
 *
 * @return The address of the given instruction.
 * @exception IllegalRegistration if the instruction does not belong to
 *            this code snippet.
 */
Address
CodeSnippet::address(const Instruction& ins) const {
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
        throw IllegalRegistration(
            __FILE__, __LINE__, __func__, "Instruction not in CodeSnippet");
    }
}

/**
 * Sets the start address of the code snippet.
 */
void
CodeSnippet::setStartAddress(Address start) {
    start_ = start;
}

/**
 * Returns the number of instructions in this code snippet.
 *
 * @return the number of instructions in this code snippet.
 */
int
CodeSnippet::instructionCount() const {
    return instructions_.size();
}

/**
 * Returns the first instruction in the code snippet.
 *
 * @return The first instruction in the code snippet.
 * @exception InstanceNotFound if there are no instructions in the code snippet.
 */
Instruction&
CodeSnippet::firstInstruction() const {
    if (!instructions_.empty()) {
        return *instructions_.at(0);
    } else {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__, "No instructions.");
    }
}

/**
 * Returns the instruction at a given address.
 *
 * The address space of the address is implied, since there is only
 * one address space for instructions.
 *
 * @param address The instruction address.
 * @exception KeyNotFound if given address is illegal.
 * @todo Rename to instruction() to match Program::procedure() and
 * Instruction::move().
 */
Instruction&
CodeSnippet::instructionAt(UIntWord address) const {
    int index = (address - start_.location());
    if (index >= 0
        && static_cast<unsigned int>(index) < instructions_.size()) {

        Instruction* ins = instructions_.at(index);
        if (ins == &NullInstruction::instance()) {
            throw KeyNotFound(__FILE__, __LINE__);
        } else {
            return *ins;
        }

    } else {
        std::string msg = "Address " + Conversion::toString(address) + 
            " not in this codesnippet( " + 
            Conversion::toString(start_.location()) + "-" +
            Conversion::toString(start_.location() + instructions_.size()-1)
            + " )";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg );
    }
}

/**
 * Returns the instruction at a given index in the container of instructions.
 *
 * This method is for quickly traversing through all the instructions within
 * time critical loops. 
 *
 * @param index The instruction index in the list of instructions.
 * @return The Instruction at the given index.
 */
Instruction&
CodeSnippet::instructionAtIndex(int index) const {
    return *instructions_.at(index);
}

Instruction&
CodeSnippet::operator[](size_t index) const {
    return *instructions_[index];
}

/**
 * Tells whether the given instruction is the last instruction of the
 * code snippet or not.
 *
 * @param ins The instruction to compare.
 * @return True if the given instruction is not the last instruction in
 *         the code snippet, false otherwise.
 */
bool
CodeSnippet::hasNextInstruction(const Instruction& ins) const {
    return (&nextInstruction(ins) != &NullInstruction::instance());
}

/**
 * Returns the next instruction in the instruction stream.
 *
 * @return The next instruction in the instruction stream or a special
 * null instruction if the given instruction is the last instruction
 * of the code snippet.
 *
 * @exception IllegalRegistration If given instruction does not belong
 * to the code snippet.
 */
Instruction&
CodeSnippet::nextInstruction(const Instruction& ins) const {
    if (&ins.parent() == this) {

        int insAddress = ins.address().location();

        unsigned int current = (insAddress - start_.location());
        unsigned int next = current + 1;

        if (next < instructions_.size()) {
            return *instructions_.at(next);
        } else {
            return NullInstruction::instance();
        }

    } else {
        throw IllegalRegistration(__FILE__, __LINE__);
    }
}

/**
 * Return the previous instruction in the instruction stream.
 *
 * @return The previous instruction in the instruction stream or a special
 * null instruction if the given instruction is the first instruction
 * of the code snippet.
 * @exception IllegalRegistration If given instruction does not belong
 * to the code snippet.
 */
Instruction&
CodeSnippet::previousInstruction(const Instruction& ins) const {
    if (&ins.parent() == this) {
        int insAddress = (ins.address().location() - start_.location()) - 1;
        while (insAddress >= 0) {
            Instruction& prevIns = *instructions_.at(insAddress);
            if (&prevIns != &NullInstruction::instance()) {
                return prevIns;
            } else {
                insAddress--;
            }
        }
        return NullInstruction::instance();
    } else {
        throw IllegalRegistration(__FILE__, __LINE__, __func__);
    }
}

/**
 * Returns the last instruction in the code snippet.
 *
 * @return The last instruction in the code snippet.
 * @exception IllegalRegistration if there are no instructions in the
 *            code snippet.
 */
Instruction&
CodeSnippet::lastInstruction() const {
    if (!instructions_.empty()) {
        return *instructions_.back();
    } else {
        throw IllegalRegistration(__FILE__, __LINE__);
    }
}

/**
 * Adds an instruction as the first instruction in the code snippet.
 *
 * The ownership of the instruction will be passed to the
 * code snippet.
 *
 * @param ins The instruction to add.
 * @exception IllegalRegistration if the instruction is already registered
 *                                in another code snippet.
 */
void
CodeSnippet::addFront(Instruction* ins) {
    if (!ins->isInProcedure()) {

        if (instructions_.size() == instructions_.capacity()) {
            instructions_.reserve(instructions_.size() * 2);
        }

        ins->setParent(*this);
        instructions_.insert(instructions_.begin(), ins);

    } else {
        throw IllegalRegistration(__FILE__, __LINE__);
    }
}

/**
 * Adds an instruction as the last instruction in the code snippet.
 *
 * The ownership of the instruction will be passed to the
 * code snippet.
 *
 * @param ins The instruction to add.
 * @exception IllegalRegistration if the instruction is already registered
 *                                in another code snippet.
 */
void
CodeSnippet::add(Instruction* ins) {
    if (!ins->isInProcedure()) {

        if (instructions_.size() == instructions_.capacity()) {
            instructions_.reserve(instructions_.size() * 2);
        }

        ins->setParent(*this);
        instructions_.push_back(ins);

    } else {
        throw IllegalRegistration(__FILE__, __LINE__, __func__,
                                  "Instruction: " +
                                  POMDisassembler::disassemble(*ins)
                                  + " allready has parent:\n " + 
                                  ins->parent().disassembly());
    }
}

/**
 * Insert the given instruction after the given position.
 *
 * Instructions from pos are relocated to make room for the new instruction.
 *
 * @param pos The position.
 * @param ins The instruction to insert.
 * @exception IllegalRegistration if pos does not belong to the code snippet
 *            or ins already belongs to a code snippet.
 */
void
CodeSnippet::insertAfter(const Instruction& pos, Instruction* ins) {
    if (!ins->isInProcedure()) {

        if (hasNextInstruction(pos)) {

            InsList::iterator iter = instructions_.begin();
            Instruction& next = nextInstruction(pos);

            while (iter != instructions_.end()) {

                if ((*iter) == &next) {

                    ins->setParent(*this);

                    iter = instructions_.insert(iter, ins);

                    iter++;

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
 * @exception IllegalRegistration if pos does not belong to the code snippet
 *            or ins already belongs to a code snippet.
 */
void
CodeSnippet::insertBefore(const Instruction& pos, Instruction* ins) {
    if (!ins->isInProcedure()) {

        InsList::iterator iter = instructions_.begin();
        const Instruction& next = pos;

        while (iter != instructions_.end()) {

            if ((*iter) == &next) {

                ins->setParent(*this);
                iter = instructions_.insert(iter, ins);
                iter++;
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
 * Remove instruction from the code snippet.
 *
 * The instruction becomes independent (it is not deleted). All
 * instructions following the removed code snippet are relocated
 * appropriately. 
 *
 * The instructions should not have code labels attached to them.
 *
 * @note Possible references to the instruction elsewhere
 * in the program are not automatically fixed! It's a responsibility of
 * the caller to fix the Program back to a consistent state.
 *
 * @param ins Instruction to remove.
 * @exception IllegalRegistration If instruction does not belong to the
 *                                code snippet.
 */
void
CodeSnippet::remove(Instruction& ins) {
    if (!ins.isInProcedure() || !(&ins.parent() == this)) {
        string msg = "Instruction doesn't belong to the procedure.";
        throw IllegalRegistration(__FILE__, __LINE__, __func__, msg);
    }

    InsList::iterator iter = instructions_.begin();

    for (; iter != instructions_.end(); iter++) {
        if ((*iter) == &ins) {

            iter = instructions_.erase(iter);
            ins.setParent(NullProcedure::instance());

            return;
        }
    }
}

/**
 * Remove instruction from the code snippet at the given address and delete it.
 *
 * All instructions following the removed code snippet are relocated
 * appropriately. All code labels attached to the removed instruction
 * are deleted from the Program.
 *
 * @note Possible references to the instruction elsewhere
 * in the program are not automatically fixed! It's a responsibility of
 * the caller to fix the Program back to a consistent state.
 *
 * @param address Index of the instruction to remove.
 * @exception IllegalRegistration If instruction does not belong to the
 *            code snippet.
 */
void
CodeSnippet::deleteInstructionAt(InstructionAddress address) {
    Instruction& instr = instructionAt(address);
    remove(instr);
    delete &instr;
}

/**
 * Make a complete copy of the code snippet.
 *
 * The copy is identical, except that it is not registered to the
 * program of the original code snippet.
 *
 * @return A complete copy of the code snippet.
 */
CodeSnippet*
CodeSnippet::copy() const {

    CodeSnippet* newProc = new CodeSnippet(start_);
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
 * Inserts a whole code snippet into this code snippet
 * just before specified instruction.
 *
 * This function does not remove the instruction references 
 * from the code snippet.
 *
 * @param ins The succeeding the place where to insert he instructions.
 * @param cs CodeSnippet containing the instructions being inserted.
 */
void 
CodeSnippet::insertBefore(
    const Instruction& pos, const CodeSnippet& cs) {
    for( int i = 0; i < cs.instructionCount(); i++ ) {
        insertBefore(pos, cs.instructionAtIndex(i).copy());
    }
 }


/**
 * Inserts a whole code snippet into this code snippet
 * just before specified instruction.
 * This function moves the ownership of the moves and deletes the 
 * another code snippet.
 *
 * @param ins The succeeding the place where to insert he instructions.
 * @param cs CodeSnippet containing the instructions being inserted.
 */
void 
CodeSnippet::insertBefore(
    const Instruction& pos, CodeSnippet* cs) {
    for( int i = 0, count = cs->instructionCount(); i < count; i++ ) {
        Instruction& ins = cs->instructionAtIndex(0);
        cs->remove(ins);
        insertBefore(pos, &ins);
    }
    delete cs;
 }

/**
 * Inserts a whole code snippet into this code snippet
 * just after specified instruction.
 *
 * This function does not remove the instruction references 
 * from the code snippet.
 *
 * @param ins The succeeding the place where to insert he instructions.
 * @param cs CodeSnippet containing the instructions being inserted.
 */
 void 
 CodeSnippet::insertAfter(
     const Instruction& pos, const CodeSnippet& cs) {

     for (int i = cs.instructionCount()-1; i >= 0 ; i-- ) {
         insertAfter(pos, cs.instructionAtIndex(i).copy());
     }
}

/**
 * Inserts a whole code snippet into this code snippet
 * just after specified instruction.
 *
 * This function moves the ownership of the moves and deletes the 
 * another code snippet.
 *
 * @param ins The succeeding the place where to insert he instructions.
 * @param cs CodeSnippet containing the instructions being inserted.
 */
 void 
 CodeSnippet::insertAfter(
     const Instruction& pos, CodeSnippet* cs) {

     for (int i = cs->instructionCount()-1; i >= 0 ; i-- ) {
         Instruction& ins = cs->instructionAtIndex(i);
         cs->remove(ins);
         insertAfter(pos, &ins);
     }
     delete cs;
}


/**
 * Inserts a whole code snippet at end of this code snippet.
 *
 * This function does not remove the instruction references 
 * from the code snippet.
 *
 * @param cs CodeSnippet containing the instructions being inserted.
 */
 void 
 CodeSnippet::append(
     const CodeSnippet& cs)  {
    for( int i = 0; i < cs.instructionCount(); i++ ) {
        add(cs.instructionAtIndex(i).copy());
    }
 }
    

/**
 * Inserts a whole code snippet at the end of this code snippet.
 *
 * This function moves the ownership of instructions from the old code snippet
 * into the new one, and deletes the given code snippet.
 *
 * @TODO: This routine is O(n^2). Optimize!
 *
 * @param cs CodeSnippet containing the instructions being inserted.
 */
void 
CodeSnippet::append(CodeSnippet* cs) {
    for (int i = 0, count = cs->instructionCount(); i < count ; i++) {
        Instruction &ins = cs->instructionAtIndex(0);
        cs->remove(ins);
        add(&ins);
    }
    delete cs;
}

/**
 * Inserts a whole code snippet at the beginning of this code snippet.
 *
 * This function does not remove the instruction references 
 * from the code snippet.
 *
 * @param cs CodeSnippet containing the instructions being inserted.
 */
void 
CodeSnippet::prepend( const CodeSnippet& cs) {
    for (int i = cs.instructionCount()-1; i >= 0 ; i--) {
        addFront(cs.instructionAtIndex(i).copy());
    }
}

/**
 * Inserts a whole code snippet at the beginning of this code snippet.
 *
 * This function moves the ownership of instructions from the old code snippet
 * into the new one, and deletes the given code snippet.
 *
 * @TODO: This routine is O(n^2). Optimize!
 *
 * @param cs CodeSnippet containing the instructions being inserted.
 */
void 
CodeSnippet::prepend(CodeSnippet* cs) {
    for (int i = cs->instructionCount()-1; i >= 0 ; i--) {
        Instruction &ins = cs->instructionAtIndex(i);
        cs->remove(ins);
        addFront(&ins);
    }
    delete cs;
}

/**
 * Returns the start address of the code snippet.
 *
 * @return The start (lowest) address of the code snippet.
 */
Address
CodeSnippet::startAddress() const {
    return start_;
}

/**
 * Returns the end address of the code snippet.
 *
 * @return The end address of the code snippet, that is, the next address
 *         that does not belong to this code snippet.
 */
Address
CodeSnippet::endAddress() const {
    int endLocation = start_.location() + instructions_.size();
    return Address(endLocation, start_.space());
}

/**
 * Returns true in case there is at least one procedure return in the
 * code snippet.
 */
bool
CodeSnippet::hasReturn() const {
    const int iCount = instructionCount();
    for (int i = 0; i < iCount; ++i) {
        if (instructionAtIndex(i).hasReturn()) {
            return true;
        }
    }
    return false;
}

/**
 * Returns the disassembly of the basic block as string.
 *
 * @return The disassembly of the basic block.
 */
std::string
CodeSnippet::disassembly() const {

    std::string content = "";
    const int iCount = instructionCount();
    for (int i = 0; i < iCount; ++i) {
        const TTAProgram::Instruction& instr = instructionAtIndex(i);
        content += POMDisassembler::disassemble(instr);
        content += "\n";
    }
    return content;
}

}

