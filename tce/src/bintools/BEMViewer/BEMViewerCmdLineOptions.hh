/**
 * @file BEMViewerCmdLineOptions.hh
 *
 * Declaration of BEMViewerCmdLineOptions class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_VIEWER_CMD_LINE_OPTIONS_HH
#define TTA_BEM_VIEWER_CMD_LINE_OPTIONS_HH

#include "CmdLineOptions.hh"

/**
 * Command line options for viewbem.
 */
class BEMViewerCmdLineOptions : public CmdLineOptions {
public:
    BEMViewerCmdLineOptions();
    virtual ~BEMViewerCmdLineOptions();

    std::string bemFile() const;
    virtual void printVersion() const;
    virtual void printHelp() const;
};

#endif
