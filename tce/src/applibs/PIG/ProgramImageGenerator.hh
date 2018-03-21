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
 * @file ProgramImageGenerator.hh
 *
 * Declaration of ProgramImageGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_IMAGE_GENERATOR_HH
#define TTA_PROGRAM_IMAGE_GENERATOR_HH

#include <iostream>
#include <set>
#include <string>

#include "CodeCompressorPlugin.hh"
#include "BaseType.hh"
#include "PluginTools.hh"
#include "Exception.hh"
#include "TCEString.hh"

namespace TPEF {
    class Binary;
    class InstructionElement;
    class DataSection;
    class Section;
}

namespace TTAMachine {
    class Machine;
}

class BinaryEncoding;
class CodeCompressorPlugin;

/**
 * The main class of program image generator module.
 *
 * This class provides the interface that is available to different user
 * interfaces.
 */
class ProgramImageGenerator {
public:

    /// Different output formats of images.
    enum OutputFormat {
        BINARY, ///< Real binary format.
        ASCII, ///< ASCII 1's and 0's.
        ARRAY, ///< ASCII 1's and 0's in array form.
        MIF, ///< MIF Memory Initialization File
        VHDL, ///< Array as a Vhdl package
        COE, ///< COE memory initialization format
        HEX  ///< HEX memory initialization format
    };
    typedef std::map<std::string, TPEF::Binary*> TPEFMap;

    ProgramImageGenerator();
    virtual ~ProgramImageGenerator();
        
    void loadCompressorPlugin(const std::string& fileName)
        throw (FileNotFound, DynamicLibraryException);
    void loadCompressorParameters(
        CodeCompressorPlugin::ParameterTable parameters);
    void loadPrograms(TPEFMap programs);
    void loadMachine(const TTAMachine::Machine& machine);
    void loadBEM(const BinaryEncoding& bem);
    
    void generateProgramImage(
        const std::string& programName,
        std::ostream& stream,
        OutputFormat format,
        int mausPerLine = 0);
    void generateDataImage(
        const std::string& programName,
        TPEF::Binary& program,
        const std::string& addressSpace, 
        std::ostream& stream, 
        OutputFormat format,
        int mausPerLine,
        bool usePregeneratedImage)
        throw (InvalidData, OutOfRange);
    void generateDecompressor(std::ostream& stream, TCEString entityStr);

    int imemMauWidth() const;
    const CodeCompressorPlugin& compressor() { return *compressor_; }

    void setEntityName(const std::string& entity);

    static std::vector<std::string> availableCompressors();
    static void printCompressorDescription(
        const std::string& fileName,
        std::ostream& stream)
        throw (FileNotFound, DynamicLibraryException);

    void writeDataSection(TPEF::Binary& program,
                          BitVector& bitVector,
                          const std::string& addressSpace,
                          TPEF::Section& section);

private:
    /// Typedef for program set.
    typedef std::set<TPEF::Binary*> ProgramSet;

    static CodeCompressorPlugin* createCompressor(
        const std::string& fileName,
        PluginTools& pluginTool)
        throw (FileNotFound, DynamicLibraryException);
    TPEF::InstructionElement* relocTarget(
        const TPEF::Binary& program,
        const TPEF::DataSection& dataSection,
        Word dataSectionOffset) const;

    /// The code compressor.
    CodeCompressorPlugin* compressor_;
    /// Toplevel entity name
    std::string entityName_;
    /// The plugin tool.
    PluginTools pluginTool_;
    
};

#endif
