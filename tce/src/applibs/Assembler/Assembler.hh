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
 * @file Assembler.hh
 *
 * Declaration of Assembler class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating yellow
 */

#ifndef TCEASM_ASSEMBLER_HH
#define TCEASM_ASSEMBLER_HH

#include "Exception.hh"
#include "ParserStructs.hh"

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
    /**
     * Compiler error and warning message type.
     */
    struct CompilerMessage {
        std::string message;       ///< Message.
        std::string assemblerLine; ///< Assembly code line number.
        UValue lineNumber;         ///< Message generation line number.
    };

    Assembler(
        TPEF::BinaryStream& assemblerFile,
        TTAMachine::Machine& assemblerMachine);

    TPEF::Binary* compile() throw (CompileError);

    UValue warningCount() const;

    const CompilerMessage& warning(UValue index) const;

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

    /// Warning messages.
    std::vector<CompilerMessage> warnings_;

    /// New line start positions in assembly file.
    std::vector<UValue> lineStarts_;

    /// String containing code to compile.
    std::string asmCode_;
};

#endif
