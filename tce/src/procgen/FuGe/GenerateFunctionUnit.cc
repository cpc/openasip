/*
    Copyright (c) 2002-2015 Tampere University of Technology.

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
 * @file GenerateFunctionUnit.cc
 *
 * Definition of command line interface for function unit generation.
 *
 * @author Nicolai Behmann 2015 (behmann-no.spam-ims.uni-hannover.de)
 * @note rating: red
 */

#include <stdlib.h>
#include <stdint.h>

#include <iostream>
#include <string>
#include <fstream>

#include "FileSystem.hh"
#include "GenerateFunctionUnit.hh"
#include "FUGenerator.hh"
#include "ADFSerializer.hh"
#include "Machine.hh"
#include "CachedHDBManager.hh"

// TODO more explanaition here
int32_t printHelp(int32_t argc, const char **argv) {
    std::cout << argv[0] << " - function unit creation utility - "
    << std::string(VERSION) << std::endl << std::endl;
    std::cout << "generates a function unit from a given ADF." << std::endl;
    std::cout << "- explanation here." << std::endl;
    std::cout << std::endl << "Usage: " << argv[0] << " [OPTIONS] ADF"
    << std::endl;
    std::cout << "- options to be added." << std::endl;

    return EXIT_SUCCESS;
}

int32_t main(int32_t argc, const char **argv) {
    // check for wrong usage
    if ((argc < 2) || std::string(argv[1]) == std::string("--help")) {
        printHelp(argc, argv);
        return EXIT_FAILURE;
    }

    // read program arguments
    std::string filenameADF;
    for (int32_t iarg = 1; iarg < argc; ++iarg) {
        if (argv[iarg][0] != '-') {
            filenameADF = std::string(argv[iarg]);
            break;
        }
        //else // TODO read options
    }
    //std::cout << " >> ADF filename: " << filenameADF << std::endl;

    ADFSerializer reader;
    reader.setSourceFile(filenameADF);
    TTAMachine::Machine* machine = NULL;

    try {
        machine = reader.readMachine();
    } catch (Exception& e) {
        std::cerr << "Error opening " << filenameADF << ":" << std::endl
        << std::endl << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    const TTAMachine::Machine::FunctionUnitNavigator& nav =
    machine->functionUnitNavigator();
    std::cout << " >> found " << nav.count() << " function units in file "
    << filenameADF << "." << std::endl;
    if (nav.count() == 0) {
        std::cerr << "The file " << filenameADF
        << " doesn't contain any function units." << std::endl;
        return EXIT_FAILURE;
    }

    for (int32_t ifu = 0; ifu < nav.count(); ++ifu) {
        std::cout << "(" << ifu << ")\t" << nav.item(ifu)->name()
        << std::endl;
    }
    std::cout << " >> choose function unit: ";

    int32_t fuid = -1;
    do {
        std::cin >> fuid;
        if (fuid < 0 || fuid >= nav.count())
            std::cerr << " >> id not in range." << std::endl;
    } while(fuid < 0 || fuid >= nav.count());

    std::cout << " >> generating vhdl description for function unit: "
    << nav.item(fuid)->name() << "." << std::endl;

    TTAMachine::FunctionUnit* functionUnit = nav.item(fuid);

    // get VHDL filename
    std::string filenameVHDL = functionUnit->name() + ".vhdl";
    int i = 1;
    // check if file already exists
    while (FileSystem::fileExists(filenameVHDL)) {
        filenameVHDL = functionUnit->name() + "_" + std::to_string(i++) +
        ".vhdl";
    }

    FUGenerator *fu_generator = new FUGenerator(functionUnit);
    fu_generator->generateVHDL(filenameVHDL);
    
    // TODO ask for HDB, where fu should be appended
    std::string filenameHDB("");
    if (filenameHDB.size() != 0) {
        // HDB already exists
    } else {
        filenameHDB = functionUnit->name() + ".hdb";
        int i = 1;
        // check if file afunctionUnitlready exists
        while (FileSystem::fileExists(filenameHDB)) {
            filenameHDB = functionUnit->name() + "_" + std::to_string(i++)
            + ".hdb";
        }
    }
    
    fu_generator->addFUImplementationToHDB(filenameHDB);

    delete fu_generator;

    //functionUnit->unsetMachine();
    //delete machine;

    return EXIT_SUCCESS;
}
