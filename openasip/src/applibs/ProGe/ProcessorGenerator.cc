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
 * @file ProcessorGenerator.cc
 *
 * Implementation of ProcessorGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Esa Määttä 2007 (esa.maatta-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#include <algorithm>
#include <boost/format.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <string>

#include "BlockSourceCopier.hh"
#include "CUOpcodeGenerator.hh"
#include "ICDecoderGeneratorPlugin.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "NetlistGenerator.hh"
#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"
#include "NetlistVisualization.hh"
#include "ProcessorGenerator.hh"
#include "VHDLNetlistWriter.hh"
#include "BlockSourceCopier.hh"
#include "CUOpcodeGenerator.hh"
#include "VerilogNetlistWriter.hh"

#include "AddressSpace.hh"
#include "Application.hh"
#include "BEMGenerator.hh"
#include "BinaryEncoding.hh"
#include "ControlUnit.hh"
#include "FUPort.hh"
#include "FUPortCode.hh"
#include "FunctionUnit.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "HWOperation.hh"
#include "Machine.hh"
#include "MachineImplementation.hh"
#include "MachineInfo.hh"
#include "MachineResourceModifier.hh"
#include "MachineValidator.hh"
#include "MachineValidatorResults.hh"
#include "NetlistFactories.hh"
#include "NetlistTools.hh"
#include "ProGeContext.hh"
#include "RFArchitecture.hh"
#include "RFEntry.hh"
#include "SpecialRegisterPort.hh"

#include "Environment.hh"
#include "FileSystem.hh"
#include "MathTools.hh"

#include "Conversion.hh"
#include "StringTools.hh"

#include "ProGeOptions.hh"

#include "FUGen.hh"
#include "RFGen.hh"

using boost::format;
using std::endl;
using std::set;
using std::string;
using namespace TTAMachine;
using namespace IDF;
using namespace HDB;

const string CALL = "CALL";
const string JUMP = "JUMP";

namespace ProGe {

const TCEString ProcessorGenerator::DEFAULT_ENTITY_STR = "tta0";

/**
 * The constructor.
 */
ProcessorGenerator::ProcessorGenerator() : coreTopBlock_(NULL) {}

/**
 * The destructor.
 */
ProcessorGenerator::~ProcessorGenerator() {
    delete coreTopBlock_;
    coreTopBlock_ = NULL;
}

/**
 * Generates the processor.
 *
 * @see ProGeUI::generateProcessor()
 */
void
ProcessorGenerator::generateProcessor(
    const ProGeOptions& options, const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& implementation,
    ICDecoderGeneratorPlugin& plugin, int imemWidthInMAUs,
    std::ostream& errorStream, std::ostream& warningStream,
    std::ostream& verboseStream) {
    entityStr_ = options.entityName;
    generatorContext_ = new ProGeContext(
        machine, implementation, options.outputDirectory,
        options.sharedOutputDirectory, options.entityName, options.language,
        imemWidthInMAUs);

    // validate the machine
    validateMachine(machine, errorStream, warningStream);
    // check the compatibility of the plugin
    plugin.verifyCompatibility();
    // check that IU implementation latencies are compatible with the
    // IC/GCU plugin
    checkIULatencies(machine, implementation, plugin);

    NetlistGenerator netlistGenerator(*generatorContext_, plugin);
    coreTopBlock_ = netlistGenerator.generate(
        options, imemWidthInMAUs, options.entityName, warningStream);

    bool created = FileSystem::createDirectory(options.outputDirectory);
    if (!created) {
        string errorMsg =
            "Unable to create directory " + options.outputDirectory;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }

    string pluginDstDir =
        options.outputDirectory + FileSystem::DIRECTORY_SEPARATOR + "gcu_ic";
    created = FileSystem::createDirectory(pluginDstDir);
    if (!created) {
        string errorMsg = "Unable to create directory " + pluginDstDir;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }

    plugin.generate(
        options.language, pluginDstDir, netlistGenerator, implementation,
        options.entityName);

    // Generate generatable FU implementations.
    std::vector<std::string> globalOptions;
    globalOptions.emplace_back("active low reset");
    globalOptions.emplace_back("asynchronous reset");
    globalOptions.emplace_back("reset everything");
    FUGen::implement(
        options, globalOptions, generatorContext_->idf().FUGenerations(),
        generatorContext_->adf(), coreTopBlock_);

    // Generate generatable RF implementations.
    RFGen::implement(
        options, globalOptions, generatorContext_->idf().RFGenerations(),
        generatorContext_->adf(), coreTopBlock_);

    coreTopBlock_->write(Path(options.outputDirectory), options.language);

    string topLevelDir = options.outputDirectory +
                         FileSystem::DIRECTORY_SEPARATOR +
                         (options.language == VHDL ? "vhdl" : "verilog");

    if (Application::spamVerbose())
        NetlistVisualization::visualizeBlockTree(
            *coreTopBlock_, verboseStream);

    if (!FileSystem::fileExists(options.sharedOutputDirectory)) {
        if (!FileSystem::createDirectory(options.sharedOutputDirectory)) {
            string errorMsg = "Unable to create directory " +
                              options.sharedOutputDirectory + ".";
            throw IOException(__FILE__, __LINE__, __func__, errorMsg);
        }
    }
    BlockSourceCopier copier(
        implementation, options.entityName, options.language);

    if (machine.isRISCVMachine()) {
        copier.getTemplateInstatiator().replacePlaceholder(
            "decomp-fetchblock-width", "INSTRUCTIONWIDTH");
    }

    copier.copyShared(options.sharedOutputDirectory);
    copier.copyProcessorSpecific(options.outputDirectory);
    generateGCUOpcodesPackage(
        options.language, machine,
        options.outputDirectory + FileSystem::DIRECTORY_SEPARATOR + "gcu_ic");

    generateGlobalsPackage(
        options.language, machine, imemWidthInMAUs, topLevelDir, plugin);
}

/**
 * Generates the package that defines global constants used in processor
 * definition files.
 *
 * @param language The language of the hardware code, VHDL/Verilog
 * @param machine The machine.
 * @param imemWidthInMAUs Width of the instruction memory in MAUs.
 * @param dstDirectory The destination directory.
 * @param plugin The ICDecoderGeneratorPlugin, which gives us bem and bit
 * info
 * @exception IOException If an IO error occurs.
 */
void
ProcessorGenerator::generateGlobalsPackage(
    HDL language, const TTAMachine::Machine& machine, int imemWidthInMAUs,
    const std::string& dstDirectory, ICDecoderGeneratorPlugin& plugin) {
    string dstFile = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
                     entityName() + "_globals_pkg." +
                     ((language == ProGe::VHDL) ? "vhdl" : "vh");

    bool created = FileSystem::createFile(dstFile);
    if (!created) {
        string errorMsg = "Unable to create file " + dstFile;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }
    std::ofstream stream(dstFile.c_str(), std::ofstream::out);

    if (language == ProGe::VHDL) {
        int bustrace_width = 0;
        for (int i = 0; i < machine.busNavigator().count(); ++i) {
            int bus_width = machine.busNavigator().item(i)->width();
            // Busess are padded to a multiple of 32 bits
            bus_width = (bus_width + 31) / 32 * 32;
            bustrace_width += bus_width;
        }

        stream << "library work;" << endl
               << "use work." << entityStr_ << "_imem_mau.all;" << endl
               << endl;

        stream << "package " << entityStr_ << "_globals is" << endl
               << "  -- address width of the instruction memory" << endl
               << "  constant IMEMADDRWIDTH : positive := "
               << iMemAddressWidth(machine) << ";" << endl
               << "  -- width of the instruction memory in MAUs" << endl
               << "  constant IMEMWIDTHINMAUS : positive := "
               << imemWidthInMAUs << ";" << endl
               << "  -- width of instruction fetch block." << endl
               << "  constant IMEMDATAWIDTH : positive := "
               << "IMEMWIDTHINMAUS*IMEMMAUWIDTH;" << endl
               << "  -- clock period" << endl
               << "  constant PERIOD : time := 10 ns;" << endl
               << "  -- number of busses." << endl
               << "  constant BUSTRACE_WIDTH : positive := " << bustrace_width
               << ";" << endl;

        // Insert plugin specific global package constants
        plugin.writeGlobalDefinitions(ProGe::VHDL, stream);

        stream << "end " << entityStr_ << "_globals;" << endl;

    } else if (language == ProGe::Verilog) {
        // todo add IMEMDATAWIDTH constant here too.

        stream << "// address width of the instruction memory" << endl
               << "parameter IMEMADDRWIDTH = " << iMemAddressWidth(machine)
               << "," << endl
               << "// width of the instruction memory in MAUs" << endl
               << "parameter IMEMWIDTHINMAUS = " << imemWidthInMAUs << ","
               << endl
               << "// clock period" << endl
               << "parameter PERIOD = 10," << endl  // 10 will equal 10ns
               << "// instruction width" << endl
               << "parameter IMEMDATAWIDTH = IMEMWIDTHINMAUS*IMEMMAUWIDTH,"
               << endl;

        plugin.writeGlobalDefinitions(ProGe::Verilog, stream);

    } else {
        abortWithError("Unsupported HDL.");
    }
    stream.close();
}

/**
 * Generates the package that defines opcodes of Global Control Unit.
 *
 * @param language The language of the hardware code, VHDL/Verilog
 * @param machine The machine.
 * @param dstDirectory The destination directory.
 */
void
ProcessorGenerator::generateGCUOpcodesPackage(
    HDL language, const TTAMachine::Machine& machine,
    const std::string& dstDirectory) {
    string dstFile = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
                     "gcu_opcodes_pkg." +
                     ((language == ProGe::VHDL) ? "vhdl" : "vh");
    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    CUOpcodeGenerator gcuOpcodeGen(machine, entityStr_);
    gcuOpcodeGen.generateOpcodePackage(language, stream);
    stream.close();
}

const NetlistPortGroup*
ProcessorGenerator::instructionBus(NetlistBlock& block) {
    assert(!block.portGroupsBy(SignalGroupType::INSTRUCTION_LINE).empty());
    return block.portGroupsBy(SignalGroupType::INSTRUCTION_LINE).at(0);
}

/**
 * Validates the machine for compatibility with the given block
 * implementations.
 *
 * If the target architecture contains errors (incomplete definition) or
 * if its structure is not compatible with this HDL generator, this method
 * throws IllegalMachine exception. For less serious errors (such as
 * non-critical bit width discrepancies), warning messages are written to
 * the given stream.
 *
 * @param machine The machine to validate.
 * @param errorStream Output stream where errors are printed
 * @param warningStream Output stream where warnings are printed
 * @exception IllegalMachine If there is a fundamental error in the
 * machine.
 */
void
ProcessorGenerator::validateMachine(
    const TTAMachine::Machine& machine, std::ostream& errorStream,
    std::ostream& warningStream) {
    MachineValidator validator(machine);
    set<MachineValidator::ErrorCode> errorsToCheck;
    errorsToCheck.insert(MachineValidator::GCU_MISSING);
    errorsToCheck.insert(MachineValidator::GCU_AS_MISSING);
    errorsToCheck.insert(MachineValidator::USED_IO_NOT_BOUND);
    errorsToCheck.insert(MachineValidator::DIFFERENT_PORT_FOR_JUMP_AND_CALL);
    errorsToCheck.insert(MachineValidator::PC_PORT_MISSING);
    errorsToCheck.insert(MachineValidator::RA_PORT_MISSING);
    errorsToCheck.insert(
        MachineValidator::PC_AND_RA_PORTS_HAVE_UNEQUAL_WIDTH);
    // we should not check for this as the designer is not supposed to
    // know the width of these ports before the design has been finalized,
    // thus we generate them automatically in ProGe
    // errorsToCheck.insert(
    //    MachineValidator::IMEM_ADDR_WIDTH_DIFFERS_FROM_RA_AND_PC);

    MachineValidatorResults* results = validator.validate(errorsToCheck);

    for (int i = 0; i < results->errorCount(); i++) {
        MachineValidator::ErrorCode code = results->error(i).first;
        string errorMsg = results->error(i).second;
        if (code ==
            MachineValidator::IMEM_ADDR_WIDTH_DIFFERS_FROM_RA_AND_PC) {
            warningStream << "Warning: " << errorMsg
                          << " ProGe uses the value set in the address space."
                          << endl;
        } else {
            string msg = "Error: " + errorMsg;
            errorStream << msg << std::endl;
            delete results;
            throw IllegalMachine(__FILE__, __LINE__, __func__, msg);
        }
    }
    delete results;
}

/**
 * Checks that the latencies of the HW implementations of the immediate
 * units are compatible with the given IC/GCU generator plugin.
 *
 * @param machine The machine.
 * @param implementation The machine implementation.
 * @param plugin The plugin.
 * @exception InvalidData If the latencies are incompatible.
 */
void
ProcessorGenerator::checkIULatencies(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& implementation,
    const ICDecoderGeneratorPlugin& plugin) {
    Machine::ImmediateUnitNavigator iuNav = machine.immediateUnitNavigator();
    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit* iu = iuNav.item(i);
        RFImplementationLocation& impl =
            implementation.iuImplementation(iu->name());

        RFEntry* entry = NULL;
        try {
            HDBManager& manager = HDBRegistry::instance().hdb(impl.hdbFile());
            entry = manager.rfByEntryID(impl.id());
        } catch (const KeyNotFound& e) {
            throw InvalidData(__FILE__, __LINE__, __func__, e.errorMessage());
        }

        if (!entry->hasArchitecture()) {
            delete entry;
            format text(
                "RF entry %1% does not have architecture defined in HDB "
                "%2%.");
            text % impl.id() % impl.hdbFile();
            throw InvalidData(__FILE__, __LINE__, __func__, text.str());
        }

        RFArchitecture& architecture = entry->architecture();
        int implLatency = architecture.latency();
        std::set<int> requiredLatencies = plugin.requiredRFLatencies(*iu);
        delete entry;

        if (requiredLatencies.count(implLatency) == 0) {
            format errorMsg(
                "Latency of the implementation of immediate "
                "unit %1% is %2%,"
                " acceptable latencies are %3%.");
            errorMsg % iu->name() % implLatency %
                TCEString::makeString(requiredLatencies);
            throw InvalidData(__FILE__, __LINE__, __func__, errorMsg.str());
        }
    }
}

/**
 * Returns the width of the memory address of instruction memory of the
 * given machine.
 *
 * If the memory address width set in machine instruction memory address
 * space differ from gcu port widths, the value set in the address space
 * is used. In this case, GCU port widths are changed accordingly.
 *
 * @param mach The machine.
 * @return The bit width.
 * @exception InvalidData If the GCU of the machine does not have an
 * address
 *                        space.
 */
int
ProcessorGenerator::iMemAddressWidth(const TTAMachine::Machine& mach) {
    ControlUnit* gcu = mach.controlUnit();
    // must have been already caught in main method: generateProcessor
    assert(gcu != NULL);

    AddressSpace* iMem = gcu->addressSpace();
    // must have been already caught in main method: generateProcessor
    assert(iMem != NULL);

    int imemASWidth = MathTools::requiredBits(iMem->end());
    // if gcu port have different width than imemASWidth, change port
    // widths
    if (gcu->hasReturnAddressPort()) {
        int gcuRAPortWidth = gcu->returnAddressPort()->width();
        if (gcuRAPortWidth != imemASWidth) {
            gcu->returnAddressPort()->setWidth(imemASWidth);
        }
    }
    if (gcu->hasOperation(CALL)) {
        FUPort* pcPort = gcu->operation(CALL)->port(1);
        if (pcPort != NULL) {
            if (pcPort->width() != imemASWidth) {
                pcPort->setWidth(imemASWidth);
            }
        }
    } else if (gcu->hasOperation(JUMP)) {
        FUPort* pcPort = gcu->operation(JUMP)->port(1);
        if (pcPort != NULL) {
            if (pcPort->width() != imemASWidth) {
                pcPort->setWidth(imemASWidth);
            }
        }
    }

    return imemASWidth;
}

/**
 * Returns the width of the instruction memory of the given machine.
 *
 * @param mach The machine.
 * @return The bit width.
 */
int
ProcessorGenerator::iMemWidth(
    const TTAMachine::Machine& mach, int imemWidthInMAUs) {
    ControlUnit* gcu = mach.controlUnit();
    // must have been already caught in main method: generateProcessor
    assert(gcu != NULL);

    AddressSpace* iMem = gcu->addressSpace();
    // must have been already caught in main method: generateProcessor
    assert(iMem != NULL);

    return iMem->width() * imemWidthInMAUs;
}

/**
 * Returns Toplevel block of the core.
 */
const NetlistBlock&
ProcessorGenerator::processorTopLevel() const {
    return *coreTopBlock_;
}

/**
 * Returns the context (adf, idf, entity name, etc.) that is used in the
 * processor generation.
 */
const ProGeContext&
ProcessorGenerator::generatorContext() const {
    return *generatorContext_;
}

TCEString
ProcessorGenerator::entityName() const {
    return entityStr_;
}

void
ProcessorGenerator::removeUnconnectedSockets(
    TTAMachine::Machine& machine, std::ostream& warningStream) {
    MachineResourceModifier modifier;
    std::list<string> removedSockets;
    modifier.removeNotConnectedSockets(machine, removedSockets);

    for (std::list<string>::iterator i = removedSockets.begin();
         i != removedSockets.end(); i++) {
        warningStream << "Warning: Removed unconnected socket " << *i
                      << std::endl;
    }
}

} // namespace ProGe
