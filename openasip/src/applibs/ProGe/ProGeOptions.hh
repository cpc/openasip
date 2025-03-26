/*
 Copyright (c) 2002-2017 Tampere University.

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
/*
 * @file ProGeOptions.hh
 *
 * Declaration of ProGeOptions class.
 *
 * @author Lasse Lehtonen 2017.
 */

#pragma once

#include "FileSystem.hh"
#include "ProGeCmdLineOptions.hh"
#include "ProGeTypes.hh"
#include <string>
#include <utility>
#include <vector>

struct ProGeOptions {

    ProGeOptions()
        : generateTestbench(false), clockFrequency(1),
          useAbsolutePaths(false), listAvailableIntegrators(false),
          forceOutputDirectory(false), asyncReset(true), syncReset(false),
          preferHDLGeneration(false), resetAllRegisters(true) {
        validate();
    }

    ProGeOptions(const ProGeCmdLineOptions& cmd)
        : processorToGenerate(cmd.processorToGenerate()),
          bemFile(cmd.bemFile()), idfFile(cmd.idfFile()),
          languageStr(cmd.hdl()), outputDirectory(cmd.outputDirectory()),
          sharedOutputDirectory(cmd.sharedOutputDirectory()),
          pluginParametersQuery(cmd.pluginParametersQuery()),
          generateTestbench(cmd.generateTestbench()),
          integratorName(cmd.integratorName()), imemType(cmd.imemType()),
          dmemType(cmd.dmemType()), clockFrequency(cmd.clockFrequency()),
          tpefName(cmd.tpefName()), entityName(cmd.entityName()),
          useAbsolutePaths(cmd.useAbsolutePaths()),
          listAvailableIntegrators(cmd.listAvailableIntegrators()),
          deviceFamilyName(cmd.deviceFamilyName()),
          deviceName(cmd.deviceName()),
          simulationRuntime(cmd.simulationRuntime()),
          forceOutputDirectory(cmd.forceOutputDirectory()),
          asyncReset(cmd.asyncReset()), syncReset(cmd.syncReset()),
          hdbList(cmd.hdbList()), rfIcGateList(cmd.rfIcGateList()),
          fuIcGateList(cmd.fuIcGateList()), icdArgList(cmd.icdArgList()),
          preferHDLGeneration(cmd.preferHDLGeneration()),
          resetAllRegisters(cmd.resetAllRegisters()),
          fuBackRegistered(cmd.fuBackRegistered()),
          fuFrontRegistered(cmd.fuFrontRegistered()),
          fuMiddleRegistered(cmd.fuMiddleRegistered()),
          dontCareInitialization(cmd.dontCareInitialization()) {
        validate();
    }

    std::string processorToGenerate;
    std::string bemFile;
    std::string idfFile;
    std::string languageStr;
    ProGe::HDL language;
    std::string outputDirectory;
    std::string sharedOutputDirectory;
    std::string pluginParametersQuery;
    bool generateTestbench;

    std::string integratorName;
    std::string imemType;
    std::string dmemType;
    int clockFrequency;
    std::string tpefName;
    std::string entityName;
    bool useAbsolutePaths;
    bool listAvailableIntegrators;
    std::string deviceFamilyName;
    std::string deviceName;
    std::string simulationRuntime;
    bool forceOutputDirectory;

    bool asyncReset;
    bool syncReset;
    std::vector<std::string> hdbList;
    std::vector<std::string> rfIcGateList;
    std::vector<std::string> fuIcGateList;
    std::vector<std::pair<std::string, std::string>> icdArgList;
    bool preferHDLGeneration;
    bool resetAllRegisters;
    std::vector<std::string> fuBackRegistered;
    std::vector<std::string> fuFrontRegistered;
    std::vector<std::string> fuMiddleRegistered;
    bool dontCareInitialization;


    void validate() {
        if (outputDirectory.empty()) {
            outputDirectory = "proge-output";
        }
        if (sharedOutputDirectory.empty()) {
            sharedOutputDirectory = outputDirectory;
        }
        FileSystem::expandTilde(sharedOutputDirectory);
        sharedOutputDirectory =
            FileSystem::absolutePathOf(sharedOutputDirectory);
        if (entityName.empty()) {
            entityName = "tta0";
        }
        if (languageStr == "verilog") {
            language = ProGe::HDL::Verilog;
        } else {
            language = ProGe::HDL::VHDL;
        }
        if (hdbList.empty()) {
            hdbList.emplace_back("generate_base32.hdb");
            hdbList.emplace_back("generate_lsu_32.hdb");
            hdbList.emplace_back("generate_rf_iu.hdb");
            hdbList.emplace_back("asic_130nm_1.5V.hdb");

        }
    }
};
