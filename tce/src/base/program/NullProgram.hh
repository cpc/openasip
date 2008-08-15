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
 * @file NullProgram.hh
 *
 * Declaration of NullProgram class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_PROGRAM_HH
#define TTA_NULL_PROGRAM_HH

#include "Program.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null program.
 *
 * Calling any method causes the program to abort.
 */
class NullProgram : public Program {
public:
    virtual ~NullProgram();
    static NullProgram& instance();

    GlobalScope& globalScope();
    const GlobalScope& globalScopeConst() const;

    TTAMachine::Machine& targetProcessor() const;

    Address startAddress() const;
    Address entryAddress() const;
    void setEntryAddress(Address address);

    void addProcedure(Procedure& proc) throw (IllegalRegistration);
    void addInstruction(Instruction& ins) throw (IllegalRegistration);

    void relocate(const Procedure& proc, UIntWord howMuch);

    Procedure& firstProcedure() const throw (InstanceNotFound);
    Procedure& lastProcedure() const throw (InstanceNotFound);
    Procedure& nextProcedure(const Procedure& proc) const
        throw (IllegalRegistration);
    int procedureCount() const;
    Procedure& procedure(int index) const throw (OutOfRange);
    Procedure& procedure(const std::string& name) const
        throw (KeyNotFound);

    Instruction& firstInstruction() const throw (InstanceNotFound);
    const Instruction& instructionAt(UIntWord address) const
        throw (KeyNotFound);
    const Instruction& nextInstruction(const Instruction&) const
        throw (IllegalRegistration);
    Instruction& lastInstruction() const throw (InstanceNotFound);

    InstructionReferenceManager& instructionReferenceManager();

protected:
    NullProgram();

private:
    /// Copying not allowed.
    NullProgram(const NullProgram&);
    /// Assignment not allowed.
    NullProgram& operator=(const NullProgram&);

    /// Unique instance of NullProgram.
    static NullProgram instance_;
};

}

#endif
