/**
 * @file ExplorerCmdLineOptions.hh
 *
 * Declaration of ExplorerCmdLineOptions.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
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
