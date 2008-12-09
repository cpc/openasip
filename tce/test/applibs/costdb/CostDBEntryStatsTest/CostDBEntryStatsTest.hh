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
 * @file CostDBEntryStatsTest.hh 
 *
 * A test suite for CostDBEntryStats and CostDBEntryStatsRF, CostDBEntryStatsFU
 * classes.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 */    

#ifndef TTA_COSTDB_ENTRY_STATS_TEST_HH
#define TTA_COSTDB_ENTRY_STATS_TEST_HH

#include <string>
#include <TestSuite.h>
#include "CostDBEntryStats.hh"
#include "CostDBEntryStatsRF.hh"
#include "CostDBEntryStatsFU.hh"
#include "Exception.hh"

using std::string;


/**
 * Tests for making exact match searches in cost database.
 */
class CostDBEntryStatsTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    
    void testBaseClass();
    void testCostDBEntryStatsRF();
    void testCostDBEntryStatsFU();
    
private:

};

const double epsilon = 1e-8;

/**
 * Called before test.
 */
void
CostDBEntryStatsTest::setUp() {

}


/**
 * Called after test.
 */
void CostDBEntryStatsTest::tearDown() {

}


/**
 * Test that tests the CostDBEntryStats class.
 */
void CostDBEntryStatsTest::testBaseClass() {
    
    double area = 1.0;
    double internalDelay = 2.0;
    CostDBEntryStats stats(area, internalDelay);
    
    TS_ASSERT_EQUALS(area, stats.area());
    TS_ASSERT_EQUALS(internalDelay, stats.delay());

    string port1 = "port1";
    double port1Delay = 1.3;
    string port2 = "port2";
    double port2Delay = 2.3;
    string port3 = "port3";
    double port3Delay = 3.3;
    
    stats.setDelay(port1, port1Delay);
    stats.setDelay(port2, port2Delay);
    stats.setDelay(port3, port3Delay);

    TS_ASSERT_EQUALS(port1Delay, stats.delayPort(port1));
    TS_ASSERT_EQUALS(port2Delay, stats.delayPort(port2));
    TS_ASSERT_EQUALS(port3Delay, stats.delayPort(port3));

    TS_ASSERT_THROWS(stats.delayPort("port4"), KeyNotFound);

    stats.setDelay(port1, port1Delay + 1.0);
    TS_ASSERT_EQUALS(port1Delay + 1.0, stats.delayPort(port1));
    TS_ASSERT_EQUALS(port3Delay, stats.delayPort(port3));

    TS_ASSERT_THROWS(stats.energyActive(), KeyNotFound);
    TS_ASSERT_THROWS(stats.energyIdle(), KeyNotFound);
    
    double activeEnergy = 55.1;
    stats.setEnergyActive(activeEnergy);
    TS_ASSERT_EQUALS(activeEnergy, stats.energyActive());
    stats.setEnergyActive(activeEnergy + 1.0);
    TS_ASSERT_EQUALS(activeEnergy + 1.0, stats.energyActive());

    double idleEnergy = 25.4;
    stats.setEnergyIdle(idleEnergy);
    TS_ASSERT_EQUALS(idleEnergy, stats.energyIdle());
    stats.setEnergyIdle(idleEnergy + 1.0);
    TS_ASSERT_EQUALS(idleEnergy + 1.0, stats.energyIdle());

    TS_ASSERT_THROWS(stats.setEnergyOperation("oper", 1.0), WrongSubclass);
    TS_ASSERT_THROWS(stats.setEnergyRead(1.0), WrongSubclass);
    TS_ASSERT_THROWS(stats.setEnergyWrite(1.0), WrongSubclass);
    TS_ASSERT_THROWS(stats.setEnergyReadWrite(1, 1, 1.0), WrongSubclass);
    
    TS_ASSERT_THROWS(stats.energyOperation("oper"), WrongSubclass);
    TS_ASSERT_THROWS(stats.energyRead(), WrongSubclass);
    TS_ASSERT_THROWS(stats.energyWrite(), WrongSubclass);
    TS_ASSERT_THROWS(stats.energyReadWrite(1, 1), WrongSubclass);    

    CostDBEntryStats* copy = stats.copy();

    TS_ASSERT_EQUALS(stats.area(), copy->area());
    TS_ASSERT_EQUALS(stats.delay(), copy->delay());
    TS_ASSERT_THROWS_NOTHING(copy->delayPort(port1));
    TS_ASSERT_EQUALS(stats.delayPort(port1), copy->delayPort(port1));
    TS_ASSERT_EQUALS(stats.delayPort(port3), copy->delayPort(port3));

    TS_ASSERT_EQUALS(stats.energyIdle(), copy->energyIdle());
    TS_ASSERT_EQUALS(stats.energyActive(), copy->energyActive());

    double area2 = 10.0;
    double internalDelay2 = 20.0;
    CostDBEntryStats stats2(area2, internalDelay2);

    // the two stats must have equal energy and delay keys
    TS_ASSERT_THROWS(CostDBEntryStats stats3(stats, stats2, 0.5), KeyNotFound);

    stats2.setDelay(port1, 21.0);
    stats2.setDelay(port2, 22.0);
    stats2.setDelay(port3, 23.0);
    
    stats2.setEnergyActive(activeEnergy + 10.0);

    // the two stats must have equal energy and delay keys
    TS_ASSERT_THROWS(CostDBEntryStats stats3(stats, stats2, 0.5), KeyNotFound);

    stats2.setEnergyIdle(idleEnergy + 10.0);

    TS_ASSERT_THROWS_NOTHING(CostDBEntryStats stats3(stats, stats2, 0.5));

    double coefficient = 0.5;
    CostDBEntryStats stats3(stats, stats2, coefficient);
    // new area should now be in the half way of 1.0 and 10.0 that is 5.5
    TS_ASSERT_DELTA(stats.area(), 1.0, epsilon);
    TS_ASSERT_DELTA(stats2.area(), 10.0, epsilon);
    TS_ASSERT_DELTA(stats3.area(), 5.5, epsilon);

    // new internal delay should is in the half way of 2.0 and 20.0 that is 11.0
    TS_ASSERT_DELTA(stats.delay(), 2.0, epsilon);
    TS_ASSERT_DELTA(stats2.delay(), 20.0, epsilon);
    TS_ASSERT_DELTA(stats3.delay(), 11.0, epsilon);
    
    // 2.3 and 21.0 halfway is 2.3 + 0.5 * (21.0 - 2.3) = 11.65
    TS_ASSERT_DELTA(stats.delayPort(port1), 2.3, epsilon);
    TS_ASSERT_DELTA(stats2.delayPort(port1), 21.0, epsilon);
    TS_ASSERT_DELTA(stats3.delayPort(port1), 11.65, epsilon);

    coefficient = 0.0;
    CostDBEntryStats stats4(stats, stats2, coefficient);
    // 1.0 + 0.0 * (10.0 - 1.0) = 1.0
    TS_ASSERT_DELTA(stats.area(), 1.0, epsilon);
    TS_ASSERT_DELTA(stats2.area(), 10.0, epsilon);
    TS_ASSERT_DELTA(stats4.area(), 1.0, epsilon);

    stats2.setDelay("port4", 10.0);
    // the two stats must have equal energy and delay keys, now stats2 have one
    // delay more
    TS_ASSERT_THROWS(CostDBEntryStats stats5(stats, stats2, 0.5), KeyNotFound);
}

