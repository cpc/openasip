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
 * @file AssemblerTest.hh
 *
 * A test suite for assembler.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef ASSEMBLER_TEST_HH
#define ASSEMBLER_TEST_HH

#include <string>
#include <TestSuite.h>

#include "Application.hh"
#include "Binary.hh"
#include "BinaryStream.hh"
#include "Assembler.hh"

#include "Machine.hh"
#include "ADFSerializer.hh"

using namespace TPEF;
using namespace TTAMachine;

class AssemblerTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testHandWrittenAssemblyCode();
    void testConvertedAssemblyCode();
    void testComplexAssemblyCode();
    void testErrorHandling();
    void testDataInitialization();

private:
    static const std::string HAND_WRITTEN_ASM_FILE;
    static const std::string HAND_WRITTEN_MACH_FILE;

    static const std::string CONVERTED_ASM_FILE;
    static const std::string CONVERTED_MACH_FILE;

    static const std::string ERROR_GENERATOR_MACH_FILE;

    static const std::string ERROR_GENERATION_CODES[];
    static const std::string WARNING_GENERATION_CODE;

    static const std::string DATA_INIT_CODE;
};

const std::string
AssemblerTest::HAND_WRITTEN_ASM_FILE = "data/memoryworm.tceasm";
const std::string
AssemblerTest::HAND_WRITTEN_MACH_FILE = "data/memoryworm.adf";

const std::string
AssemblerTest::CONVERTED_ASM_FILE = "data/jarincase.tceasm";
const std::string
AssemblerTest::CONVERTED_MACH_FILE = "data/jarincase.adf";

const std::string
AssemblerTest::ERROR_GENERATOR_MACH_FILE = "data/errortest.adf";

const std::string
AssemblerTest::ERROR_GENERATION_CODES[] =
{
    // syntax error
    "CODE ; \n"
    "ADD.p3.add , ... ;"
    ,

    // unknown FU
    "CODE ; \n"
    "IMNOTREALFU.p3.add -> RF.1, ... ;"
    ,

    // unknown FU port 1
    "CODE ; \n"
    "ADD.unknownport.add -> RF.1, ... ;"
    ,

    // unknown FU port 2
    "CODE ; \n"
    "ADD.unknownport -> RF.1, ... ;"
    ,

    // port reference with no special register port (Not error anymore!)
    //   "CODE ; \n"
    //    "ADD.P3 -> RF.1, ... ;"
    //    ,

    // invalid port opcode
    "CODE ; \n"
    "ADD.P3.imnotoperation -> RF.1, ... ;"
    ,

    // unknown operation
    "CODE ; \n"
    "ADD.imnotoperation.1 -> RF.1, ... ;"
    ,

    // unknown operation index
    "CODE ; \n"
    "ADD.add.4 -> RF.1, ... ;"
    ,

    // invalid operation index
    "CODE ; \n"
    "ADD.add.1 -> RF.1, ... ;"
    ,

    // invalid connections
    "CODE ; \n"
    "... , RF.1 -> ADD.add.1;"
    ,

    // initializing too great value in annotation
    "CODE ; \n"
    "RF.1 -> ADD.add.1{0x1 2:-65536}, ...;"
    ,

    // unknown RF
    "CODE ; \n"
    "NOT_REAL_RF.1 -> ADD.add.1, ...;"
    ,

    // invalid RF index
    "CODE ; \n"
    "RF.10 -> ADD.add.1, ...;"
    ,

    // using RF like FU
    "CODE ; \n"
    "RF.P2.add -> ADD.add.1, ...;"
    ,

    // index not given
    "CODE ; \n"
    "RF.P2 -> ADD.add.1, ...;"
    ,

    // undefined label
    "CODE ; \n"
    "label -> ADD.add.1, ...;"
    ,

    // multiple defined label
    "CODE ; \n"
    "label:\nlabel:\n . . . ;"
    ,

    // code label without valid address
    "CODE ; \n"
    "label -> RF.1, ... ;\n"
    "label:"
    ,

    // try to set unknown label global
    "CODE ; \n"
    ":global label ;"
    ,

    // multiple same procedures
    "CODE ; \n"
    ":procedure me ;\n"
    ":procedure me ;\n"
    ". . . ;"
    ,

    // procedure without valid address
    "CODE ; \n"
    ". . . ;\n"
    ":procedure me ;"
    ,

    // procedure in data section
    "DATA datamemory ;\n"
    ":procedure proced;\n"
    "DA 4;"
    ,

    // code label without valid address
    "CODE ; \n"
    ". . . ;\n"
    "codelabel:"
    ,

    // too long init data field
    "DATA datamemory;\n"
    "DA 4 1:256;\n",

    // too long init data line
    "DATA datamemory;\n"
    "DA 4 2:256 2:16000 1;"
    ,

    // colliding data sections
    "DATA datamemory;\n"
    "DA 4 2:256 2:16000;\n"
    "DATA datamemory 3;\n"
    "DA 4;"
    ,

    // unknown address space
    "DATA invalidaddressspace;\n"
    "DA 4 2:256 2:16000;"
    ,

    // try to write data to code address space
    "DATA instructionmemory;\n"
    "DA 4 2:256 2:16000;"
    ,

    // wrong expression value in data section
    "DATA datamemory;\n"
    "labelwithvalue_0:"
    "DA 4 2:256 2:labelwithvalue_0=1;"
    ,

    // wrong expression value in code section
    "CODE;\n"
    "label:\n"
    "label+1=5 -> RF.1, ...;"
    ,

    // unknown register guard
    "CODE;\n"
    "! RF.3 RF.1 -> ADD.add.1, ...;"
    ,

    // unknown port guard
    "CODE;\n"
    "? ADD.add.3 RF.1 -> ADD.add.1, ...;"
    ,

    // too many move slots
    "CODE ;\n"
    "RF.1 -> RF.2, RF.3 -> RF.4, ... , ..., ..., ...;"
    ,

    // expression pointing outside data sections 1
    "DATA datamemory;\n"
    "labelwithvalue_0:\n"
    "DA 4 2:256 2:labelwithvalue_0+515;"
    ,

    // expression pointing outside data sections 2
    "DATA datamemory;\n"
    "labelwithvalue_0:\n"
    "DA 4 2:256 2:labelwithvalue_0-20;"
    ,

    // expression pointing to unused address of data address space
    "DATA datamemory;\n"
    "labelwithvalue_0:\n"
    "DA 4 2:256 2:labelwithvalue_0+20;"
    ,

    // expression pointing outside code section 1
    "CODE;\n"
    "label:\n"
    "label-2 -> RF.1, ...;\n"
    ,

    // expression pointing outside code section 2
    "CODE;\n"
    "label:\n"
    "label+1025 -> RF.1, ...;\n"
    ,

    // expression pointing to code section to unused address
    "CODE;\n"
    "label:\n"
    "label+100 -> RF.1, ...;\n"
};

