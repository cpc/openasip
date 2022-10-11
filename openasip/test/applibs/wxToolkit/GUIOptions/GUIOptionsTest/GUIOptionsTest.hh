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
 * @file GUIOptionsTest.hh 
 * A test suite for GUIOptions.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef GUIOptionsTest_HH
#define GUIOptionsTest_HH

#include <string>

#include <TestSuite.h>
#include "GUIOptions.hh"
#include "GUIOptionsSerializer.hh"

using std::string;

const bool FULL_SCREEN = true;
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 400;
const int X_POS = 50;
const int Y_POS = 50;
const bool TOOLBAR_VISIBILITY = true;
const GUIOptions::ToolbarLayout TOOLBAR_LAYOUT = GUIOptions::BOTH;

class GUIOptionsTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    
    void testLoadState();

private:
    GUIOptions* createOptions() const;
};


/**
 * Called before each test.
 */
void
GUIOptionsTest::setUp() {
}


/**
 * Called after each test.
 */
void
GUIOptionsTest::tearDown() {
}


/**
 * Tests the functionality of loading state from an ObjectState instance.
 */
void
GUIOptionsTest::testLoadState() {
    GUIOptionsSerializer* serializer = new GUIOptionsSerializer("test");
    serializer->setSourceFile("./data/gui.conf");
    GUIOptions* options = serializer->readOptions();
    
    TS_ASSERT(options->fullScreen() == true);
    TS_ASSERT(options->windowWidth() == 400);
    TS_ASSERT(options->windowHeight() == 400);
    TS_ASSERT(options->xPosition() == 50);
    TS_ASSERT(options->yPosition() == 50);
    TS_ASSERT(options->toolbarVisibility() == true);
    TS_ASSERT(options->toolbarLayout() == GUIOptions::BOTH);

    delete serializer;
    delete options;
}
   

/**
 * Creates example options.
 *
 * @return The created options.
 */
GUIOptions*
GUIOptionsTest::createOptions() const {
    GUIOptions* options = new GUIOptions("test");
    options->setFullScreen(FULL_SCREEN);
    options->setWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    options->setWindowPosition(X_POS, Y_POS);
    options->setToolbarVisibility(TOOLBAR_VISIBILITY);
    options->setToolbarLayout(TOOLBAR_LAYOUT);
    return options;
}
    
#endif
