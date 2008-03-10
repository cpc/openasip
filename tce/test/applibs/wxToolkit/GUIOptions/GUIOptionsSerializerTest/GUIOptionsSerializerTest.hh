/** 
 * @file GUIOptionsSerializerTest.hh 
 * A test suite for GUIOptionsSerializer.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
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
