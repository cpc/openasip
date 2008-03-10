/**
 * @file ProGeCmdLineOptions.hh
 *
 * Declaration of ProGeCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGE_CMD_LINE_OPTIONS_HH
#define TTA_PROGE_CMD_LINE_OPTIONS_HH

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
    int imemWidthInMAUs() const;
    std::string outputDirectory() const;
    std::string pluginParametersQuery() const;

    virtual void printVersion() const;
    virtual void printHelp() const;
};

#endif
