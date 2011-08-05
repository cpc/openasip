/**
 * OSAL behavior definition file.
 */
#include <iostream>
#include <fstream>
#include <math.h> // isnan()

#include "OSAL.hh"
#include "OperationGlobals.hh"
#include "Application.hh"
#include "Conversion.hh"

#define FIFO_SIZE 256

//////////////////////////////////////////////////////////////////////////////
// INPUT_STREAM_V1 - State definition for the STREAM_IN_V1.
//
// Opens a file simulating the input stream. Default filename is tta_stream.in,
// and can be changed with the environment variable TTASIM_STREAM_IN_FILE_V1.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(INPUT_STREAM_V1)
    std::ifstream inputFile;

INIT_STATE(INPUT_STREAM_V1)
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_IN_FILE_V1");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v1.in";
    } else {
        fileName = fileNameFromEnv;
    }
    inputFile.open(fileName.c_str(), std::ios_base::binary);
    if (!inputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open input stream file " 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(INPUT_STREAM_V1)
    inputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_V1 - Reads a sample from the default input stream.
//
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_V1, INPUT_STREAM_V1)

TRIGGER
    if (BWIDTH(2) != 32) {
        Application::logStream() 
            << "STREAM_IN_V1 works with 32 bit signed integers only at the moment." 
            << std::endl;
    }
    if (!STATE.inputFile.is_open()) {
        IO(2) = 0;
        return true;
    }
	int inum;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_V1)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_STATUS_V1 - Reads the status of the input buffer.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_STATUS_V1, INPUT_STREAM_V1)

TRIGGER
	unsigned int offset;
	offset = FIFO_SIZE*10 - STATE.inputFile.tellg();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_STATUS_V1)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_PEEK_V1 - Checks the value of the next token
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_PEEK_V1, INPUT_STREAM_V1)

TRIGGER
	int inum, i;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
	for(i = 9; i >= 0; i--) STATE.inputFile.putback(input[i]);
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_PEEK_V1)

//////////////////////////////////////////////////////////////////////////////
// INPUT_STREAM_V2 - State definition for the STREAM_IN_V2.
//
// Opens a file simulating the input stream. Default filename is tta_stream.in,
// and can be changed with the environment variable TTASIM_STREAM_IN_FILE_V2.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(INPUT_STREAM_V2)
    std::ifstream inputFile;

INIT_STATE(INPUT_STREAM_V2)
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_IN_FILE_V2");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v2.in";
    } else {
        fileName = fileNameFromEnv;
    }
    inputFile.open(fileName.c_str(), std::ios_base::binary);
    if (!inputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open input stream file " 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(INPUT_STREAM_V2)
    inputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_V2 - Reads a sample from the default input stream.
//
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_V2, INPUT_STREAM_V2)

TRIGGER
    if (BWIDTH(2) != 32) {
        Application::logStream() 
            << "STREAM_IN_V2 works with 32 bit signed integers only at the moment." 
            << std::endl;
    }
    if (!STATE.inputFile.is_open()) {
        IO(2) = 0;
        return true;
    }
	int inum;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_V2)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_STATUS_V2 - Reads the status of the input buffer.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_STATUS_V2, INPUT_STREAM_V2)

TRIGGER
	unsigned int offset;
	offset = FIFO_SIZE*10 - STATE.inputFile.tellg();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_STATUS_V2)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_PEEK_V2 - Checks the value of the next token
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_PEEK_V2, INPUT_STREAM_V2)

TRIGGER
	int inum, i;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
	for(i = 9; i >= 0; i--) STATE.inputFile.putback(input[i]);
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_PEEK_V2)


//////////////////////////////////////////////////////////////////////////////
// INPUT_STREAM_V3 - State definition for the STREAM_IN_V3.
//
// Opens a file simulating the input stream. Default filename is tta_stream.in,
// and can be changed with the environment variable TTASIM_STREAM_IN_FILE_V3.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(INPUT_STREAM_V3)
    std::ifstream inputFile;

