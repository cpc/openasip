/*
    Copyright (C) 2025 Tampere University.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA
 */
/**
 * @file RVCoprocessorGenerator.cc derived from ProcessorGenerator
 */

#include "RVCoprocessorGenerator.hh"

#include <algorithm>
#include <boost/format.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <string>

#include "AddressSpace.hh"
#include "Application.hh"
#include "BEMGenerator.hh"
#include "BinaryEncoding.hh"
#include "BlockSourceCopier.hh"
#include "ControlUnit.hh"
#include "Conversion.hh"
#include "Environment.hh"
#include "FUGen.hh"
#include "FUPort.hh"
#include "FUPortCode.hh"
#include "FileSystem.hh"
#include "FunctionUnit.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "HWOperation.hh"
#include "ICDecoderGeneratorPlugin.hh"
#include "Machine.hh"
#include "MachineImplementation.hh"
#include "MachineInfo.hh"
#include "MachineResourceModifier.hh"
#include "MachineValidator.hh"
#include "MachineValidatorResults.hh"
#include "MathTools.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "NetlistFactories.hh"
#include "NetlistGenerator.hh"
#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"
#include "NetlistTools.hh"
#include "NetlistVisualization.hh"
#include "ProGeContext.hh"
#include "ProGeOptions.hh"
#include "RFArchitecture.hh"
#include "RFEntry.hh"
#include "SpecialRegisterPort.hh"
#include "StringTools.hh"
#include "TrackerGen.hh"
#include "VHDLNetlistWriter.hh"
#include "VerilogNetlistWriter.hh"

using boost::format;
using std::endl;
using std::set;
using std::string;
using namespace TTAMachine;
using namespace IDF;
using namespace HDB;

