/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file ProGeUI.hh
 *
 * Declaration of ProGeUI class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Esa M‰‰tt‰ 2007 (esa.maatta-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2011
 * @note rating: red
 */

#ifndef TTA_PROGE_UI_HH
#define TTA_PROGE_UI_HH

#include <string>

#include "TCEString.hh"
#include "ProcessorGenerator.hh"
#include "ProGeTypes.hh"
#include "ICDecoderGeneratorPlugin.hh"
#include "Exception.hh"
#include "PluginTools.hh"
#include "MemoryGenerator.hh"


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
        TCEString dstDirectory,
        TCEString sharedDstDirectory,
        TCEString entityString,
        std::ostream& errorStream,
        std::ostream& warningStream)
        throw (InvalidData, DynamicLibraryException, IOException,
               InvalidData, IllegalMachine, OutOfRange, InstanceNotFound);
    void generateTestBench(
        const std::string& dstDir, 
        const std::string& progeOutDir);
    void generateScripts(
        const std::string& dstDir,
        const std::string& progeOutDir,
        const std::string& sharedOutDir,
        const std::string& testBenchDir);

    void integrateProcessor(
        std::ostream& warningStream,
        std::ostream& errorStream,
        std::string progeOutDir,
        const std::string& platformIntegrator,
        const std::string& coreEntityName,
        const std::string& programName,
        const std::string& deviceFamily,
        MemType imem,
        MemType dmem,
        HDL language,
        int fmax);

private:
    void checkIfNull(void * nullPointer, const std::string& errorMsg)
        throw (InvalidData);

    void readLSUParameters(MemInfo& dmem) const;

    void readImemParameters(MemInfo& imem) const;

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
    /// Name of the toplevel entity
    std::string entityName_;
    
    ProcessorGenerator generator_;

    static const std::string DEFAULT_ENTITY_STR;
};
}

#endif
