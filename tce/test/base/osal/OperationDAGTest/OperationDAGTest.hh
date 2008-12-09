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
 * @file OperationDAGTest.hh
 * 
 * A test suite for OperationDAG and related classes.
 *
 * @author Mikael Lepist� 2007 (mikael.lepisto-no.spam-tut.fi)
 */

#ifndef OPERATION_DAG_TEST_HH
#define OPERATION_DAG_TEST_HH

#include <TestSuite.h>
#include <vector>

#include "SimValue.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "OperationContext.hh"

#include "OperationDAGConverter.hh"
#include "OperationDAG.hh"
#include "OperationDAGBehavior.hh"
#include "OperationContext.hh"
#include "TCEString.hh"

/**
 * This class is used to test OperationPool.
 */
class OperationDAGTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testCreatingDAG();
    void testCreateCode();
    void testIfCanSimulate();
private:
};

/**
 * Called before each test.
 */
void
OperationDAGTest::setUp() {
}


/**
 * Called after each test.
 */
void
OperationDAGTest::tearDown() {
}

/**
 * Test that operation() method works.
 */
void
OperationDAGTest::testCreatingDAG() {
    std::string sourceCode =         
        std::string(
            "// Check that comments at the start of code works.\n"
            "SimValue a,b,c;\n"
            "a = IO(1);\n"
            "b = IO(2);\n"
            "c =  IO(3);\n"
            "EXEC_OPERATION(sub, a, b, c);\n" 
            "EXEC_OPERATION(add, IO(3), c, c);\n"
            "// We could give warning if we make stupid things like this.\n"
            "IO(4) = c;\n"
            "//IO(4) = IO(2);\n"
            "// or this.. IO(3) cannot be input and output.\n"
            "//IO(3) = c;\n"
            );
    
    
    // try {
    OperationDAG* dag = OperationDAGConverter::createDAG(sourceCode);   
    OperationDAGBehavior behave(*dag, 4);        
    
    SimValue a,b,c,d;
    a = 1;
    b = 2;
    c = 3;
    d = 4;
    
    SimValue* params[] = {&a, &b, &c, &d};
    
    SimValue** paramsBlah = params;
    
    OperationContext context;
    
    behave.simulateTrigger(paramsBlah, context);
    
    TS_ASSERT_EQUALS(d.intValue(), 2);   

    delete dag;
    //     std::cerr << "Test runned succesful" << std::endl;
    //     } catch (Exception &e) {
    //     std::cerr << "Error occured" << std::endl;      
    //     std::cerr << e.errorMessageStack() << std::endl;
    //     }
}
void 
OperationDAGTest::testCreateCode() {

}

void 
OperationDAGTest::testIfCanSimulate() {
    OperationPool opPool;
    Operation& okOp = opPool.operation("addsubmuldivadd");    
    Operation& okOpWithConstant = opPool.operation("neg2");

    Operation& noSimOp1 = opPool.operation("cannotsimulate");
    Operation& noSimOp2 = opPool.operation("cannotsimulate2");
    Operation& allSimulationCodeOperations = opPool.operation("megadag");    
        
//     std::cerr << "Osal code: " << noSimOp1.name() << std::endl
//               << OperationDAGConverter::createOsalCode(noSimOp1.dag()) 
//               << std::endl;
        
//     std::cerr << "Simu code: " << noSimOp1.name() << std::endl
//               << OperationDAGConverter::createSimulationCode(noSimOp1.dag()) 
//               << std::endl;
         
//     std::cerr << "Expanded simu code: " << noSimOp1.name() << std::endl
//               << OperationDAGConverter::createSimulationCode(noSimOp1.expandedDag()) 
//               << std::endl;
        

//      std::cerr << "*********** Osal code: " << okOp.name() << std::endl
//                << OperationDAGConverter::createOsalCode(okOp.dag(0)) 
//                << std::endl;
    
//      std::cerr << "********* Expanded osal code: " << okOp.name() << std::endl
//                << OperationDAGConverter::createOsalCode(okOp.expandedDag()) 
//                << std::endl;
        
        
//      std::cerr << "************ Simu code: " << okOp.name() << std::endl
//                << OperationDAGConverter::createSimulationCode(okOp.dag()) 
//                << std::endl;
        
//      std::cerr << "*********** Expanded simu code: " << okOp.name() << std::endl
//                << OperationDAGConverter::createSimulationCode(okOp.expandedDag()) 
//                << std::endl;
    
//    okOp.dag(0).writeToDotFile("normal.dot");
       
    TS_ASSERT_EQUALS(noSimOp1.canBeSimulated(), false);
    TS_ASSERT_EQUALS(noSimOp2.canBeSimulated(), false);
    TS_ASSERT_EQUALS(okOp.canBeSimulated(), true);
    TS_ASSERT_EQUALS(okOpWithConstant.canBeSimulated(), true);
    TS_ASSERT_EQUALS(allSimulationCodeOperations.canBeSimulated(), true);

    OperationDAGBehavior behave(okOp.dag(0), 4);            
    SimValue a,b,c,d;
    a = 5;
    b = 2;
    c = 4;
    d = 0;
    
    SimValue* params[] = {&a, &b, &c, &d};    
    SimValue** paramsBlah = params;
    OperationContext context;
    behave.simulateTrigger(paramsBlah, context);    
    TS_ASSERT_EQUALS(d.intValue(), -10);   
}

#endif
