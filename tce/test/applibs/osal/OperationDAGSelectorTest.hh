/**
 * @file OperationDAGSelectorTest.hh
 *
 * @author Mikael Lepistö 2008 (tmlepist@cs.tut.fi)
 * @note rating: red
 */

#ifndef OPERATION_DAG_SELECTOR_TEST_HH
#define OPERATION_DAG_SELECTOR_TEST_HH

#include <string>
#include <TestSuite.h>

#include "OperationDAGSelector.hh"
#include "OperationPool.hh"

class OperationDAGSelectorTest : public CxxTest::TestSuite {
public:

    virtual ~OperationDAGSelectorTest(); 

    void setUp();
    void tearDown();
    void testDAGSelection();    
};

OperationDAGSelectorTest::~OperationDAGSelectorTest() {
}

void
OperationDAGSelectorTest::setUp() {
}

void
OperationDAGSelectorTest::tearDown() {
}

/**
 * Tests selecting different set of DAGs of operation.
 */
void
OperationDAGSelectorTest::testDAGSelection() {

    // only mul op 0 dags
    std::set<std::string> opSet1;
    opSet1.insert("MUL");
    TS_ASSERT_EQUALS(
        OperationDAGSelector::findDags("test", opSet1).size(), (unsigned)0);

    // with sub,mul = 1
    std::set<std::string> opSet2;
    opSet2.insert("MUL");
    opSet2.insert("SUB");
    TS_ASSERT_EQUALS(
        OperationDAGSelector::findDags("test", opSet2).size(), (unsigned)1);



    // with addsub, mul = 1   
    std::set<std::string> opSet3;
    opSet3.insert("ADDSUB");
    opSet3.insert("MUL");
    TS_ASSERT_EQUALS(
        OperationDAGSelector::findDags("test", opSet3).size(), (unsigned)1);

    // with addsub, sub, mul = 3
    std::set<std::string> opSet4;
    opSet4.insert("ADDSUB");
    opSet4.insert("SUB");
    opSet4.insert("MUL");
    TS_ASSERT_EQUALS(
        OperationDAGSelector::findDags("test", opSet4).size(), (unsigned)3);

    

    // TODO: after recursive search there should be also 3 hits with
    // add, sub, mul 
    std::set<std::string> opSet5;
    opSet5.insert("ADD");
    opSet5.insert("SUB");
    opSet5.insert("MUL");
    TS_ASSERT_EQUALS(
        OperationDAGSelector::findDags("test", opSet5).size(), (unsigned)1);
    
    Operation& op = OperationPool().operation("test2");
    
    OperationDAG* expandedDag1 = 
        OperationDAGSelector::createExpandedDAG(op, opSet1);

    TS_ASSERT_EQUALS(expandedDag1->nodeCount(), 5);   

    OperationDAG* expandedDag2 = 
        OperationDAGSelector::createExpandedDAG(op, opSet5);

    TS_ASSERT_EQUALS(expandedDag2->nodeCount(), 6);

    delete expandedDag1;
    delete expandedDag2;         
}

#endif
