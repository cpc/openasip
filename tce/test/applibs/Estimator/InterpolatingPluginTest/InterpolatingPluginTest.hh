/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file InterpolatingPluginTest.hh 
 *
 * A test suite for interpolating cost estimation plugins.
 * 
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_INTERPOLATING_PLUGIN_TEST_HH
#define TTA_INTERPOLATING_PLUGIN_TEST_HH

#include <string>
#include <TestSuite.h>
#include <iostream>

#include "Estimator.hh"
#include "FileSystem.hh"
#include "HDBManager.hh"
#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "FUImplementation.hh"
#include "FUPortImplementation.hh"
#include "RFEntry.hh"
#include "RFArchitecture.hh"
#include "RFImplementation.hh"
#include "RFPortImplementation.hh"
#include "FUExternalPort.hh"
#include "BaseRegisterFile.hh"
#include "RegisterFile.hh"
#include "Machine.hh"
#include "RFPort.hh"
#include "ADFSerializer.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "PipelineElement.hh"
#include "FUPort.hh"
#include "ADFSerializer.hh"
#include "Application.hh"
#include "DataObject.hh"
#include "AssocTools.hh"

using std::string;
using namespace HDB;
using namespace TTAMachine;
using namespace IDF;
using namespace CostEstimator;

const string DS = FileSystem::DIRECTORY_SEPARATOR;
const string HDB_FILE = "data" + DS + "test.hdb";
const string ADF_FILE = "data" + DS + "test.adf";
const double EPSILON = 1e-8;

/**
 * Class that tests cost estimator plugins.
 */
class InterpolatingPluginTest : public CxxTest::TestSuite {
public:
//    void setUp();
//    void tearDown();
    void testInterpolatingRFPlugin();
    void testStrictMatchWithInterpolatingRFPlugin();
    void testInterpolatingFUPlugin();
    void testStrictMatchWithInterpolatingFUPlugin();

private:
    Estimator estimator_;
};

/**
 * Test to find values to register file that is not included in the HDB but
 * it can be interpolated.
 */
void
InterpolatingPluginTest::testInterpolatingRFPlugin() {

    ADFSerializer* serializer = new ADFSerializer();
    serializer->setSourceFile(ADF_FILE);

    Machine* machine = serializer->readMachine();
    
    Machine::RegisterFileNavigator rfNav = machine->registerFileNavigator();

    TS_ASSERT_EQUALS(rfNav.count(), 2);
    RegisterFile* rf = rfNav.item(0);
    
    // points to parameterized entry that has Interpolating plugin set
    // this entry is register:
    // Width: parameterized
    // Size: parameterized
    // Read ports: 2
    // Write ports: 2
    // Bidir ports: 0
    // Max reads: 2
    // Max RW: 2
    // Latency: 1
    // Guard support: no
    UnitImplementationLocation rfImpl(HDB_FILE, 247, "RF");
    
    // There is no entry for register with Width of 30 and Size of 2, but
    // the InterpolatingPlugin should find the nearest matches that are 
    // entries 88 with Width: 1, Size: 2 and 96 with Width: 32, Size: 2
    
    // Coefficient to calculate the interpolation is following:
    // (30 - 1) / (32 - 1) = 29/31
    
    // The area values of W: 1, S: 2 entries are 48,50 gates.
    // The area values of W: 32, S: 2 entries are 878,25 gates.
    
    // So the results of register with W: 30, S: 2 should be counted like this:
    // value of Entry 88 + coefficient * (value of Entry 96 - value of Entry 88)
    // Result for area is approximately 824,718 gates
    // Result for write delay is approximately 0.3425 
    // Result for read delay is approximately 0.3206
    // Result for computation delay is approximately 3.1180

    MachineImplementation idf;
    rfImpl.setParent(idf);

    AreaInGates rfArea = estimator_.registerFileArea(*rf, rfImpl);
    DelayInNanoSeconds rfWriteDelay = estimator_.registerFilePortWriteDelay(
        *rf->port(0), rfImpl);
    DelayInNanoSeconds rfReadDelay = estimator_.registerFilePortReadDelay(
        *rf->port(0), rfImpl);
    DelayInNanoSeconds rfCompDelay = 
        estimator_.registerFileMaximumComputationDelay(*rf, rfImpl);

    TS_ASSERT_DELTA(
        rfArea, (48.50 + (29.0 / 31.0) * (878.25 - 48.50)), EPSILON);
    TS_ASSERT_DELTA(
        rfWriteDelay, (0.38 + (29.0 / 31.0) * (0.34 - 0.38)), EPSILON);
    TS_ASSERT_DELTA(
        rfReadDelay, (0.33 + (29.0 / 31.0) * (0.32 - 0.33)), EPSILON);
    TS_ASSERT_DELTA(
        rfCompDelay, (3.38 + (29.0 / 31.0) * (3.10 - 3.38)), EPSILON);
}

/**
 * Test to find out exact match from HDB with interpolating plugin.
 */
