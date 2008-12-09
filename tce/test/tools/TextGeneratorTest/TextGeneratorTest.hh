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
 * @file TextGeneratorTest.hh 
 *
 * A test suite for TextGenerator.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
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
    void testReplace();
    
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

/**
 * Tests that replace works correctly.
 */
void TextGeneratorTest::testReplace() {
    TextGenerator gen;
    gen.addText(Id_, testText_);
    
    try {
        gen.replaceText(Id_, "Is that so?");
        format fmt = gen.text(Id_);
        TS_ASSERT_EQUALS(fmt.str(), "Is that so?");
        TS_ASSERT_THROWS(gen.replaceText(666, "darn"), KeyNotFound);
    } catch (...) {
        TS_FAIL("Unexpected exception thrown");
    }
}

#endif
