/**
 * @file CreateHDBCmdLineOptions.cc
 *
 * Declaration of CreateHDBCmdLineOptions.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "CmdLineOptions.hh"
#include "CreateHDBCmdLineOptions.hh"
#include "config.h"

/**
 * Constructor.
 */
CreateHDBCmdLineOptions::CreateHDBCmdLineOptions() : CmdLineOptions("") {

}

/**
 * Destructor.
 */
CreateHDBCmdLineOptions::~CreateHDBCmdLineOptions() {
}

/**
 * Prints the version of the program.
 */
void
CreateHDBCmdLineOptions::printVersion() const {
    std::cout << "CreateHDB - TCE Hardware Database (HDB) initializer "
              << VERSION << std::endl; 
}

/**
 * Prints the help menu of the program.
 */
void
CreateHDBCmdLineOptions::printHelp() const {
    printVersion();
    std::cout << std::endl 
              << "usage: createhdb [target file] "
              << std::endl
              << std::endl
              << "The target file is mandatory."
              << std::endl;
    CmdLineOptions::printHelp();
}

