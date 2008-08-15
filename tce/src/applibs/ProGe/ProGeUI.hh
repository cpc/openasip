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
        const std::string& progeOutDir);
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
