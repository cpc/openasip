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
 * @file TPEFProgramFactoryTest.hh
 * 
 * A test suite for .
 *
 * @author Mikael Lepistö 2005 (mikael.lepisto@tut.fi)
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

// #include "Program.hh"

using namespace TTAProgram;
using namespace TPEF;
using namespace TTAMachine;

class TPEFProgramFactoryTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
 
    void testSequentialFromAOut();
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
 * Tests creating POM out of a.out file.
 */
void
TPEFProgramFactoryTest::testSequentialFromAOut() {

    // read a.out
    BinaryStream aOutFile(LINKED_A_OUT);
    
    try {
	Binary *tpefBin = BinaryReader::readBinary(aOutFile);
	
	OperationPool opPool;
	UniversalMachine universalMach(opPool);
	
	TPEFProgramFactory progFactory(*tpefBin, universalMach);
	Program *prog = progFactory.build();

	checkProgramVsTPEF(tpefBin, prog);
	
	delete tpefBin;
	tpefBin = NULL;
	delete prog;
	prog = NULL;

    } catch ( ... ) {
	
    }
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