/**
 * Test for CostDBEntryStatsRF class data.
 */
void CostDBEntryStatsTest::testCostDBEntryStatsRF() {

    double area = 1.0;
    double internalDelay = 2.0;
    CostDBEntryStatsRF stats(area, internalDelay);
    
    TS_ASSERT_EQUALS(area, stats.area());
    TS_ASSERT_EQUALS(internalDelay, stats.delay());

    TS_ASSERT_THROWS(stats.energyActive(), WrongSubclass);
    TS_ASSERT_THROWS(stats.setEnergyActive(1.0), WrongSubclass);
    // no energy data set
    TS_ASSERT_THROWS(stats.energyRead(), KeyNotFound);
    TS_ASSERT_THROWS(stats.energyWrite(), KeyNotFound);
    TS_ASSERT_THROWS(stats.energyReadWrite(1, 1), KeyNotFound);

    double readEnergy = 22.0;
    stats.setEnergyRead(readEnergy);
    double writeEnergy = 33.0;
    stats.setEnergyWrite(writeEnergy);
    double readWriteEnergy = 44.4;
    stats.setEnergyReadWrite(1, 1, readWriteEnergy);
    stats.setEnergyReadWrite(2, 1, readWriteEnergy + 1.0);
    stats.setEnergyReadWrite(2, 2, readWriteEnergy + 2.0);

    TS_ASSERT_EQUALS(stats.energyRead(), readEnergy);
    TS_ASSERT_EQUALS(stats.energyWrite(), writeEnergy);
    TS_ASSERT_EQUALS(stats.energyReadWrite(1, 1), readWriteEnergy);
    TS_ASSERT_EQUALS(stats.energyReadWrite(2, 1), readWriteEnergy + 1.0);
    TS_ASSERT_EQUALS(stats.energyReadWrite(2, 2), readWriteEnergy + 2.0);

    // test resetting of values
    stats.setEnergyRead(readEnergy + 1.0);
    stats.setEnergyWrite(writeEnergy + 1.0);
    stats.setEnergyReadWrite(1, 1, readWriteEnergy + 1.0);
    stats.setEnergyReadWrite(2, 1, readWriteEnergy + 2.0);
    stats.setEnergyReadWrite(2, 2, readWriteEnergy + 3.0);
    TS_ASSERT_EQUALS(stats.energyRead(), readEnergy + 1.0);
    TS_ASSERT_EQUALS(stats.energyWrite(), writeEnergy + 1.0);
    TS_ASSERT_EQUALS(stats.energyReadWrite(1, 1), readWriteEnergy + 1.0);
    TS_ASSERT_EQUALS(stats.energyReadWrite(2, 1), readWriteEnergy + 2.0);
    TS_ASSERT_EQUALS(stats.energyReadWrite(2, 2), readWriteEnergy + 3.0);

    CostDBEntryStats* copy = stats.copy();

    TS_ASSERT_EQUALS(stats.area(), copy->area());
    TS_ASSERT_EQUALS(stats.delay(), copy->delay());
    TS_ASSERT_THROWS(copy->delayPort("port1"), KeyNotFound);

    TS_ASSERT_EQUALS(stats.energyRead(), copy->energyRead());
    TS_ASSERT_EQUALS(stats.energyReadWrite(2, 1), copy->energyReadWrite(2, 1));

    double area2 = 10.0;
    double internalDelay2 = 20.0;
    CostDBEntryStatsRF stats2(area2, internalDelay2);

    double readEnergy2 = 222.0;
    stats2.setEnergyRead(readEnergy2);
    double writeEnergy2 = 333.0;
    stats2.setEnergyWrite(writeEnergy2);
    double readWriteEnergy2 = 444.0;
    stats2.setEnergyReadWrite(1, 1, readWriteEnergy2);
    stats2.setEnergyReadWrite(2, 1, readWriteEnergy2 + 1.0);

    double coefficient = 0.5;
    TS_ASSERT_THROWS(
        CostDBEntryStatsRF stats3(stats, stats2, coefficient), KeyNotFound);

    stats2.setEnergyReadWrite(2, 2, readWriteEnergy2 + 2.0);

    TS_ASSERT_THROWS_NOTHING(
        CostDBEntryStatsRF stats3(stats, stats2, coefficient));

    CostDBEntryStatsRF stats3(stats, stats2, coefficient);
    TS_ASSERT_DELTA(stats.energyWrite(), 34.0, epsilon);
    TS_ASSERT_DELTA(stats.energyReadWrite(1, 1), 45.4, epsilon);
    TS_ASSERT_DELTA(stats2.energyWrite(), 333.0, epsilon);
    TS_ASSERT_DELTA(stats2.energyReadWrite(1, 1), 444.0, epsilon);

    // stats3: energy write should be 34.0 + 0.5 * (333.0 - 34.0) = 183.5
    TS_ASSERT_DELTA(stats3.energyWrite(), 183.5, epsilon);
    // stats3: energy write should be 45.4 + 0.5 * (444.0 - 45.4) = 244.7
    TS_ASSERT_DELTA(stats3.energyReadWrite(1, 1), 244.7, epsilon);
    
}


