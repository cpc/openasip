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
 * @file SchemaTest.hh 
 *
 * A test suite for MDF Schema file. 
 *
 * @author Atte Oksman 2004 (oksman-no.spam-cs.tut.fi)
 * @note rating: red
 */    

#ifndef TTA_SCHEMA_TEST_HH
#define TTA_SCHEMA_TEST_HH

#include <string>
#include <iostream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <TestSuite.h>

#include "DOMBuilderErrorHandler.hh"
#include "Conversion.hh"
#include "FileSystem.hh"
#include "Environment.hh"

using std::string;
using std::cout;


const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
const string ADF_SCHEMA_FILE = "mach/ADF_Schema.xsd";

/**
 * A class that tests the MDF Schema file.
 */
class SchemaTest : public CxxTest::TestSuite {

public:
    SchemaTest();

    void setUp();
    void tearDown();

    void testSimpleFile();
    void testComplexFile();

    void testDuplicateBusName();
    void testDuplicateSocketName();
    void testDuplicateBridgeName();
    void testDuplicateFunctionUnitName();
    void testDuplicateRegisterFileName();
    void testDuplicateAddressSpaceName();
    void testDuplicateGlobalControlUnitName();
    void testDuplicateImmediateUnitName();
    void testSameBusAndImmediateSlotName();

    void testInvalidRegisterFileReferenceInGuard();
    void testInvalidFunctionUnitReferenceInGuard();
    void testInvalidBusReferenceInSocket();
    void testInvalidBusReferenceInBridge();
    void testInvalidBusReferenceInImmediateUnit();

    void testSetsOpcodeElement();

private:
    /// The XML parser.
    DOMBuilder* parser_;
    /// Error handler for the parser.
    DOMBuilderErrorHandler* errHandler_;
    /// Implementation of the DOM.
    DOMImplementation* domImplementation_;
    /// The (root of the) DOM tree that is the result of the parsing of the
    /// XML file.
    DOMDocument* dom_;
    /// The Schema file path.
    XMLCh* schema_;
};


/**
 * Constructor.
 */
inline
SchemaTest::SchemaTest() :
    parser_(NULL), errHandler_(NULL), domImplementation_(NULL), dom_(NULL),
    schema_(NULL) {
}


/**
 * Sets up the test for each individual test.
 */
inline void
SchemaTest::setUp() {

    TS_ASSERT_THROWS_NOTHING(XMLPlatformUtils::Initialize());

    const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    domImplementation_ =
        DOMImplementationRegistry::getDOMImplementation(gLS);

    parser_ = domImplementation_->createDOMBuilder(
        DOMImplementationLS::MODE_SYNCHRONOUS, 0);

    parser_->setFeature(XMLUni::fgXercesSchema, true);
    parser_->setFeature(XMLUni::fgDOMValidation, true);
    parser_->setFeature(XMLUni::fgDOMNamespaces, true);
    parser_->setFeature(XMLUni::fgXercesSchemaFullChecking, true);

    string fullPath = Environment::schemaDirPath(ADF_SCHEMA_FILE);
    schema_ = Conversion::toXMLCh(fullPath);
    parser_->setProperty(
        XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, schema_);

    errHandler_ = new DOMBuilderErrorHandler();
    parser_->setErrorHandler(errHandler_);

    parser_->setFeature(XMLUni::fgDOMWhitespaceInElementContent, false);
    parser_->setFeature(XMLUni::fgDOMComments, false);
}


/**
 * Tears down the test for each individual test.
 */
inline void
SchemaTest::tearDown() {

    XMLString::release(&schema_);

    delete parser_;
    parser_ = NULL;

    delete errHandler_;
    errHandler_ = NULL;

    domImplementation_ = NULL;
    dom_ = NULL;

    XMLPlatformUtils::Terminate();
}


/**
 * Tests the Schema file with a simple, yet correct example of MDF (XML file).
 */
inline void
SchemaTest::testSimpleFile() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + "simple.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 0);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with a correct and a bit more complex example of MDF.
 */
inline void
SchemaTest::testComplexFile() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + "complex.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 0);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with a duplicate bus name.
 */
inline void
SchemaTest::testDuplicateBusName() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "duplicateBusName.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 3);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with a duplicate socket name.
 */
inline void
SchemaTest::testDuplicateSocketName() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "duplicateSocketName.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 2);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with a duplicate bridge name.
 */
inline void
SchemaTest::testDuplicateBridgeName() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "duplicateBridgeName.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 1);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with a duplicate function unit name.
 */
inline void
SchemaTest::testDuplicateFunctionUnitName() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "duplicateFunctionUnitName.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 2);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with a duplicate register file name.
 */
inline void
SchemaTest::testDuplicateRegisterFileName() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "duplicateRegisterFileName.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 2);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with a duplicate address space name.
 */
inline void
SchemaTest::testDuplicateAddressSpaceName() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "duplicateAddressSpaceName.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 2);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with a duplicate global control unit name.
 */
inline void
SchemaTest::testDuplicateGlobalControlUnitName() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "duplicateGlobalControlUnitName.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 1);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with a duplicate immediate unit name.
 */
inline void
SchemaTest::testDuplicateImmediateUnitName() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "duplicateImmediateUnitName.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 1);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with same bus and immediate slot name.
 */
inline void
SchemaTest::testSameBusAndImmediateSlotName() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP +
        "sameBusAndImmediateSlotName.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 2);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with an invalid reference to a register file from a
 * bus/guard/.../bool element.
 */
inline void
SchemaTest::testInvalidRegisterFileReferenceInGuard() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "invalidRegisterFileReferenceInGuard.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 3);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with an invalid reference to a function unit from a
 * bus/guard/.../unit element.
 */
inline void
SchemaTest::testInvalidFunctionUnitReferenceInGuard() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "invalidFunctionUnitReferenceInGuard.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 3);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with an invalid reference to a bus from a socket.
 */
inline void
SchemaTest::testInvalidBusReferenceInSocket() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "invalidBusReferenceInSocket.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 1);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with an invalid reference to a bus from a bridge.
 */
inline void
SchemaTest::testInvalidBusReferenceInBridge() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "invalidBusReferenceInBridge.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 4);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file with an invalid reference to a bus from the
 * immediate unit.
 */
inline void
SchemaTest::testInvalidBusReferenceInImmediateUnit() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "invalidBusReferenceInImmediateUnit.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 1);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


/**
 * Tests the Schema file's ability to validate sets-opcode element in
 * function unit.
 */
inline void
SchemaTest::testSetsOpcodeElement() {

    const string xmlFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "setsOpcodeElement.xml";
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile.c_str()));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 1);

    // uncomment the line below to see more information on found errors
    // handy when modifying the test after changes to schema
//    cout << errHandler_->errorLog();
}


#endif
