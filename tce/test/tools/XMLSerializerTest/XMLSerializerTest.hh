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
 * @file XMLSerializerTest.hh
 * A test suite for XMLSerializer.
 *
 * @author Lasse Laasonen (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 8 Jun 2004 by tr, jm, am, ll
 * @note rating: red
 */

#ifndef XMLSerializerTest_HH
#define XMLSerializerTest_HH

#include <TestSuite.h>
#include "XMLSerializer.hh"


/**
 * Test suite for XMLSerializer class.
 */
class XMLSerializerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testReadState();
    void testWriteState();
    void testStringReadAndWrite();

private:
    XMLSerializer* serializer_;
};


/**
 * Called before each test.
 */
void
XMLSerializerTest::setUp() {
    serializer_ = new XMLSerializer();
}


/**
 * Called after each test.
 */
void
XMLSerializerTest::tearDown() {
    delete serializer_;
}


/**
 * Tests the functionality of readState function.
 */
void
XMLSerializerTest::testReadState() {

    TS_ASSERT_THROWS(serializer_->readState(), SerializerException);

    serializer_->setSourceFile("./data/prode.conf");
    serializer_->setUseSchema(true);
    TS_ASSERT_THROWS(serializer_->readState(), SerializerException);
    serializer_->setUseSchema(false);
    TS_ASSERT_THROWS_NOTHING(delete serializer_->readState());

    serializer_->setUseSchema(true);
    serializer_->setSchemaFile("./data/confschema.xsd");

    ObjectState* optionsState = serializer_->readState();
    
    TS_ASSERT_THROWS(serializer_->writeState(optionsState), 
                     SerializerException);

    TS_ASSERT(optionsState->name() == "prode-configuration");
    TS_ASSERT(optionsState->hasChild("window"));
    TS_ASSERT(optionsState->hasChild("toolbar"));
    TS_ASSERT(optionsState->hasChild("keyboard-shortcut"));

    ObjectState* toolbarState = optionsState->childByName("toolbar");
    TS_ASSERT(toolbarState->hasChild("slot"));
    TS_ASSERT(toolbarState->hasChild("separator"));
    TS_ASSERT(toolbarState->hasChild("layout"));
    TS_ASSERT(toolbarState->hasChild("visible"));

    delete optionsState;

    serializer_->setUseSchema(false);
    serializer_->setSourceFile("./data/foobar");
    TS_ASSERT_THROWS(serializer_->readState(), SerializerException);
    serializer_->setSourceFile("./data/prode.conf");
    serializer_->setSchemaFile("./data/foobar");
    serializer_->setUseSchema(true);
    TS_ASSERT_THROWS(serializer_->readState(), SerializerException);
    serializer_->setSourceFile("./data/prodeerr.conf");
    serializer_->setSchemaFile("./data/confschema.xsd");
    TS_ASSERT_THROWS(serializer_->readState(), SerializerException);
    serializer_->setSourceFile("./data/prode.conf");
    serializer_->setSchemaFile("./data/confschemaerr.xsd");
    TS_ASSERT_THROWS(serializer_->readState(), SerializerException);
}


/**
 * Tests the functionality of writeState function.
 */
void
XMLSerializerTest::testWriteState() {

    ObjectState* root = new ObjectState("root");
    root->setAttribute("testAttrib", "testValue");
    ObjectState* child1 = new ObjectState("child1");
    root->addChild(child1);
    child1->setAttribute("attrib", "value");
    ObjectState* child2 = new ObjectState("child2");
    root->addChild(child2);
    ObjectState* subchild = new ObjectState("subchild");
    child2->addChild(subchild);
    subchild->setValue("value");

    serializer_->setDestinationFile("./data/written.conf");
    TS_ASSERT_THROWS_NOTHING(serializer_->writeState(root));
    serializer_->setDestinationFile("./foobar/written.conf");
    TS_ASSERT_THROWS(serializer_->writeState(root), SerializerException);
    delete root;
}


/**
 * Tests the functionality of readState function.
 */
void
XMLSerializerTest::testStringReadAndWrite() {

    TS_ASSERT_THROWS(serializer_->readState(), SerializerException);

    std::string source =
        "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
        "<testi>\n"
        "  <test_attribute path=\"/usr/local/bin/mozilla\"/>\n"
        "  <test_element>\n"
        "    <foo>true</foo>\n"
        "    <bar>400</bar>\n"
        "    <bas>\"jeijee\"</bas>\n"
        "  </test_element>\n"
        "</testi>";

    serializer_->setSourceString(source);
    serializer_->setUseSchema(false);

    ObjectState* state = NULL;
    TS_ASSERT_THROWS_NOTHING(state = serializer_->readState());

    TS_ASSERT(state->name() == "testi");
    TS_ASSERT(state->hasChild("test_attribute"));
    TS_ASSERT(state->hasChild("test_element"));

    ObjectState* elementState = state->childByName("test_element");
    TS_ASSERT(elementState->hasChild("foo"));
    TS_ASSERT(elementState->hasChild("bar"));
    TS_ASSERT(elementState->hasChild("bas"));
    TS_ASSERT(!elementState->hasChild("jee"));

    std::string dest1;
    std::string dest2;

    // Write state to dest1.
    serializer_->setDestinationString(dest1);
    serializer_->writeState(state);

    // Read from dest1.
    serializer_->setSourceString(dest1);
    ObjectState* state2 = serializer_->readState();

    // Write to dest2.
    serializer_->setDestinationString(dest2);
    serializer_->writeState(state2);

    TS_ASSERT(dest1 == dest2);

    delete state;
    delete state2;
}
#endif
