/**
 * @file BEMViewerCmdLineOptions.cc
 *
 * Implementation of BEMViewerCmdLineOptions class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "BEMViewerCmdLineOptions.hh"
#include "config.h"

/**
 * The constructor.
 */
BEMViewerCmdLineOptions::BEMViewerCmdLineOptions() :
    CmdLineOptions("BEM viewer", VERSION) {
}


/**
 * The destructor.
 */
BEMViewerCmdLineOptions::~BEMViewerCmdLineOptions() {
}


/**
 * Returns the given BEM file.
 *
 * @return The BEM file.
 */
std::string
BEMViewerCmdLineOptions::bemFile() const {
    if (numberOfArguments() < 1) {
        return "";
    } else {
        return argument(1);
    }
}


/**
 * Prints the version of the application.
 */
void
BEMViewerCmdLineOptions::printVersion() const {
    std::cout << "viewbem - BEM Viewer " << VERSION << std::endl;
}


/**
 * Prints the help.
 */
void
BEMViewerCmdLineOptions::printHelp() const {
    printVersion();
    std::cout << "Usage: viewbem <bemfile>" << std::endl;
}
