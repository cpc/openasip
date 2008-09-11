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
 * @file PipelineResourceModelTest.hh
 *
 * A test suite for the different pipeline resource models.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef PIPELINE_RESOURCE_MODEL_TEST_HH
#define PIPELINE_RESOURCE_MODEL_TEST_HH

#include <TestSuite.h>
#include <string>
#include <fstream>

#include "FUFiniteStateAutomaton.hh"
#include "ADFSerializer.hh"
#include "Application.hh"
#include "Machine.hh"
#include "ReservationTable.hh"
#include "CollisionMatrix.hh"
#include "FUReservationTableIndex.hh"

class PipelineResourceModelTest : public CxxTest::TestSuite {
public:
    PipelineResourceModelTest();

    void setUp();
    void tearDown();

    void testCRT();
    void testFU15();
private:
    const TTAMachine::FunctionUnit* ALU_;
    const TTAMachine::FunctionUnit* mul_;
    const TTAMachine::FunctionUnit* FPU_;
    /// An FU that had some problems in system tests.
    const TTAMachine::FunctionUnit* fu15_;
};

PipelineResourceModelTest::PipelineResourceModelTest() {
}

/**
 * Loads the tested FUs from the ADF.
 */
void 
PipelineResourceModelTest::setUp() {

    ALU_ = 
        TTAMachine::Machine::loadFromADF(
            "data/alu.adf")->functionUnitNavigator().item("alu");
    mul_ = 
        TTAMachine::Machine::loadFromADF(
            "data/mul.adf")->functionUnitNavigator().item("mul");
    FPU_ = 
        TTAMachine::Machine::loadFromADF(
            "data/fpu.adf")->functionUnitNavigator().item("fpu");

    fu15_ = 
        TTAMachine::Machine::loadFromADF(
            "data/fu15.adf")->functionUnitNavigator().item("fu15");

}

void 
PipelineResourceModelTest::tearDown() {
}

/**
 * Tests the Conventional Reservation Table model.
 */
void
PipelineResourceModelTest::testCRT() {
    
    ReservationTable ALU(*ALU_);
    TS_ASSERT_EQUALS(
        ALU.toString(), 
        "0 0 \n"
        "0 0 \n"
        "0 0 \n");

    ReservationTable mul(*mul_->operation("mul"));
    
    TS_ASSERT(mul.isReserved(0, 0)); // in1
    TS_ASSERT(mul.isReserved(1, 0)); // t
    TS_ASSERT(!mul.isReserved(2, 0)); // out1
    TS_ASSERT(mul.isReserved(2, 2)); // out1

    TS_ASSERT(!mul.isReserved(3, 0)); // X
    TS_ASSERT(mul.isReserved(3, 1)); // X
    TS_ASSERT(mul.isReserved(3, 2)); // X

    TS_ASSERT(mul.isReserved(4, 2)); // Y

    mul.advanceCycle();

    TS_ASSERT(!mul.isReserved(0, 0)); // in1
    TS_ASSERT(!mul.isReserved(1, 0)); // t
    TS_ASSERT(!mul.isReserved(2, 0)); // out1
    TS_ASSERT(mul.isReserved(2, 1)); // out1

    TS_ASSERT(mul.isReserved(3, 0)); // X
    TS_ASSERT(mul.isReserved(3, 1)); // X

    TS_ASSERT(mul.isReserved(4, 1)); // Y

    TS_ASSERT(!mul.isReserved(0, 2)); 
    TS_ASSERT(!mul.isReserved(1, 2)); 
    TS_ASSERT(!mul.isReserved(2, 2)); 
    TS_ASSERT(!mul.isReserved(3, 2)); 

    ReservationTable sqrt(*FPU_->operation("sqrtf"));

    //Application::logStream() << std::endl << sqrt.toString();

    TS_ASSERT(sqrt.isReserved(1, 0)); // R
    TS_ASSERT(!sqrt.isReserved(1, 1)); // R
    TS_ASSERT(sqrt.isReserved(1, 50)); // R
    TS_ASSERT(!sqrt.isReserved(1, 51)); // R
    TS_ASSERT(sqrt.isReserved(2, 111)); // W
    TS_ASSERT(!sqrt.isReserved(2, 110)); // W

    TS_ASSERT(sqrt.isReserved(4, 110)); // A
    TS_ASSERT(!sqrt.isReserved(4, 111)); // A

    TS_ASSERT(sqrt.isReserved(4, 2)); // A
    TS_ASSERT(!sqrt.isReserved(4, 1)); // A

    ReservationTable FPU(*FPU_);

    TS_ASSERT(!FPU.conflictsWith(sqrt));

    FPU.issueOperation(sqrt);

    TS_ASSERT(FPU.conflictsWith(sqrt));

    TS_ASSERT_EQUALS(sqrt.toString(), FPU.toString());
}

/**
 * Tests the resource models for a problematic FU from system tests.
 *
 * Had couple of bugs with this one.
 */
void
PipelineResourceModelTest::testFU15() {

    ReservationTable mul(*fu15_->operation("mul"));   
    TS_ASSERT_EQUALS(
        mul.toString(),
        "1 0 0 \n"
        "1 0 0 \n"
        "0 0 1 \n");

    ReservationTable add(*fu15_->operation("add"));   
    TS_ASSERT_EQUALS(
        add.toString(),
        "1 0 0 \n"
        "1 0 0 \n"
        "1 0 0 \n");

    FUReservationTableIndex rti(*fu15_);

    CollisionMatrix addCM(rti, 0); 
    TS_ASSERT_EQUALS(
        addCM.toString(),
        "1 0 0 \n"
        "1 0 0 \n");

    CollisionMatrix mulCM(rti, 1); 
    TS_ASSERT_EQUALS(
        mulCM.toString(),
        "1 0 1 \n"
        "1 0 0 \n");
}

#endif
