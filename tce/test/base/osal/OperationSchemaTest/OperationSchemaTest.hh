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
 * @file OperationSchemaTest.hh
 *
 * A test suite for Opearation Schema.
 *
 * @author Atte Oksman 2004 (atte.oksman-no.spam-tut.fi)
 * @note: rating: red
 */

#ifndef TTA_OPERATION_SCHEMA_TEST_HH
#define TTA_OPERATION_SCHEMA_TEST_HH

#include <string>

#include <xercesc/util/XercesVersion.hpp>

#if XERCES_VERSION_MAJOR >= 3
#include <xercesc/dom/DOMLSParser.hpp>
#else
#include <xercesc/dom/DOMBuilder.hpp>
#endif

#include <xercesc/dom/DOM.hpp>
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
#if XERCES_VERSION_MAJOR >= 3
    DOMLSParser* parser_;
#else
    DOMBuilder* parser_;
#endif
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

    string fullPath = FileSystem::currentWorkingDir() +
        FileSystem::DIRECTORY_SEPARATOR + "data/Operation_Schema.xsd";
    schema_ = Conversion::toXMLCh(fullPath);
    errHandler_ = new DOMBuilderErrorHandler();

#if XERCES_VERSION_MAJOR >= 3
    parser_ = domImplementation_->createLSParser(
        DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    parser_->getDomConfig()->setParameter(XMLUni::fgXercesSchema, true);
    parser_->getDomConfig()->setParameter(XMLUni::fgDOMValidate, true);
    parser_->getDomConfig()->setParameter(XMLUni::fgDOMNamespaces, true);
    parser_->getDomConfig()->setParameter(
        XMLUni::fgXercesSchemaFullChecking, true);
    parser_->getDomConfig()->setParameter(
        XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, schema_);
    parser_->getDomConfig()->setParameter(
        XMLUni::fgDOMErrorHandler, errHandler_);
    parser_->getDomConfig()->setParameter(
        XMLUni::fgDOMElementContentWhitespace, false);
    parser_->getDomConfig()->setParameter(XMLUni::fgDOMComments, false);
#else
    parser_ = domImplementation_->createDOMBuilder(
        DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    parser_->setFeature(XMLUni::fgXercesSchema, true);
    parser_->setFeature(XMLUni::fgDOMValidation, true);
    parser_->setFeature(XMLUni::fgDOMNamespaces, true);
    parser_->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
    parser_->setProperty(
        XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, schema_);
    parser_->setErrorHandler(errHandler_);
    parser_->setFeature(XMLUni::fgDOMWhitespaceInElementContent, false);
    parser_->setFeature(XMLUni::fgDOMComments, false);
#endif

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
