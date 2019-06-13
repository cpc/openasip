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
 * @file CreateBEM.cc
 *
 * Implements the main function of createbem application which creates a
 * binary encoding map.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <string>

#include "BEMGeneratorCmdLineOptions.hh"
#include "BEMSerializer.hh"
#include "BEMGenerator.hh"
#include "BinaryEncoding.hh"
#include "Machine.hh"
#include "ADFSerializer.hh"
#include "FileSystem.hh"

using std::cerr;
using std::endl;
using std::string;

using namespace TTAMachine;


/**
 * Loads the given ADF file and creates a Machine instance from it.
 *
 * @param adfFile The ADF file.
 * @return The newly created Machine instance.
 * @exception SerializerException If an error occurs while reading the file.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state of Machine instance.
 */
static Machine*
loadMachine(const std::string& adfFile) {
    ADFSerializer serializer;
    serializer.setSourceFile(adfFile);
    return serializer.readMachine();
}

/**
 * Generates the name of the output file from the given ADF file name.
 * 
 * The generated name has same body but .bem suffix.
 *
 * @param adfFile The name of the ADF file.
 * @return The name of the output file.
 */
static std::string
outputFileName(const std::string& adfFile) {
    string file = FileSystem::fileNameBody(adfFile);
    return file + ".bem";
}
    

/**
 * The main function.
 */
int main(int argc, char* argv[]) {
    BEMGeneratorCmdLineOptions options;
    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& exception) {
        cerr << exception.errorMessage() << endl;
        return EXIT_FAILURE;
    }

    string adfFile = options.adfFile();
    string outputFile = options.outputFile();
    if (adfFile == "") {
        options.printHelp();
        return EXIT_FAILURE;
    }

    Machine* mach = NULL;
    try {
        mach = loadMachine(adfFile);
    } catch (const Exception& e) {
        cerr << "Error while loading machine from adf:" << endl
             << e.fileName() << ", line: " << e.lineNum() << endl
             << e.errorMessage() << endl;
        return EXIT_FAILURE;
    }

    BEMGenerator generator(*mach);
    BinaryEncoding* bem = generator.generate();
    
    BEMSerializer serializer;

    if (outputFile == "") {
        outputFile = outputFileName(adfFile);
    }

    serializer.setDestinationFile(outputFile);
    serializer.writeBinaryEncoding(*bem);

    delete mach;
    delete bem;

    return EXIT_SUCCESS;
}
