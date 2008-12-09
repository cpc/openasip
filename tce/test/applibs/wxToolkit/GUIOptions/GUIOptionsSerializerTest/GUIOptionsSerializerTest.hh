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
 * @file GUIOptionsSerializerTest.hh 
 * A test suite for GUIOptionsSerializer.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef GUIOptionsSerializerTest_HH
#define GUIOptionsSerializerTest_HH

#include <string>

#include <TestSuite.h>

#include "GUIOptionsSerializer.hh"
#include "GUIOptions.hh"
#include "KeyboardShortcut.hh"
#include "ToolbarButton.hh"
#include "Exception.hh"

using std::string;

/**
 * Test suite for GUIOptionsSerializer class.
 */
class GUIOptionsSerializerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testWriteState();
    void testReadState();
private:
};

const string SAVE_DOC_ACTION = "Save Document";
const string OPEN_DOC_ACTION = "Open Document";

/**
 * Called before each test.
 */
void
GUIOptionsSerializerTest::setUp() {
}


/**
 * Called after each test.
 */
void
GUIOptionsSerializerTest::tearDown() {
}


/**
 * Tests the functionality of writeState function.
 */
void
GUIOptionsSerializerTest::testWriteState() {
    GUIOptions options("test");
    KeyboardShortcut* sc1 = new KeyboardShortcut(
        SAVE_DOC_ACTION, 3, true, false, 0);
    KeyboardShortcut* sc2 = new KeyboardShortcut(
        OPEN_DOC_ACTION, 0, true, false, 'e');
    options.addKeyboardShortcut(sc1);
    options.addKeyboardShortcut(sc2);
    ToolbarButton* btn1 = new ToolbarButton(0, OPEN_DOC_ACTION);
    ToolbarButton* btn2 = new ToolbarButton(1, SAVE_DOC_ACTION);
    options.addToolbarButton(btn1);
    options.addToolbarButton(btn2);

    GUIOptionsSerializer serializer("test");
    serializer.setDestinationFile("gui.config");
    TS_ASSERT_THROWS_NOTHING(serializer.writeOptions(options));
}
    

/**
 * Tests the functionality of readState function.
 */
void
GUIOptionsSerializerTest::testReadState() {
    GUIOptionsSerializer serializer("test");
    serializer.setSourceFile("gui.config");
    GUIOptions* options = serializer.readOptions();
    
    ToolbarButton* firstBtn = options->firstToolbarButton();
    ToolbarButton* secondBtn = options->nextToolbarButton();
    TS_ASSERT(firstBtn->action() == OPEN_DOC_ACTION);
    TS_ASSERT(secondBtn->action() == SAVE_DOC_ACTION);

    KeyboardShortcut* sc1 = options->firstShortcut();
    KeyboardShortcut* sc2 = options->nextShortcut();
    TS_ASSERT(sc1->action() == SAVE_DOC_ACTION);
    TS_ASSERT(sc2->action() == OPEN_DOC_ACTION);

    delete options;
}
    
    

#endif
