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
 * @file Procedure.hh
 *
 * Declaration of Procedure class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROCEDURE_HH
#define TTA_PROCEDURE_HH

#include <vector>
#include <string>

#include "Exception.hh"
#include "Address.hh"
#include "CodeSnippet.hh"

namespace TTAProgram {

class Instruction;
class Program;


/**
 * Represents a TTA procedure.
 */
class Procedure : public CodeSnippet {

    public:
        Procedure(const std::string& name,
              const TTAMachine::AddressSpace& space);
    Procedure(
        const std::string& name,
        const TTAMachine::AddressSpace& space,
        UIntWord startLocation);
    virtual ~Procedure();

    std::string name() const;

    int alignment() const;

    Address address(const Instruction& ins) const
        throw (IllegalRegistration);

    void addFront(Instruction*) 
        throw (IllegalRegistration) {
        abortWithError("Not Implemented yet.");
    }

    void add(Instruction* ins) 
        throw (IllegalRegistration);
    void insertAfter(const Instruction& pos, Instruction* ins)
        throw (IllegalRegistration);
    void insertBefore(const Instruction& pos, Instruction* ins)
        throw (IllegalRegistration);

    using CodeSnippet::insertAfter;
    using CodeSnippet::insertBefore;
    
    void remove(Instruction& ins) 
        throw (IllegalRegistration);

    CodeSnippet* copy() const;

private:
    /// Copying not allowed.
    Procedure(const Procedure&);
    /// Assignment not allowed.
    Procedure& operator=(const Procedure&);

    /// The name of the procedure.
    const std::string name_;
    /// The alignment of instructions.
    int alignment_;
    /// The default alignment of instructions.
    static const int INSTRUCTION_INDEX_ALIGNMENT;
};

#include "Procedure.icc"

}

#endif
