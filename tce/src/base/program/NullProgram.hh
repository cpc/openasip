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
 * @file NullProgram.hh
 *
 * Declaration of NullProgram class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
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

    void addProcedure(Procedure& proc);
    void addInstruction(Instruction& ins);

    void relocate(const Procedure& proc, UIntWord howMuch);

    Procedure& firstProcedure() const;
    Procedure& lastProcedure() const;
    Procedure& nextProcedure(const Procedure& proc) const;
    int procedureCount() const;
    Procedure& procedure(int index) const;
    Procedure& procedure(const std::string& name) const;

    Instruction& firstInstruction() const;
    const Instruction& instructionAt(UIntWord address) const;
    const Instruction& nextInstruction(const Instruction&) const;
    Instruction& lastInstruction() const;

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
