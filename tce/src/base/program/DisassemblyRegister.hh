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
 * @file DisassemblyRegister.hh
 *
 * Declaration of DisassemblyRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_REGISTER_HH
#define TTA_DISASSEMBLY_REGISTER_HH

#include <string>
#include "DisassemblyElement.hh"
#include "BaseType.hh"
#include "TCEString.hh"

namespace TTAMachine {
    class RegisterFile;
}

namespace TTAProgram {
    class Terminal;
}
/**
 * Represents a register in the disassembler.
 */
class DisassemblyRegister : public DisassemblyElement {
public:
    DisassemblyRegister(std::string rfName, Word index);
    virtual ~DisassemblyRegister();
    std::string toString() const;

    static TCEString registerName(
        const TTAMachine::RegisterFile& rf,
        int index, 
        char delim = '.');

    static TCEString registerName(
        const TTAProgram::Terminal& term);

    static TCEString registerName(
        const TCEString& registerFileName,
        int index, 
        char delim = '.');

private:
    /// Name of the register file.
    std::string rfName_;
    /// Index of the register in the register file.
    Word index_;
};

#endif
