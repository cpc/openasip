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
 * @file InstructionDictionary.cc
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
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2009 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <boost/format.hpp>

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

/**
 * Implements a simple instruction based dictionary compression scheme.
 */
class InstructionDictionary : public CodeCompressorPlugin { 
public:
    
    /**
     * The constructor.
     */
    InstructionDictionary() :
        CodeCompressorPlugin(), compatibilityProgDone_(false),
        dictionaryCreated_(false) {
    }
    

    /**
     * Creates the compressed code and returns the bit vector of it.
     */
    virtual InstructionBitVector*
    compress(const std::string& programName) 
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
        if (!dictionaryCreated_) {
            createDictionary();

            unsigned int compressedImemWidth = 
                MathTools::requiredBits(dictionary_.size());
            assert(compressedImemWidth <= sizeof(long long unsigned int)*8
                   && "Compressed instruction width is too big");

            // fix imem width (mau == instruction width)
            setImemWidth(compressedImemWidth);
        }
        startNewProgram(programName);
        setAllInstructionsToStartAtBeginningOfMAU();
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
    virtual void 
    generateDecompressor(std::ostream& stream) {

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
        stream << "use work.dict_init.all;" << endl;
        stream << "use work.imem_mau.all;" << endl << endl;
     
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
    virtual void 
    printDescription(std::ostream& stream) {
        stream  << "Generates the program image using instruction-based "
                << "dictionary compression." << endl << endl 
                << "Warning! This compressor works correctly only when "
                << "there is one instruction per MAU in the final program "
                << "image. That is, the minimum addressable unit of the "
                << "address space should be "
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
     * Creates the whole dictionary
     *
     */
    void 
    createDictionary() {
        for (int i = 0; i < numberOfPrograms(); i++) {
            TPEFMap::const_iterator iter = programElement(i);
            string name = iter->first;
            startNewProgram(name);
            setAllInstructionsToStartAtBeginningOfMAU();
            updateDictionary(currentProgram());
        }
        dictionaryCreated_ = true;
        if (Application::verboseLevel() > 0 && dictionary_.size() > 0) {
            std::size_t keyWidth = 
                MathTools::requiredBits(dictionary_.size() - 1);
            std::size_t entrySize = binaryEncoding().width();
            std::size_t entries = dictionary_.size();
            Application::logStream() 
                << (boost::format(                    
                        "dictionary width: %d bits, entries: %d, "
                        "total size: %d bits (%d bytes)\n" )
                    % keyWidth % entries % (entries * entrySize)
                    % std::size_t(std::ceil(entries * entrySize / 8.0))).str();
        }
    }

    /**
     * Adds the given instruction bits to the dictionary.
     *
     * @param instructionBits The instruction bits to add.
     */
    void 
    addToDictionary(const BitVector& instructionBits) {
        if (!MapTools::containsKey(dictionary_, instructionBits)) {
            unsigned int code = dictionary_.size();
            dictionary_.insert(
                std::pair<BitVector, unsigned int>(instructionBits, code));
        }
    }

    
    /**
     * Creates the dictionary.
     */
    void 
    updateDictionary(const Program& program) {        
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
    void 
    addInstructions() {
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



    /// The dictionary.
    Dictionary dictionary_;
    /// Indicates whether the compatibility program is in dictionary.
    bool compatibilityProgDone_;
    /// Indicates whether the whole dictionary has been created
    bool dictionaryCreated_;

};

EXPORT_CODE_COMPRESSOR(InstructionDictionary)