INIT_STATE(INPUT_STREAM_V3)
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_IN_FILE_V3");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v3.in";
    } else {
        fileName = fileNameFromEnv;
    }
    inputFile.open(fileName.c_str(), std::ios_base::binary);
    if (!inputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open input stream file " 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(INPUT_STREAM_V3)
    inputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_V3 - Reads a sample from the default input stream.
//
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_V3, INPUT_STREAM_V3)

TRIGGER
    if (BWIDTH(2) != 32) {
        Application::logStream() 
            << "STREAM_IN_V3 works with 32 bit signed integers only at the moment." 
            << std::endl;
    }
    if (!STATE.inputFile.is_open()) {
        IO(2) = 0;
        return true;
    }
	int inum;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_V3)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_STATUS_V3 - Reads the status of the input buffer.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_STATUS_V3, INPUT_STREAM_V3)

TRIGGER
	unsigned int offset;
	offset = FIFO_SIZE*10 - STATE.inputFile.tellg();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_STATUS_V3)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_PEEK_V3 - Checks the value of the next token
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_PEEK_V3, INPUT_STREAM_V3)

TRIGGER
	int inum, i;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
	for(i = 9; i >= 0; i--) STATE.inputFile.putback(input[i]);
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_PEEK_V3)


//////////////////////////////////////////////////////////////////////////////
// INPUT_STREAM_V4 - State definition for the STREAM_IN_V4.
//
// Opens a file simulating the input stream. Default filename is tta_stream.in,
// and can be changed with the environment variable TTASIM_STREAM_IN_FILE_V4.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(INPUT_STREAM_V4)
    std::ifstream inputFile;

INIT_STATE(INPUT_STREAM_V4)
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_IN_FILE_V4");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v4.in";
    } else {
        fileName = fileNameFromEnv;
    }
    inputFile.open(fileName.c_str(), std::ios_base::binary);
    if (!inputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open input stream file " 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(INPUT_STREAM_V4)
    inputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_V4 - Reads a sample from the default input stream.
//
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_V4, INPUT_STREAM_V4)

TRIGGER
    if (BWIDTH(2) != 32) {
        Application::logStream() 
            << "STREAM_IN_V4 works with 32 bit signed integers only at the moment." 
            << std::endl;
    }
    if (!STATE.inputFile.is_open()) {
        IO(2) = 0;
        return true;
    }
	int inum;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_V4)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_STATUS_V4 - Reads the status of the input buffer.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_STATUS_V4, INPUT_STREAM_V4)

TRIGGER
	unsigned int offset;
	offset = FIFO_SIZE*10 - STATE.inputFile.tellg();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_STATUS_V4)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_PEEK_V4 - Checks the value of the next token
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_PEEK_V4, INPUT_STREAM_V4)

TRIGGER
	int inum, i;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
	for(i = 9; i >= 0; i--) STATE.inputFile.putback(input[i]);
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_PEEK_V4)


//////////////////////////////////////////////////////////////////////////////
// INPUT_STREAM_V5 - State definition for the STREAM_IN_V5.
//
// Opens a file simulating the input stream. Default filename is tta_stream.in,
// and can be changed with the environment variable TTASIM_STREAM_IN_FILE_V5.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(INPUT_STREAM_V5)
    std::ifstream inputFile;

INIT_STATE(INPUT_STREAM_V5)
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_IN_FILE_V5");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v5.in";
    } else {
        fileName = fileNameFromEnv;
    }
    inputFile.open(fileName.c_str(), std::ios_base::binary);
    if (!inputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open input stream file " 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(INPUT_STREAM_V5)
    inputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_V5 - Reads a sample from the default input stream.
//
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_V5, INPUT_STREAM_V5)

