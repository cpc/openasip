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

    ins_(&ins) {
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
    proc_(&proc) {
    ins_ = NULL;
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
    if (ins_ != NULL) {
        return ins_->instruction().address();
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
const InstructionReference&
CodeLabel::instructionReference() const throw (IllegalRegistration) {
    if (ins_ != NULL) {
        return *ins_;
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
            &(ins_->instruction().parent()));
        assert(proc != NULL);
        return *proc;
    } else {
        return *proc_;
    }
}

}
