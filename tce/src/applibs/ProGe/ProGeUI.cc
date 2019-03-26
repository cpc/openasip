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
 * @file ProGeUI.cc
 *
 * Implementation of ProGeUI class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Esa M‰‰tt‰ 2007 (esa.maatta-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2011
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#include "ProGeUI.hh"
#include "ProGeTypes.hh"
#include "ProcessorGenerator.hh"

#include "Machine.hh"
#include "ADFSerializer.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "FUPort.hh"

#include "BinaryEncoding.hh"
#include "BEMSerializer.hh"
#include "BEMGenerator.hh"
#include "BEMValidator.hh"

#include "MachineImplementation.hh"
#include "IDFSerializer.hh"
#include "IDFValidator.hh"

#include "ProcessorConfigurationFile.hh"
#include "Environment.hh"
#include "FileSystem.hh"

#include "ProGeScriptGenerator.hh"
#include "ProGeTestBenchGenerator.hh"

#include "MathTools.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "PlatformIntegrator.hh"
#include "Stratix2DSPBoardIntegrator.hh"
#include "Stratix3DevKitIntegrator.hh"
#include "KoskiIntegrator.hh"
#include "AvalonIntegrator.hh"
#include "AlmaIFIntegrator.hh"

using namespace IDF;
using std::string;
using std::vector;



namespace ProGe {

const std::string ProGeUI::DEFAULT_ENTITY_STR = "tta0";

/**
 * The constructor.
 */
ProGeUI::ProGeUI() :
    machine_(NULL), bem_(NULL), idf_(NULL), plugin_(NULL), pluginFile_(""),
    entityName_(DEFAULT_ENTITY_STR) {
}


/**
 * The destructor.
 */
ProGeUI::~ProGeUI() {
    if (machine_ != NULL) {
        delete machine_;
    }
    if (bem_ != NULL) {
        delete bem_;
    }
    if (idf_ != NULL) {
        delete idf_;
    }
    if (plugin_ != NULL) {
        delete plugin_;
    }
}


/**
 * Loads machine from the given ADF file.
 *
 * @param adfFile The ADF file.
 * @exception SerializerException If the file cannot be read or is erroneous.
 * @exception ObjectStateLoadingException If the machine cannot be
 *                                        constructed.
 */
void
ProGeUI::loadMachine(const std::string& adfFile)
    throw (SerializerException, ObjectStateLoadingException) {

    ADFSerializer serializer;
    serializer.setSourceFile(adfFile);
    machine_ = serializer.readMachine();
}


/**
 * Loads the binary encoding from the given BEM file.
 *
 * @param bemFile The BEM file.
 * @exception SerializerException If the file cannot be read or is erroneous.
 * @exception ObjectStateLoadingException If the object model cannot be
 *                                        constructed.
 */
void
ProGeUI::loadBinaryEncoding(const std::string& bemFile)
    throw (SerializerException, ObjectStateLoadingException) {

    BEMSerializer serializer;
    serializer.setSourceFile(bemFile);
    bem_ = serializer.readBinaryEncoding();
}


/**
 * Loads the machine implementation from the given IDF file.
 *
 * @param idfFile The IDF file.
 * @exception SerializerException If the file cannot be read or is erroneous.
 * @exception ObjectStateLoadingException If the object model cannot be
 *                                        constructed.
 */
void
ProGeUI::loadMachineImplementation(const std::string& idfFile)
    throw (SerializerException, ObjectStateLoadingException) {

    IDFSerializer serializer;
    serializer.setSourceFile(idfFile);
    idf_ = serializer.readMachineImplementation();
}


/**
 * Loads the given processor configuration.
 *
 * @param configurationFile The PCF file.
 * @exception UnreachableStream If the PCF file cannot be read.
 * @exception SerializerException If some files defined in PCF cannot be
 *                                read or is erroneous.
 * @exception ObjectStateLoadingException If some of the object models cannot
 *                                        be constructed.
 */
void
ProGeUI::loadProcessorConfiguration(const std::string& configurationFile)
    throw (UnreachableStream, SerializerException,
           ObjectStateLoadingException) {

    std::ifstream fileStream(configurationFile.c_str());
    if (!fileStream.good()) {
        string errorMsg = "Unable to read the PCF from '" +
            configurationFile + "'.";
        throw UnreachableStream(__FILE__, __LINE__, __func__, errorMsg);
    }

    ProcessorConfigurationFile pcf(fileStream);
    pcf.setPCFDirectory(FileSystem::directoryOfPath(configurationFile));

    // load machine
    try {
        string adfFile = pcf.architectureName();
        ADFSerializer serializer;
        serializer.setSourceFile(adfFile);
        machine_ = serializer.readMachine();
    } catch (const KeyNotFound&) {
    }

    // load BEM
    try {
        string bemFile = pcf.encodingMapName();
        BEMSerializer serializer;
        serializer.setSourceFile(bemFile);
        bem_ = serializer.readBinaryEncoding();
    } catch (const KeyNotFound&) {
    }

    // load IDF
    try {
        string idfFile = pcf.implementationName();
        IDFSerializer serializer;
        serializer.setSourceFile(idfFile);
        idf_ = serializer.readMachineImplementation();
    } catch (const KeyNotFound&) {
    }
}


/**
 * Loads the given IC/decoder generator plugin.
 *
 * @param pluginFile The file that implements the plugin.
 * @param pluginName Name of the plugin.
 * @exception FileNotFound If the module is not found.
 * @exception DynamicLibraryException If the module cannot be opened.
 * @exception InvalidData If the machine or BEM is not loaded yet.
 */
void
ProGeUI::loadICDecoderGeneratorPlugin(
    const std::string& pluginFile,
    const std::string& pluginName)
    throw (FileNotFound, DynamicLibraryException, InvalidData) {

    pluginFile_ = pluginFile;

    checkIfNull(machine_, "ADF not loaded");
    if (bem_ == NULL) {
        string errorMsg = "BEM not loaded";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    // initialize the plugin tool
    vector<string> pluginPaths = Environment::icDecoderPluginPaths();
    for (vector<string>::const_iterator iter = pluginPaths.begin();
         iter != pluginPaths.end(); iter++) {
        try {
            pluginTool_.addSearchPath(*iter);
        } catch (const FileNotFound&) {
        }
    }

    try {
        pluginTool_.registerModule(pluginFile);
    } catch (const FileNotFound&) {
        string errorMsg = "Plugin file '" + pluginFile + "' doesn't exist";
        throw FileNotFound(__FILE__, __LINE__, __func__, errorMsg);
    }

    ICDecoderGeneratorPlugin* (*creator)(
        TTAMachine::Machine&, BinaryEncoding&);
    pluginTool_.importSymbol(
        "create_generator_plugin_" + pluginName, creator, pluginFile);
    plugin_ = creator(*machine_, *bem_);
    assert(plugin_ != NULL);
}

/**
 * Generates the processor with the loaded data.
 *
 * @param imemWidthInMAUs Width of the instruction memory in MAUs.
 * @param language The language to generate.
 * @param dstDirectory The destination directory.
 * @param sharedDstDirectory The destination directory for VHDL files that
 *                           are potentially shared between multiple TTAs in
 *                           the same toplevel design.
 * @param entityString The string that is used as the top level entity name
 *                     and to differentiate the entity and package names in 
 *                     processor-specific HDL files.  
 * @param errorStream Stream where error messages are written.
 * @param warningStream Stream where warning messages are written.
 * @exception InvalidData If ADF or IDF is not loaded.
 * @exception IOException If an IO exception occurs.
 * @exception DynamicLibraryException If the default plugin cannot be opened.
 * @exception InvalidData If HDB or IDF is erroneous or if BEM is not
 *                        compatible with the machine.
 * @exception IllegalMachine If the machine is illegal.
 * @exception OutOfRange If the given instruction memory width is not positive.
 * @exception InstanceNotFound Something missing from HDB.
 */
void
ProGeUI::generateProcessor(
    int imemWidthInMAUs,
    HDL language,
    TCEString dstDirectory,
    TCEString sharedDstDirectory = "",
    TCEString entityString = ProGeUI::DEFAULT_ENTITY_STR,
    std::ostream& errorStream = std::cerr,
    std::ostream& warningStream = std::cerr)
    throw (InvalidData, DynamicLibraryException, IOException,
           IllegalMachine, OutOfRange, InstanceNotFound) {

    if (sharedDstDirectory == "")
        sharedDstDirectory = dstDirectory;

    if (!entityString.empty())
        entityName_ = entityString;

    checkIfNull(machine_, "ADF not loaded");
    checkIfNull(idf_, "IDF not loaded");
    if (bem_ == NULL) {
        BEMGenerator generator(*machine_);
        bem_ = generator.generate();
    }

    // validate IDF against machine
    IDFValidator idfValidator(*idf_, *machine_);
    if (!idfValidator.validate()) {
        string errorMsg = idfValidator.errorMessage(0);
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    // validate BEM against machine
    BEMValidator bemValidator(*bem_, *machine_);
    if (!bemValidator.validate()) {
        string errorMsg("");
        if (bemValidator.errorCount() > 0) {
            errorMsg = bemValidator.errorMessage(0);
        }
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    } else if (bemValidator.warningCount() > 0) {
        for (int i = 0; i < bemValidator.warningCount(); i++) {
            warningStream << bemValidator.warningMessage(i) << std::endl;
        }
    }

    if (plugin_ == NULL) {
        if (!idf_->hasICDecoderPluginFile() ||
            !idf_->hasICDecoderPluginName()) {
            string errorMsg = "IC/decoder generator plugin not defined";
            throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
        } else {
            loadICDecoderGeneratorPlugin(
                idf_->icDecoderPluginFile(), idf_->icDecoderPluginName());

            // set plugin parameters
            for (unsigned i = 0; i < idf_->icDecoderParameterCount(); i++) {
                plugin_->setParameter(
                    idf_->icDecoderParameterName(i),
                    idf_->icDecoderParameterValue(i));
            }
        }
    }
    // remove unconnected sockets (if any) before generation
    ProGe::ProcessorGenerator::removeUnconnectedSockets(
        *machine_, warningStream);

    generator_.generateProcessor(
        language, *machine_, *idf_, *plugin_, imemWidthInMAUs,
        dstDirectory, sharedDstDirectory, entityName_, errorStream, 
        warningStream);
}


/** 
 * Generates a test bench for simulating.
 *
 * @param dstDir Destination directory for the test bench.
 * @param progeOutDir ProGe output directory.
 *
 * @exception InvalidData If Machine or implementation not loaded.
 */
void
ProGeUI::generateTestBench(
    const ProGe::HDL language,
    const std::string& dstDir, 
    const std::string& progeOutDir) {

    checkIfNull(machine_, "ADF not loaded");
    checkIfNull(idf_, "IDF not loaded");

    ProGeTestBenchGenerator tbGen = ProGeTestBenchGenerator();
    tbGen.generate(language,*machine_, *idf_, dstDir, progeOutDir, entityName_);
}

/** 
 * Generates vhdl compilation and simulation scripts.
 * 
 * @param dstDir Destination directory for the scripts.
 * @param progeOutDir ProGe output directory.
 * @param progeOutDir Shared HDL output directory.
 * @param testBenchDir Directory where a test bench is stored. 
 */
void 
ProGeUI::generateScripts(
    const ProGe::HDL language,
    const std::string& dstDir,
    const std::string& progeOutDir,
    const std::string& sharedOutDir,
    const std::string& testBenchDir) {

    ProGeScriptGenerator sGen(
        language,
        *idf_, dstDir, progeOutDir, sharedOutDir, testBenchDir, entityName_);
    sGen.generateAll();
}

/** 
 * Checks if given pointer is NULL.
 *
 * Throws a InvalidData exception if given pointer is NULL and sets given
 * error message as exceptions error message.
 * 
 * @param nullPointer Some pointer. 
 * @param errorMsg Error message for the InvalidData exception.
 */
void
ProGeUI::checkIfNull(void * nullPointer, const std::string& errorMsg)
    throw (InvalidData) {

    if (nullPointer == NULL) {
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
}

void
ProGeUI::integrateProcessor(
    std::ostream& warningStream,
    std::ostream& errorStream,
    std::string progeOutDir,
    std::string sharedOutputDir,
    const std::string& platformIntegrator,
    const std::string& coreEntityName,
    const std::string& programName,
    const std::string& deviceFamily,
    MemType imem,
    MemType dmem,
    HDL language,
    int fmax) {

    string platformDir = progeOutDir + FileSystem::DIRECTORY_SEPARATOR +
        "platform";

    MemInfo imemInfo;
    imemInfo.type = imem;
    readImemParameters(imemInfo);

    PlatformIntegrator* integrator = NULL;
    // TODO: append new integrators here
    if (platformIntegrator == "Stratix2DSP") {
        integrator = new Stratix2DSPBoardIntegrator(
            machine_, idf_, language, progeOutDir, coreEntityName,
            platformDir, programName, fmax, warningStream, errorStream,
            imemInfo, dmem);
    } else if (platformIntegrator == "KoskiIntegrator") {
        integrator = new KoskiIntegrator(
            machine_, idf_, language, progeOutDir, coreEntityName,
            platformDir, programName, fmax, warningStream, errorStream,
            imemInfo, dmem);
    } else if (platformIntegrator == "AvalonIntegrator") {
        integrator = new AvalonIntegrator(
            machine_, idf_, language, progeOutDir, coreEntityName,
            platformDir, programName, fmax, warningStream, errorStream,
            imemInfo, dmem);
    } else if (platformIntegrator == "Stratix3DevKit") {
        integrator = new Stratix3DevKitIntegrator(
            machine_, idf_, language, progeOutDir, coreEntityName,
            platformDir, programName, fmax, warningStream, errorStream,
            imemInfo, dmem);
    } else if (platformIntegrator == "AlmaIFIntegrator") {
        integrator = new AlmaIFIntegrator(
            machine_, idf_, language, progeOutDir, coreEntityName,
            platformDir, programName, fmax, warningStream, errorStream,
            imemInfo, dmem);
    } else {
        string errorMsg = "Unknown platform integrator: "
            + platformIntegrator;
        InvalidData exc(__FILE__, __LINE__, __func__, errorMsg);
        throw exc;
    }
    if (!deviceFamily.empty()) {
        integrator->setDeviceFamily(deviceFamily);
    }

    if (FileSystem::absolutePathOf(sharedOutputDir) != 
        FileSystem::absolutePathOf(progeOutDir)) {
        integrator->setSharedOutputDir(sharedOutputDir);
    }

    NetlistBlock& ttaToplevel = generator_.netlist()->topLevelBlock();

    try {
        integrator->integrateProcessor(&ttaToplevel);
    } catch (Exception& e) {
        delete integrator;
        throw e;
    }
    delete integrator;
}

void
ProGeUI::readImemParameters(MemInfo& imem) const {
        
    imem.mauWidth = bem_->width();
    // imem width in MAUs is fixed to 1 in ProGe
    imem.widthInMaus = 1;
    imem.asName = machine_->controlUnit()->addressSpace()->name();
    imem.portAddrw = machine_->controlUnit()->returnAddressPort()->width();
    
    int lastAddr = machine_->controlUnit()->addressSpace()->end();
    imem.asAddrw = MathTools::requiredBits(lastAddr);
}
    
} // end of namespace ProGe

