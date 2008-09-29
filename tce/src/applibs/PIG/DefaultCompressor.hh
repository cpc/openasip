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

    virtual InstructionBitVector* compress(TPEF::Binary& program) 
        throw (InvalidData) {

        try {
            startNewProgram(program);
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
