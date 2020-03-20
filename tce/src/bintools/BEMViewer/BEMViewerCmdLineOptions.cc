/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file BEMViewerCmdLineOptions.cc
 *
 * Implementation of BEMViewerCmdLineOptions class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "BEMViewerCmdLineOptions.hh"
#include "tce_config.h"

/**
 * The constructor.
 */
BEMViewerCmdLineOptions::BEMViewerCmdLineOptions() :
    CmdLineOptions("BEM viewer", Application::TCEVersionString()) {
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
    std::cout << "viewbem - BEM Viewer " 
              << Application::TCEVersionString() << std::endl;
}


/**
 * Prints the help.
 */
void
BEMViewerCmdLineOptions::printHelp() const {
    printVersion();
    std::cout << "Usage: viewbem <bemfile>" << std::endl;
}
