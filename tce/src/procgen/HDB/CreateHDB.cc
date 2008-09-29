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
 * @file CreateHDB.cc
 *
 * The command line tool that creates a new HDB.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
