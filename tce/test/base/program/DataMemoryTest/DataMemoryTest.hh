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
 * @file DataMemoryTest.hh
 *
 * A test suite for DataMemory class, copying POM and
 * ProgramWriter and TPEFProgramFactory classes.
 *
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Mikael Lepistö 2007 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef DATA_MEMORY_TEST_HH
#define DATA_MEMORY_TEST_HH

#include <string>
#include <TestSuite.h>

#include "OperationPool.hh"
#include "BinaryReader.hh"
#include "TPEFProgramFactory.hh"
#include "ProgramWriter.hh"

#include "Machine.hh"
#include "ADFSerializer.hh"

#include "Program.hh"
#include "ControlUnit.hh"
#include "DataMemory.hh"
#include "DataDefinition.hh"
#include "Move.hh"
#include "Instruction.hh"
#include "Immediate.hh"
#include "InstructionReference.hh"
#include "TerminalImmediate.hh"
#include "NullAddressSpace.hh"
#include "BinaryStream.hh"

using namespace TTAProgram;
using namespace TTAMachine;
using namespace TPEF;

class DataMemoryTest : public CxxTest::TestSuite {
public:
    DataMemoryTest();
    ~DataMemoryTest();

    void setUp();
    void tearDown();
    void testCopying();
    void testProgramReaderAndWriter();
    void testDataMemoryRequestsAndOrganizing();
    void testDataMemoryWritingToTPEF();
    
private:
    void comparePrograms(Program& prog1, Program& prog2);

    Program* firstRead_;
    Program* copied_;
    Program* secondRead_;
    Machine* mach_;
    
    static const std::string PARALLEL_TPEF;
    static const std::string PARALLEL_ADF;
};

const std::string DataMemoryTest::PARALLEL_TPEF = "data/multiaspace.tpef";
const std::string DataMemoryTest::PARALLEL_ADF = "data/multiaspace.adf";

DataMemoryTest::DataMemoryTest() {
    // read stuff from TPEF
    BinaryStream tpefFile(PARALLEL_TPEF);
    Binary* origTPEF = BinaryReader::readBinary(tpefFile);

    // create POM
    ADFSerializer machineReader;
    machineReader.setSourceFile(PARALLEL_ADF);
    
    mach_ = machineReader.readMachine();
    
    TPEFProgramFactory progFactory1(*origTPEF, *mach_);
    firstRead_ = progFactory1.build();
    
    // copy POM to another
    copied_ = firstRead_->copy(); 
    
    // write copied POM back to TPEF and reread it to POM
    ProgramWriter progWriter(*copied_);
    Binary* newBin = progWriter.createBinary();
                        
    // read copied POM back
    TPEFProgramFactory progFactory2(*newBin, *mach_);
    secondRead_ = progFactory2.build();
    
    delete origTPEF;
    delete newBin;
}

DataMemoryTest::~DataMemoryTest() {
    delete firstRead_;
    delete copied_;
    delete secondRead_;
    delete mach_;
}

/**
 * Called before each test.
 */
void
DataMemoryTest::setUp() {
}

/**
 * Called after each test.
 */
void
DataMemoryTest::tearDown() {
}

/**
 * Check that copied POM matches with original.
 */
void 
DataMemoryTest::testCopying() {
    comparePrograms(*firstRead_, *copied_);
}

/**
 * Check that POM that has been written to TPEF and read back matches
 * with original.
 */
void 
DataMemoryTest::testProgramReaderAndWriter() {
    comparePrograms(*copied_, *secondRead_);
}

/**
 * Checks that getting data definition by address and sorting data definitions 
 * on fly works as they should.
 */
