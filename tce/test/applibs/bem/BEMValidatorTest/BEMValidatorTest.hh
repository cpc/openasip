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
 * @file BEMValidatorTest.hh 
 *
 * A test suite for BEMValidator.
 * 
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_VALIDATOR_TEST_HH
#define TTA_BEM_VALIDATOR_TEST_HH

#include <string>
#include <iostream>
#include <TestSuite.h>

#include "BEMValidator.hh"
#include "BEMGenerator.hh"
#include "FileSystem.hh"
#include "ADFSerializer.hh"
#include "BEMSerializer.hh"
#include "Machine.hh"
#include "BinaryEncoding.hh"

using std::string;
using namespace TTAMachine;

const string DATA_FOLDER = "." + FileSystem::DIRECTORY_SEPARATOR + "data" + 
    FileSystem::DIRECTORY_SEPARATOR;
const string ADF_FILE = DATA_FOLDER + "complex.adf";
const string VALID_BEM = DATA_FOLDER + "valid.bem";
const string PORT_CODE_MISSING = DATA_FOLDER + "port_code_missing.bem";
const string SOCKET_ENCODING_MISSING = DATA_FOLDER + 
    "socket_encoding_missing.bem";
const string BRIDGE_ENCODING_MISSING = DATA_FOLDER + 
    "bridge_encoding_missing.bem";
const string IMMEDIATE_ENCODING_MISSING = DATA_FOLDER +
    "immediate_encoding_missing.bem";
const string ITEMP_ENCODING_MISSING = DATA_FOLDER + 
    "itemp_encoding_missing.bem";
const string TOO_NARROW_DST_REG_FIELD = DATA_FOLDER +
    "too_narrow_dst_reg_field.bem";

/**
 * Class that tests BEMGenerator class.
 */
class BEMValidatorTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testValidation();
};


/**
 * Called before each test.
 */
void
BEMValidatorTest::setUp() {
}


/**
 * Called after each test.
 */
void
BEMValidatorTest::tearDown() {
}


/**
 * Tests validation of different BEM files against a complex machine.
 */
void
BEMValidatorTest::testValidation() {

    try {

    ADFSerializer adfSerializer;
    adfSerializer.setSourceFile(ADF_FILE);
    Machine* mach = adfSerializer.readMachine();

    BEMSerializer bemSerializer;

    bemSerializer.setSourceFile(VALID_BEM);
    BinaryEncoding* validBEM = bemSerializer.readBinaryEncoding();
    BEMValidator validator1(*validBEM, *mach);
    TS_ASSERT(validator1.validate());
    delete validBEM;

    bemSerializer.setSourceFile(PORT_CODE_MISSING);
    BinaryEncoding* errBEM1 = bemSerializer.readBinaryEncoding();
    BEMValidator validator2(*errBEM1, *mach);
    TS_ASSERT(!validator2.validate());
    delete errBEM1;

    bemSerializer.setSourceFile(SOCKET_ENCODING_MISSING);
    BinaryEncoding* errBEM2 = bemSerializer.readBinaryEncoding();
    BEMValidator validator3(*errBEM2, *mach);
    TS_ASSERT(!validator3.validate());    
    delete errBEM2;

    bemSerializer.setSourceFile(BRIDGE_ENCODING_MISSING);
    BinaryEncoding* errBEM3 = bemSerializer.readBinaryEncoding();
    BEMValidator validator4(*errBEM3, *mach);
    TS_ASSERT(!validator4.validate());    
    delete errBEM3;

    bemSerializer.setSourceFile(IMMEDIATE_ENCODING_MISSING);
    BinaryEncoding* errBEM4 = bemSerializer.readBinaryEncoding();
    BEMValidator validator5(*errBEM4, *mach);
    TS_ASSERT(!validator5.validate());    
    delete errBEM4;

    bemSerializer.setSourceFile(ITEMP_ENCODING_MISSING);
    BinaryEncoding* errBEM5 = bemSerializer.readBinaryEncoding();
    BEMValidator validator6(*errBEM5, *mach);
    TS_ASSERT(!validator6.validate());    
    delete errBEM5;

    bemSerializer.setSourceFile(TOO_NARROW_DST_REG_FIELD);
    BinaryEncoding* errBEM6 = bemSerializer.readBinaryEncoding();
    BEMValidator validator7(*errBEM6, *mach);
    TS_ASSERT(!validator7.validate());    
    delete errBEM6;

    delete mach;

    } catch (const Exception& e) {
        std::cerr << e.errorMessage() << std::endl;
    }
}

#endif
