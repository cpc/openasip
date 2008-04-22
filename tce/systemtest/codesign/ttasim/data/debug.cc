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
