/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file MoveSlotDictionary.cc
 *
 * Implementation of move slot dictionary compressor.  Warning! This
 * compressor works correctly only when there is one instruction per
 * MAU in the final program image. That is, the MAU of the address
 * space should be the same as the width of the compressed
 * instructions or wider. Otherwise jump and call addresses are
 * invalid in the code.
 *
 * This compressor creates the dictionary on move slot level. Furthermore
 * it uses so called vertical compression which creates a separate dictionary
 * for each move slot.
 *
 * @author Otto Esko 2009 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <boost/format.hpp>

#include "CodeCompressor.hh"
#include "CodeCompressorPlugin.hh"
#include "Program.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "ImmediateSlotField.hh"
#include "ImmediateControlField.hh"
#include "InstructionBitVector.hh"
#include "NullInstruction.hh"
#include "AsciiImageWriter.hh"
#include "MapTools.hh"
#include "MathTools.hh"

using std::vector;
using std::string;
using std::endl;
using std::pair;

using TTAProgram::Program;
using TTAProgram::Instruction;
using TTAProgram::NullInstruction;
using namespace TPEF;


class MoveSlotDictionary : public CodeCompressorPlugin {
public:

    /**
     * The constructor
     */
    MoveSlotDictionary(): CodeCompressorPlugin(), dictionaryCreated_(false),
                          compressedWidth_(0) {
    }

    /**
     * The destructor
     */
    ~MoveSlotDictionary() {
        for (unsigned int i = 0; i < dictionary_.size(); i++) {
            if (dictionary_.at(i) != NULL) {
                delete dictionary_.at(i);
            }
        }   
    }

