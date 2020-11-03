/*
    Copyright (c) 2002-2009 Tampere University.

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

#include "TCEString.hh"
#include "Exception.hh"
#include "Address.hh"
#include "CodeSnippet.hh"
#include "Application.hh"

class TCEString;

namespace TTAProgram {

class Instruction;
class Program;


/**
 * Represents a TTA procedure.
 */
class Procedure : public CodeSnippet {
public:
    Procedure(
        const TCEString& name,
        const TTAMachine::AddressSpace& space);
    Procedure(
        const TCEString& name,
        const TTAMachine::AddressSpace& space,
        UIntWord startLocation);
    virtual ~Procedure();

    TCEString name() const { return name_; }

    int alignment() const;

    Address address(const Instruction& ins) const;

    void
    addFront(Instruction*) {
        abortWithError("Not Implemented yet.");
    }

    void add(Instruction* ins);
    void insertAfter(const Instruction& pos, Instruction* ins);
    void insertBefore(const Instruction& pos, Instruction* ins);

    void clear();

    using CodeSnippet::insertAfter;
    using CodeSnippet::insertBefore;

    void remove(Instruction& ins);

    CodeSnippet* copy() const;

private:
    /// Copying not allowed.
    Procedure(const Procedure&);
    /// Assignment not allowed.
    Procedure& operator=(const Procedure&);

    /// The name of the procedure.
    const TCEString name_;
    /// The alignment of instructions.
    int alignment_;
    /// The default alignment of instructions.
    static const int INSTRUCTION_INDEX_ALIGNMENT;
};

#include "Procedure.icc"

}

#endif
