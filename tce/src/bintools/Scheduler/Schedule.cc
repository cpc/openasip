/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file Schedule.cc
 *
 * Implementation of the main function of the schedule application.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include "Exception.hh"
#include "SchedulerFrontend.hh"
#include "SchedulerCmdLineOptions.hh"

using std::cerr;
using std::endl;

/**
 * The main function of schedule application.
 */
int main(int argc, char* argv[]) {

    SchedulerCmdLineOptions options;
    SchedulerFrontend frontend;

    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& e) {
        cerr << e.errorMessage() << endl;
        return EXIT_FAILURE;
    }

    if (options.numberOfArguments() == 0) {
        options.printHelp();
        return EXIT_SUCCESS;
    } else if (options.numberOfArguments() > 1) {
        cerr << "Illegal number of arguments" << endl;
        return EXIT_FAILURE;
    }

    try {
        frontend.schedule(options);
    } catch (const Exception& e) {
        cerr << "Run-time error in a scheduler pass:"
             << e.errorMessageStack() << endl;
        return EXIT_FAILURE;
    } catch (...) {
        abortWithError("Unexpected exception trying to start scheduling!");
    }

    return EXIT_SUCCESS;
}
