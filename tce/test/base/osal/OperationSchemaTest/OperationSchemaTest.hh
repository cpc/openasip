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
 * @file OperationSchemaTest.hh
 *
 * A test suite for Opearation Schema.
 *
 * @author Atte Oksman 2004 (atte.oksman@tut.fi)
 * @note: rating: red
 */

#ifndef TTA_OPERATION_SCHEMA_TEST_HH
#define TTA_OPERATION_SCHEMA_TEST_HH

#include <string>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <TestSuite.h>

#include "DOMBuilderErrorHandler.hh"
#include "Conversion.hh"
#include "Application.hh"
#include "Environment.hh"
#include "FileSystem.hh"

using std::string;


/**
 * A class that tests the Operation Schema file.
 */
class OperationSchemaTest : public CxxTest::TestSuite {

public:
    OperationSchemaTest();

    void setUp();
    void tearDown();

    void testSimpleFile();
    void testComplexFile();

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
OperationSchemaTest::OperationSchemaTest() : parser_(NULL), errHandler_(NULL),
                                             domImplementation_(NULL),
                                             dom_(NULL), schema_(NULL) {
}


/**
 * Sets up the test for each individual test.
 */
inline void
OperationSchemaTest::setUp() {

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

    string fullPath = FileSystem::currentWorkingDir() +
        FileSystem::DIRECTORY_SEPARATOR + "data/Operation_Schema.xsd";
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
OperationSchemaTest::tearDown() {

    // the XMLCh* is deleted and set to NULL
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
 * Tests the Schema with a correct, but simple XML file.
 */
inline void
OperationSchemaTest::testSimpleFile() {

    string xmlFileString = "./data/simple.opp"; 
    const char* xmlFile = xmlFileString.c_str();
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 0);
}


/**
 * Tests the Schema with a correct and a bit more complex XML file.
 */
inline void
OperationSchemaTest::testComplexFile() {

    string xmlFileString = "./data/complex.opp"; 
    const char* xmlFile = xmlFileString.c_str();
    TS_ASSERT_THROWS_NOTHING(dom_ = parser_->parseURI(xmlFile));

    int errCount = errHandler_->errorCount();
    TS_ASSERT_EQUALS(errCount, 0);
}


#endif
