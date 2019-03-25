/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file ProgramImageGenerator.cc
 *
 * Implementation of ProgramImageGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008-2009 (otto.esko-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2009 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Otto Esko 2010(otto.esko-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */

#include <vector>
#include <string>
#include <cmath>

#include "ProgramImageGenerator.hh"
#include "AsciiProgramImageWriter.hh"
#include "ArrayProgramImageWriter.hh"
#include "RawImageWriter.hh"
#include "MifImageWriter.hh"
#include "VhdlImageWriter.hh"
#include "VhdlProgramImageWriter.hh"
#include "CoeImageWriter.hh"
#include "HexImageWriter.hh"
#include "InstructionBitVector.hh"
#include "CodeCompressorPlugin.hh"
#include "DefaultCompressor.hh"
#include "PIGTextGenerator.hh"

#include "ControlUnit.hh"
#include "BinaryEncoding.hh"
#include "StringSection.hh"
#include "CodeSection.hh"
#include "RelocSection.hh"
#include "RelocElement.hh"
#include "Binary.hh"
#include "InstructionElement.hh"
#include "Program.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "TerminalImmediate.hh"
#include "Immediate.hh"
#include "Machine.hh"

#include "PluginTools.hh"
#include "Environment.hh"
#include "FileSystem.hh"

using namespace TTAMachine;
using namespace TPEF;
using namespace TTAProgram;

using std::vector;
using std::string;
using boost::format;

/**
 * The constructor.
 *
 * @param program The program.
 * @param bem The binary encoding map.
 * @param machine The machine.
 */
ProgramImageGenerator::ProgramImageGenerator(): 
    compressor_(new DEFAULT_Compressor()), entityName_("") {
}


/**
 * The destructor.
 */
ProgramImageGenerator::~ProgramImageGenerator() {
    delete compressor_;
}


/**
 * Loads the code compressor plugin from the given object file.
 *
 * @param fileName The file name.
 * @exception FileNotFound If the given file is not found from the search
 *                         paths of code compressor plugins.
 * @exception DynamicLibraryException If the dynamic library cannot be 
 *                                    opened.
 */
void
ProgramImageGenerator::loadCompressorPlugin(const std::string& fileName) {
    CodeCompressorPlugin* newCompressor = createCompressor(
        fileName, pluginTool_);
    delete compressor_;
    compressor_ = newCompressor;
}

/**
 * Loads the given code compressor plugin parameters to the plugin.
 *
 * @param parameters The parameters.
 */
void
ProgramImageGenerator::loadCompressorParameters(
    CodeCompressorPlugin::ParameterTable parameters) {
        
    compressor_->setParameters(parameters);
}


/**
 * Loads the programs to be generated to the compressor plugin.
 *
 * @param programs The programs.
 */
void
ProgramImageGenerator::loadPrograms(TPEFMap programs) {
    compressor_->setPrograms(programs);
}


/**
 * Sets the machine which executes the programs.
 *
 * @param machine The machine.
 */
void
ProgramImageGenerator::loadMachine(const TTAMachine::Machine& machine) {
    compressor_->setMachine(machine);
}


/**
 * Sets the binary encoding map used when generating the program image.
 *
 * @param bem The binary encoding map.
 */
void
ProgramImageGenerator::loadBEM(const BinaryEncoding& bem) {
    compressor_->setBEM(bem);
}


/**
 * Generates the program image to the given output stream in the given
 * format.
 *
 * If the output is in ASCII format, mausPerLine parameter defines the number
 * of MAUs printed per line. If 0 is given, each instruction is printed on
 * different line.
 *
 * @param stream The output stream.
 * @param format The output format.
 * @param mausPerLine If the output is ASCII format, defines the number of
                      MAUs printed per line.
 * @exception InvalidData If machine or BEM is not loaded or if they are
 *                        erroneous or if the given program is not in the
 *                        program set.
 * @exception OutOfRange If mausPerLine is negative.
 */
void
ProgramImageGenerator::generateProgramImage(
    const std::string& programName,
    std::ostream& stream,
    OutputFormat format,
    int mausPerLine) {

    if (mausPerLine < 0) {
        string errorMsg = "Negative number of MAUs printed per line.";
        throw OutOfRange(__FILE__, __LINE__, __func__, errorMsg);
    }
    InstructionBitVector* programBits = compressor_->compress(programName);
    int mau = compressor_->machine().controlUnit()->addressSpace()->width();
    BitImageWriter* writer = NULL;

    if (Application::verboseLevel() > 0) {
        size_t instructionCount = 
            compressor_->currentProgram().instructionCount();
        size_t uncompressedWidth = compressor_->binaryEncoding().width();
        Application::logStream()
            << (boost::format(
                    "%s: %d instructions\n"
                    "uncompressed total size %d bits (%d bytes),\n")
                % programName % instructionCount 
                % (uncompressedWidth * instructionCount)
                % int(std::ceil(uncompressedWidth * instructionCount / 8.0))).
            str();

        Application::logStream()
            << (boost::format(
                    "compressed total size %d bits (%d bytes)\n")
                % programBits->size() 
                % size_t(std::ceil(programBits->size() / 8.0))).str();

        // count some stats from the program that affect instruction 
        // memory usage
        TTAProgram::Program& prog = compressor_->currentProgram();
        TTAMachine::Machine mach = prog.targetProcessor();

        TTAProgram::Program::InstructionVector instructions =
            prog.instructionVector();

        std::size_t moveSlots = mach.busNavigator().count();
        std::size_t maxMoves = moveSlots * prog.instructionCount();
        std::size_t moves = prog.moveCount();
        std::size_t NOPCount = maxMoves - moves;

        Application::logStream()
            << (boost::format(
                    "number of NOP moves: %d (%.1f%% of total move slots)\n")
                % NOPCount % (float(NOPCount) * 100 / maxMoves)).str();


        // immediate stats
        int totalImmediates = 0;
        int totalLongImmediates = 0;
        std::set<int> allImmediates;
        std::set<int> programAddresses;
        std::set<int> dataAddresses;

        for (int m = 0; m < prog.moveCount(); ++m) {
            const TTAProgram::Move& move = prog.moveAt(m);
            if (move.source().isImmediate()) {
                const auto value = move.source().value().sIntWordValue();
                ++totalImmediates;
                allImmediates.insert(value);
                if (move.source().isAddress()) {
                    dataAddresses.insert(value);
                } else if (move.source().isInstructionAddress()) {
                    programAddresses.insert(value);
                }
            }
        }
        // find the long immediates also
        for (TTAProgram::Program::InstructionVector::const_iterator i = 
                 instructions.begin(); i != instructions.end(); ++i) {
            const TTAProgram::Instruction& instruction = **i;
            for (int imm = 0; imm < instruction.immediateCount(); ++imm) {
                const Immediate& immediate = instruction.immediate(imm);
                const auto value = immediate.value().value().sIntWordValue();
                ++totalImmediates;
                ++totalLongImmediates;
                allImmediates.insert(value);
                if (immediate.value().isAddress()) {
                    dataAddresses.insert(value);
                } else if (immediate.value().isInstructionAddress()) {
                    programAddresses.insert(value);
                }
            }
                
        }

        // variables for NOP counting
        int totalFullNOPs = 0;
        int totalTwoConsNOPs = 0;
        int totalThreeConsNOPs = 0;
        int totalFourConsNOPs = 0;
        bool oneConsecutiveNOP = false;
        bool twoConsecutiveNOPs = false;
        bool threeConsecutiveNOPs = false;
        bool fourConsecutiveNOPs = false;
        
        // Find information about NOP instructions from program's instr.vectors
        for (TTAProgram::Program::InstructionVector::const_iterator i = 
                 instructions.begin(); i != instructions.end(); ++i) {
            const TTAProgram::Instruction& instruction = **i;
            if (instruction.isNOP()) {

                // Increase count for each single full instruction NOP
                ++totalFullNOPs;

                // Find full instruction NOP groups iteratively, ie. if there
                // are 4 NOPs in a row, decrease count for 3 NOPs, etc.
                if (fourConsecutiveNOPs) {
                    // We don't change count beyond 4 or more consecutive
                    // NOP instructions.
                } else if (threeConsecutiveNOPs) {
                    fourConsecutiveNOPs = true;
                    ++totalFourConsNOPs;
                    --totalThreeConsNOPs; // Actually 4 NOPs in a row, not 3
                } else if (twoConsecutiveNOPs) {
                    threeConsecutiveNOPs = true;
                    ++totalThreeConsNOPs;
                    --totalTwoConsNOPs; // Actually 3 NOPs in a row, not 2
                } else if (oneConsecutiveNOP) {
                    twoConsecutiveNOPs = true;
                    ++totalTwoConsNOPs;
                }
                oneConsecutiveNOP = true;
            } else {
                oneConsecutiveNOP = false;
                twoConsecutiveNOPs = false;
                threeConsecutiveNOPs = false;
                fourConsecutiveNOPs = false;
            }
        } 

        // Print information about immediates, addresses and NOPs
        Application::logStream()
            << (boost::format(
                    "total immediates: %d (long %.1f%%), "
                    "different immediate values: %d,\n"
                    "instr. addresses %d (%.1f%%), "
                    "data addresses %d (%.1f%%),\n"
                    "total full instruction NOPs: %d (%.1f%% of instructions),\n"
                    "two consecutive full instruction NOPs: %d,\n" 
                    "three consecutive full instruction NOPs: %d,\n" 
                    "four consecutive full instruction NOPs: %d\n") 
                % totalImmediates 
                % (totalLongImmediates * 100.0 / totalImmediates)
                % allImmediates.size() % programAddresses.size() 
                % (programAddresses.size() * 100.0 / allImmediates.size())
                % dataAddresses.size()
                % (dataAddresses.size() * 100.0 / allImmediates.size())
                % totalFullNOPs
                % (totalFullNOPs * 100.0 / instructionCount)
                % totalTwoConsNOPs
                % totalThreeConsNOPs
                % totalFourConsNOPs)
                .str();
    }


    if (format == BINARY) {
        writer = new RawImageWriter(*programBits);
    } else if (format == ASCII) {
        // change this to "mausPerLine > 0" when mau == instructionwidth
        // does not apply anymore
        if (mausPerLine > 1) {
            writer = new AsciiImageWriter(*programBits, mau * mausPerLine);
        } else {
           writer = new AsciiProgramImageWriter(*programBits);
       }
    } else if (format == ARRAY) {
        if (mausPerLine > 1) {
            writer = new ArrayImageWriter(*programBits, mau * mausPerLine);
        } else {
            writer = new ArrayProgramImageWriter(*programBits);
        }
    } else if (format == MIF) {
        writer = new MifImageWriter(*programBits, mau);
    } else if (format == VHDL) {
        writer = new VhdlProgramImageWriter(*programBits, entityName_);
    } else if (format == COE) {
        writer = new CoeImageWriter(*programBits, mau);
    } else if (format == HEX) {
    	writer = new HexImageWriter(*programBits, mau);
    } else {
        assert(false);
    }
    

    writer->writeImage(stream);

    delete writer;
    delete programBits;
}


/**
 * Generates the data image of the given address space to the given
 * output stream in the given format.
 *
 * @param program The program of which data image is generated.
 * @param addressSpace The address space.
 * @param stream The output stream.
 * @param format The output format.
 * @param mausPerLine Tells how many MAUs of data is generated on one line.
 * @param usePregeneratedImage Tells whether use the program image that was 
 *                             previously generated for relocation of data
 *                             elements.
 * @exception InvalidData If tried to use pregenerated program but it has not
 *                        been generated or if the given program is not in
 *                        the program set loaded or if machine is not loaded
 *                        or if is does not have the given address space.
 * @exception OutOfRange If the given MAUs per line is not positive.
 */
void
ProgramImageGenerator::generateDataImage(
    const std::string& programName, TPEF::Binary& program,
    const std::string& addressSpace, std::ostream& stream, OutputFormat format,
    int mausPerLine, bool usePregeneratedImage) {
    if (mausPerLine < 1) {
        string errorMsg = "Data memory width in MAUs must be positive.";
        throw OutOfRange(__FILE__, __LINE__, __func__, errorMsg);
    }

    const Machine* mach = NULL;
    try {
        mach = &compressor_->machine();
    } catch (const Exception& e) {
        throw InvalidData(__FILE__, __LINE__, __func__, e.errorMessage());
    }
    
    Machine::AddressSpaceNavigator navigator = 
        mach->addressSpaceNavigator();
    if (!navigator.hasItem(addressSpace)) {
        const string procName = "ProgramImageGenerator::generateDataImage";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }
    AddressSpace* as = navigator.item(addressSpace);

    if (!usePregeneratedImage) {
        InstructionBitVector* programBits = compressor_->compress(programName);
        delete programBits;
    }

    BitVector dataBits;
    
    // TODO: LEDATA? if le adf?
    for (unsigned int i = 0; i < program.sectionCount(Section::ST_DATA); 
         i++) {
        writeDataSection(program, dataBits, addressSpace,
                         *program.section(Section::ST_DATA, i));
    }
    for (unsigned int i = 0; i < program.sectionCount(Section::ST_LEDATA); 
         i++) {

        unsigned int lineOffset = dataBits.size(); // start afer previous
        // this writes data into the databits struct, not from it!
        writeDataSection(program, dataBits, addressSpace,
                         *program.section(Section::ST_LEDATA, i));

        // The stupid binary images are in big-endian bitness
        // format(bit 0 == highest bit).
        // Have to convert LE data into nasty mixed endian for
        // initialization with those tools.
        while (lineOffset < dataBits.size()) {
            // Pad to full line width
            if (lineOffset + ((mausPerLine-1)*as->width()) > dataBits.size()) {
                unsigned int preferredSize =
                    ((dataBits.size() / (mausPerLine*as->width()))+1) *
                    (mausPerLine*as->width());
                while (dataBits.size() < preferredSize) {
                    dataBits.push_back(0);
                }
            }

            for (int k = 0; k < mausPerLine/2; k++) {
                int bitOffset0 = k * as->width();
                int bitOffset1 = (mausPerLine-k-1) * as->width();
                // swap bytes of one MAU.
                for (int j = 0; j < as->width(); j++) {
                    bool bit0 = dataBits[lineOffset+bitOffset0 + j];
                    dataBits[lineOffset+bitOffset0+j] =
                        dataBits[lineOffset+bitOffset1+j];
                    dataBits[lineOffset+bitOffset1+j] = bit0;
                }
            }
            lineOffset += (mausPerLine*as->width());
        }
    }

    BitImageWriter* writer = NULL;
    if (format == BINARY) {
        writer = new RawImageWriter(dataBits);
    } else if (format == ASCII) {
        writer = new AsciiImageWriter(dataBits, as->width() * mausPerLine);
    } else if (format == ARRAY) {
        writer = new ArrayImageWriter(dataBits, as->width() * mausPerLine);
    } else if (format == MIF) {
        writer = new MifImageWriter(dataBits, as->width() * mausPerLine);
    } else if (format == VHDL) {
        writer = new VhdlImageWriter(
            dataBits, as->width() * mausPerLine, entityName_);
    } else if (format == COE) {
        writer = new CoeImageWriter(dataBits, as->width() * mausPerLine);
    } else if (format == HEX) {
        writer = new HexImageWriter(dataBits, as->width() * mausPerLine);
    } else {
        assert(false);
    }

    writer->writeImage(stream);
    delete writer;
}

void
ProgramImageGenerator::writeDataSection(
    TPEF::Binary& program, BitVector& dataBits,
    const std::string& addressSpace,
    Section& section) {
    
    // get the data section
    StringSection* stringSection = program.strings();

    CodeSection* codeSection = dynamic_cast<CodeSection*>(
        program.section(Section::ST_CODE, 0));

    if (stringSection->chunk2String(section.aSpace()->name()) == 
        addressSpace) {
        
        // correct data section found
        DataSection& dataSection = dynamic_cast<DataSection&>(section);
        
        // fill the beginning of the data image with zeros
        AddressImage startingAddress = dataSection.startingAddress();
        // If we already have put something, fill the correct number,
        // not from 0.
        unsigned int zeroFillStart = dataBits.size() / 8; // 8 bits per byte
        for (AddressImage i = zeroFillStart; i < startingAddress; i++) {
            dataBits.pushBack(0, 8);
        }
        if (zeroFillStart > startingAddress) {
            throw InvalidData(
                __FILE__,__LINE__,__func__, "Illegal order of data sections.");
        }
        
        // fill the data
        Word sectionLength = dataSection.length();
        for (Word offset = 0; offset < sectionLength;) {
            InstructionElement* relocTarget = 
                this->relocTarget(program, dataSection, offset);
            if (relocTarget != NULL) {
                Word indexOfInstruction = 
                    codeSection->indexOfInstruction(*relocTarget);
                Instruction& instruction = 
                    compressor_->currentProgram().instructionAt(
                        indexOfInstruction);
                try {
                    unsigned int memAddress = compressor_->memoryAddress(
                        instruction);
                    dataBits.pushBack(memAddress, 32);
                    offset += 4;
                    } catch (const Exception& e) {
                    string errorMsg = 
                            "Program image must be generated before "
                        "generating data image.";
                    throw InvalidData(
                        __FILE__, __LINE__, __func__, errorMsg);
                }
            } else {                    
                Byte byte = dataSection.byte(offset);
                dataBits.pushBack(byte, 8);
                offset++;
            }
        }
    }
}


/**
 * Generates the decompressor to the given output stream.
 *
 * Note! The program image should have been generated at first. Otherwise
 * this may not function properly.
 *
 * @param stream The output stream.
 * @param entityStr The entity string used to make HDL entity/component
 *                  names unique for multiprocessor designs.
 */
void
ProgramImageGenerator::generateDecompressor(
    std::ostream& stream,
    TCEString entityStr) {
    if (compressor_ == NULL) {
        return;
    } else {
        compressor_->generateDecompressor(stream, entityStr);
    }
}


/**
 * Returns a vector containing paths to the compressors available.
 *
 * @return The vector.
 */
std::vector<std::string>
ProgramImageGenerator::availableCompressors() {

    std::vector<string> paths = Environment::codeCompressorPaths();
    std::vector<string> files;
    for (std::vector<string>::const_iterator iter = paths.begin();
         iter != paths.end(); iter++) {
        if (FileSystem::fileExists(*iter)) {
            std::vector<string> filesInPath = FileSystem::directoryContents(
                *iter);
            for (std::vector<string>::const_iterator iter = 
                     filesInPath.begin();
                 iter != filesInPath.end(); iter++) {
                if (!FileSystem::fileIsDirectory(*iter) && 
                    FileSystem::fileExtension(*iter) == ".so") {
                    files.push_back(*iter);
                }
            }
        }
    }

    return files;
}


/**
 * Loads the code compressor plugin from the given file and prints its
 * description to the given stream.
 *
 * @param fileName The code compressor plugin file.
 * @param stream The output stream.
 * @exception FileNotFound If the given file is not found from the search
 *                         paths of code compressor plugins.
 * @exception DynamicLibraryException If the dynamic library cannot be 
 *                                    opened.
 */
void
ProgramImageGenerator::printCompressorDescription(
    const std::string& fileName, std::ostream& stream) {
    PluginTools pluginTool;
    CodeCompressorPlugin* compressor = createCompressor(
        fileName, pluginTool);
    compressor->printDescription(stream);
    delete compressor;
}

/**
 * Creates the code compressor from the given dynamic module.
 *
 * @param fileName Name of the file.
 * @param pluginTool The plugin tool to use.
 * @return The newly created code compressor plugin.
 * @exception FileNotFound If the given file is not found from the search
 *                         paths of code compressor plugins.
 * @exception DynamicLibraryException If the dynamic library cannot be 
 *                                    opened.
 */
CodeCompressorPlugin*
ProgramImageGenerator::createCompressor(
    const std::string& fileName, PluginTools& pluginTool) {
    vector<string> searchPaths = Environment::codeCompressorPaths();
    for (vector<string>::const_iterator iter = searchPaths.begin();
         iter != searchPaths.end(); iter++) {
        if (FileSystem::fileExists(*iter)) {
            pluginTool.addSearchPath(*iter);
        }
    }

    pluginTool.registerModule(fileName);
    CodeCompressorPlugin* (*pluginCreator)();
    pluginTool.importSymbol(
        "create_code_compressor", pluginCreator, fileName);

    return pluginCreator();
}

/**
 * Returns an InstructionElement that is relocation target of the data in
 * data section at the given offset. Returns NULL if the data doesn't need
 * to be altered.
 *
 * @param dataSection The data section,
 * @param dataSectionOffset The offset.
 * @return The InstructionElement or NULL.
 */
TPEF::InstructionElement*
ProgramImageGenerator::relocTarget(
    const TPEF::Binary& program,
    const TPEF::DataSection& dataSection,
    Word dataSectionOffset) const {

    // find the correct reloc section
    for (unsigned int i = 0; i < program.sectionCount(Section::ST_RELOC);
         i++) {
        RelocSection* section =  dynamic_cast<RelocSection*>(
            program.section(Section::ST_RELOC, i));
        assert(section != NULL);
        if (section->referencedSection() == &dataSection) {
            // correct reloc section found
            Word elemCount = section->elementCount();
            for (Word elemIndex = 0; elemIndex < elemCount; elemIndex++) {
                RelocElement* relocElem = dynamic_cast<RelocElement*>(
                    section->element(elemIndex));
                assert(relocElem != NULL);
                Chunk* location = dynamic_cast<Chunk*>(
                    relocElem->location());
                assert(location != NULL);
                if (location->offset() == dataSectionOffset) {
                    InstructionElement* destination = 
                        dynamic_cast<InstructionElement*>(
                            relocElem->destination());
                    if (destination != NULL) {
                        return destination;
                    } else {
                        return NULL;
                    }
                }
            }
        }
    }

    return NULL;
}

/**
 * Asks the instruction memory mau width from code compressor plugin and 
 * returns it
 *
 * @return instruction memory mau width
 */
int
ProgramImageGenerator::imemMauWidth() const {

    return compressor_->imemMauWidth();
}
    

void
ProgramImageGenerator::setEntityName(const std::string& entity) {
    
    entityName_ = entity;
}
