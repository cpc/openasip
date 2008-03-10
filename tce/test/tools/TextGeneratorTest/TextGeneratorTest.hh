/** 
 * @file TextGeneratorTest.hh 
 *
 * A test suite for TextGenerator.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

#ifndef TTA_TEXT_GENERATOR_TEST_HH
#define TTA_TEXT_GENERATOR_TEST_HH

#include <string>
#include <TestSuite.h>
#include <boost/format.hpp>
#include "TextGenerator.hh"
#include "Conversion.hh"

using std::string;
using Texts::TextGenerator;
using boost::format;

class TextGeneratorTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testText();
    void testAddText();
    
private:
    static const string helloText_;
    static const int Id_;
    static const string testText_;
};

const string TextGeneratorTest::helloText_ = "Hello all world!";
const int TextGeneratorTest::Id_ = Texts::LAST__;
const string TextGeneratorTest::testText_ = "%s is %d years old";

/**
 * Called before each test.
 */
void
TextGeneratorTest::setUp() {
}


/**
 * Called after each test.
 */
void
TextGeneratorTest::tearDown() {
}

/**
 * Tests that text method works correctly.
 */
void
TextGeneratorTest::testText() {

    TextGenerator gen;

    try {
        format fmt = gen.text(Texts::TXT_HELLO_WORLD);
//        TS_TRACE(fmt.str());
        fmt % "all";
        TS_ASSERT_EQUALS(fmt.str(), helloText_);
    } catch (const KeyNotFound& k) {
        TS_FAIL("Unexpected exception thrown.");
    }
}

/**
 * Tests that addText method works correctly.
 */
void TextGeneratorTest::testAddText() {

    TextGenerator gen;
    gen.addText(Id_, testText_);
    
    try {
    
        format fmt = gen.text(Id_);
        int age = 26;
        fmt % "Jussi" % age;
        TS_ASSERT_EQUALS(fmt.str(), "Jussi is 26 years old");
        TS_ASSERT_THROWS(gen.text(666), KeyNotFound);
    } catch (const KeyNotFound& k) {
        TS_FAIL("Unexpected exception thrown");
    }
}

#endif
