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
