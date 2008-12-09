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
 * @file FunctionUnitTest.hh 
 *
 * A test suite for FunctionUnit class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef FunctionUnitTest_HH
#define FunctionUnitTest_HH

#include <string>

#include <TestSuite.h>
#include "ADFSerializer.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "PipelineElement.hh"
#include "ExecutionPipeline.hh"
#include "FileSystem.hh"

using std::string;
using namespace TTAMachine;

/**
 * Tests the functionality of FunctionUnit class.
 */
class FunctionUnitTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testSetName();
    void testMaxLatency();
    void testPortGetters();
    void testOperationGetters();
    void testDeleteOperation();
    void testAddPipelineElement();
    void testPipelineElementGetters();
    void testObjectStateLoadingErrors();
};


/**
 * Called before each test.
 */
void
FunctionUnitTest::setUp() {
}


/**
 * Called after each test.
 */
void
FunctionUnitTest::tearDown() {
}


/**
 * Tests setting the name of the function unit.
 */
void
FunctionUnitTest::testSetName() {

    const string fu1Name = "fu1";
    const string fu2Name = "fu2";
    const string fu3Name = "fu3";

    Machine machine;
    FunctionUnit* fu1 = new FunctionUnit(fu1Name);
    TS_ASSERT(fu1->name() == fu1Name);
    fu1->setName(fu2Name);
    TS_ASSERT(fu1->name() == fu2Name);
    fu1->setName(fu1Name);

    machine.addFunctionUnit(*fu1);
    FunctionUnit* fu2 = new FunctionUnit(fu2Name);
    machine.addFunctionUnit(*fu2);
    
    TS_ASSERT_THROWS_NOTHING(fu2->setName(fu3Name));
    TS_ASSERT_THROWS(fu2->setName(fu1Name), ComponentAlreadyExists);
    TS_ASSERT_THROWS_NOTHING(fu2->setName(fu3Name));
}


/**
 * Tests the functionality of maxLatency method.
 */
void
FunctionUnitTest::testMaxLatency() {
    
    FunctionUnit* fu = new FunctionUnit("fu");
    HWOperation* op1 = new HWOperation("op1", *fu);
    HWOperation* op2 = new HWOperation("op2", *fu);

    TS_ASSERT(fu->maxLatency() == 0);
    
    op1->pipeline()->addResourceUse("res1", 0, 5);
    op2->pipeline()->addResourceUse("res1", 1, 6);
    
    TS_ASSERT(fu->maxLatency() == 7);
    delete fu;
}


/**
 * Tests the port getter functions of function unit.
 */
void
FunctionUnitTest::testPortGetters() {

    const string fu1Name = "fu1";
    const string port1Name = "port1";

    FunctionUnit fu(fu1Name);
    TS_ASSERT_THROWS(fu.port(0), OutOfRange);
    FUPort* port1 = new FUPort(port1Name, 32, fu, false, false);
    TS_ASSERT(fu.port(0) == port1);
    TS_ASSERT_THROWS(fu.port(-1), OutOfRange);
    TS_ASSERT_THROWS(fu.port(1), OutOfRange);
    TS_ASSERT(fu.port(port1Name) == port1);
    TS_ASSERT_THROWS(fu.port("nonExisting"), InstanceNotFound);
}


/**
 * Tests the operation getters of function unit.
 */
void
FunctionUnitTest::testOperationGetters() {

    const string fu1Name = "fu1";
    const string op1Name = "op1";
    const string op1NAME = "OP1";

    FunctionUnit fu(fu1Name);
    TS_ASSERT(fu.operationCount() == 0);
    TS_ASSERT_THROWS(fu.operation(0), OutOfRange);
    HWOperation* op1 = new HWOperation(op1Name, fu);
    TS_ASSERT(fu.operationCount() == 1);
    TS_ASSERT(fu.operation(0) == op1);
    TS_ASSERT_THROWS(fu.operation(-1), OutOfRange);
    TS_ASSERT_THROWS(fu.operation(1), OutOfRange);
    TS_ASSERT(fu.operation(op1Name) == op1);
    TS_ASSERT(fu.hasOperation(op1NAME));
    TS_ASSERT(fu.operation(op1NAME) == op1);
    TS_ASSERT_THROWS(fu.operation("nonExisting"), InstanceNotFound);
}


/**
 * Tests deleting an operation.
 */
void
FunctionUnitTest::testDeleteOperation() {

    const string fu1Name = "fu1";
    const string op1Name = "op1";

    FunctionUnit fu(fu1Name);
    HWOperation* op1 = new HWOperation(op1Name, fu);
    TS_ASSERT(fu.operationCount() == 1);
    fu.deleteOperation(*op1);
    TS_ASSERT(fu.operationCount() == 0);

    FunctionUnit fu2(fu1Name);
    HWOperation* op2 = new HWOperation(op1Name, fu2);
    TS_ASSERT_THROWS(fu.deleteOperation(*op2), InstanceNotFound);
}


