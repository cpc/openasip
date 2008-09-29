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
 * @file ExecutionPipelineTest.hh 
 * A test suite for ExecutionPipeline.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef ExecutionPipelineTest_HH
#define ExecutionPipelineTest_HH

#include <string>

#include <TestSuite.h>
#include "FUPort.hh"
#include "HWOperation.hh"
#include "FunctionUnit.hh"
#include "ExecutionPipeline.hh"
#include "AssocTools.hh"

using std::string;
using namespace TTAMachine;

const string FU_NAME = "fu";
const string PORT_1_NAME = "port1";
const string PORT_2_NAME = "port2";
const string OP_NAME = "op";

/**
 * Tests the functionality of ExecutionPipeline class.
 */
class ExecutionPipelineTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testResourceUsages();
    void testOperandUsages();
    void testLatencyFunctions();
    void testLoadState();

private:
    FunctionUnit* fu_;
};


/**
 * Called before each test.
 */
void
ExecutionPipelineTest::setUp() {
    fu_ = new FunctionUnit(FU_NAME);
    new FUPort(PORT_1_NAME, 32, *fu_, true, true);
    new FUPort(PORT_2_NAME, 32, *fu_, false, false);
    new HWOperation(OP_NAME, *fu_);
}


/**
 * Called after each test.
 */
void
ExecutionPipelineTest::tearDown() {
    delete fu_;
}


/**
 * Tests adding and removing a resource usage.
 */
void
ExecutionPipelineTest::testResourceUsages() {

    HWOperation* op = fu_->operation(0);
    ExecutionPipeline* pLine = op->pipeline();
    
    const string res1Name = "res1";
    const string res2Name = "res2";
    const string invalidRes = "2inv";

    TS_ASSERT_THROWS(pLine->addResourceUse(res1Name, 2, 3), StartTooLate);
    TS_ASSERT_THROWS(pLine->addResourceUse(res1Name, -1, 3), OutOfRange);
    TS_ASSERT_THROWS(pLine->addResourceUse(res1Name, 0, -2), OutOfRange);
    TS_ASSERT_THROWS(pLine->addResourceUse(invalidRes, 0, 2), InvalidName);

    pLine->addResourceUse(res1Name, 0, 3);
    TS_ASSERT(pLine->isResourceUsed(res1Name, 2));
    TS_ASSERT(!pLine->isResourceUsed(res1Name, 3));
    TS_ASSERT_THROWS(pLine->addResourceUse(res1Name, 2, 3), NotAvailable);
    
    pLine->addResourceUse(res2Name, 4, 2);
    TS_ASSERT(pLine->isResourceUsed(res2Name, 4));

    TS_ASSERT(fu_->hasPipelineElement(res1Name));
    TS_ASSERT(fu_->hasPipelineElement(res2Name));

    TS_ASSERT_THROWS(pLine->removeResourceUse(res1Name), StartTooLate);

    pLine->removeResourceUse(res2Name);
    TS_ASSERT(!fu_->hasPipelineElement(res2Name));
    TS_ASSERT(!pLine->isResourceUsed(res2Name, 4));
    TS_ASSERT(pLine->isResourceUsed(res1Name, 0));
             
    pLine->removeResourceUse(res1Name, 0);
    TS_ASSERT(!pLine->isResourceUsed(res1Name, 0));
    TS_ASSERT_THROWS(pLine->removeResourceUse(res1Name, 1), StartTooLate);
    TS_ASSERT_THROWS(pLine->removeResourceUse(res1Name, -1), OutOfRange);

    TS_ASSERT(pLine->latency() == 3);
    pLine->removeAllResourceUses();
    TS_ASSERT(pLine->latency() == 0);
    TS_ASSERT(fu_->pipelineElementCount() == 0);

    pLine->addResourceUse(res1Name, 0, 3);
    TS_ASSERT_THROWS_NOTHING(pLine->removeResourceUse(res1Name));
}


/**
 * Tests adding and removing operand usages.
 */
