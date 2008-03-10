/**
 * @file ProGeUI.hh
 *
 * Declaration of ProGeUI class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @author Esa Määttä 2007 (esa.maatta@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGE_UI_HH
#define TTA_PROGE_UI_HH

#include <string>

#include "ProGeTypes.hh"
#include "ICDecoderGeneratorPlugin.hh"
#include "Exception.hh"
#include "PluginTools.hh"


namespace TTAMachine {
    class Machine;
}

namespace IDF {
    class MachineImplementation;
}

class BinaryEncoding;

namespace ProGe {

/**
 * Base class for user intefaces of ProGe.
 */
class ProGeUI {
public:
    virtual ~ProGeUI();

protected:
    ProGeUI();
    void loadMachine(const std::string& adfFile)
        throw (SerializerException, ObjectStateLoadingException);
    void loadBinaryEncoding(const std::string& bemFile)
        throw (SerializerException, ObjectStateLoadingException);
    void loadMachineImplementation(const std::string& idfFile)
        throw (SerializerException, ObjectStateLoadingException);
    void loadProcessorConfiguration(const std::string& configurationFile)
        throw (UnreachableStream, SerializerException,
               ObjectStateLoadingException);
    void loadICDecoderGeneratorPlugin(
        const std::string& pluginFile,
        const std::string& pluginName)
        throw (FileNotFound, DynamicLibraryException, InvalidData);

    void generateProcessor(
        int imemWidthInMAUs,
        HDL language,
        const std::string& dstDirectory,
        std::ostream& outputStream)
        throw (InvalidData, DynamicLibraryException, IOException,
               InvalidData, IllegalMachine, OutOfRange, InstanceNotFound);
    void generateTestBench(
        const std::string& dstDir, 
        const std::string& progeOutDir)
        throw (InvalidData);
    void generateScripts(
        const std::string& dstDir,
        const std::string& progeOutDir,
        const std::string& testBenchDir);

private:
    void checkIfNull(void * nullPointer, const std::string& errorMsg)
        throw (InvalidData);
    /// The loaded machine.
    TTAMachine::Machine* machine_;
    /// The loaded binary encoding map.
    BinaryEncoding* bem_;
    /// The loaded machine implementation.
    IDF::MachineImplementation* idf_;
    /// Tool for loading plugin.
    PluginTools pluginTool_;
    /// The loaded IC/decoder generator plugin.
    ICDecoderGeneratorPlugin* plugin_;
    /// The plugin file.
    std::string pluginFile_;
};
}

#endif