const std::string
AssemblerTest::WARNING_GENERATION_CODE =
"CODE ; \n"

// warning: invalid port for reading or writing some RF register
// (only warning if selected port is different) and source is
// bigger than destination
"RF.P4.1 -> ADD.add.1, ...;\n"

// warning of too narrow bus for the move
"..., RF.1 -> RF.2 ;\n"

// warning of too narrow destination for the move
"RF.1 -> RF_8bit.1, ...;\n"

// warnings of too narrow destination and bus for the move
"..., RF.1 -> RF_8bit.1;\n"

// warning multiple writes to destination warning
"..., ..., ADD.add.3 -> RF.3, ADD.add.3 -> RF.3;\n"

// warning same destination one guarded one not
"..., ..., ? RF.0 ADD.add.3 -> RF.1, ADD.add.3 -> RF.1, ...; \n"

// warning same destination two opposite guards and one different guard
"..., ..., "
"? RF.0 ADD.add.3 -> RF.1, "
"! RF.0 ADD.add.3 -> RF.1, "
"? ADD.P3.add ADD.add.3 -> RF.1; \n"

// warning same destination two same guards
"..., ..., "
"? RF.0 ADD.add.3 -> RF.1, "
"? RF.0 ADD.add.3 -> RF.1, ...; \n"

;

const std::string
AssemblerTest::DATA_INIT_CODE =
"DATA datamemory;\n"

// automatic extension from decimal positive numbers
"DA 4  0 3 127 255;\n"
"DA 4  256 65535;\n"
"DA 6  65536 65537;\n"
"DA 3  16777215;\n"
"DA 4  16777216;\n"
"DA 8  16777217 4294967294;\n"
"DA 4  4294967295;\n"

// automatic extension from decimal negative numbers
"DA 4  -0 -3 -127 -128;\n"
"DA 4  -255 -256;\n"
"DA 4  -32767 -32768;\n"
"DA 6  -65535 -65536;\n"
"DA 6  -65537 -8388607;\n"
"DA 3  -8388608;\n"
"DA 8  -8388609 -268435455;\n"
"DA 8 -268435456 -268435457;\n"
"DA 8 -2147483647 -2147483648;\n"