TRIGGER
    if (BWIDTH(2) != 32) {
        Application::logStream() 
            << "STREAM_IN_V5 works with 32 bit signed integers only at the moment." 
            << std::endl;
    }
    if (!STATE.inputFile.is_open()) {
        IO(2) = 0;
        return true;
    }
	int inum;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_V5)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_STATUS_V5 - Reads the status of the input buffer.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_STATUS_V5, INPUT_STREAM_V5)

TRIGGER
	unsigned int offset;
	offset = FIFO_SIZE*10 - STATE.inputFile.tellg();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_STATUS_V5)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_PEEK_V5 - Checks the value of the next token
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_PEEK_V5, INPUT_STREAM_V5)

TRIGGER
	int inum, i;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
	for(i = 9; i >= 0; i--) STATE.inputFile.putback(input[i]);
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_PEEK_V5)


//////////////////////////////////////////////////////////////////////////////
// INPUT_STREAM_V6 - State definition for the STREAM_IN_V6.
//
// Opens a file simulating the input stream. Default filename is tta_stream.in,
// and can be changed with the environment variable TTASIM_STREAM_IN_FILE_V6.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(INPUT_STREAM_V6)
    std::ifstream inputFile;

INIT_STATE(INPUT_STREAM_V6)
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_IN_FILE_V6");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v6.in";
    } else {
        fileName = fileNameFromEnv;
    }
    inputFile.open(fileName.c_str(), std::ios_base::binary);
    if (!inputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open input stream file " 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(INPUT_STREAM_V6)
    inputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_V6 - Reads a sample from the default input stream.
//
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_V6, INPUT_STREAM_V6)

TRIGGER
    if (BWIDTH(2) != 32) {
        Application::logStream() 
            << "STREAM_IN_V6 works with 32 bit signed integers only at the moment." 
            << std::endl;
    }
    if (!STATE.inputFile.is_open()) {
        IO(2) = 0;
        return true;
    }
	int inum;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_V6)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_STATUS_V6 - Reads the status of the input buffer.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_STATUS_V6, INPUT_STREAM_V6)

TRIGGER
	unsigned int offset;
	offset = FIFO_SIZE*10 - STATE.inputFile.tellg();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_STATUS_V6)

//////////////////////////////////////////////////////////////////////////////
// STREAM_IN_PEEK_V6 - Checks the value of the next token
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_IN_PEEK_V6, INPUT_STREAM_V6)

TRIGGER
	int inum, i;
    char input[12];
    STATE.inputFile.getline(input, 12);
	std::istringstream iss(input);
    iss >> std::dec >> inum;
	for(i = 9; i >= 0; i--) STATE.inputFile.putback(input[i]);
    IO(2) = inum;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_IN_PEEK_V6)




//////////////////////////////////////////////////////////////////////////////
// OUTPUT_STREAM_V1 - State definition for the STREAM_OUT_V1.
//
// Opens a file simulating the output stream. Default filename is 
// tta_stream.out, and can be changed with the environment variable 
// TTASIM_STREAM_IN_FILE_V1.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(OUTPUT_STREAM_V1)
    std::ofstream outputFile;

INIT_STATE(OUTPUT_STREAM_V1)
 
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_OUT_FILE_V1");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v1.out";
    } else {
        fileName = fileNameFromEnv;
    }
    outputFile.open(
        fileName.c_str(), 
        std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open output file!" 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(OUTPUT_STREAM_V1)
    outputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_V1 - Writes a sample to the default output stream.
//
// @todo: Support for other sample sizes than 32.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_V1, OUTPUT_STREAM_V1)

TRIGGER

    if (BWIDTH(1) != 32) 
        Application::logStream() 
            << "STREAM_OUT_V1 works with signed integers only at the moment." 
            << std::endl;

    int data = UINT(1);
	STATE.outputFile.width(9);
	STATE.outputFile << data << std::endl;
    STATE.outputFile << std::flush;

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_V1)

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_STATUS_V1 - Reads the status of the output buffer.
//
// This simulation behavior always returns 1, which means output buffer
// is empty and can be written to.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_STATUS_V1, OUTPUT_STREAM_V1)

