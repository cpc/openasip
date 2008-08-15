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
