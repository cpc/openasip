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
 * @file FUFiniteStateAutomatonTest.hh
 *
 * A test suite for the function unit finite state automaton (FSA) classes.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef FU_FSA_TEST_HH
#define FU_FSA_TEST_HH

#include <TestSuite.h>
#include <string>
#include <fstream>

#include "FUFiniteStateAutomaton.hh"
#include "ADFSerializer.hh"
#include "Application.hh"
#include "Machine.hh"
#include "ResourceVectorSet.hh"
#include "CollisionMatrix.hh"

class FUFiniteStateAutomatonTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCollisionMatrices();
    void testFSA();
    void testLazyFSA();

private:
    TTAMachine::FunctionUnit* fullPipeAluFU;
    TTAMachine::FunctionUnit* simpleFU;
    TTAMachine::FunctionUnit* complexFU;
};

const std::string FU_ADF = "data/test_fus.adf";

void 
FUFiniteStateAutomatonTest::setUp() {

    ADFSerializer serializer;
    serializer.setSourceFile(FU_ADF);

    TTAMachine::Machine* testMachine = NULL;
    CATCH_ANY(testMachine = serializer.readMachine());
    CATCH_ANY(fullPipeAluFU = 
              testMachine->functionUnitNavigator().item("full_pipe_alu"));
    CATCH_ANY(simpleFU = 
              testMachine->functionUnitNavigator().item("simple_mul"));
    CATCH_ANY(complexFU = 
              testMachine->functionUnitNavigator().item("complex"));
}

void 
FUFiniteStateAutomatonTest::tearDown() {
}

/**
 * Tests building the collision matrices.
 */
void
FUFiniteStateAutomatonTest::testCollisionMatrices() {

    FUFiniteStateAutomaton fsa(*complexFU, true);

    const CollisionMatrix& matrixAddf = fsa.operationCollisionMatrix("addf");

    TS_ASSERT_EQUALS(matrixAddf.columnCount(), 111);
    TS_ASSERT_EQUALS(matrixAddf.rowCount(), 3);

    FUFiniteStateAutomaton fsaSimple(*simpleFU);

    const CollisionMatrix& matrixMul = 
        fsaSimple.operationCollisionMatrix("mul");

    TS_ASSERT_EQUALS(matrixMul.columnCount(), 3);
    TS_ASSERT_EQUALS(matrixMul.rowCount(), 1);
    TS_ASSERT(matrixMul.isCollision(0, 0));

    CollisionMatrix matrix(matrixMul);
    matrix.shiftLeft();
    matrix.shiftLeft();
    TS_ASSERT(!matrix.isCollision(0, 0));

    matrix.orWith(matrixMul);
    TS_ASSERT(matrix.isCollision(0, 0));

#if 0
    // verified the correctness by printing the matrices
    Application::logStream() 
        << std::endl << matrixMul.toString() << std::endl;
#endif

}

/**
 * Tests using the FU FSA.
 */
void
FUFiniteStateAutomatonTest::testFSA() {

    FUFiniteStateAutomaton fsa(*simpleFU, false);

#if 0
    // verified the correctness by printing the FSAs
    std::ofstream dot("dot.txt");
    dot << fsa.toDotString() << std::endl;
    dot.close();
#endif

    TS_ASSERT(fsa.isLegalTransition(0, 0)); // S0[MUL]->S1
    TS_ASSERT_EQUALS(fsa.destinationState(0, 0), 1);

    TS_ASSERT(fsa.isLegalTransition(0, 1)); // S0[NOP] -> S0
    TS_ASSERT_EQUALS(fsa.destinationState(0, 1), 0);

    TS_ASSERT(!fsa.isLegalTransition(1, 0)); // S1[MUL] (not legal)

}

/**
 * Tests using the FU lazy FSA.
 */
void
FUFiniteStateAutomatonTest::testLazyFSA() {

    FUFiniteStateAutomaton fsa(*simpleFU, true);

    TS_ASSERT(fsa.isLegalTransition(0, 0)); // S0[MUL]->S1
    TS_ASSERT_EQUALS(fsa.destinationState(0, 0), 1);

    TS_ASSERT(fsa.isLegalTransition(0, 1)); // S0[NOP] -> S0
    TS_ASSERT_EQUALS(fsa.destinationState(0, 1), 0);

    TS_ASSERT(!fsa.isLegalTransition(1, 0)); // S1[MUL] (not legal)

    FUFiniteStateAutomaton fsa2(*complexFU, true);

    TS_ASSERT(fsa2.isLegalTransition(0, 3)); //S0[NOP]
    TS_ASSERT_EQUALS(fsa2.destinationState(0, 3), 0);

    TS_ASSERT(fsa2.isLegalTransition(0, 0)); //S0[ADDF]
    TS_ASSERT_EQUALS(fsa2.destinationState(0, 0), 1);

    TS_ASSERT(fsa2.isLegalTransition(0, 1)); //S0[MULF] 

    TS_ASSERT(!fsa2.isLegalTransition(1, 0)); //S1[ADDF]
    TS_ASSERT(fsa2.isLegalTransition(1, 1)); //S1[MULF]

    TS_ASSERT(fsa2.isLegalTransition(0, 2)); //S0[DIVF]
    TS_ASSERT_EQUALS(fsa2.destinationState(0, 2), 4);

    TS_ASSERT(!fsa2.isLegalTransition(1, 2)); //S1[DIVF]

#if 0
    // verified the correctness by printing the FSAs
    std::ofstream dot("dot.txt");
    dot << fsa2.toDotString() << std::endl;
    dot.close();
#endif


}


#endif
