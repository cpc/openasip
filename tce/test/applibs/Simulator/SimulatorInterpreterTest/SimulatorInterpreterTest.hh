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
