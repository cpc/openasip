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
 * @file ProgramImageGenerator.hh
 *
 * Declaration of ProgramImageGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_IMAGE_GENERATOR_HH
#define TTA_PROGRAM_IMAGE_GENERATOR_HH

#include <iostream>
#include <set>

#include "CodeCompressorPlugin.hh"
#include "BaseType.hh"
#include "PluginTools.hh"
#include "Exception.hh"

namespace TPEF {
    class Binary;
    class InstructionElement;
    class DataSection;
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
        ARRAY ///< ASCII 1's and 0's in array form.
    };

    ProgramImageGenerator();
    virtual ~ProgramImageGenerator();
        
    void loadCompressorPlugin(const std::string& fileName)
        throw (FileNotFound, DynamicLibraryException);
    void loadCompressorParameters(
        CodeCompressorPlugin::ParameterTable parameters);
    void loadPrograms(std::set<TPEF::Binary*> programs);
    void loadMachine(const TTAMachine::Machine& machine);
    void loadBEM(const BinaryEncoding& bem);
    
    void generateProgramImage(
        TPEF::Binary& program,
        std::ostream& stream,
        OutputFormat format,
        int mausPerLine = 0)
        throw (InvalidData);
    void generateDataImage(
        TPEF::Binary& program,
        const std::string& addressSpace, 
        std::ostream& stream, 
        OutputFormat format,
        int mausPerLine,
        bool usePregeneratedImage)
        throw (InvalidData, OutOfRange);
    void generateDecompressor(std::ostream& stream);

    static std::vector<std::string> availableCompressors();
    static void printCompressorDescription(
        const std::string& fileName,
        std::ostream& stream)
        throw (FileNotFound, DynamicLibraryException);

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
    /// The plugin tool.
    PluginTools pluginTool_;
};

#endif
