/**
 * @file ProgramImageGenerator.hh
 *
 * Declaration of ProgramImageGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
