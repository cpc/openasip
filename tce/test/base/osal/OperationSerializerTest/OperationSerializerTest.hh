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
 * @file OperationSerializerTest.hh
 * 
 * A test suite for OperationSerializer.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2008 (mikael.lepisto-no.spam-tut.fi)
 */

#ifndef OPERATION_SERIALIZER_TEST_HH
#define OPERATION_SERIALIZER_TEST_HH

#include <TestSuite.h>
#include <string>
#include <iostream>

#include "OperationSerializer.hh"
#include "Operation.hh"
#include "ObjectState.hh"
#include "OperationBehavior.hh"
#include "FileSystem.hh"
#include "StringTools.hh"
#include "TCEString.hh"
#include "Operand.hh"

using std::string;
using std::cout;
using std::endl;

class OperationSerializerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    
    void testReadAndWriteState();

private:
    /// The name of the XML file.
    static const string XMLFILE;
    /// The name of the destination file.
    static const string DESTFILE;
};

const string OperationSerializerTest::XMLFILE = "data" +
FileSystem::DIRECTORY_SEPARATOR + "correct.opp";
const string OperationSerializerTest::DESTFILE = "data" +
FileSystem::DIRECTORY_SEPARATOR + "written.opp";

/**
 * Called before each test.
 */
void
OperationSerializerTest::setUp() {
}


/**
 * Called after each test.
 */
void
OperationSerializerTest::tearDown() {
}

/**
 * Tests that readState() amd writeState() works.
 */
void
OperationSerializerTest::testReadAndWriteState() {
    try {
        OperationSerializer serializer;
        serializer.setSourceFile(XMLFILE);
        ObjectState* root = NULL;
        
        // TS_ASSERT_THROWS_NOTHING(root = serializer.readState());
        try {
            root = serializer.readState();
        } catch (Exception &e) {
            std::cerr << std::endl << e.errorMessageStack() << std::endl;
        }
        
        ObjectState* child1 = root->child(0);
        Operation operation1("foo", NullOperationBehavior::instance());
        
        TS_ASSERT_THROWS_NOTHING(operation1.loadState(child1));
        TS_ASSERT_EQUALS(operation1.name(), "OPER1");
        TS_ASSERT_EQUALS(operation1.numberOfInputs(), 2);
        TS_ASSERT_EQUALS(operation1.numberOfOutputs(), 1);
        TS_ASSERT_EQUALS(operation1.readsMemory(), true);
    
        ObjectState* child2 = root->child(1);
        Operation operation2("daa", NullOperationBehavior::instance());
        TS_ASSERT_THROWS_NOTHING(operation2.loadState(child2));
        TS_ASSERT_EQUALS(operation2.name(), "OPER2");
        TS_ASSERT_EQUALS(operation2.numberOfInputs(), 4);
        TS_ASSERT_EQUALS(operation2.numberOfOutputs(), 1);
        TS_ASSERT_EQUALS(operation2.readsMemory(), true);
        TS_ASSERT_EQUALS(operation2.writesMemory(), true);
        TS_ASSERT_EQUALS(operation2.canTrap(), true);
        TS_ASSERT_EQUALS(operation2.hasSideEffects(), true);
        TS_ASSERT_EQUALS(operation2.isClocked(), true);
        TS_ASSERT_EQUALS(operation2.dependsOn(operation1), true);

        Operand& op1 = operation2.operand(1);
        TS_ASSERT_DIFFERS(&op1, &NullOperand::instance()); 
        Operand& op2 = operation2.operand(2);
        TS_ASSERT_DIFFERS(&op2, &NullOperand::instance()); 
        Operand& op3 = operation2.operand(3);
        TS_ASSERT_DIFFERS(&op3, &NullOperand::instance()); 
        Operand& op4 = operation2.operand(4);
        TS_ASSERT_DIFFERS(&op4, &NullOperand::instance()); 
        Operand& op5 = operation2.operand(5);
        TS_ASSERT_DIFFERS(&op5, &NullOperand::instance()); 

        TS_ASSERT_EQUALS(op2.canSwap(op4), true);
    

        delete root;
        root = NULL;
    
        serializer.setDestinationFile(DESTFILE);
    
        root = new ObjectState("");
        root->addChild(operation1.saveState());
        root->addChild(operation2.saveState());

        TS_ASSERT_THROWS_NOTHING(serializer.writeState(root));

        delete root;
        root = NULL;

    } catch (Exception &e) {
        std::cerr << e.errorMessageStack() << std::endl;
    }
}

#endif
