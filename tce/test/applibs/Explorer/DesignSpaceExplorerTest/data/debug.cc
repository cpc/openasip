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
 * @file debug.cc
 *
 * Behavior definition file for simulator debugging operations.
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
// STDOUT - treats the only input as a char and prints it to the output stream
//          thus simulating output from the TTA
//////////////////////////////////////////////////////////////////////////////
OPERATION(STDOUT)

TRIGGER
    OUTPUT_STREAM << static_cast<char>(UINT(1));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(STDOUT)

//////////////////////////////////////////////////////////////////////////////
// PRINT_INT_AS_HEX - Treats the only input as an integer and prints it to the 
//             output stream in hexadecimal format. 
//////////////////////////////////////////////////////////////////////////////
OPERATION(PRINT_INT_AS_HEX)

TRIGGER
    OUTPUT_STREAM << Conversion::toHexString(UINT(1));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(PRINT_INT_AS_HEX)


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

  //////////////////////////////////////////////////////////////////////////////
  // OUTPUTDATA - When operand 1 written arbitrary data, a new byte is written
  //               to the output file defined in env TTASIM_OUTPUT_FILE.
  //////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(OUTPUTDATA, OUTPUT_STREAM)

TRIGGER
  std::cerr << "DEBUG: OUTPUTDATA(" << INT(1) << ")" << std::endl;
STATE.outputFile << static_cast<char>(INT(1)) << std::flush;

if (STATE.outputFile.fail()) {
  OUTPUT_STREAM << "error while writing the output file" << std::endl;
}
RETURN_READY;
END_TRIGGER;

END_OPERATION_WITH_STATE(OUTPUTDATA)

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
