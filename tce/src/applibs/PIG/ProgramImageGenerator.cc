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
 * @file ProgramImageGenerator.cc
 *
 * Implementation of ProgramImageGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>

#include "ProgramImageGenerator.hh"
#include "AsciiProgramImageWriter.hh"
#include "ArrayProgramImageWriter.hh"
#include "RawImageWriter.hh"
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
ProgramImageGenerator::ProgramImageGenerator() :
    compressor_(new DEFAULT_Compressor()) {
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
ProgramImageGenerator::loadCompressorPlugin(const std::string& fileName) 
    throw (FileNotFound, DynamicLibraryException) {

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
ProgramImageGenerator::loadPrograms(std::set<TPEF::Binary*> programs) {
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
    TPEF::Binary& program,
    std::ostream& stream,
    OutputFormat format,
    int mausPerLine) 
    throw (InvalidData) {

    if (mausPerLine < 0) {
        string errorMsg = "Negative number of MAUs printed per line.";
        throw OutOfRange(__FILE__, __LINE__, __func__, errorMsg);
    }

    InstructionBitVector* programBits = compressor_->compress(program);
    int mau = compressor_->machine().controlUnit()->addressSpace()->width();
    BitImageWriter* writer = NULL;

    if (format == BINARY) {
        writer = new RawImageWriter(*programBits);
    } else if (format == ASCII) {
        if (mausPerLine > 0) {
            writer = new AsciiImageWriter(*programBits, mau * mausPerLine);
        } else {
           writer = new AsciiProgramImageWriter(*programBits);
        }
    } else if (format == ARRAY) {
        if (mausPerLine > 0) {
            writer = new ArrayImageWriter(*programBits, mau * mausPerLine);
        } else {
            writer = new ArrayProgramImageWriter(*programBits);
        }
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
    TPEF::Binary& program,
    const std::string& addressSpace,
    std::ostream& stream,
    OutputFormat format,
    int mausPerLine,
    bool usePregeneratedImage) 
    throw (InvalidData, OutOfRange) {

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
        InstructionBitVector* programBits = compressor_->compress(program);
        delete programBits;
    }

    CodeSection* codeSection = dynamic_cast<CodeSection*>(
        program.section(Section::ST_CODE, 0));
    BitVector dataBits;
    
    // get the data section
    StringSection* stringSection = program.strings();
    for (unsigned int i = 0; i < program.sectionCount(Section::ST_DATA); 
         i++) {
        Section* section = program.section(Section::ST_DATA, i);
        if (stringSection->chunk2String(section->aSpace()->name()) == 
            addressSpace) {
            
            // correct data section found
            DataSection* dataSection = dynamic_cast<DataSection*>(section);
            assert(dataSection != NULL);

            // fill the beginning of the data image with zeros
            AddressImage startingAddress = dataSection->startingAddress();
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
            Word sectionLength = dataSection->length();
            for (Word offset = 0; offset < sectionLength;) {
                InstructionElement* relocTarget = 
                    this->relocTarget(program, *dataSection, offset);
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
                    Byte byte = dataSection->byte(offset);
                    dataBits.pushBack(byte, 8);
                    offset++;
                }
            }
        }
    }
    
    BitImageWriter* writer = NULL;
    if (format == BINARY) {
        writer = new RawImageWriter(dataBits);
    } else if (format == ASCII) {
        writer = new AsciiImageWriter(dataBits, as->width() * mausPerLine);
    } else if (format == ARRAY) {
        writer = new ArrayImageWriter(dataBits, as->width() * mausPerLine);
    } else {
        assert(false);
    }

    writer->writeImage(stream);
    delete writer;
}


/**
 * Generates the decompressor to the given output stream.
 *
 * Note! The program image should have been generated at first. Otherwise
 * this may not function properly.
 *
 * @param stream The output stream.
 */
void
ProgramImageGenerator::generateDecompressor(std::ostream& stream) {
    if (compressor_ == NULL) {
        return;
    } else {
        compressor_->generateDecompressor(stream);
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
    const std::string& fileName,
    std::ostream& stream) 
    throw (FileNotFound, DynamicLibraryException) {

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
    const std::string& fileName, PluginTools& pluginTool)
    throw (FileNotFound, DynamicLibraryException) {

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
