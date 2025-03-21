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
 * @file ProGeCmdLineOptions.hh
 *
 * Declaration of ProGeCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGE_CMD_LINE_OPTIONS_HH
#define TTA_PROGE_CMD_LINE_OPTIONS_HH

#include <utility> // std::pair
#include "CmdLineOptions.hh"
/**
 * Command line options for the command line interface of ProGe
 * (generateprocessor).
 */
class ProGeCmdLineOptions : public CmdLineOptions {
public:
    ProGeCmdLineOptions();
    virtual ~ProGeCmdLineOptions();

    std::string processorToGenerate() const;
    std::string bemFile() const;
    std::string idfFile() const;
    std::string hdl() const;
    std::string outputDirectory() const;
    std::string sharedOutputDirectory() const;
    std::string pluginParametersQuery() const;
    bool generateTestbench() const;

    std::string mode() const;

    // platform integrator params
    std::string integratorName() const;
    std::string imemType() const;
    std::string dmemType() const;
    int clockFrequency() const;
    std::string tpefName() const;
    std::string entityName() const;
    bool useAbsolutePaths() const;
    bool listAvailableIntegrators() const;
    std::string deviceFamilyName() const;
    std::string deviceName() const;
    std::string simulationRuntime() const;
    bool forceOutputDirectory() const;
    bool asyncReset() const;
    bool syncReset() const;
    std::vector<std::string> commaSeparatedList(const std::string argumentName) const;
    std::vector<std::string> hdbList() const;
    std::vector<std::string> rfIcGateList() const;
    std::vector<std::string> fuIcGateList() const;
    std::vector<std::pair<std::string, std::string>> icdArgList() const;
    bool preferHDLGeneration() const;
    bool resetAllRegisters() const;
    std::vector<std::string> fuBackRegistered() const;
    std::vector<std::string> fuFrontRegistered() const;
    std::vector<std::string> fuMiddleRegistered() const;
    bool dontCareInitialization() const;

    virtual void printVersion() const;
    virtual void printHelp() const;
};

#endif
