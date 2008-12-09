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
 * @file ObjectStateTest.hh
 * A test suite for ObjectState class.
 *
 * @author Lasse Laasonen (lasse.laasonen-no.spam-tut.fi)
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
