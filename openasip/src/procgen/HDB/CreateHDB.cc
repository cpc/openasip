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
 * @file CreateHDB.cc
 *
 * The command line tool that creates a new HDB.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
    } catch (ParserStopRequest const&) {
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
