/**
 * @file Assembler.cc
 *
 * Definition of Assembler class.
 *
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 *
 * @note rating: yellow
 */

#include "Assembler.hh"
#include "AssemblerParser.hh"

#include "Machine.hh"

#include "BinaryStream.hh"
#include "Binary.hh"

using TPEF::Binary;
using TPEF::BinaryStream;

using TTAMachine::Machine;

/**
 * Constructor.
 *
 * @param assemblerFile Stream containing file to assemble.
 * @param assemblerMachine Machine to use for running compiled binary.
 */
Assembler::Assembler(
    BinaryStream& assemblerFile,
    Machine& assemblerMachine) :
    stream_(assemblerFile), mach_(&assemblerMachine) {
}

/**
 * Compiles assembler and returns TPEF hierarchy.
 *
 * @return Compiled TPEF.
 * @exception CompileError If there was any problems during the compilation.
 */
Binary*
Assembler::compile()
    throw (CompileError) {

    // clear old warnings from previous compilation
    warnings_.clear();

    // read binary stream to string
    readAssemblerCode();

    Binary* newBinary = new Binary();

    // this one does the actual parsing work
    AssemblerParser parser(*newBinary, *mach_, this);

    try {
        try {
            if (!parser.compile(asmCode_)) {

                CompileError error(
                    __FILE__, __LINE__, __func__, "Syntax error.");

                error.setCodeFileLineNumber(parser.errorLine());

                throw error;
            }

        // ObjectAllreadyExists and IllegalMachine exceptions.
        } catch (Exception& e) {

            CompileError error(
                __FILE__, __LINE__, __func__, e.errorMessage());

            error.setCause(e);
            error.setCodeFileLineNumber(parser.errorLine());

            throw error;
        }

        parser.finalize();

    } catch (CompileError& e) {

        error_.lineNumber = e.codeFileLineNumber();
        error_.assemblerLine = codeLine(e.codeFileLineNumber());
        error_.message = e.errorMessage();

        // clean up internals of creators and managers and delete 
        // partial binary
        parser.cleanup();

        delete newBinary;
        newBinary = NULL;

        CompileError error(
            __FILE__, __LINE__, __func__,
            "Error in line " + Conversion::toString(error_.lineNumber) +
            ": " + error_.assemblerLine + "\nMessage: " + error_.message);

        error.setCause(e);
        error.setCodeFileLineNumber(error_.lineNumber);

        throw error;
    }

    return newBinary;
}

/**
 * Adds warning message for client to see.
 *
 * @param lineNumber Line number of assembly code, where error accured.
 * @param errorMessage Warning message.
 */
void
Assembler::addWarning(UValue lineNumber, std::string errorMessage) {
    // add new warning to right position of warning list
    CompilerMessage newWarning;
    newWarning.lineNumber = lineNumber;
    newWarning.message = errorMessage;
    newWarning.assemblerLine = codeLine(lineNumber);

    for (unsigned int i = 0; i < warnings_.size(); i++) {
        if (warnings_[i].lineNumber > lineNumber) {
            warnings_.push_back(newWarning);

            for (unsigned int j = warnings_.size() - 2;
                 j != i; j--) {

                warnings_[j+1] = warnings_[j];
            }

            warnings_[i] = newWarning;

            return;
        }
    }

    warnings_.push_back(newWarning);
}

/**
 * Returns the target machine of the assembler.
 *
 * @return The target machine.
 */
const TTAMachine::Machine&
Assembler::targetMachine() const {
    return *mach_;
}


/**
 * Returns number of stored warnings.
 *
 * @return Number of stored warnings.
 */
UValue
Assembler::warningCount() const {
    return warnings_.size();
}

/**
 * Returns warning of requested index.
 *
 * @param index Warning index to return.
 * @return Warning message and line number.
 */
const Assembler::CompilerMessage&
Assembler::warning(UValue index) const {
    assert(index < warningCount());
    return warnings_[index];
}

/**
 * Returns last thrown error message..
 *
 * @return Error that halted compilation.
 */
const Assembler::CompilerMessage&
Assembler::error() const {
    return error_;
}

/**
 * Reads assembly code from binary stream to a string and
 * stores start positions of new lines of assembly code.
 */
void
Assembler::readAssemblerCode() {
    // first line starts from index zero
    lineStarts_.push_back(0);

    unsigned int streamPosition = stream_.readPosition();

    // read file to string and mark line start indexes.
    for (unsigned int i = streamPosition; i < stream_.sizeOfFile(); i++) {
        Byte readByte = stream_.readByte();

        // next is line start
        if (readByte == '\n') {
            lineStarts_.push_back(i - streamPosition + 1);
        }

        asmCode_ += readByte;
    }

    // restore stream position...
    stream_.setReadPosition(streamPosition);
}

/***
 * Returns requested code line.
 *
 * @param Line number to return.
 * @return Requested assembly code line.
 */

std::string
Assembler::codeLine(UValue lineNumber) const {

    std::string errorLine;

    unsigned int errorLineNum =
        static_cast<unsigned int>(lineNumber - 1);

    if (errorLineNum < lineStarts_.size()) {

        std::string::size_type startPos =
            lineStarts_[errorLineNum];

        std::string::size_type endPos = asmCode_.find('\n', startPos);

        // no line feed at end of file
        if (endPos == std::string::npos) {
            endPos = asmCode_.length();
        }

        errorLine = asmCode_.substr(startPos, endPos - startPos);

    } else {
        errorLine = "Invalid line number info, probably last line of file.";
    }

    return errorLine;
}

