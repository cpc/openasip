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
 * @file CodeLabel.cc
 *
 * Implementation of CodeLabel class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "CodeLabel.hh"
#include "Instruction.hh"
#include "InstructionReference.hh"
#include "Procedure.hh"
#include "Program.hh"
#include "InstructionReferenceManager.hh"
#include "NullInstruction.hh"
#include "GlobalScope.hh"

using std::string;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// CodeLabel
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * Registers this label to the owning scope.
 *
 * @param ins Instruction corresponding to this label.
 * @param name Name of the label. Must be unique within the owning scope.
 * @exception IllegalRegistration if the given instruction does not belong
 *            to a procedure, or its parent procedure does not belong to a
 *            program.
 */
CodeLabel::CodeLabel(const InstructionReference& ins, std::string name)
    throw (IllegalRegistration) :

    ins_(ins) {
    proc_ = NULL;
    setName(name);
    setAddress(ins.instruction().address());
    // scope is the global scope for now
    setScope(ins.instruction().parent().parent().globalScope());
}

/**
 * An alternative constructor that takes the procedure.
 *
 * @param proc Procedure corresponding to this label
 * @exception IllegalRegistration if the given procedure does not belong
 *            to a program.
 */
CodeLabel::CodeLabel(const Procedure& proc) throw (IllegalRegistration):
    ins_(InstructionReference(NULL)), proc_(&proc) {
    setName(proc.name());
    setAddress(proc.startAddress());
    // scope is the global scope for now
    setScope(proc.parent().globalScope());
}

/**
 * The destructor.
 */
CodeLabel::~CodeLabel() {
}

/**
 * Returns the address of the instruction or procedure corresponding to this
 * label.
 *
 * @return The address of the instruction or procedure corresponding to this
 *         label.
 */
Address
CodeLabel::address() const {
    if (&ins_.instruction() != &NullInstruction::instance()) {
        return ins_.instruction().address();
    } else {
        return proc_->startAddress();
    }
}

/**
 * Returns a reference to the instruction corresponding to this label.
 *
 * @return a reference to the instruction corresponding to this label.
 * @exception IllegalRegistration if the label points to a procedure that
 *            has no instructions.
 */
const InstructionReference
CodeLabel::instructionReference() const throw (IllegalRegistration) {
    if (&ins_.instruction() != &NullInstruction::instance()) {
        return ins_;
    } else if (proc_->instructionCount() == 0) {
        throw IllegalRegistration(__FILE__, __LINE__);
    } else {
        return proc_->parent().instructionReferenceManager().createReference(proc_->firstInstruction());
    }
}

/**
 * Return the procedure that contains this label.
 *
 * @return The procedure that contains this label or the procedure at which
 *         this labels points.
 * @exception IllegalRegistration if the label is not registered in a
 *                                procedure.
 */
const Procedure&
CodeLabel::procedure() const throw (IllegalRegistration) {
    if (proc_ == NULL) {
        const Procedure* proc = dynamic_cast<const Procedure*>(
            &(ins_.instruction().parent()));
        assert(proc != NULL);
        return *proc;
    } else {
        return *proc_;
    }
}

}
