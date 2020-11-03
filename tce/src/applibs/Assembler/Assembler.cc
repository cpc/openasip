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
 * @file Assembler.cc
 *
 * Definition of Assembler class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
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
Assembler::compile() {
    // read binary stream to string
    readAssemblerCode();

    // clear old warnings from previous compilation
    parserDiagnostic_.reset(asmCode_);

    Binary* newBinary = new Binary();

    // this one does the actual parsing work
    AssemblerParser parser(*newBinary, *mach_, &parserDiagnostic_);

    try {
        try {
            if (!parser.compile(*asmCode_)) {

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

        parser.finalize(mach_->isLittleEndian());

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
    parserDiagnostic_.addWarning(lineNumber, errorMessage);
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

const std::set<CompilerMessage>&
Assembler::warnings() const {
    return parserDiagnostic_.warnings();
}

/**
 * Returns last thrown error message..
 *
 * @return Error that halted compilation.
 */
const CompilerMessage&
Assembler::error() const {
    return error_;
}

/**
 * Reads assembly code from binary stream to a string.
 */
void
Assembler::readAssemblerCode() {
    unsigned int streamPosition = stream_.readPosition();

    asmCode_ = std::make_shared<std::string>();

    // read file to string and mark line start indexes.
    for (unsigned int i = streamPosition; i < stream_.sizeOfFile(); i++) {
        Byte readByte = stream_.readByte();
        *asmCode_ += readByte;
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
    return parserDiagnostic_.codeLine(lineNumber);
}

