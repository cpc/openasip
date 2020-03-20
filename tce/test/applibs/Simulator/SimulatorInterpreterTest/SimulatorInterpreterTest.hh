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
 * @file SimulatorInterpreterTest.hh
 * 
 * A test suite for SimulatorInterpreter.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef SIMULATOR_INTERPRETER_TEST_HH
#define SIMULATOR_INTERPRETER_TEST_HH

#include <TestSuite.h>
#include <string>
#include "SimulatorInterpreter.hh"
#include "ProgCommand.hh"
#include "MachCommand.hh"
#include "ConfCommand.hh"
#include "BaseLineReader.hh"

using std::string;

class SimulatorInterpreterTest : public CxxTest::TestSuite {
public:
    SimulatorInterpreterTest();
    virtual ~SimulatorInterpreterTest();
    void setUp();
    void tearDown();
    void testProg();
    void testMach();
    void testConf();
private:
    SimulatorFrontend frontend_;
    SimulatorInterpreterContext context_;
    BaseLineReader reader_;
    SimulatorInterpreter interpreter_;
};

/// A working sequential a.out.
const string WORKING_SEQ_PROGRAM = 
"../../../base/tpef/AOutReaderTest/data/test_main.o";

/// Unknown format file (for BinaryReader).
const string BROKEN_TPEF = "SimulatorInterpreterTest.hh";

/// A working machine.
const string WORKING_MACHINE = "data/working.adf";

/// Broken ADF.
const string BROKEN_ADF = "SimulatorInterpreterTest.hh";

/// Working PCF.
const string WORKING_CONF = "data/working.pcf";

/// A PCF that contains no ADF.
const string CONF_WITH_NO_ADF = "data/noadf.pcf";

/// A PCF that contains an ADF definition but the ADF is not found.
const string CONF_WITH_ADF_NOT_FOUND = "data/adfnotfound.pcf";

/// A PCF that contains an ADF and the ADF is broken.
const string CONF_WITH_BROKEN_ADF = "data/brokenadf.pcf";

/**
 * Constructor.
 *
 * Creates the SimulatorInterpreter instance to be tested.
 */
SimulatorInterpreterTest::SimulatorInterpreterTest() : 
    frontend_(), context_(frontend_), reader_(),
    interpreter_(0, NULL, context_, reader_)  {
}

/**
 * Destructor.
 *
 * Frees the dynamically allocated memory.
 */
SimulatorInterpreterTest::~SimulatorInterpreterTest() {
}

/**
 * Called before each test.
 */
void
SimulatorInterpreterTest::setUp() {
}


/**
 * Called after each test.
 */
void
SimulatorInterpreterTest::tearDown() {
}

/**
 * Tests the ProgCommand.
 *
 * Also tests SimulatorFrontend::loadProgram().
 */
void 
SimulatorInterpreterTest::testProg() {

    TS_ASSERT(!interpreter_.error());

    interpreter_.interpret("prog foo");
    TS_ASSERT(interpreter_.error());

    interpreter_.interpret("prog");
    TS_ASSERT(interpreter_.error());

    interpreter_.interpret("prog " + BROKEN_TPEF);
    TS_ASSERT(interpreter_.error());
}

/**
 * Tests the MachCommand.
 *
 * Also tests SimulatorFrontend::loadMachine().
 */
void 
SimulatorInterpreterTest::testMach() {

    interpreter_.setError(false);
    TS_ASSERT(!interpreter_.error());

    interpreter_.interpret("mach foo");
    TS_ASSERT(interpreter_.error());

    interpreter_.interpret("mach");
    TS_ASSERT(interpreter_.error());

    interpreter_.interpret("mach " + WORKING_MACHINE);
    TS_ASSERT(!interpreter_.error());

    interpreter_.interpret("mach " + BROKEN_ADF);
    TS_ASSERT(interpreter_.error());
}

/**
 * Tests the ConfCommand.
 *
 * Also tests SimulatorFrontend::loadMachine().
 */
void 
SimulatorInterpreterTest::testConf() {

    interpreter_.setError(false);
    TS_ASSERT(!interpreter_.error());

    interpreter_.interpret("conf foo");
    TS_ASSERT(interpreter_.error());

    interpreter_.interpret("conf");
    TS_ASSERT(interpreter_.error());

    interpreter_.interpret("conf " + WORKING_CONF);
    TS_ASSERT(!interpreter_.error());

    interpreter_.interpret("conf " + CONF_WITH_NO_ADF);
    TS_ASSERT(interpreter_.error());

    interpreter_.interpret("conf " + CONF_WITH_ADF_NOT_FOUND);
    TS_ASSERT(interpreter_.error());

    interpreter_.interpret("conf " + CONF_WITH_BROKEN_ADF);
    TS_ASSERT(interpreter_.error());
}




#endif
