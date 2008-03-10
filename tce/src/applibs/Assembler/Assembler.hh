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
