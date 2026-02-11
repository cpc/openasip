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
/**
 * @file ExplorerCmdLineOptions.hh
 *
 * Declaration of ExplorerCmdLineOptions.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011, 2017
 * @note rating: red
 */

#ifndef TTA_EXPLORER_CMDLINE_OPTIONS_HH
#define TTA_EXPLORER_CMDLINE_OPTIONS_HH

#include <string>

#include "CmdLineOptions.hh"
#include "DBTypes.hh"

/**
 * Command line option class for Explorer.
 */
class ExplorerCmdLineOptions : public CmdLineOptions {
public:
    ExplorerCmdLineOptions();
    virtual ~ExplorerCmdLineOptions();

    virtual void printVersion() const;
    virtual void printHelp() const;

    std::string explorerPlugin() const;
    std::string pluginInfo() const;
    int testApplicationDirectoryCount() const;
    std::string testApplicationDirectory(int index) const;
    int explorerPluginParameterCount() const;
    std::string explorerPluginParameter(int index) const;
    bool numberOfConfigurations() const;
    bool printSummary() const;
    std::string summaryOrdering() const;
    bool writeOutConfiguration() const;
    bool writeOutBestConfiguration() const;

    int numberOfConfigurationsToWrite() const;
    RowID configurationToWrite(int index) const;
    bool printApplications() const;
    bool printPlugins() const;
    int applicationIDToRemoveCount() const;
    RowID applicationIDToRemove(int index) const;
    bool hdbFileNames() const;
    int hdbFileNameCount() const;
    std::string hdbFileName(int index) const;
    RowID startConfiguration() const;
    bool adfFile() const;
    bool adfOutFile() const;

    bool idfFile() const;
    std::string adfFileName() const;
    std::string adfOutFileName() const;
    std::string idfFileName() const;

    std::string paretoSetValues() const;

    bool compilerOptions() const;
    std::string compilerOptionsString() const;

    ExplorerCmdLineOptions(const ExplorerCmdLineOptions&) = delete;
    ExplorerCmdLineOptions& operator=(const ExplorerCmdLineOptions&) = delete;
};

#endif
