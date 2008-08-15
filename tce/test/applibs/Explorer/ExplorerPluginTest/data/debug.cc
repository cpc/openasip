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
 * @file debug.cc
 *
 * Behavior definition file for simulator debugging operations.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include <fstream>
#include <cstdlib>
#include <ios>

#include "OSAL.hh"
#include "OperationGlobals.hh"

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