    /**
     * Creates compressed code of the program and returns it in bit vector
     */
    virtual InstructionBitVector*
    compress(const string& programName)
        throw (InvalidData) {

        if (!dictionaryCreated_) {
            createDictionary();
            // fix imem width (mau == instruction width)
            int imemWidth = 0;
            for (unsigned int i = 0; i < dictionary_.size(); i++) {
                // add dictionary sizes
                imemWidth += 
                    MathTools::requiredBits(dictionary_.at(i)->size());
            }
            // add limm fields, if any
            imemWidth += firstMoveSlotIndex();
            compressedWidth_ = imemWidth;
            setImemWidth(compressedWidth_);

            if (Application::verboseLevel() > 0 && dictionary_.size() > 0) {
                printDetails();
            }
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
    generateDecompressor(std::ostream& stream, TCEString entityStr) {
        generateDictionaryVhdl(stream, entityStr);
        generateDecompressorEntity(stream, entityStr);
        generateDecompressorArchitecture(stream, entityStr);
    }
    
    /**
     * Prints the description of the plugin to the given stream.
     *
     * @param stream The stream.
     */
    virtual void 
    printDescription(std::ostream& stream) {
        stream  << "Generates the program image using move slot based "
                << "dictionary compression." << endl << endl 
                << "Warning! This compressor works correctly only when "
                << "there is one instruction per MAU in the final program "
                << "image. That is, the MAU of the address space should be "
                << "the same as the width of the compressed instructions or "
                << "wider. Otherwise jump and call addresses are invalid in "
                << "the code. This compressor creates the dictionary on the "
                << "move slot level." << endl << endl;
    }

private:
    
    /**
     * Creates the whole dictionary
     *
     */
    void 
    createDictionary() {
        for (int i = 0; i < moveSlotCount(); i++) {
            dictionary_.push_back(new Dictionary);
        }

        for (int i = 0; i < numberOfPrograms(); i++) {
            TPEFMap::const_iterator iter = programElement(i);
            string name = iter->first;
            startNewProgram(name);
            setAllInstructionsToStartAtBeginningOfMAU();
            updateDictionary(currentProgram());
        }

        dictionaryCreated_ = true;
    }

    /**
     * Creates dictionary for one program.
     */
    void 
    updateDictionary(const Program& program) {
        Instruction* instruction = &program.firstInstruction();
        while (instruction != &NullInstruction::instance()) {
            InstructionBitVector* instructionBits = bemInstructionBits(
                *instruction);
            unsigned int begin = firstMoveSlotIndex();
            unsigned int end = begin;
            for (int i = 0; i < moveSlotCount(); i++) {
                end = begin + moveSlotWidth(i) - 1;
                BitVector moveSlot(*instructionBits, begin, end);
                addToDictionary(moveSlot, i);
                begin = end + 1;
            }
            instruction = &program.nextInstruction(*instruction);
            delete instructionBits;
        }   
    }

    /**
     * Adds the given instruction bits to the move slot dictionary.
     *
     * @param instructionBits The instruction bits to add.
     * @param slotIndex Index of the move slot
     */
    void 
    addToDictionary(const BitVector& instructionBits, int slotIndex) {
        if (!MapTools::containsKey(
                *(dictionary_.at(slotIndex)), instructionBits)) {
            unsigned int code = dictionary_.at(slotIndex)->size();
            dictionary_.at(slotIndex)->insert(
                std::pair<BitVector, unsigned int>(instructionBits, code));
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
            InstructionBitVector* compressedInstruction = 
                new InstructionBitVector();
            // Take a BitVector pointer to the compressed instruction because
            // we _need_ to use BitVector pushBack-methods!
            BitVector* compressPtr = 
                static_cast<BitVector*>(compressedInstruction);

            // handle limm fields, if any
            if (firstMoveSlotIndex() != 0) {
                for (int i = 0; i < firstMoveSlotIndex(); i++) {
                    // false is defined as 0, true is then != 0
                    compressPtr->pushBack(bemBits->at(i) != 0);
                }
            }
            unsigned int begin = firstMoveSlotIndex();
            unsigned int end = begin;
            for (int i = 0; i < moveSlotCount(); i++) {
                end = begin + moveSlotWidth(i)-1;
                BitVector moveSlot(*bemBits, begin, end);
                unsigned int code = MapTools::valueForKey<unsigned int>(
                    *(dictionary_.at(i)), moveSlot);
                // add move slot key to compressed instruction
                compressPtr->pushBack(
                    code, MathTools::requiredBits(dictionary_.at(i)->size()));

                begin = end + 1;
            }
            addInstruction(*instruction, compressedInstruction);
            instruction = &currentProgram().nextInstruction(*instruction);
            delete bemBits;
        }
    }

    void generateDictionaryVhdl(std::ostream& stream, TCEString entityStr) {
        stream << "library ieee;" << endl;
        stream << "use ieee.std_logic_1164.all;" << endl;
        stream << "use ieee.std_logic_arith.all;" << endl << endl;
        
        TCEString packageName = entityStr + "_dict_init";

        stream << "package " << packageName << " is" << endl << endl;

        for (int i = 0; i < moveSlotCount(); i++) {
            if (dictionary_.at(i)->size() > 1) {
                stream << indentation(1)
                       << "type std_logic_dict_matrix_" << i 
                       << " is array (natural range <>) "
                       << "of std_logic_vector(" << moveSlotWidth(i)-1
                       << " downto 0);" << endl << endl;
            }
        }

        // write the dictionary contents
        for (int i = 0; i < moveSlotCount(); i++) {
            if (dictionary_.at(i)->size() > 1) {
                stream << indentation(1)
                       << "constant dict_init_slot_" << i 
                       << " : std_logic_dict_matrix_" << i << " := (" << endl;
                for (unsigned int j = 0; j < dictionary_.at(i)->size(); j++) {
                    BitVector instr = MapTools::keyForValue<BitVector>(
                        *(dictionary_.at(i)), j);
                    AsciiImageWriter writer(instr, instr.size());
                    stream << indentation(2) << "\"";
                    writer.writeImage(stream);
                    stream << "\"";
                    if (j+1 < dictionary_.at(i)->size()) {
                        stream << "," << endl;
                    } else {
                        stream << ");" << endl;
                    }
                }
            } else {
                // there's always at least 1 instruction in dictionary
                unsigned int index = 0;
                BitVector instr = MapTools::keyForValue<BitVector>(
                        *(dictionary_.at(i)), index);
                AsciiImageWriter writer(instr, instr.size());
                stream << indentation(1)
                       << "constant dict_init_slot_" << i
                       << " : std_logic_vector("
                       << moveSlotWidth(i) << "-1 downto 0) := (" << endl
                       << indentation(2) << "\"";
                writer.writeImage(stream);
                stream << "\");" << endl; 
            }
            stream << endl;
        }
        stream << "end " << packageName << ";" << endl << endl;
    }

    void generateDecompressorEntity(std::ostream& stream, TCEString entityStr) {
        // write the decompressor entity
        stream << "library ieee;" << endl;
        stream << "use ieee.std_logic_1164.all;" << endl;
        stream << "use ieee.std_logic_arith.all;" << endl;
        stream << "use work." << entityStr << "_globals.all;" << endl;
        stream << "use work." << entityStr << "_dict_init.all;" << endl;
        stream << "use work." << entityStr << "_imem_mau.all;" << endl << endl;
        
        stream << "entity " << entityStr << "_decompressor is" << endl;
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
        stream << "end " << entityStr << "_decompressor;" << endl << endl;
    }

    void generateDecompressorArchitecture(
        std::ostream& stream, TCEString entityStr) {
        stream << "architecture move_slot_dict of " << entityStr 
               << "_decompressor is" << endl << endl;

        bool haveLimm = false;
        generateDecompressorSignals(stream,haveLimm);
        
        generateDecompressorBody(stream, haveLimm);
        
        stream << "end move_slot_dict;" << endl;
    }

    void generateDecompressorSignals(std::ostream& stream, bool& haveLimm) {
        // signal types & signals for dictionaries
        for (int i = 0; i < moveSlotCount(); i++) {
            if (dictionary_.at(i)->size() > 1) {
                stream << indentation(1)
                       << "subtype dict_index_"<< i 
                       <<" is integer range 0 to "
                       << "dict_init_slot_" << i 
                       << "'length-1;" << endl;
                stream << indentation(1)
                       << "signal dict_line_" 
                       << i <<" : dict_index_" << i << ";" << endl;
                stream << indentation(1)
                       << "constant dict_" << i 
                       << " : std_logic_dict_matrix_"
                       << i << "(0 to dict_init_slot_" << i 
                       << "'length-1) := dict_init_slot_"
                       << i << ";" << endl << endl;
            } else {
                stream << indentation(1)
                       << "constant dict_" << i
                       << " : std_logic_vector("
                       << moveSlotWidth(i)
                       << "-1 downto 0) := dict_init_slot_"
                       << i << ";" << endl << endl;
            }
        }
        // handle limm fields if present
        int limmEndIndex = 0;
        if (firstMoveSlotIndex() != 0) {
            haveLimm = true;
            limmEndIndex = firstMoveSlotIndex();
            stream << indentation(1)
                   << "signal limm_field : std_logic_vector("
                   << limmEndIndex << "-1 downto 0);" << endl << endl; 
        }
    }
    
    void generateDecompressorBody(std::ostream& stream, bool& haveLimm) {
        stream << indentation(1) << "begin" << endl << endl;
        stream << indentation(1) << "glock <= lock;" << endl;
        stream << indentation(1) << "fetch_en <= not lock_r;" << endl << endl;

        
        if (haveLimm) {
            int limmEndIndex = firstMoveSlotIndex();
            stream << indentation(1)
                   << "limm_field <= fetchblock(fetchblock'length-1 downto "
                   << "fetchblock'length-" << limmEndIndex << ");" << endl
                   << endl;
        }
        // pair<beginIndex,endIndex>
        vector<pair<int,int> > moveSlotBoundaries;
        evaluateMoveSlotBoundaries(moveSlotBoundaries);
        
        for (int i = 0; i < moveSlotCount(); i++) {
            if (dictionary_.at(i)->size() > 1) {
                stream << indentation(1)
                       << "dict_line_" << i 
                       << " <= conv_integer(unsigned(fetchblock("
                       << "fetchblock'length-" 
                       << moveSlotBoundaries.at(i).first
                       << " downto fetchblock'length-" 
                       << moveSlotBoundaries.at(i).second
                       << ")));" << endl << endl;
            }
        }
        
        generateDecompressorProcess(stream, haveLimm);
    }

    void evaluateMoveSlotBoundaries(vector<pair<int,int> >& boundaries) {
        for (int i = 0; i < moveSlotCount(); i++) {
            int temp = 0;
            if (boundaries.size() == 0) {
                temp = firstMoveSlotIndex();
            } else {
                // end point of previous boundary
                temp = boundaries.at(boundaries.size()-1).second;
            }
            int begin = temp + 1;
            int end = temp + MathTools::requiredBits(dictionary_.at(i)->size());
            boundaries.push_back(std::make_pair(begin,end));
        }
    }

    void generateDecompressorProcess(std::ostream& stream, bool& haveLimm) {
        stream << indentation(1) << "process (";
        if (haveLimm) {
            stream << "limm_field, ";
        }
        for (int i = 0; i < moveSlotCount(); i++) {
            if (dictionary_.at(i)->size() > 1) {
                stream << "dict_line_" << i;
                if (i+1 < moveSlotCount() 
                    && dictionary_.at(i+1)->size() > 1) {
                    stream << ", ";
                }
            }
        }
        stream << ")" << endl
               << indentation(1) << "begin" << endl
               << indentation(2) << "instructionword <= ";
        if (haveLimm) {
            stream << "limm_field&";
        }
        for (int i = 0; i < moveSlotCount(); i++) {
            if (dictionary_.at(i)->size() > 1) {
                stream << "dict_" << i <<"(dict_line_" << i << ")";
            } else {
                stream << "dict_" << i;
            }
            if (i+1 < moveSlotCount()) {
                stream << "&";
            }
        }
        stream << ";" << endl
               << indentation(1) << "end process;" << endl << endl;
    }

    void printDetails() {
        int widthInBytes = static_cast<int>(
            std::ceil(compressedWidth_ / 8.0));
        Application::logStream() << "compressed instruction width: "
                                 << compressedWidth_ << " ("
                                 << widthInBytes << " bytes)" << endl;
        std::size_t totalSize = 0;
        for (unsigned int i = 0; i < dictionary_.size(); i++) {
            std::size_t index = static_cast<std::size_t>(i);
            std::size_t keyWidth = 
                MathTools::requiredBits(dictionary_.at(i)->size());
            std::size_t entrySize = binaryEncoding().width();
            std::size_t entries = dictionary_.at(i)->size();
            totalSize += entries * entrySize;
            Application::logStream() 
                << (boost::format(
                        "Dictionary %d:\n"
                        "dictionary width: %d bits, entries: %d, "
                        "dictionary size: %d bits (%d bytes)\n")
                    % index % keyWidth % entries % (entries * entrySize)
                    % std::size_t(
                        std::ceil(entries * entrySize / 8.0))).str();
        }
        Application::logStream() 
            << (boost::format(
                    "Total dictionary size: %d bits (%d bytes)\n\n")
                % totalSize 
                % std::size_t(std::ceil(totalSize / 8.0))).str();
    }
    
    /// Map type for dictionary.
    typedef std::map<BitVector, unsigned int> Dictionary;
    
    /// The dictionary.
    vector<Dictionary*> dictionary_;
    
    /// Indicates whether the dictionary has been created
    bool dictionaryCreated_;

    /// Total width of compressed instruction (limm fields + move slots)
    unsigned int compressedWidth_;
};

EXPORT_CODE_COMPRESSOR(MoveSlotDictionary)