void
InterpolatingPluginTest::testStrictMatchWithInterpolatingRFPlugin() {

    ADFSerializer* serializer = new ADFSerializer();
    serializer->setSourceFile(ADF_FILE);

    Machine* machine = serializer->readMachine();
    
    Machine::RegisterFileNavigator rfNav = machine->registerFileNavigator();

    TS_ASSERT_EQUALS(rfNav.count(), 2);
    RegisterFile* rf = rfNav.item(1);
    
    // points to parameterized entry that has Interpolating plugin set
    UnitImplementationLocation rfImpl(HDB_FILE, 100, "RF_1wr_3rd");

    MachineImplementation idf;
    rfImpl.setParent(idf);

    AreaInGates rfArea = estimator_.registerFileArea(*rf, rfImpl);
    DelayInNanoSeconds rfWriteDelay =
        estimator_.registerFilePortWriteDelay(*rf->port(0), rfImpl);
    DelayInNanoSeconds rfReadDelay =
        estimator_.registerFilePortReadDelay(*rf->port(0), rfImpl);
    DelayInNanoSeconds rfCompDelay =
        estimator_.registerFileMaximumComputationDelay(*rf, rfImpl);

    TS_ASSERT_DELTA(rfArea, 2890.50, EPSILON);
    TS_ASSERT_DELTA(rfWriteDelay, 0.13, EPSILON);
    TS_ASSERT_DELTA(rfReadDelay, 0.13, EPSILON);
    TS_ASSERT_DELTA(rfCompDelay, 3.10, EPSILON);
}

/**
 * Test to find values to function unit that is not included in the HDB but
 * it can be interpolated.
 */
void
InterpolatingPluginTest::testInterpolatingFUPlugin() {

    ADFSerializer* serializer = new ADFSerializer();
    serializer->setSourceFile(ADF_FILE);

    Machine* machine = serializer->readMachine();
    
    Machine::FunctionUnitNavigator fuNav = machine->functionUnitNavigator();
   

    TS_ASSERT_EQUALS(fuNav.count(), 2);
    FunctionUnit* fu = fuNav.item(0);

    // points to parameterized entry that has Interpolating plugin set
    // There is no entry for fu with port width of 30, but
    // the InterpolatingPlugin should find the nearest matches that are 
    // entries 3 with Width: 32 and 4 with Width: 20
    
    // Coefficient to calculate the interpolation is following:
    // (30 - 20) / (32 - 20) = 10/12
    
    // The area value of entry 3 is 1230 gates.
    // The area value of entry 4 is 1000 gates.
    // entry 3 output delay 2233
    // entry 4 output delay 4000
    // entry 3 input delay 1122
    // entry 4 input delay 5000
    // entry 3 computation delay 7777
    // entry 4 computation delay 2000

    // So the results of fu with width 30 should be counted like this:
    // value of Entry 4 + coefficient * (value of Entry 3 - value of Entry 4)

    UnitImplementationLocation fuImpl(HDB_FILE, 1, "fu");
    MachineImplementation idf;
    fuImpl.setParent(idf);

    AreaInGates fuArea = estimator_.functionUnitArea(*fu, fuImpl);
    DelayInNanoSeconds fuWriteDelay = estimator_.functionUnitPortWriteDelay(
        *fu->operationPort(0), fuImpl);
    DelayInNanoSeconds fuReadDelay = estimator_.functionUnitPortReadDelay(
        *fu->operationPort(0), fuImpl);
    DelayInNanoSeconds fuCompDelay = 
        estimator_.functionUnitMaximumComputationDelay(*fu, fuImpl);

    // Result for area is approximately 1191.67 gates
    TS_ASSERT_DELTA(
        fuArea, (1000.0 + (10.0 / 12.0) * (1230.0 - 1000.0)), EPSILON);
    // Result for read (output) delay is approximately 2527.50 ns
    TS_ASSERT_DELTA(
        fuReadDelay, (4000.0 + (10.0 / 12.0) * (2233.0 - 4000.0)), EPSILON);
    // Result for write (input) delay is approximately  1768.33 ns
    TS_ASSERT_DELTA(
        fuWriteDelay, (5000.0 + (10.0 / 12.0) * (1122.0 - 5000.0)), EPSILON);
    // Result for computation delay is approximately 6817,17 ns
    TS_ASSERT_DELTA(
        fuCompDelay, (2000.0 + (10.0 / 12.0) * (7777.0 - 2000.0)), EPSILON);
}

/**
 * Test to find out exact match for FU from HDB with interpolating plugin.
 */
void
InterpolatingPluginTest::testStrictMatchWithInterpolatingFUPlugin() {

    ADFSerializer* serializer = new ADFSerializer();
    serializer->setSourceFile(ADF_FILE);

    Machine* machine = serializer->readMachine();
    
    Machine::FunctionUnitNavigator fuNav = machine->functionUnitNavigator();   

    TS_ASSERT_EQUALS(fuNav.count(), 2);
    FunctionUnit* fu = fuNav.item(1);

    // points to parameterized entry that has Interpolating plugin set
    UnitImplementationLocation fuImpl(HDB_FILE, 1, "fu");
    MachineImplementation idf;
    fuImpl.setParent(idf);

    AreaInGates fuArea = estimator_.functionUnitArea(*fu, fuImpl);
    DelayInNanoSeconds fuWriteDelay = estimator_.functionUnitPortWriteDelay(
        *fu->operationPort(0), fuImpl);
    DelayInNanoSeconds fuReadDelay = estimator_.functionUnitPortReadDelay(
        *fu->operationPort(0), fuImpl);
    DelayInNanoSeconds fuCompDelay = 
        estimator_.functionUnitMaximumComputationDelay(*fu, fuImpl);
    
    TS_ASSERT_EQUALS(fuArea, 1230);
    TS_ASSERT_EQUALS(fuWriteDelay, 1122);
    TS_ASSERT_EQUALS(fuReadDelay, 2233);
    TS_ASSERT_EQUALS(fuCompDelay, 7777);
}

#endif
