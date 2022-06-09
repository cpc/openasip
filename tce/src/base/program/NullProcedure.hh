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

    Program& parent() const;
    void setParent(Program& prog);
    bool isInProgram() const;
    std::string name() const;
    int alignment() const;

    Address address(const Instruction& ins) const;

    Address startAddress() const;
    void setStartAddress(Address start);
    Address endAddress() const;

    int instructionCount() const;
    Instruction& firstInstruction() const;
    Instruction& instructionAt(UIntWord address) const;
    bool hasNextInstruction(const Instruction& ins) const;
    Instruction& nextInstruction(const Instruction& ins) const;
    Instruction& previousInstruction(const Instruction& ins) const;
    Instruction& lastInstruction() const;

    void addInstruction(Instruction& ins);
    void insertInstructionAfter(const Instruction& pos, Instruction* ins);

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