void 
DataMemoryTest::testDataMemoryRequestsAndOrganizing() {

    DataMemory testMem(NullAddressSpace::instance());

    DataDefinition* testDef1 = new DataDefinition(
        Address(10, NullAddressSpace::instance()), 10, false);
    DataDefinition* testDef2 = new DataDefinition(
        Address(20, NullAddressSpace::instance()), 10, false);
    DataDefinition* testDef3 = new DataDefinition(
        Address(30, NullAddressSpace::instance()), 10, false);
    DataDefinition* testDef4 = new DataDefinition(
        Address(40, NullAddressSpace::instance()), 10, false);
    DataDefinition* testDef5 = new DataDefinition(
        Address(50, NullAddressSpace::instance()), 10, false);
    DataDefinition* testDef6 = new DataDefinition(
        Address(60, NullAddressSpace::instance()), 10, false);
    DataDefinition* testDef7 = new DataDefinition(
        Address(70, NullAddressSpace::instance()), 10, false);
    DataDefinition* testDef8 = new DataDefinition(
        Address(80, NullAddressSpace::instance()), 10, false);
    DataDefinition* testDef9 = new DataDefinition(
        Address(90, NullAddressSpace::instance()), 10, false);
    DataDefinition* testDef10 = new DataDefinition(
        Address(100, NullAddressSpace::instance()), 10, false);

    // add definitions to memory in random order
    testMem.addDataDefinition(testDef2);
    testMem.addDataDefinition(testDef10);
    testMem.addDataDefinition(testDef7);
    testMem.addDataDefinition(testDef5);
    testMem.addDataDefinition(testDef6);
    testMem.addDataDefinition(testDef1);
    testMem.addDataDefinition(testDef4);
    testMem.addDataDefinition(testDef9);
    testMem.addDataDefinition(testDef3);
    testMem.addDataDefinition(testDef8);

    TS_ASSERT_EQUALS(
        &testMem.dataDefinition(Address(10, NullAddressSpace::instance())), 
        testDef1);
                     
    TS_ASSERT_EQUALS(
        &testMem.dataDefinition(Address(21, NullAddressSpace::instance())), 
        testDef2);
    
    TS_ASSERT_EQUALS(
        &testMem.dataDefinition(Address(32,NullAddressSpace::instance())), 
        testDef3);

    TS_ASSERT_EQUALS(
        &testMem.dataDefinition(Address(43,NullAddressSpace::instance())), 
        testDef4);

    TS_ASSERT_EQUALS(
        &testMem.dataDefinition(Address(54,NullAddressSpace::instance())), 
        testDef5);

    TS_ASSERT_EQUALS(
        &testMem.dataDefinition(Address(65,NullAddressSpace::instance())), 
        testDef6);

    TS_ASSERT_EQUALS(
        &testMem.dataDefinition(Address(76,NullAddressSpace::instance())), 
        testDef7);

    TS_ASSERT_EQUALS(
        &testMem.dataDefinition(Address(87,NullAddressSpace::instance())), 
        testDef8);

    TS_ASSERT_EQUALS(
        &testMem.dataDefinition(Address(98,NullAddressSpace::instance())), 
        testDef9);

    TS_ASSERT_EQUALS(
        &testMem.dataDefinition(Address(109,NullAddressSpace::instance())),
        testDef10);

}

/**
 * Tests that initialization data in DataMemory are written correctly to
 * a TPEF DataSection.
 */
void
DataMemoryTest::testDataMemoryWritingToTPEF() {

    Program testProgram(*mach_->controlUnit()->addressSpace());
    const AddressSpace& aSpace = 
        *mach_->addressSpaceNavigator().item("dmem8b");
    DataMemory* dmem8b = new DataMemory(aSpace);

    MinimumAddressableUnit data1[1], data2[1];
    data1[0] = 10;
    data2[0] = static_cast<MinimumAddressableUnit>(-10);

    DataDefinition* testDef1 = 
        new DataDefinition(Address(0, aSpace), 1, false, data1);
    DataDefinition* testDef2 = 
        new DataDefinition(Address(1, aSpace), 1, false, data2);

    dmem8b->addDataDefinition(testDef1);
    dmem8b->addDataDefinition(testDef2);

    testProgram.addDataMemory(dmem8b);

    ProgramWriter progWriter(testProgram);
    Binary* newBin = NULL;
    TS_ASSERT_THROWS_NOTHING(newBin = progWriter.createBinary());

    DataSection* dsect = dynamic_cast<DataSection*>(
        newBin->section(Section::ST_DATA, 0));

    TS_ASSERT_DIFFERS(dsect, static_cast<DataSection*>(NULL));

    int number = static_cast<int>(dsect->byte(1));

    TS_ASSERT_EQUALS(number, 246);

    delete newBin;
    newBin = NULL;
}

/**
 * Compares that two programs are quite much the same.
 *
 * Currently checks:
 * - That moves refers to same machine resources and same addresses.
 */
