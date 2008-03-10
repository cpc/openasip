/**
 * @file GenerateProcessor.hh
 *
 * Declaration of GenerateProcessor class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_GENERATE_PROCESSOR_HH
#define TTA_GENERATE_PROCESSOR_HH

#include "ProGeUI.hh"
#include "ProGeCmdLineOptions.hh"
#include "Exception.hh"

/**
 * Implements the command line user interface 'generateprocessor'.
 */
class GenerateProcessor : public ProGe::ProGeUI {
public:
    GenerateProcessor();
    virtual ~GenerateProcessor();

    bool generateProcessor(int argc, char* argv[]);

private:
    void getOutputDir(
        const ProGeCmdLineOptions& options,
        std::string& outputDir);
    bool listICDecPluginParameters(const std::string& pluginFile) const;
};

#endif
