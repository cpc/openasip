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
 * @file ObjectStateTest.hh
 * A test suite for ObjectState class.
 *
 * @author Lasse Laasonen (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef ObjectStateTest_HH
#define ObjectStateTest_HH

#include <string>

#include <TestSuite.h>

#include "Exception.hh"
#include "ObjectState.hh"

using std::string;

/**
 * Test suite for ObjectState class.
 */
class ObjectStateTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testSetName();
    void testSetValue();
    void testSetAttribute();
    void testChildren();
    void testCopying();
    void testInequalityOperator();
};


/**
 * Called before each test.
 */
void
ObjectStateTest::setUp() {
}


/**
 * Called after each test.
 */
void
ObjectStateTest::tearDown() {
}


/**
 * Tests setting the name of the ObjectState instance.
 */
void
ObjectStateTest::testSetName() {

    const string NAME1 = "name1";
    const string NAME2 = "name2";

    ObjectState state(NAME1);
    TS_ASSERT(state.name() == NAME1);
    state.setName(NAME2);
    TS_ASSERT(state.name() == NAME2);
}


/**
 * Tests setting the value of the ObjectState instance.
 */
void
ObjectStateTest::testSetValue() {

    const string STRING_VALUE = "test";
    const int INT_VALUE = 32;
    const double DOUBLE_VALUE = -4.2;
    
    ObjectState state("");
    TS_ASSERT(state.stringValue() == "");
    state.setValue(STRING_VALUE);
    TS_ASSERT(state.stringValue() == STRING_VALUE);
    TS_ASSERT_THROWS(state.intValue(), NumberFormatException);
    TS_ASSERT_THROWS(state.doubleValue(), NumberFormatException);
    TS_ASSERT_THROWS(state.boolValue(), TypeMismatch);

    state.setValue(INT_VALUE);
    TS_ASSERT(state.intValue() == INT_VALUE);
    TS_ASSERT(state.doubleValue() == INT_VALUE);
    TS_ASSERT(state.boolValue() == true);
    
    state.setValue(DOUBLE_VALUE);
    TS_ASSERT(state.doubleValue() == DOUBLE_VALUE);
    TS_ASSERT_THROWS(state.intValue(), NumberFormatException);
    TS_ASSERT_THROWS(state.boolValue(), TypeMismatch);

    state.setValue(true);
    TS_ASSERT(state.boolValue() == true);
    state.setValue(false);
    TS_ASSERT(state.boolValue() == false);
}


/**
 * Tests setting attributes.
 */
void
ObjectStateTest::testSetAttribute() {
    
    const string ATTR1 = "attr1";
    const string STRING_VALUE = "string";
    const int INT_VALUE = -34;
    const double DOUBLE_VALUE = 55.23;
    const unsigned int UINT_VALUE = 100;

    ObjectState state("");
    TS_ASSERT(state.attributeCount() == 0);
    state.setAttribute(ATTR1, STRING_VALUE);
    TS_ASSERT(state.attributeCount() == 1);
    TS_ASSERT(state.stringAttribute(ATTR1) == STRING_VALUE);
    TS_ASSERT_THROWS(state.stringAttribute("foo"), KeyNotFound);
    TS_ASSERT_THROWS(state.intAttribute("foo"), KeyNotFound);
    TS_ASSERT_THROWS(state.doubleAttribute("foo"), KeyNotFound);
    TS_ASSERT_THROWS(state.boolAttribute("foo"), KeyNotFound);
    TS_ASSERT_THROWS(state.unsignedIntAttribute("foo"), KeyNotFound);
    TS_ASSERT_THROWS(state.intAttribute(ATTR1), NumberFormatException);
    TS_ASSERT_THROWS(state.doubleAttribute(ATTR1), NumberFormatException);
    TS_ASSERT_THROWS(state.boolAttribute(ATTR1), TypeMismatch);
    TS_ASSERT_THROWS(
        state.unsignedIntAttribute(ATTR1), NumberFormatException);

    state.setAttribute(ATTR1, INT_VALUE);
    TS_ASSERT(state.attributeCount() == 1);
    TS_ASSERT(state.intAttribute(ATTR1) == INT_VALUE);
    TS_ASSERT(state.doubleAttribute(ATTR1) == INT_VALUE);
    TS_ASSERT(state.boolAttribute(ATTR1) == true);
    TS_ASSERT_THROWS(
        state.unsignedIntAttribute(ATTR1), NumberFormatException);
    
    state.setAttribute(ATTR1, DOUBLE_VALUE);
    TS_ASSERT(state.attributeCount() == 1);
    TS_ASSERT(state.doubleAttribute(ATTR1) == DOUBLE_VALUE);
    TS_ASSERT_THROWS(state.intAttribute(ATTR1), NumberFormatException);
    TS_ASSERT_THROWS(state.boolAttribute(ATTR1), TypeMismatch);
    TS_ASSERT_THROWS(
        state.unsignedIntAttribute(ATTR1), NumberFormatException);

    state.setAttribute(ATTR1, true);
    TS_ASSERT(state.attributeCount() == 1);
    TS_ASSERT(state.boolAttribute(ATTR1) == true);
    state.setAttribute(ATTR1, false);
    TS_ASSERT(state.boolAttribute(ATTR1) == false);

    state.setAttribute(ATTR1, UINT_VALUE);
    TS_ASSERT(state.attributeCount() == 1);
    TS_ASSERT(state.intAttribute(ATTR1) == static_cast<int>(UINT_VALUE));
    TS_ASSERT(state.doubleAttribute(ATTR1) == UINT_VALUE);
    TS_ASSERT(state.boolAttribute(ATTR1) == true);
    TS_ASSERT(state.unsignedIntAttribute(ATTR1) == UINT_VALUE);
}