// automatic extension from decimal signed positive numbers
// note that + sign also means that number is explicitely
// interpret as signed so... "+128" needs two 8bit MAUs, but
// "128" needs only one (interpret as unsigned)
"DA 5  +0 +3 +127 +129;\n"
"DA 2  +255;\n"
"DA 2  +256;\n"
"DA 2  +32767;\n"
"DA 3  +32768;\n"
"DA 6  +65535 +65536;\n"
"DA 6  +65537 +8388607;\n"
"DA 4  +8388608;\n"
"DA 8  +8388609 +268435455;\n"
"DA 8 +268435456 +268435457;\n"
"DA 4 +2147483647;\n"

// automatic extension from hexadecimal numbers
"DA 4  0x0 0x03 0x7F 0xFF;\n"
"DA 4  0x100 0xFFFF;\n"
"DA 6  0x010000 0x010001;\n"
"DA 6  0x0FFFFF 0x7FFFFF;\n"
"DA 6  0x8FFFFF 0xFFFFFF;\n"
"DA 8  0x1000000 0x7FFFFFF;\n"
"DA 8  0x8000000 0x8000001;\n"
"DA 8  0xFFFFFFE 0xFFFFFFF;\n"

// automatic extension from binary numbers
"DA 4  0b0 0b011 0b11 0b01111111;\n"

// automatic zero-filling of partial initialisations
"DA 4 20400;\n"
"DA 4 +33 20400;\n"
"DA 4 0x00A8 0x12;\n"
"DA 4 0b0000000010100100 0x12;\n"

// forced extensions and automatic zero-filling of partial initialisation

;


/**
 * Called before each test.
 */
void
AssemblerTest::setUp() {
}


/**
 * Called after each test.
 */
void
AssemblerTest::tearDown() {
}

/**
 * Tests assembler written by hand.
 */
void
AssemblerTest::testHandWrittenAssemblyCode() {

    BinaryStream asmFile(HAND_WRITTEN_ASM_FILE);

    ADFSerializer machineReader;

    machineReader.setSourceFile(HAND_WRITTEN_MACH_FILE);

    Machine* mach = machineReader.readMachine();

    Assembler assembler(asmFile, *mach);

    Binary* tpef = assembler.compile();

    delete tpef;
    delete mach;
}

/**
 */
void
AssemblerTest::testConvertedAssemblyCode() {
    BinaryStream asmFile(CONVERTED_ASM_FILE);

    ADFSerializer machineReader;

    machineReader.setSourceFile(CONVERTED_MACH_FILE);

    Machine* mach = machineReader.readMachine();

    Assembler assembler(asmFile, *mach);

    Binary* tpef = assembler.compile();

    delete tpef;
    delete mach;
}

/**
 * Tests with all features of assembler are used
 */
void
AssemblerTest::testComplexAssemblyCode() {
}


/**
 * Test various errors
 */
void
AssemblerTest::testErrorHandling() {

    ADFSerializer machineReader;
    machineReader.setSourceFile(ERROR_GENERATOR_MACH_FILE);
    Machine* mach = machineReader.readMachine();

    for (const std::string& asmCode : ERROR_GENERATION_CODES) {
        BinaryStream stream("data/temp.tceasm");

        for (unsigned int j = 0; j < asmCode.length(); j++) {
            stream.writeByte(asmCode.at(j));
        }

        Assembler assembler(stream, *mach);

        // uncomment this to see which case failed
//         try {
//             assembler.compile();
//         } catch (CompileError& e) {
//             std::cerr << e.errorMessage()
//                       << std::endl << std::endl;
//         }

        TS_ASSERT_THROWS(assembler.compile(), CompileError);
    }

    BinaryStream stream("data/temp.tceasm");
    const std::string& asmCode = WARNING_GENERATION_CODE;

    for (unsigned int j = 0; j < asmCode.length(); j++) {
        stream.writeByte(asmCode.at(j));
    }

    Assembler assembler(stream, *mach);

    Binary* newBin = assembler.compile();
    delete newBin;

    TS_ASSERT_EQUALS(static_cast<int>(assembler.warningCount()), 10);

    delete mach;
}


/**
 * Test various types of initialisation in DA directives. Test extension,
 * sign treatment, filling.
 */
void
AssemblerTest::testDataInitialization() {

    ADFSerializer machineReader;
    machineReader.setSourceFile(ERROR_GENERATOR_MACH_FILE);
    Machine* mach = machineReader.readMachine();

    const std::string INPUT_FILENAME("data/temp.tceasm");

    const std::string& asmCode = DATA_INIT_CODE;
    BinaryStream stream(INPUT_FILENAME);

    for (unsigned int j = 0; j < asmCode.length(); j++) {
        stream.writeByte(asmCode.at(j));
    }

    Assembler assembler(stream, *mach);
    try {
        assembler.compile();
    } catch (CompileError& e) {
        std::cerr << e.errorMessage()
                  << std::endl << std::endl;
    }
//    TS_ASSERT_THROWS_NOTHING(delete assembler.compile());

    delete mach;
}

#endif
