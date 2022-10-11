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
 * @file TPEFProgramFactoryTest.hh
 * 
 * A test suite for .
 *
 * @author Mikael Lepistö 2005 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TPEF_PROGRAM_FACTORY_TEST_HH
#define TPEF_PROGRAM_FACTORY_TEST_HH

#include <string>
#include <TestSuite.h>

#include "OperationPool.hh"
#include "BinaryReader.hh"
#include "TPEFProgramFactory.hh"

#include "TPEFDisassembler.hh"
#include "POMDisassembler.hh"
#include "DisassemblyInstruction.hh"
#include "ADFSerializer.hh"
#include "Machine.hh"
#include "Program.hh"
#include "BinaryStream.hh"

using namespace TTAProgram;
using namespace TPEF;
using namespace TTAMachine;

class TPEFProgramFactoryTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
 
    void testPortAllocation();
    void checkProgramVsTPEF(Binary *bin, Program *prog);

private:
    static const std::string LINKED_A_OUT;
    static const std::string PORTALLOCATION_TPEF;
    static const std::string PORTALLOCATION_ADF;
};

const std::string TPEFProgramFactoryTest::LINKED_A_OUT = "data/linked_binary.aout";
const std::string TPEFProgramFactoryTest::PORTALLOCATION_TPEF = "data/portallocation.tpef";
const std::string TPEFProgramFactoryTest::PORTALLOCATION_ADF = "data/portallocation.adf";

/**
 * Called before each test.
 */
void
TPEFProgramFactoryTest::setUp() {
}

/**
 * Called after each test.
 */
void
TPEFProgramFactoryTest::tearDown() {
}

/**
 * Tests that port allocation for registerfiles works.
 */
void
TPEFProgramFactoryTest::testPortAllocation() {
    try {
        BinaryStream binFile(PORTALLOCATION_TPEF);
    
        Binary *tpefBin = BinaryReader::readBinary(binFile);
        Machine *mach = NULL;
        ADFSerializer machineReader;
        machineReader.setSourceFile(PORTALLOCATION_ADF);
        
        mach = machineReader.readMachine();
    
        TPEFProgramFactory progFactory(*tpefBin, *mach);
        Program *prog = progFactory.build();
        
        delete tpefBin;
        tpefBin = NULL;
        delete prog;
        prog = NULL;
        delete mach;
        mach = NULL;

    } catch ( ... ) {
        TS_ASSERT(false);
    }
}

/**
 * Compares TPEF to program, and tries to check if program and tpef are same.
 */
void 
TPEFProgramFactoryTest::checkProgramVsTPEF(Binary *bin, Program *prog) {
    POMDisassembler pomDisasm(*prog);
    TPEFDisassembler tpefDisasm(*bin);
    
    TS_ASSERT_EQUALS(
	pomDisasm.startAddress(), tpefDisasm.startAddress());

    TS_ASSERT_EQUALS(
	pomDisasm.instructionCount(), tpefDisasm.instructionCount());
    
    for (Word i = 0; i < tpefDisasm.instructionCount(); i++) {
	DisassemblyInstruction *pomInstr = pomDisasm.createInstruction(i);
	DisassemblyInstruction *tpefInstr = tpefDisasm.createInstruction(i);

	// for now ignore all call lines from test, since
	// tpef disassembler does not support labels and 
	// guards seems a bit different.

	if (pomInstr->toString().find("call.1") == std::string::npos && 
	    pomInstr->toString().find_first_of("?!") == std::string::npos) {
	    TS_ASSERT_EQUALS(pomInstr->toString(), tpefInstr->toString());
	}

	delete pomInstr;
	pomInstr = NULL;
	delete tpefInstr;
	tpefInstr = NULL;
    }
}

#endif