TRIGGER
	unsigned int offset;
	offset = STATE.outputFile.tellp();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_STATUS_V1)



//////////////////////////////////////////////////////////////////////////////
// OUTPUT_STREAM_V2 - State definition for the STREAM_OUT_V2.
//
// Opens a file simulating the output stream. Default filename is 
// tta_stream.out, and can be changed with the environment variable 
// TTASIM_STREAM_IN_FILE_V2.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(OUTPUT_STREAM_V2)
    std::ofstream outputFile;

INIT_STATE(OUTPUT_STREAM_V2)
 
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_OUT_FILE_V2");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v2.out";
    } else {
        fileName = fileNameFromEnv;
    }
    outputFile.open(
        fileName.c_str(), 
        std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open output file!" 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(OUTPUT_STREAM_V2)
    outputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_V2 - Writes a sample to the default output stream.
//
// @todo: Support for other sample sizes than 32.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_V2, OUTPUT_STREAM_V2)

TRIGGER

    if (BWIDTH(1) != 32) 
        Application::logStream() 
            << "STREAM_OUT_V2 works with signed integers only at the moment." 
            << std::endl;

    int data = UINT(1);
	STATE.outputFile.width(9);
	STATE.outputFile << data << std::endl;
    STATE.outputFile << std::flush;

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_V2)

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_STATUS_V2 - Reads the status of the output buffer.
//
// This simulation behavior always returns 0, which means output buffer
// is empty and can be written to.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_STATUS_V2, OUTPUT_STREAM_V2)

TRIGGER
	unsigned int offset;
	offset = STATE.outputFile.tellp();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_STATUS_V2)



//////////////////////////////////////////////////////////////////////////////
// OUTPUT_STREAM_V3 - State definition for the STREAM_OUT_V3.
//
// Opens a file simulating the output stream. Default filename is 
// tta_stream.out, and can be changed with the environment variable 
// TTASIM_STREAM_IN_FILE_V3.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(OUTPUT_STREAM_V3)
    std::ofstream outputFile;

INIT_STATE(OUTPUT_STREAM_V3)
 
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_OUT_FILE_V3");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v3.out";
    } else {
        fileName = fileNameFromEnv;
    }
    outputFile.open(
        fileName.c_str(), 
        std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open output file!" 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(OUTPUT_STREAM_V3)
    outputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_V3 - Writes a sample to the default output stream.
//
// @todo: Support for other sample sizes than 32.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_V3, OUTPUT_STREAM_V3)

TRIGGER

    if (BWIDTH(1) != 32) 
        Application::logStream() 
            << "STREAM_OUT_V3 works with signed integers only at the moment." 
            << std::endl;

    int data = UINT(1);
	STATE.outputFile.width(9);
	STATE.outputFile << data << std::endl;
    STATE.outputFile << std::flush;

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_V3)

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_STATUS_V3 - Reads the status of the output buffer.
//
// This simulation behavior always returns 0, which means output buffer
// is empty and can be written to.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_STATUS_V3, OUTPUT_STREAM_V3)

TRIGGER
	unsigned int offset;
	offset = STATE.outputFile.tellp();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_STATUS_V3)



//////////////////////////////////////////////////////////////////////////////
// OUTPUT_STREAM_V4 - State definition for the STREAM_OUT_V4.
//
// Opens a file simulating the output stream. Default filename is 
// tta_stream.out, and can be changed with the environment variable 
// TTASIM_STREAM_IN_FILE_V4.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(OUTPUT_STREAM_V4)
    std::ofstream outputFile;

