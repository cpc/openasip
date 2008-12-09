/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file DefaultCompressor.hh
 *
 * Declaration and implementation of DefaultCompressor class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "CodeCompressor.hh"
#include "Program.hh"
#include "NullInstruction.hh"

using TTAProgram::Instruction;
using TTAProgram::NullInstruction;

class DEFAULT_Compressor : public CodeCompressorPlugin { 
public: 
    DEFAULT_Compressor() : CodeCompressorPlugin() {}

    virtual InstructionBitVector* compress(std::string& programName) 
        throw (InvalidData) {

        try {
            startNewProgram(programName);
            setAllInstructionsToStartAtBeginningOfMAU();
            Instruction* instruction = &currentProgram().firstInstruction();
            while (instruction != &NullInstruction::instance()) {
                InstructionBitVector* instructionBits = bemInstructionBits(
                    *instruction);
                addInstruction(*instruction, instructionBits);
                instruction = 
                    &currentProgram().nextInstruction(*instruction);
            }

            return programBits();
        } catch (const Exception& e) {
            throw InvalidData(
                __FILE__, __LINE__, __func__, e.errorMessage());
        }
    }

    virtual void printDescription(std::ostream& stream) {
        stream << "Does not compress instructions at all." << std::endl;
    }

    virtual void generateDecompressor(std::ostream&) {}
};

EXPORT_CODE_COMPRESSOR(DEFAULT_Compressor)
