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
 * @file CreateBEM.cc
 *
 * Implements the main function of createbem application which creates a
 * binary encoding map.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
loadMachine(const std::string& adfFile) 
    throw (SerializerException, ObjectStateLoadingException) {

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
