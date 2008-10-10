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
 * @file ExplorerCmdLineOptions.hh
 *
 * Declaration of ExplorerCmdLineOptions.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
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
    int testApplicationDirectoryCount() const;
    std::string testApplicationDirectory(int index) const
        throw (OutOfRange);
    int explorerPluginParameterCount() const;
    std::string explorerPluginParameter(int index) const
        throw (OutOfRange);
    bool numberOfConfigurations() const;
    bool printSummary() const;
    std::string summaryOrdering() const;
    bool writeOutConfiguration() const;
    int numberOfConfigurationsToWrite() const;
    RowID configurationToWrite(int index) const
        throw (OutOfRange);
    bool printApplications() const;
    int applicationIDToRemoveCount() const;
    RowID applicationIDToRemove(int index) const
        throw (OutOfRange);
    bool hdbFileNames() const;
    int hdbFileNameCount() const;
    std::string hdbFileName(int index) const
        throw (OutOfRange);
    RowID startConfiguration() const;
    int verboseLevel() const;
    bool adfFile() const;
    bool idfFile() const;
    std::string adfFileName() const;
    std::string idfFileName() const;

private:
    /// Copying not allowed.
    ExplorerCmdLineOptions(const ExplorerCmdLineOptions&);
    /// Assignment not allowed.
    ExplorerCmdLineOptions& operator=(const ExplorerCmdLineOptions&);
};

#endif
