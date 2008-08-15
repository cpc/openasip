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