/**
 * Tests adding, removing, and replacing children.
 */
void
ObjectStateTest::testChildren() {

    const string CHILD1 = "child1";
    const string CHILD2 = "child2";
    const string NEW_CHILD = "new_child";

    ObjectState* state = new ObjectState("");
    ObjectState* child1 = new ObjectState(CHILD1);

    TS_ASSERT(state->childCount() == 0);
    state->addChild(child1);
    TS_ASSERT(state->childCount() == 1);
    TS_ASSERT(state->hasChild(CHILD1));
    
    ObjectState* child2 = new ObjectState(CHILD2);
    state->addChild(child2);
    TS_ASSERT(state->childCount() == 2);
    TS_ASSERT(state->hasChild(CHILD2));
    TS_ASSERT(state->childByName(CHILD1) == child1);
    TS_ASSERT(state->childByName(CHILD2) == child2);
    TS_ASSERT_THROWS(state->childByName("foo"), InstanceNotFound);
    TS_ASSERT(state->child(0) == child1 || state->child(1) == child1);
    TS_ASSERT(state->child(0) == child2 || state->child(1) == child2);
    TS_ASSERT_THROWS(state->child(-1), OutOfRange);
    TS_ASSERT_THROWS(state->child(2), OutOfRange);

    ObjectState* newChild = new ObjectState(NEW_CHILD);
    state->replaceChild(child1, newChild);
    TS_ASSERT(state->childCount() == 2);
    TS_ASSERT(newChild->parent() == state);
    TS_ASSERT(state->hasChild(NEW_CHILD));
    TS_ASSERT(!state->hasChild(CHILD1));

    TS_ASSERT_THROWS(state->replaceChild(child2, newChild), InvalidData);

    delete newChild;
    TS_ASSERT(!state->hasChild(NEW_CHILD));
    TS_ASSERT(state->childCount() == 1);
    
    delete state;
}


/**
 * Tests the copy constructor.
 */
void
ObjectStateTest::testCopying() {

    const string level1 = "level1";
    const string level2_1 = "level2_1";
    const string level2_2 = "level2_2";
    const string level3 = "level3";

    const string param = "param";
    const string value = "value";

    ObjectState* l1State = new ObjectState(level1);
    ObjectState* l2State1 = new ObjectState(level2_1);
    l2State1->setAttribute(param, value);
    l1State->addChild(l2State1);
    ObjectState* l2State2 = new ObjectState(level2_2);
    l1State->addChild(l2State2);
    ObjectState* l3State = new ObjectState(level3);
    l3State->setAttribute(param, value);
    l2State2->addChild(l3State);

    ObjectState* copied = new ObjectState(*l1State);

    TS_ASSERT(copied->hasChild(level2_1));
    TS_ASSERT(copied->hasChild(level2_2));
    
    TS_ASSERT(
        copied->childByName(level2_1)->stringAttribute(param) == value);
    TS_ASSERT(copied->childByName(level2_2)->hasChild(level3));
    TS_ASSERT(
        copied->childByName(level2_2)->child(0)->stringAttribute(param) == 
        value);

    delete l1State;
    delete copied;
}


/**
 * Tests the inequality operator.
 */
void
ObjectStateTest::testInequalityOperator() {

    const string level1 = "level1";
    const string level2_1 = "level2_1";
    const string level2_2 = "level2_2";
    const string level3 = "level3";

    const string param = "param";
    const string value = "value";

    ObjectState* l1State = new ObjectState(level1);
    ObjectState* l2State1 = new ObjectState(level2_1);
    l2State1->setAttribute(param, value);
    l1State->addChild(l2State1);
    ObjectState* l2State2 = new ObjectState(level2_2);
    l1State->addChild(l2State2);
    ObjectState* l3State = new ObjectState(level3);
    l3State->setAttribute(param, value);
    l2State2->addChild(l3State);

    ObjectState* copied = new ObjectState(*l1State);
    TS_ASSERT(!(*copied != *l1State));
    TS_ASSERT(!(*l1State != *copied));

    copied->setName("foo");
    TS_ASSERT(*copied != *l1State);
    TS_ASSERT(*l1State != *copied);

    copied->setName(level1);
    TS_ASSERT(!(*copied != *l1State));
    TS_ASSERT(!(*l1State != *copied));
    
    copied->setAttribute(param, value);
    TS_ASSERT(*copied != *l1State);
    TS_ASSERT(*l1State != *copied);

    l1State->setAttribute(param, param);
    TS_ASSERT(*copied != *l1State);
    TS_ASSERT(*l1State != *copied);

    l1State->setAttribute(param, value);
    TS_ASSERT(!(*copied != *l1State));
    TS_ASSERT(!(*l1State != *copied));
    
    ObjectState* newChild = new ObjectState("newChild");
    copied->addChild(newChild);
    TS_ASSERT(*copied != *l1State);
    TS_ASSERT(*l1State != *copied);

    delete l1State;
    delete copied;
}

#endif