/**
 * Tests adding a pipeline element to function unit.
 */
void
FunctionUnitTest::testAddPipelineElement() {

    const string fu1Name = "fu1";
    const string elem1Name = "elem1";

    FunctionUnit fu(fu1Name);

    // this is illegal way to create pipeline elements but the purpose is
    // just to test that ComponentAlreadyExists is thrown
    PipelineElement* elem1 = new PipelineElement(elem1Name, fu);
    TS_ASSERT_THROWS(
        new PipelineElement(elem1Name, fu), ComponentAlreadyExists);
    delete elem1;
}


/**
 * Tests the pipeline element getters of function unit.
 */
void
FunctionUnitTest::testPipelineElementGetters() {

    const string fu1Name = "fu1";
    const string op1Name = "op1";
    const string port1Name = "port1";
    const string res1Name = "res1";

    FunctionUnit fu(fu1Name);
    FUPort* port1 = new FUPort(port1Name, 32, fu, true, true);
    HWOperation* op = new HWOperation(op1Name, fu);
    TS_ASSERT(fu.pipelineElementCount() == 0);
    TS_ASSERT_THROWS(fu.pipelineElement(0), OutOfRange);
    op->bindPort(1, *port1);
    ExecutionPipeline* pLine = op->pipeline();
    pLine->addResourceUse(res1Name, 0, 3);
    TS_ASSERT(fu.pipelineElementCount() == 1);
    TS_ASSERT(fu.pipelineElement(0)->name() == res1Name);
    TS_ASSERT(fu.pipelineElement(res1Name)->name() == res1Name);
    TS_ASSERT_THROWS(fu.pipelineElement(-1), OutOfRange);
    TS_ASSERT_THROWS(fu.pipelineElement(1), OutOfRange);
    TS_ASSERT_THROWS(fu.pipelineElement("nonExisting"), InstanceNotFound);
}


/**
 * Tests creating function unit from an erronous mdf file.
 */
void
FunctionUnitTest::testObjectStateLoadingErrors() {

    const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
    const string INVALID_SOCKET_NAMES = "." + DIR_SEP + "data" + DIR_SEP +
        "InvalidSocketNames.mdf";
    const string SAME_DIR_SOCKETS = "." + DIR_SEP + "data" + DIR_SEP + 
        "SameDirSockets.mdf";
    const string TWO_OPCODE_SETTING_PORTS = "." + DIR_SEP + "data" + 
        DIR_SEP + "TwoOpcodeSettingPorts.mdf";
    const string OPERATIONS_WITH_SAME_NAME = "." + DIR_SEP + "data" + 
        DIR_SEP + "OperationsWithSameName.mdf";
    const string INVALID_PORT_NAME = "." + DIR_SEP + "data" + DIR_SEP + 
        "InvalidPortName.mdf";
    const string PORT_BOUND_TWICE = "." + DIR_SEP + "data" + DIR_SEP +
        "PortBoundTwice.mdf";
    const string OPERAND_BOUND_TWICE = "." + DIR_SEP + "data" + DIR_SEP + 
        "OperandBoundTwice.mdf";
    const string EARLIEST_RESOURCE_TOO_LATE = "." + DIR_SEP + "data" + 
        DIR_SEP + "EarliestResourceTooLate.mdf";
    const string PIPELINE_NOT_CANONICAL = "." + DIR_SEP + "data" + DIR_SEP +
        "PipelineNotCanonical.mdf";
    const string SIMULTANEOUS_READS_OF_OPERAND = "." + DIR_SEP + "data" +
        DIR_SEP + "SimultaneousReadsOfOperand.mdf";
    const string READS_AND_WRITES_TO_OPERAND = "." + DIR_SEP + "data" +
        DIR_SEP + "ReadsAndWritesToOperand.mdf";
    const string UNKNOWN_ADDRESS_SPACE = "." + DIR_SEP + "data" +
        DIR_SEP + "UnknownAddressSpace.adf";
    const string VALID = "." + DIR_SEP + "data" + DIR_SEP + "Valid.mdf";

    ADFSerializer serializer;
    Machine mach;

    serializer.setSourceFile(INVALID_SOCKET_NAMES);
    ObjectState* machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(SAME_DIR_SOCKETS);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(TWO_OPCODE_SETTING_PORTS);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(OPERATIONS_WITH_SAME_NAME);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;
     
    serializer.setSourceFile(INVALID_PORT_NAME);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(PORT_BOUND_TWICE);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;
    
    serializer.setSourceFile(OPERAND_BOUND_TWICE);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;
        
    serializer.setSourceFile(EARLIEST_RESOURCE_TOO_LATE);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(PIPELINE_NOT_CANONICAL);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(SIMULTANEOUS_READS_OF_OPERAND);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(READS_AND_WRITES_TO_OPERAND);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(UNKNOWN_ADDRESS_SPACE);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(VALID);
    machState = serializer.readState();
    TS_ASSERT_THROWS_NOTHING(mach.loadState(machState));
    delete machState;
}

#endif
