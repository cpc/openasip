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