namespace ProGe {

/**
 * The constructor.
 */
RVCoprocessorGenerator::RVCoprocessorGenerator() : coreTopBlock_(NULL) {}

/**
 * The destructor.
 */
RVCoprocessorGenerator::~RVCoprocessorGenerator() {
    delete coreTopBlock_;
    coreTopBlock_ = NULL;
}

/**
 * Generates HDL for CV-X-IF and ROCC interface based coprocessor tops, FUs 
 * and supporting files
 */
void
RVCoprocessorGenerator::generateRVCoprocessor(
    const ProGeOptions& options, const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& implementation,
    ProGe::ICDecoderGeneratorPlugin& plugin, int imemWidthInMAUs,
    std::ostream& errorStream, std::ostream& warningStream,
    std::ostream& verboseStream) {
    entityStr_ = options.entityName;
    generatorContext_ = new ProGe::ProGeContext(
        machine, implementation, options.outputDirectory,
        options.sharedOutputDirectory, options.entityName, options.language,
        imemWidthInMAUs);

    // validate the machine
    validateMachine(machine, errorStream, warningStream);

    ProGe::NetlistGenerator netlistGenerator(*generatorContext_, plugin);
    // HDLTemplateInstantiator HDLTemplateInstantiator(*generatorContext_);
    coreTopBlock_ = netlistGenerator.generatecopro(
        options, imemWidthInMAUs, options.entityName, warningStream);

    bool created = FileSystem::createDirectory(options.outputDirectory);
    if (!created) {
        string errorMsg =
            "Unable to create directory " + options.outputDirectory;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }

    // Generate generatable FU implementations.
    std::vector<std::string> globalOptions;
    globalOptions.emplace_back("active low reset");
    globalOptions.emplace_back("asynchronous reset");
    globalOptions.emplace_back("reset everything");

    // Implementing FUs
    FUGen::implement(
        options, globalOptions, generatorContext_->idf().FUGenerations(),
        generatorContext_->adf(), coreTopBlock_);

    string topLevelDir =
        options.outputDirectory + FileSystem::DIRECTORY_SEPARATOR +
        (options.language == ProGe::VHDL ? "vhdl" : "systemverilog");

    if (Application::spamVerbose())
        ProGe::NetlistVisualization::visualizeBlockTree(
            *coreTopBlock_, verboseStream);

    if (!FileSystem::fileExists(options.sharedOutputDirectory)) {
        if (!FileSystem::createDirectory(options.sharedOutputDirectory)) {
            string errorMsg = "Unable to create directory " +
                              options.sharedOutputDirectory + ".";
            throw IOException(__FILE__, __LINE__, __func__, errorMsg);
        }
    }
    // Makes CVX Coprocessor TOP for each FU
    if (options.roccGen == true) {
        for (auto FU : generatorContext_->idf().FUGenerations()) {
            makeROCCcoprocessor(options, FU, generatorContext_->adf());
        }
    } else {
        for (auto FU : generatorContext_->idf().FUGenerations()) {
            makecoprocessor(options, FU, generatorContext_->adf());
        }
        generateInstructionDecoder(options);  // Generates Compressed decoder
        generateSupportPackage(
            options.outputDirectory);  // Generates the support package
        // Implementing Instruction Tracker
        TrackerGen::generateTracker(
            options, generatorContext_->idf().FUGenerations(),
            generatorContext_->adf());
    }
}

/*
 * Makes the top of the ROCC Coprocessor
 */
void
RVCoprocessorGenerator::makeROCCcoprocessor(
    const ProGeOptions& options, IDF::FUGenerated& FU,
    const TTAMachine::Machine& machine) {
    TTAMachine::FunctionUnit* adfFU =
        machine.functionUnitNavigator().item(FU.name());
    const std::string dstDirectory =
        options.outputDirectory;  // destination directory
    const std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    std::string coproTargetDir = dstDirectory;
    if (!FileSystem::fileExists(coproTargetDir)) {
        if (!FileSystem::createDirectory(coproTargetDir)) {
            std::string errorMsg =
                "Unable to create directory " + coproTargetDir;
            throw IOException(__FILE__, __LINE__, __func__, errorMsg);
        }
    }
    std::string sourceFile;
    std::string dstFile;
    sourceFile =
        Environment::dataDirPath("ProGe") + DS + "rocc_copro.sv.tmpl";
    std::string file =
        StringTools::stringToLower("coprocessor_" + FU.name()) + ".sv";
    dstFile = coproTargetDir + DS + "systemverilog/" + file;
    std::string replcements1[8], replcements2[8];  // replacement keys
    replcements1[0] = {"FUNAME"};
    replcements2[0] = StringTools::stringToLower(FU.name());

    if (!FileSystem::fileExists(dstFile)) {
        instantiate_.instantiateCoprocessorTemplateFile(
            sourceFile, dstFile, replcements1, replcements2, 1);
    }
}

/*
 * Makes the top of the Coprocessor
 */
void
RVCoprocessorGenerator::makecoprocessor(
    const ProGeOptions& options, IDF::FUGenerated& FU,
    const TTAMachine::Machine& machine) {
    TTAMachine::FunctionUnit* adfFU =
        machine.functionUnitNavigator().item(FU.name());
    const std::string dstDirectory =
        options.outputDirectory;  // destination directory
    const std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    std::string coproTargetDir = dstDirectory;
    if (!FileSystem::fileExists(coproTargetDir)) {
        if (!FileSystem::createDirectory(coproTargetDir)) {
            std::string errorMsg =
                "Unable to create directory " + coproTargetDir;
            throw IOException(__FILE__, __LINE__, __func__, errorMsg);
        }
    }
    std::string sourceFile;
    std::string dstFile;
    sourceFile =
        Environment::dataDirPath("ProGe") + DS + "cvxif_coprocessor.sv.tmpl";
    std::string file =
        StringTools::stringToLower(FU.name()) + "_coprocessor.sv";
    dstFile = coproTargetDir + DS + "systemverilog/" + file;
    std::string coproreplcements1[8] = {
        "FUNAME",      "INPUT1",        "OUTPUTF",
        "CONFIG_EN",   "CONFIG_DEFINE", "OUT_COMMIT_TRACKER",
        "CONFIG_BITS", "SEARCH_CONFIG"};  // replacement keys REMOVE INPUT2
    std::string coproreplcements2[8];
    coproreplcements2[0] = StringTools::stringToLower(FU.name());
    int j = 0;
    int k = 1;
    for (int i = 0; i < adfFU->portCount();
         ++i) {  // iterating through ports in the FU
        TTAMachine::FUPort* adfPort =
            static_cast<TTAMachine::FUPort*>(adfFU->port(i));
        if (adfPort->isInput()) {
            j++;
            if (j == 4) {  // TODO:Make it to handle the number of inputs with
                           // a parameter which can be controlled
                throw std::runtime_error(
                    " More than 3 input ports are in the FU");
            } else {
                coproreplcements2[1] =
                    coproreplcements2[1] + ".data_" + adfPort->name() +
                    "_in(register.rs[" + std::to_string(j - 1) +
                    "]),\n";  // adding input port names to the array
            }
        } else {
            k++;
            if (k == 4) {
                throw std::runtime_error(
                    " More than 1 output port in the FU");
            } else {
                coproreplcements2[k] =
                    adfPort->name();  // adding Output names
            }
        }
    }
    for (int i = 0; i < adfFU->operationCount(); ++i) {
        TTAMachine::HWOperation* hwop = adfFU->operation(i);
        std::string op = hwop->name();

        coproreplcements2[3] = coproreplcements2[3] + ".config_" + op +
                               "_enable_in(commit_" + op +
                               "_tracker_to_FU),\n" + ".config_" + op +
                               "_kill_in(kill_" + op + "_tracker_to_FU),\n";
        coproreplcements2[4] =
            coproreplcements2[4] + "logic commit_" + op +
            "_tracker_to_FU;\n" + "logic kill_" + op + "_tracker_to_FU;\n" +
            "logic [cvxif_sup_pkg::NConfigbits_C-1 : 0] configbits_" + op +
            "_to_tracker; \n";
        coproreplcements2[5] = coproreplcements2[5] + ".out_committed_" + op +
                               "_o(commit_" + op + "_tracker_to_FU),\n" +
                               ".out_killed_" + op + "_o(kill_" + op +
                               "_tracker_to_FU),\n";
        coproreplcements2[6] = coproreplcements2[6] + ".configbits_" + op +
                               "_out(configbits_" + op + "_to_tracker),\n";
        coproreplcements2[7] =
            coproreplcements2[7] + ".search_id_" + op + "_i(configbits_" +
            op + "_to_tracker[cvxif_sup_pkg::X_ID_WIDTH - 1 : 0]),\n" +
            ".search_hartid_" + op + "_i(configbits_" + op +
            "_to_tracker[cvxif_sup_pkg::X_ID_WIDTH + "
            "cvxif_sup_pkg::X_HARTID_WIDTH - 1 : "
            "cvxif_sup_pkg::X_ID_WIDTH]),\n" +
            ".search_valid_" + op + "_i(configbits_" + op +
            "_to_tracker[cvxif_sup_pkg::NConfigbits_C-1]),\n";
    }

    if (!FileSystem::fileExists(dstFile)) {
        instantiate_.instantiateCoprocessorTemplateFile(
            sourceFile, dstFile, coproreplcements1, coproreplcements2, 8);
    }
}

// Instantiates the compressed decoder TODO
void
RVCoprocessorGenerator::generateInstructionDecoder(const ProGeOptions& options) {
    const std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    const std::string dstDirectory = options.outputDirectory;
    std::string sourceFile;
    std::string sourceFileComp;
    std::string dstFile;
    std::string dstFileComp;

    sourceFileComp = Environment::dataDirPath("ProGe") + DS +
                     "cvxifcompressed_decoder.sv.tmpl";

    std::string fileComp = "cvxifcompressed_decoder.sv";
    dstFileComp = dstDirectory + DS + "systemverilog/" + fileComp;

    FileSystem::copy(sourceFileComp, dstFileComp);
}

/**
 * Instruction opcode support package generator.
 * @param language The langauge:currently only verilog.
 * @param dstDirectory The destination directory.
 */
void
RVCoprocessorGenerator::generateSupportPackage(const std::string& dstDirectory) {
    string dstFile = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
                     "systemverilog/cvxif_sup_pkg.sv";
    bool created = FileSystem::createFile(dstFile);
    if (!created) {
        string errorMsg = "Unable to create file " + dstFile;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }
    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    stream << "package cvxif_sup_pkg;" << endl
           << "  parameter int unsigned X_RFW_WIDTH = "
              "cva6_config_pkg::CVA6ConfigXlen;"
           << endl
           << "  parameter int unsigned X_HARTID_WIDTH = "
              "cva6_config_pkg::CVA6ConfigXlen;"
           << endl
           << "  parameter int unsigned X_DUALWRITE = 0;" << endl
           << "  parameter int unsigned X_ID_WIDTH = "
              "$clog2(cva6_config_pkg::cva6_cfg.NrScoreboardEntries);"
           << endl
           << "  parameter int unsigned IdBits = 2**X_ID_WIDTH;" << endl
           << "  parameter int unsigned NConfigbits_C = 1 + X_ID_WIDTH + "
              "X_HARTID_WIDTH + 5 + (X_DUALWRITE + 1);"
           << endl
           << "  parameter logic [31:0] OpcodeMask = "
              "32'b11111_11_00000_00000_111_00000_1111111;"
           << endl  // For R type ones
           << "endpackage" << endl;
    stream.close();
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
RVCoprocessorGenerator::validateMachine(
    const TTAMachine::Machine& machine, std::ostream& errorStream,
    std::ostream& warningStream) {
    MachineValidator validator(machine);
    set<MachineValidator::ErrorCode> errorsToCheck;
    errorsToCheck.insert(MachineValidator::USED_IO_NOT_BOUND);
    errorsToCheck.insert(MachineValidator::FU_PORT_MISSING);

    MachineValidatorResults* results = validator.validate(errorsToCheck);

    for (int i = 0; i < results->errorCount(); i++) {
        MachineValidator::ErrorCode code = results->error(i).first;
        string errorMsg = results->error(i).second;
        if (code == MachineValidator::USED_IO_NOT_BOUND) {
            warningStream << "Warning: " << errorMsg << endl;
            delete results;
            throw IllegalMachine(__FILE__, __LINE__, __func__, "Error");
        } else {
            string msg = "Error: " + errorMsg;
            errorStream << msg << std::endl;
            delete results;
            throw IllegalMachine(__FILE__, __LINE__, __func__, msg);
        }
    }
    delete results;
}

}  // namespace ProGe
