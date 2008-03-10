/**
 * @file BEMGeneratorCmdLineOptions.cc
 *
 * Declaration of BEMGeneratorCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_GENERATOR_CMD_LINE_OPTIONS_HH
#define TTA_BEM_GENERATOR_CMD_LINE_OPTIONS_HH

#include "CmdLineOptions.hh"

/**
 * Command line options for the command line interface of bem generator 
 * (createbem).
 */
class BEMGeneratorCmdLineOptions : public CmdLineOptions {
public:
    BEMGeneratorCmdLineOptions();
    virtual ~BEMGeneratorCmdLineOptions();

    std::string adfFile() const;
    std::string outputFile() const;
    virtual void printVersion() const;
    virtual void printHelp() const;

private:
    /// Long name of the ADF file parameter.
    static const std::string ADF_PARAM_NAME;
    /// Long name of the output file parameter.
    static const std::string OUTPUT_FILE_PARAM_NAME;
};

#endif
