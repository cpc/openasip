/*
    Copyright (c) 2002-2009 Tampere University.

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
