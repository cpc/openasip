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
    Operation& okOp = opPool.operation("addsubmuldivdivu");    
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
//                << OperationDAGConverter::createOsalCode(okOp.dag()) 
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
    
//     okOp.expandedDag().writeToDotFile("expanded.dot");
//    okOp.dag(0).writeToDotFile("normal.dot");
       
    TS_ASSERT_EQUALS(noSimOp1.canBeSimulated(), false);
    TS_ASSERT_EQUALS(noSimOp2.canBeSimulated(), false);
    TS_ASSERT_EQUALS(okOp.canBeSimulated(), true);
    TS_ASSERT_EQUALS(okOpWithConstant.canBeSimulated(), true);
    TS_ASSERT_EQUALS(allSimulationCodeOperations.canBeSimulated(), true);
}

#endif
