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
 * @file NullProcedure.hh
 *
 * Declaration of NullProcedure class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_PROCEDURE_HH
#define TTA_NULL_PROCEDURE_HH

#include "BaseType.hh"
#include "Procedure.hh"
#include "Address.hh"

namespace TTAProgram {

class Instruction;
class CodeSnippet;

/**
 * A singleton class that represents a null procedure.
 *
 * Calling any method causes the program to abort.
 */
class NullProcedure : public Procedure {
public:
    virtual ~NullProcedure();
    static NullProcedure& instance();

    Program& parent() const throw (IllegalRegistration);
    void setParent(const Program& prog);
    bool isInProgram() const;
    std::string name() const;
    int alignment() const;

    Address address(const Instruction& ins) const
        throw (IllegalRegistration);

    Address startAddress() const;
    void setStartAddress(Address start);
    Address endAddress() const;

    int instructionCount() const;
    Instruction& firstInstruction() const throw (InstanceNotFound);
    Instruction& instructionAt(UIntWord address) const
        throw (KeyNotFound);
    bool hasNextInstruction(const Instruction& ins) const
        throw (IllegalRegistration);
    Instruction& nextInstruction(const Instruction& ins) const
        throw (IllegalRegistration);
    Instruction& previousInstruction(const Instruction& ins) const
        throw (IllegalRegistration);
    Instruction& lastInstruction() const throw (IllegalRegistration);

    void addInstruction(Instruction& ins) throw (IllegalRegistration);
    void insertInstructionAfter(const Instruction& pos, Instruction* ins)
        throw (IllegalRegistration);

protected:
    NullProcedure();

private:
    /// Copying not allowed.
    NullProcedure(const NullProcedure&);
    /// Assignment not allowed.
    NullProcedure& operator=(const NullProcedure&);

    /// Unique instance of NullProcedure.
    static NullProcedure instance_;
};

}

#endif
