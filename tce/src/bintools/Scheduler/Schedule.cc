/**
 * @file Schedule.cc
 *
 * Implementation of the main function of the schedule application.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
