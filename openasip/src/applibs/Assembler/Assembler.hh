/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file Assembler.hh
 *
 * Declaration of Assembler class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
 *
 * @note rating yellow
 */

#ifndef TCEASM_ASSEMBLER_HH
#define TCEASM_ASSEMBLER_HH

#include "Exception.hh"
#include "ParserStructs.hh"
#include "AssemblyParserDiagnostic.hh"

namespace TPEF {
    class BinaryStream;
    class Binary;
}

namespace TTAMachine {
    class Machine;
}

/**
 * TCE Assembler user interface.
 */
class Assembler {
public:
    Assembler(
        TPEF::BinaryStream& assemblerFile,
        TTAMachine::Machine& assemblerMachine);

    TPEF::Binary* compile();

    const std::set<CompilerMessage>& warnings() const;

    const CompilerMessage& error() const;

    void addWarning(UValue lineNumber, std::string errorMessage);

    const TTAMachine::Machine& targetMachine() const;

private:
    void readAssemblerCode();

    std::string codeLine(UValue lineNumber) const;

    /// File containing file to assemble.
    TPEF::BinaryStream& stream_;

    /// Machine for assembled TPEF.
    TTAMachine::Machine* mach_;

    /// Last thrown error message
    CompilerMessage error_;

    /// Parser messages.
    AssemblyParserDiagnostic parserDiagnostic_;

    /// String containing code to compile.
    std::shared_ptr<std::string> asmCode_;
};

#endif
