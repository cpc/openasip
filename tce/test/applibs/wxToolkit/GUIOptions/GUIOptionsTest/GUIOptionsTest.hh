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
