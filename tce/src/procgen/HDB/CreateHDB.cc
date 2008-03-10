/**
 * @file CreateHDB.cc
 *
 * The command line tool that creates a new HDB.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "Application.hh"
#include "CreateHDBCmdLineOptions.hh"
#include "HDBManager.hh"
#include "Exception.hh"
#include "FileSystem.hh"

/**
 * Main function.
 *
 * Parses the command line and creates a new HDB.
 *
 * @param argc The command line argument count.
 * @param argv The command line arguments (passed to the interpreter).
 * @return The return status.
 */
int 
main(int argc, char* argv[]) {

    Application::initialize();

    CreateHDBCmdLineOptions options;
    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
            return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& i) {
        std::cerr << i.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    if (options.numberOfArguments() != 1) {
        std::cerr << "Illegal number of arguments." << std::endl;
        return EXIT_FAILURE;
    }
    
    const std::string fileName = options.argument(1);

    if (FileSystem::fileExists(fileName)) {
        std::cerr << "File already exists." << std::endl;
        return EXIT_SUCCESS;
    }

    try {
        HDB::HDBManager::createNew(fileName);
    } catch (const Exception& e) {
        std::cerr << "Error while creating HDB. " 
                  << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
