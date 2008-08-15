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
 * @file GUIOptionsTest.hh 
 * A test suite for GUIOptions.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
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
