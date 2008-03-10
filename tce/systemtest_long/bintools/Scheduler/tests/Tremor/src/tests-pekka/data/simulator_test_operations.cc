/**
 * @file simulator_test_operations.cc
 *
 * Behavior definition file for simulator test operations.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include <fstream>
#include <cstdlib>
#include <ios>

#include "OSAL.hh"


//////////////////////////////////////////////////////////////////////////////
// STDOUT - treats the only input as a char and prints it to the output stream
//          thus simulating output from the TTA
//////////////////////////////////////////////////////////////////////////////
OPERATION(STDOUT)

TRIGGER
    OUTPUT_STREAM << static_cast<char>(INT(1));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(STDOUT);


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
        OUTPUT_STREAM << "No TTASIM_INPUT_FILE set, using default file name, "
                      << "'ttasim.in'." << std::endl;
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

END_DEFINE_STATE;

OPERATION_WITH_STATE(INPUT_DATA, INPUT_STREAM)

TRIGGER
    char input;
    STATE.inputFile >> input;
    INT(2) = static_cast<int>(input);
    OUTPUT_STREAM << "reading\n";
    // test if there's more data to read
    STATE.inputFile >> input;    
    if (STATE.inputFile.eof()) {
        INT(3) = 0;
    } else {
        INT(3) = 1;
    }
    STATE.inputFile.putback(input);

    RETURN_READY;
END_TRIGGER;

END_OPERATION_WITH_STATE(INPUT_DATA);

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
        OUTPUT_STREAM << "No TTASIM_OUTPUT_FILE set, using default file name, "
                      << "'ttasim.out'." << std::endl;
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

END_DEFINE_STATE;


OPERATION_WITH_STATE(OUTPUT_DATA, OUTPUT_STREAM)

TRIGGER

    STATE.outputFile << static_cast<char>(INT(1)) << std::flush;
//    OUTPUT_STREAM << "writing\n";

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
    RETURN_READY;
END_TRIGGER;

END_OPERATION_WITH_STATE(OUTPUT_DATA);


//////////////////////////////////////////////////////////////////////////////
// PRINT_INT - Treats the only input as an integer and prints it to the 
//             output stream in decimal format. 
//////////////////////////////////////////////////////////////////////////////
OPERATION(PRINT_INT)

TRIGGER
    OUTPUT_STREAM << static_cast<int>(INT(1));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(PRINT_INT);

//////////////////////////////////////////////////////////////////////////////
// PRINT_INT_AS_HEX - Treats the only input as an integer and prints it to the 
//             output stream in hexadecimal format. 
//////////////////////////////////////////////////////////////////////////////
OPERATION(PRINT_INT_AS_HEX)

TRIGGER
    OUTPUT_STREAM << Conversion::toHexString(INT(1));
    RETURN_READY;
END_TRIGGER;

END_OPERATION(PRINT_INT_AS_HEX);