void
ExecutionPipelineTest::testOperandUsages() {

    HWOperation* op = fu_->operation(0);
    ExecutionPipeline* pLine = op->pipeline();
    FUPort* port1 = fu_->operationPort(0);
    FUPort* port2 = fu_->operationPort(1);

    TS_ASSERT_THROWS(pLine->addPortRead(0, 0, 2), OutOfRange);
    TS_ASSERT_THROWS(pLine->addPortRead(1, -1, 2), OutOfRange);
    TS_ASSERT_THROWS(pLine->addPortRead(1, 0, 0), OutOfRange);

    TS_ASSERT_THROWS(pLine->addPortWrite(0, 0, 2), OutOfRange);
    TS_ASSERT_THROWS(pLine->addPortWrite(1, -1, 2), OutOfRange);
    TS_ASSERT_THROWS(pLine->addPortWrite(1, 0, 0), OutOfRange);

    TS_ASSERT_THROWS(pLine->addPortRead(1, 2, 2), StartTooLate);

    pLine->addPortRead(1, 0, 3);
    op->bindPort(1, *port1);
    TS_ASSERT(pLine->isPortRead(*port1, 0));
    TS_ASSERT(pLine->isPortRead(*port1, 2));
    TS_ASSERT(!pLine->isPortRead(*port1, 3));
    TS_ASSERT_THROWS(pLine->addPortRead(1, 2, 2), NotAvailable);

    TS_ASSERT(pLine->latency() == 3);

    TS_ASSERT_THROWS(pLine->addPortWrite(1, 2, 3), WrongOperandType);
    pLine->addPortWrite(2, 2, 3);
    op->bindPort(2, *port2);
    TS_ASSERT(pLine->isPortWritten(*port2, 2));
    TS_ASSERT(pLine->isPortUsed(*port2, 4));
    TS_ASSERT(!pLine->isPortUsed(*port1, 4));
    TS_ASSERT_THROWS(pLine->addPortWrite(2, 4, 2), NotAvailable);

    TS_ASSERT_THROWS(pLine->readOperands(-1), OutOfRange);
    TS_ASSERT_THROWS(pLine->writtenOperands(-1), OutOfRange);

    TS_ASSERT(AssocTools::containsKey(pLine->readOperands(0), 1));
    TS_ASSERT(AssocTools::containsKey(pLine->writtenOperands(2), 2));

    pLine->removeOperandUse(1, 0);
    TS_ASSERT(!pLine->isPortRead(*port1, 0));
    TS_ASSERT_THROWS(pLine->removeOperandUse(1, 1), StartTooLate);
    TS_ASSERT_THROWS(pLine->removeOperandUse(1, -1), OutOfRange);
    TS_ASSERT_THROWS(pLine->removeOperandUse(-1, 0), OutOfRange);
}


/**
 * Tests the latency and slack functions.
 */
void
ExecutionPipelineTest::testLatencyFunctions() {

    HWOperation* op = fu_->operation(0);
    ExecutionPipeline* pLine = op->pipeline();

    TS_ASSERT_THROWS(op->latency(1), IllegalParameters);
    TS_ASSERT_THROWS(op->latency(2), IllegalParameters);
    TS_ASSERT_THROWS(op->slack(1), IllegalParameters);
    TS_ASSERT_THROWS(op->slack(2), IllegalParameters);

    new FUPort("new_input", 32, *fu_, false, false);
    new FUPort("new_output", 32, *fu_, false, false);

    pLine->addPortRead(1, 0, 1);
    pLine->addPortRead(2, 3, 2);
    pLine->addPortWrite(3, 4, 1);
    pLine->addPortWrite(4, 6, 3);

    TS_ASSERT_THROWS(op->latency(1), IllegalParameters);
    TS_ASSERT(op->slack(1) == 0);
    TS_ASSERT_THROWS(op->latency(2), IllegalParameters);
    TS_ASSERT(op->slack(2) == 3);
    TS_ASSERT(op->latency(3) == 5);
    TS_ASSERT_THROWS(op->slack(3), IllegalParameters);
    TS_ASSERT(op->latency(4) == 9);
    TS_ASSERT_THROWS(op->slack(4), IllegalParameters);
} 


/**
 * Tests loading the state of the pipeline from an ObjectState tree.
 */
void
ExecutionPipelineTest::testLoadState() {

    const string res1Name = "res1";
    const string illegalName = "illegal name";

    HWOperation* op = fu_->operation(0);
    ExecutionPipeline* pLine = op->pipeline();
    
    FUPort* opcodeSetting = fu_->operationPort(PORT_1_NAME);
    FUPort* normalPort = fu_->operationPort(PORT_2_NAME);

    op->bindPort(1, *opcodeSetting);
    
    pLine->addPortRead(1, 0, 2);
    pLine->addPortWrite(2, 3, 3);
    pLine->addResourceUse(res1Name, 0, 5);

    ObjectState* pLineState = pLine->saveState();

    TS_ASSERT_THROWS_NOTHING(pLine->loadState(pLineState));
    TS_ASSERT(pLine->isPortRead(*opcodeSetting, 0));
    TS_ASSERT(pLine->isPortRead(*opcodeSetting, 1));
    TS_ASSERT(!pLine->isPortRead(*opcodeSetting, 2));
    TS_ASSERT(pLine->isResourceUsed(res1Name, 0));
    TS_ASSERT(pLine->isResourceUsed(res1Name, 4));
    TS_ASSERT(!pLine->isResourceUsed(res1Name, 5));

    op->bindPort(2, *normalPort);
    TS_ASSERT(pLine->isPortWritten(*normalPort, 3));
    TS_ASSERT(pLine->isPortWritten(*normalPort, 5));

    // change the name of the resource
    ObjectState* usageState = 
        pLineState->childByName(ExecutionPipeline::OSNAME_RESOURCE_USAGE);
    usageState->setAttribute(
        ExecutionPipeline::OSKEY_RESOURCE_NAME, illegalName);
    TS_ASSERT_THROWS(
        pLine->loadState(pLineState), ObjectStateLoadingException);

    delete pLineState;    
}
    

#endif