INIT_STATE(OUTPUT_STREAM_V4)
 
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_OUT_FILE_V4");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v4.out";
    } else {
        fileName = fileNameFromEnv;
    }
    outputFile.open(
        fileName.c_str(), 
        std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open output file!" 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(OUTPUT_STREAM_V4)
    outputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_V4 - Writes a sample to the default output stream.
//
// @todo: Support for other sample sizes than 32.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_V4, OUTPUT_STREAM_V4)

TRIGGER

    if (BWIDTH(1) != 32) 
        Application::logStream() 
            << "STREAM_OUT_V4 works with signed integers only at the moment." 
            << std::endl;

    int data = UINT(1);
	STATE.outputFile.width(9);
	STATE.outputFile << data << std::endl;
    STATE.outputFile << std::flush;

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_V4)

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_STATUS_V4 - Reads the status of the output buffer.
//
// This simulation behavior always returns 0, which means output buffer
// is empty and can be written to.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_STATUS_V4, OUTPUT_STREAM_V4)

TRIGGER
	unsigned int offset;
	offset = STATE.outputFile.tellp();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_STATUS_V4)



//////////////////////////////////////////////////////////////////////////////
// OUTPUT_STREAM_V5 - State definition for the STREAM_OUT_V5.
//
// Opens a file simulating the output stream. Default filename is 
// tta_stream.out, and can be changed with the environment variable 
// TTASIM_STREAM_IN_FILE_V5.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(OUTPUT_STREAM_V5)
    std::ofstream outputFile;

INIT_STATE(OUTPUT_STREAM_V5)
 
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_OUT_FILE_V5");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v5.out";
    } else {
        fileName = fileNameFromEnv;
    }
    outputFile.open(
        fileName.c_str(), 
        std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open output file!" 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(OUTPUT_STREAM_V5)
    outputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_V5 - Writes a sample to the default output stream.
//
// @todo: Support for other sample sizes than 32.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_V5, OUTPUT_STREAM_V5)

TRIGGER

    if (BWIDTH(1) != 32) 
        Application::logStream() 
            << "STREAM_OUT_V5 works with signed integers only at the moment." 
            << std::endl;

    int data = UINT(1);
	STATE.outputFile.width(9);
	STATE.outputFile << data << std::endl;
    STATE.outputFile << std::flush;

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_V5)

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_STATUS_V5 - Reads the status of the output buffer.
//
// This simulation behavior always returns 0, which means output buffer
// is empty and can be written to.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_STATUS_V5, OUTPUT_STREAM_V5)

TRIGGER
	unsigned int offset;
	offset = STATE.outputFile.tellp();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_STATUS_V5)



//////////////////////////////////////////////////////////////////////////////
// OUTPUT_STREAM_V6 - State definition for the STREAM_OUT_V6.
//
// Opens a file simulating the output stream. Default filename is 
// tta_stream.out, and can be changed with the environment variable 
// TTASIM_STREAM_IN_FILE_V6.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(OUTPUT_STREAM_V6)
    std::ofstream outputFile;

INIT_STATE(OUTPUT_STREAM_V6)
 
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_OUT_FILE_V6");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v6.out";
    } else {
        fileName = fileNameFromEnv;
    }
    outputFile.open(
        fileName.c_str(), 
        std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open output file!" 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(OUTPUT_STREAM_V6)
    outputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_V6 - Writes a sample to the default output stream.
//
// @todo: Support for other sample sizes than 32.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_V6, OUTPUT_STREAM_V6)

TRIGGER

    if (BWIDTH(1) != 32) 
        Application::logStream() 
            << "STREAM_OUT_V6 works with signed integers only at the moment." 
            << std::endl;

    int data = UINT(1);
	STATE.outputFile.width(9);
	STATE.outputFile << data << std::endl;
    STATE.outputFile << std::flush;

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_V6)

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_STATUS_V6 - Reads the status of the output buffer.
//
// This simulation behavior always returns 0, which means output buffer
// is empty and can be written to.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_STATUS_V6, OUTPUT_STREAM_V6)

TRIGGER
	unsigned int offset;
	offset = STATE.outputFile.tellp();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_STATUS_V6)



