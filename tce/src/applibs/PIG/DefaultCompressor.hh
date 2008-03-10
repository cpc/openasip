/**
 * @file DefaultCompressor.hh
 *
 * Declaration and implementation of DefaultCompressor class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
