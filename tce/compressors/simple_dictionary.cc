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
 * @file simple_dictionary.cc
 *
 * Implementation of a simple dictionary compressor.  Warning! This
 * compressor works correctly only when there is one instruction per
 * MAU in the final program image. That is, the MAU of the address
 * space should be the same as the width of the compressed
 * instructions or wider. Otherwise jump and call addresses are
 * invalid in the code.
 *
 * This compressor creates the dictionary on the level of whole instruction.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include <iostream>

#include "CodeCompressor.hh"
#include "Program.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "ImmediateSlotField.hh"
#include "ImmediateControlField.hh"
#include "InstructionBitVector.hh"
#include "NullInstruction.hh"
#include "AsciiImageWriter.hh"
#include "ProgrammabilityValidator.hh"
#include "ProgrammabilityValidatorResults.hh"
#include "Binary.hh"
#include "TPEFProgramFactory.hh"
#include "MapTools.hh"
#include "MathTools.hh"

using std::vector;
using std::string;
using std::endl;
using TTAProgram::Program;
using TTAProgram::Instruction;
using TTAProgram::NullInstruction;
using namespace TPEF;

const string ENSURE_PROGRAMMABILITY = "ensure_programmability";
const string YES = "yes";

class SimpleDictionary : public CodeCompressorPlugin { 
public:
    
    /**
     * The constructor.
     */
    SimpleDictionary() :
        CodeCompressorPlugin(), compatibilityProgDone_(false) {
    }
    

    /**
     * Creates the compressed code and returns the bit vector of it.
     */
    virtual InstructionBitVector* compress(TPEF::Binary& program) 
        throw (InvalidData) {

        try {
            if (hasParameter(ENSURE_PROGRAMMABILITY) &&
                parameterValue(ENSURE_PROGRAMMABILITY) == YES && 
                !compatibilityProgDone_) {
                ProgrammabilityValidator validator(machine());
                ProgrammabilityValidatorResults results;
                Binary* compatibilityBin = validator.profile(results);
                TPEFProgramFactory factory(*compatibilityBin, machine());
                Program* compatibilityProg = factory.build();
                updateDictionary(*compatibilityProg);
                delete compatibilityBin;
                delete compatibilityProg;
                compatibilityProgDone_ = true;
            }
        } catch (const Exception& e) {
            string errorMsg = "Unable to ensure programmability: " + 
                e.errorMessage();
            throw InvalidData(
                __FILE__, __LINE__, __func__, errorMsg);
        }            

        startNewProgram(program);
        setAllInstructionsToStartAtBeginningOfMAU();
        updateDictionary(currentProgram());
        addInstructions();
        return programBits();
    }

    
    /**
     * Generates the decompressor in VHDL.
     *
     * Note! The programs must be compressed by compress method before
     * calling this method.
     *
     * @param stream The stream to write.
     */
    virtual void generateDecompressor(std::ostream& stream) {

        stream << "library ieee;" << endl;
        stream << "use ieee.std_logic_1164.all;" << endl;
        stream << "use ieee.std_logic_arith.all;" << endl << endl;
        
        stream << "package dict_init is" << endl << endl;
        stream << indentation(1)
               << "type std_logic_dict_matrix is array (natural range <>) "
               << "of std_logic_vector(" << binaryEncoding().width() - 1
               << " downto 0);" << endl << endl;
        
        // write the dictionary contents
        stream << indentation(1)
               << "constant dict_init : std_logic_dict_matrix := (" << endl;
        for (unsigned int i = 0; i < dictionary_.size(); i++) {
            BitVector instr = MapTools::keyForValue<BitVector>(
                dictionary_, i);
            AsciiImageWriter writer(instr, instr.size());
            stream << indentation(2) << "\"";
            writer.writeImage(stream);
            stream << "\"";
            if (i+1 < dictionary_.size()) {
                stream << "," << endl;
            } else {
                stream << ");" << endl;
            }
        }
        stream << "end dict_init;" << endl << endl;

        stream << "library ieee;" << endl;
        stream << "use ieee.std_logic_1164.all;" << endl;
        stream << "use ieee.std_logic_arith.all;" << endl;
        stream << "use work.globals.all;" << endl;
        stream << "use work.dict_init.all;" << endl << endl;
     
        // write the decompressor entity
        stream << "entity decompressor is" << endl;
        stream << indentation(1) << "port (" << endl;
        stream << indentation(2) << "fetch_en : out std_logic;" << endl;
        stream << indentation(2) << "lock : in std_logic;" << endl;
        stream << indentation(2)
               << "fetchblock : in std_logic_vector("
               << "IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0);" << endl;
        stream << indentation(2)
               << "instructionword : out std_logic_vector("
               << "INSTRUCTIONWIDTH-1 downto 0);" << endl;
        stream << indentation(2) << "glock : out std_logic;" << endl;
        stream << indentation(2) << "lock_r : in std_logic;" << endl;
        stream << indentation(2) << "clk : in std_logic;" << endl;
        stream << indentation(2) << "rstx : in std_logic);" << endl << endl;
        stream << "end decompressor;" << endl << endl;

        stream << "architecture simple_dict of decompressor is"
               << endl << endl;

        stream << indentation(1)
               << "subtype dict_index is integer range 0 to "
               << "dict_init'length-1;" << endl;
        stream << indentation(1) << "signal dict_line : dict_index;" << endl;
        stream << indentation(1)
               << "constant dict : std_logic_dict_matrix("
               << "0 to dict_init'length-1) := dict_init;" << endl << endl;

        stream << "begin" << endl << endl;
        stream << indentation(1) << "glock <= lock;" << endl;
        stream << indentation(1) << "fetch_en <= not lock_r;" << endl;
        stream << indentation(1)
               << "dict_line <= conv_integer(unsigned(fetchblock("
               << "fetchblock'length-1 downto fetchblock'length-"
               << MathTools::requiredBits(dictionary_.size()-1)
               << ")));" << endl << endl;
        
        stream << indentation(1) << "process (dict_line)" << endl;
        stream << indentation(1) << "begin" << endl;
        stream << indentation(2) << "instructionword <= dict(dict_line);"
               << endl;
        stream << indentation(1) << "end process;" << endl << endl;
        stream << "end simple_dict;" << endl;
    }


