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
 * @file simulator_test_operations.cc
 *
 * Behavior definition file for simulator test operations.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 */

#include <fstream>
#include <cstdlib>
#include <ios>

#include "OSAL.hh"
#include "OperationGlobals.hh"
#include "Conversion.hh"

//////////////////////////////////////////////////////////////////////////////
// INPUT_DATA - When operand 1 written arbitrary data, a new byte is read from
//              the input file and made readable in the second operand.
//              3rd operand is 1 in case there is more data to read, and 0 
//              in case of end of file.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(INPUT_STREAM)
    std::ifstream inputFile;

INIT_STATE(INPUT_STREAM)
    const char* fileNameFromEnv = getenv("TTASIM_INPUT_FILE");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "ttasim.in";
    } else {
        fileName = fileNameFromEnv;
    }
    inputFile.open(fileName.c_str());
    if (!inputFile.is_open()) {
        OUTPUT_STREAM << "Cannot open input file!" << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(INPUT_STREAM)
    inputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

OPERATION_WITH_STATE(INPUT_DATA, INPUT_STREAM)

TRIGGER
    char input;
    STATE.inputFile >> input;
    IO(2) = static_cast<int>(input);
    OUTPUT_STREAM << "reading\n";
    // test if there's more data to read
    STATE.inputFile >> input;    
    if (STATE.inputFile.eof()) {
        IO(3) = 0;
    } else {
        IO(3) = 1;
    }
    STATE.inputFile.putback(input);

    RETURN_READY;
END_TRIGGER;

END_OPERATION_WITH_STATE(INPUT_DATA)

//////////////////////////////////////////////////////////////////////////////
// OUTPUT_DATA - When operand 1 written arbitrary data, a new byte is written
//               to the output file defined in env TTASIM_OUTPUT_FILE.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(OUTPUT_STREAM)
    std::ofstream outputFile;

INIT_STATE(OUTPUT_STREAM)
    const char* fileNameFromEnv = getenv("TTASIM_OUTPUT_FILE");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "ttasim.out";
    } else {
        fileName = fileNameFromEnv;
    }
    outputFile.open(
        fileName.c_str(), 
        std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outputFile.is_open()) {
        OUTPUT_STREAM << "Cannot open output file!" << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(OUTPUT_STREAM)
    outputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE


OPERATION_WITH_STATE(OUTPUT_DATA, OUTPUT_STREAM)

TRIGGER

    STATE.outputFile << static_cast<char>(INT(1)) << std::flush;
    OUTPUT_STREAM << "writing\n";

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
    RETURN_READY;
END_TRIGGER;

END_OPERATION_WITH_STATE(OUTPUT_DATA)


//////////////////////////////////////////////////////////////////////////////
// PRINT_INT - Treats the only input as an integer and prints it to the 
//             output stream in decimal format. 
//////////////////////////////////////////////////////////////////////////////
OPERATION(PRINT_INT)

TRIGGER
    OUTPUT_STREAM << static_cast<int>(INT(1));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(PRINT_INT)

//////////////////////////////////////////////////////////////////////////////
// PRINT_INT_AS_HEX - Treats the only input as an integer and prints it to the 
//             output stream in hexadecimal format. 
//////////////////////////////////////////////////////////////////////////////
OPERATION(PRINT_INT_AS_HEX)

TRIGGER
    OUTPUT_STREAM << Conversion::toHexString(INT(1));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(PRINT_INT_AS_HEX)

//////////////////////////////////////////////////////////////////////////////
// OUTPUTDATA - When operand 1 written arbitrary data, a new byte is written
//               to the output file defined in env TTASIM_OUTPUT_FILE.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(OUTPUTDATA, OUTPUT_STREAM)

TRIGGER

    STATE.outputFile << static_cast<char>(INT(1)) << std::flush;

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
    RETURN_READY;
END_TRIGGER;

END_OPERATION_WITH_STATE(OUTPUTDATA)