/**
 * Test for CostDBEntryStatsRF class data.
 */
void CostDBEntryStatsTest::testCostDBEntryStatsFU() {

    double area = 1.0;
    double internalDelay = 2.0;
    CostDBEntryStatsFU stats(area, internalDelay);
    
    TS_ASSERT_EQUALS(area, stats.area());
    TS_ASSERT_EQUALS(internalDelay, stats.delay());

    TS_ASSERT_THROWS(stats.energyActive(), WrongSubclass);
    TS_ASSERT_THROWS(stats.setEnergyActive(1.0), WrongSubclass);
    TS_ASSERT_THROWS(stats.energyRead(), WrongSubclass);
    TS_ASSERT_THROWS(stats.setEnergyRead(1.0), WrongSubclass);
    TS_ASSERT_THROWS(stats.energyWrite(), WrongSubclass);
    TS_ASSERT_THROWS(stats.setEnergyWrite(1.0), WrongSubclass);
    TS_ASSERT_THROWS(stats.energyReadWrite(1, 1), WrongSubclass);
   
    string operation = "oper";
    // no data set
    TS_ASSERT_THROWS(stats.energyOperation(operation), KeyNotFound);
    TS_ASSERT_EQUALS(stats.hasEnergyOperation(operation), false);

    double operEnergy = 11.1;
    stats.setEnergyOperation(operation, operEnergy);
    string operation2 = "oper2";
    double oper2Energy = 22.2;
    stats.setEnergyOperation(operation2, oper2Energy);
    TS_ASSERT_EQUALS(stats.energyOperation(operation), operEnergy);
    TS_ASSERT_EQUALS(stats.hasEnergyOperation(operation), true);
    TS_ASSERT_EQUALS(stats.energyOperation(operation2), oper2Energy);

    // test resetting of values
    stats.setEnergyOperation(operation, operEnergy + 1.0);
    TS_ASSERT_EQUALS(stats.energyOperation(operation), operEnergy + 1.0);
    TS_ASSERT_EQUALS(stats.hasEnergyOperation(operation), true);

    CostDBEntryStats* copy = stats.copy();

    TS_ASSERT_EQUALS(stats.area(), copy->area());
    TS_ASSERT_EQUALS(stats.delay(), copy->delay());
    TS_ASSERT_THROWS(copy->delayPort("port1"), KeyNotFound);

    TS_ASSERT_EQUALS(
        stats.energyOperation(operation), copy->energyOperation(operation));
    TS_ASSERT_EQUALS(
        stats.energyOperation(operation2), copy->energyOperation(operation2));

    double area2 = 10.0;
    double internalDelay2 = 20.0;
    CostDBEntryStatsFU stats2(area2, internalDelay2);

    double operEnergy2 = 44.1;
    stats2.setEnergyOperation(operation, operEnergy2);

    double coefficient = 0.5;
    TS_ASSERT_THROWS(
        CostDBEntryStatsFU stats3(stats, stats2, coefficient), KeyNotFound);

    double oper2Energy2 = 22.2;
    stats2.setEnergyOperation(operation2, oper2Energy2);

    TS_ASSERT_THROWS_NOTHING(
        CostDBEntryStatsFU stats3(stats, stats2, coefficient));

    CostDBEntryStatsFU stats3(stats, stats2, coefficient);

    // stats3: energy for operation oper should be
    // 12.1 + 0.5 * (44.1 - 12.1) = 28.1
    TS_ASSERT_DELTA(stats3.energyOperation(operation), 28.1, epsilon);
    // stats3: energy for operation oper2 should be
    // 22.2 + 0.5 * (22.2 - 22.2) = 22.2
    TS_ASSERT_DELTA(stats3.energyOperation(operation2), 22.2, epsilon);
}

#endif