//////////////////////////////////////////////////////////////////////////////
// OUTPUT_STREAM_V7 - State definition for the STREAM_OUT_V7.
//
// Opens a file simulating the output stream. Default filename is 
// tta_stream.out, and can be changed with the environment variable 
// TTASIM_STREAM_IN_FILE_V7.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(OUTPUT_STREAM_V7)
    std::ofstream outputFile;

INIT_STATE(OUTPUT_STREAM_V7)
 
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_OUT_FILE_V7");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v7.out";
    } else {
        fileName = fileNameFromEnv;
    }
    outputFile.open(
        fileName.c_str(), 
        std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open output file!" 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(OUTPUT_STREAM_V7)
    outputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_V7 - Writes a sample to the default output stream.
//
// @todo: Support for other sample sizes than 32.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_V7, OUTPUT_STREAM_V7)

TRIGGER

    if (BWIDTH(1) != 32) 
        Application::logStream() 
            << "STREAM_OUT_V7 works with signed integers only at the moment." 
            << std::endl;

    int data = UINT(1);
	STATE.outputFile.width(9);
	STATE.outputFile << data << std::endl;
    STATE.outputFile << std::flush;

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_V7)

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_STATUS_V7 - Reads the status of the output buffer.
//
// This simulation behavior always returns 0, which means output buffer
// is empty and can be written to.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_STATUS_V7, OUTPUT_STREAM_V7)

TRIGGER
	unsigned int offset;
	offset = STATE.outputFile.tellp();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_STATUS_V7)



//////////////////////////////////////////////////////////////////////////////
// OUTPUT_STREAM_V8 - State definition for the STREAM_OUT_V8.
//
// Opens a file simulating the output stream. Default filename is 
// tta_stream.out, and can be changed with the environment variable 
// TTASIM_STREAM_IN_FILE_V8.
//////////////////////////////////////////////////////////////////////////////

DEFINE_STATE(OUTPUT_STREAM_V8)
    std::ofstream outputFile;

INIT_STATE(OUTPUT_STREAM_V8)
 
    const char* fileNameFromEnv = getenv("TTASIM_STREAM_OUT_FILE_V8");
    std::string fileName = "";
    if (fileNameFromEnv == NULL) {
        fileName = "tta_stream_v8.out";
    } else {
        fileName = fileNameFromEnv;
    }
    outputFile.open(
        fileName.c_str(), 
        std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    if (!outputFile.is_open()) {
        OUTPUT_STREAM 
            << "Cannot open output file!" 
            << fileName << std::endl;
    }
END_INIT_STATE;

FINALIZE_STATE(OUTPUT_STREAM_V8)
    outputFile.close();
END_FINALIZE_STATE;

END_DEFINE_STATE

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_V8 - Writes a sample to the default output stream.
//
// @todo: Support for other sample sizes than 32.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_V8, OUTPUT_STREAM_V8)

TRIGGER

    if (BWIDTH(1) != 32) 
        Application::logStream() 
            << "STREAM_OUT_V8 works with signed integers only at the moment." 
            << std::endl;

    int data = UINT(1);
	STATE.outputFile.width(9);
	STATE.outputFile << data << std::endl;
    STATE.outputFile << std::flush;

    if (STATE.outputFile.fail()) {
        OUTPUT_STREAM << "error while writing the output file" << std::endl;
    }
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_V8)

//////////////////////////////////////////////////////////////////////////////
// STREAM_OUT_STATUS_V8 - Reads the status of the output buffer.
//
// This simulation behavior always returns 0, which means output buffer
// is empty and can be written to.
//////////////////////////////////////////////////////////////////////////////

OPERATION_WITH_STATE(STREAM_OUT_STATUS_V8, OUTPUT_STREAM_V8)

TRIGGER
	unsigned int offset;
	offset = STATE.outputFile.tellp();
	offset /= 10;
    IO(2) = offset;
END_TRIGGER;

END_OPERATION_WITH_STATE(STREAM_OUT_STATUS_V8)