    /**
     * Prints the description of the plugin to the given stream.
     *
     * @param stream The stream.
     */
    virtual void printDescription(std::ostream& stream) {
        stream  << "Generates the program image using dictionary "
                << "compression." << endl << endl 
                << "Warning! This compressor works correctly only when "
                << "there is one instruction per MAU in the final program "
                << "image. That is, the MAU of the address space should be "
                << "the same as the width of the compressed instructions or "
                << "wider. Otherwise jump and call addresses are invalid in "
                << "the code. This compressor creates the dictionary on the "
                << "level of whole instruction." << endl << endl
                << "Parameters accepted:" << endl
                << "----------------------" << endl
                << "ensure_programmability" << endl
                << "If the value is 'yes', instructions that ensure "
                << "programmability of the processor are added to the "
                << "dictionary automatically." << endl;
    }
    
private:
    /// Map type for dictionary.
    typedef std::map<BitVector, unsigned int> Dictionary;


    /**
     * Adds the given instruction bits to the dictionary.
     *
     * @param instructionBits The instruction bits to add.
     */
    void addToDictionary(const BitVector& instructionBits) {
        if (!MapTools::containsKey(dictionary_, instructionBits)) {
            unsigned int code = dictionary_.size();
            dictionary_.insert(
                std::pair<BitVector, unsigned int>(instructionBits, code));
        }
    }

    
    /**
     * Creates the dictionary.
     */
    void updateDictionary(const Program& program) {        
        Instruction* instruction = &program.firstInstruction();
        while (instruction != &NullInstruction::instance()) {
            InstructionBitVector* instructionBits = bemInstructionBits(
                *instruction);
            addToDictionary(*instructionBits);
            instruction = &program.nextInstruction(*instruction);
        }   
    }


    /**
     * Adds the compressed instructions to the program.
     */
    void addInstructions() {
        Instruction* instruction = &currentProgram().firstInstruction();
        while (instruction != &NullInstruction::instance()) {
            InstructionBitVector* bemBits = bemInstructionBits(*instruction);
            unsigned int code = MapTools::valueForKey<unsigned int>(
                dictionary_, *bemBits);
            InstructionBitVector* compressedInstruction = 
                new InstructionBitVector();
            static_cast<BitVector*>(compressedInstruction)->pushBack(
                code, static_cast<int>(
                    MathTools::requiredBits(dictionary_.size() - 1)));
            addInstruction(*instruction, compressedInstruction);
            instruction = &currentProgram().nextInstruction(*instruction);
        }
    }


    /**
     * Returns the indentation string of the given level.
     *
     * @param level The indentation level.
     */
    static std::string indentation(int level) {
        string ind;
        for (int i = 0; i < level; i++) {
            ind += "  ";
        }
        return ind;
    }

    /// The dictionary.
    Dictionary dictionary_;
    /// Indicates whether the compatibility program is in dictionary.
    bool compatibilityProgDone_;

};

EXPORT_CODE_COMPRESSOR(SimpleDictionary)