void 
DataMemoryTest::comparePrograms(Program& prog1, Program& prog2) {

    std::vector<unsigned long> mem1;
    std::vector<unsigned long> mem2;
    
    // ---------------- Checking data memories    
    TS_ASSERT_EQUALS(prog1.dataMemoryCount(), prog2.dataMemoryCount());

    for (int i = 0; i < prog1.dataMemoryCount(); i++) {
        DataMemory& p1Mem = prog1.dataMemory(i);
        DataMemory& p2Mem = prog2.dataMemory(i);
        
        // create image of first mem
        for (int j = 0; j < p1Mem.dataDefinitionCount(); j++) { 
            DataDefinition& p1Def = p1Mem.dataDefinition(j);
            
            for (int k = 0; k < p1Def.size(); k++) {
                
                // -1 means uninitialized data
                unsigned long pushValue = static_cast<unsigned long>(-1);
                
                if (p1Def.isInitialized()) {
                    pushValue = p1Def.MAU(k);
                    
                    // first bit to 1 for indicating an address
                    if (p1Def.isAddress()) {
                        pushValue = pushValue & (1 << 31);
                    }                    
                }

                mem1.push_back(pushValue);
            }
        }        

        // create image of secong mem
        for (int j = 0; j < p2Mem.dataDefinitionCount(); j++) { 
            DataDefinition& p2Def = p2Mem.dataDefinition(j);
            
            for (int k = 0; k < p2Def.size(); k++) {
                
                // -1 means uninitialized data
                unsigned long pushValue = static_cast<unsigned long>(-1);
                
                if (p2Def.isInitialized()) {
                    pushValue = p2Def.MAU(k);
                    
                    // first bit to 1 for indicating an address
                    if (p2Def.isAddress()) {
                        pushValue = pushValue & (1 << 31);
                    }                    
                }

                mem2.push_back(pushValue);
            }
        }
        
        TS_ASSERT_EQUALS(mem1.size(), mem2.size());
        for (unsigned j = 0; j < mem1.size(); j++) {
            TS_ASSERT_EQUALS(mem1[j], mem2[j]);
        }
    }

    // ---------------- Checking code
    Instruction *curIns1 = &prog1.firstInstruction();
    Instruction* curIns2 = &prog2.firstInstruction();
    
    // NOTE: this loop presumes that there is more than one instruction in program
    while (curIns1 != &prog1.lastInstruction() && 
           curIns2 != &prog2.lastInstruction()) {
        
        // Check size
        TS_ASSERT_EQUALS(curIns1->size(), curIns2->size());

        // Check address
        TS_ASSERT_EQUALS(curIns1->address().location(), 
                         curIns2->address().location());

        // Check instr template
        TS_ASSERT_EQUALS(&curIns1->instructionTemplate(), 
                         &curIns2->instructionTemplate());
                
        // Check moves (currently checks basically only source addresses)
        TS_ASSERT_EQUALS(curIns1->moveCount(), curIns2->moveCount());
        for (int i = 0; i < curIns1->moveCount(); i++) {
            Move& p1Move = curIns1->move(i);
            Move& p2Move = curIns2->move(i);
             
            TS_ASSERT_EQUALS(p1Move.source().isAddress(), p2Move.source().isAddress());
            if (p1Move.source().isAddress()) {
                TS_ASSERT_EQUALS(p1Move.source().address().location(), 
                                 p2Move.source().address().location());            
            }
            
            TS_ASSERT_EQUALS(p1Move.source().isInstructionAddress(), 
                             p2Move.source().isInstructionAddress());

            if (p1Move.source().isInstructionAddress()) {
                TS_ASSERT_EQUALS(
                    p1Move.source().instructionReference().instruction().address().
                    location(),
                    p2Move.source().instructionReference().instruction().address().
                    location());
            }
            
            // TODO: write compareTerminal() function... 
            // compares sources etc. by machine resource addresses
        }

        // Check immediates
        TS_ASSERT_EQUALS(curIns1->immediateCount(), curIns2->immediateCount());
        for (int i = 0; i < curIns1->immediateCount(); i++) {
            Immediate& p1Imm = curIns1->immediate(i);
            Immediate& p2Imm = curIns2->immediate(i);
            
            TS_ASSERT_EQUALS(p1Imm.value().isAddress(), p2Imm.value().isAddress());
            if (p1Imm.value().isAddress()) {
                TS_ASSERT_EQUALS(p1Imm.value().address().location(), 
                                 p2Imm.value().address().location());            
            }
            
            TS_ASSERT_EQUALS(p1Imm.value().isInstructionAddress(), 
                             p2Imm.value().isInstructionAddress());

            if (p1Imm.value().isInstructionAddress()) {
                TS_ASSERT_EQUALS(
                    p1Imm.value().instructionReference().instruction().address().
                    location(),
                    p2Imm.value().instructionReference().instruction().address().
                    location());
            }
            
        }

        curIns1 = &prog1.nextInstruction(*curIns1);
        curIns2 = &prog2.nextInstruction(*curIns2);        
    } 
}

#endif
