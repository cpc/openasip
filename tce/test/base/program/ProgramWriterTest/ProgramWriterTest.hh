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
 * @file ProgramWriter.hh
 *
 * A test suite for ProgramWriter class.
 *
 * @author Mikael Lepistö 2005 (mikael.lepisto-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef PROGRAM_WRITER_TEST_HH
#define PROGRAM_WRITER_TEST_HH

#include <string>
#include <TestSuite.h>

#include "OperationPool.hh"
#include "BinaryReader.hh"
#include "TPEFProgramFactory.hh"
#include "ProgramWriter.hh"
#include "TPEFWriter.hh"

#include "TPEFDisassembler.hh"
#include "POMDisassembler.hh"
#include "DisassemblyInstruction.hh"
#include "DataMemory.hh"
#include "Machine.hh"
#include "ADFSerializer.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "TerminalAddress.hh"
#include "TerminalRegister.hh"
#include "UnboundedRegisterFile.hh"
#include "Procedure.hh"
#include "DataDefinition.hh"
#include "ProgramAnnotation.hh"
#include "CodeSection.hh"
#include "InstructionElement.hh"
#include "Move.hh"
#include "UniversalMachine.hh"
#include "BinaryStream.hh"

using namespace TTAProgram;
using namespace TTAMachine;
using namespace TPEF;

class ProgramWriterTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testSequentialFromAOut();
    void testParallelFromTPEF();
    void testCreateSequential();
    void testCreateSequential2();
    void testAnnotationReadingAndWriting();
};

const std::string LINKED_A_OUT = "data/linked_binary.aout";
const std::string PARALLEL_TPEF = "data/worm.tpef";
const std::string PARALLEL_ADF = "data/worm.adf";
const std::string NEW_SEQUENTIAL = "data/sequential.tpf";
const std::string NEW_SEQUENTIAL2 = "data/sequential2.tpf";
const std::string NEW_ANNOTATED_TPEF = "data/annotated.tpf";

/**
 * Called before each test.
 */
void
ProgramWriterTest::setUp() {
}

/**
 * Called after each test.
 */
void
ProgramWriterTest::tearDown() {
}

/**
 * Tests creating POM out of a.out file.
 */
void
ProgramWriterTest::testSequentialFromAOut() {
    // read a.out
    BinaryStream aOutFile(LINKED_A_OUT);

    Binary* origBin = BinaryReader::readBinary(aOutFile);
    
    TPEFProgramFactory progFactory(*origBin, &UniversalMachine::instance());
    
    Program* prog = progFactory.build();
    
    ProgramWriter progWriter(*prog);

    Binary* newBin = progWriter.createBinary();

    TPEFDisassembler origDisasm(*origBin);
    TPEFDisassembler newDisasm(*newBin);

    TS_ASSERT_EQUALS(
        origDisasm.startAddress(), newDisasm.startAddress());

    TS_ASSERT_EQUALS(
        origDisasm.instructionCount(), newDisasm.instructionCount());

    // write new to program to disk
    BinaryStream output("data/aout_output.tpef");
    TPEFWriter::instance().writeBinary(output, newBin);

    delete origBin;
    origBin = NULL;
    delete newBin;
    newBin = NULL;
    delete prog;
    prog = NULL;
}

/**
 * Tests creating POM out of parallel tpef.
 */
void
ProgramWriterTest::testParallelFromTPEF() {

    // read a.out
    BinaryStream tpefFile(PARALLEL_TPEF);

    Binary* origBin = BinaryReader::readBinary(tpefFile);

    Machine* mach = NULL;
    ADFSerializer machineReader;
    machineReader.setSourceFile(PARALLEL_ADF);

    try {
        mach = machineReader.readMachine();
    } catch ( ... ) {
    }

    TPEFProgramFactory progFactory(*origBin, *mach);

    Program* prog = progFactory.build();

    ProgramWriter progWriter(*prog);

    Binary* newBin = progWriter.createBinary();
    // write new to program to disk
    BinaryStream output("data/worm_output.tpef");
    TPEFWriter::instance().writeBinary(output, newBin);

    delete origBin;
    origBin = NULL;
    delete newBin;
    newBin = NULL;
    delete prog;
    prog = NULL;

}


/**
 * Tests creating a sequential TPEF using ProgramWriter.
 */
void
ProgramWriterTest::testCreateSequential() {
    
    UniversalMachine* uMach = &UniversalMachine::instance();
    
    DataMemory* dataMemory = 
        new DataMemory(uMach->dataAddressSpace());

    Program seqProgram(uMach->instructionAddressSpace());
    seqProgram.addDataMemory(dataMemory);

    dataMemory->addDataDefinition(
        new DataDefinition(Address(12, uMach->dataAddressSpace()), 1, false));

    Instruction* instr = new Instruction();

    // create a reference to address 12 in data memory
    TerminalAddress* srcTerminal = 
        new TerminalAddress(SimValue(12, 32), uMach->dataAddressSpace());

    // move it to a register r13
    TerminalRegister* dstTerminal =
        new TerminalRegister(*uMach->integerRegisterFile().port(1), 13);
            
    instr->addMove(std::make_shared<Move>(srcTerminal, dstTerminal, uMach->universalBus()));

    Procedure* procedure = 
        new Procedure("main", uMach->instructionAddressSpace());
    TS_ASSERT_THROWS_NOTHING(seqProgram.addProcedure(procedure));
    TS_ASSERT_THROWS_NOTHING(seqProgram.addInstruction(instr));

    // write the program to a TPEF
    ProgramWriter* writer = NULL;
    TS_ASSERT_THROWS_NOTHING(writer = new ProgramWriter(seqProgram));
    Binary* binary = NULL;  
    TS_ASSERT_THROWS_NOTHING(binary = writer->createBinary());
    
    BinaryStream* output = new BinaryStream(NEW_SEQUENTIAL);

    TS_ASSERT_THROWS_NOTHING(
        TPEFWriter::instance().writeBinary(*output, binary));

    delete writer;
    writer = NULL;
    delete binary;
    binary = NULL;
    delete output; // should force flushing the BinaryStream
    output = NULL; 

    // load the program back and check that it looks alright

    BinaryStream input(NEW_SEQUENTIAL);
    Binary* writtenBinary = BinaryReader::readBinary(input);   

    TPEFProgramFactory progFactory(*writtenBinary, uMach);   
    Program* loadedProg = progFactory.build();

    TS_ASSERT_EQUALS(loadedProg->procedureCount(), 1);
    TS_ASSERT_EQUALS(loadedProg->procedure(0).name(), "main");
    TS_ASSERT_EQUALS(loadedProg->procedure(0).instructionAt(0).moveCount(), 1);
    TS_ASSERT_EQUALS(loadedProg->procedure(0).instructionAt(0).move(0).
                     source().value().width(), 32);
    TS_ASSERT_EQUALS(loadedProg->procedure(0).instructionAt(0).move(0).
                     source().value().unsignedValue(), 
                     static_cast<unsigned>(12));
    TS_ASSERT_EQUALS(loadedProg->procedure(0).instructionAt(0).move(0).
                     source().value().intValue(), 12);

    delete writtenBinary;
    writtenBinary = NULL;
    delete loadedProg;
    loadedProg = NULL;
}

/**
 * Another test for creating a sequential TPEF using ProgramWriter.
 */
void
ProgramWriterTest::testCreateSequential2() {

    OperationPool operationPool;    
    UniversalMachine& universalMachine = UniversalMachine::instance();

    TTAMachine::AddressSpace& dataAddressSpace =
        universalMachine.dataAddressSpace();

    DataMemory* dataMemory1 = new TTAProgram::DataMemory(dataAddressSpace);

    Program program1(universalMachine.instructionAddressSpace());

    program1.addDataMemory(dataMemory1);

    SimValue svAddress(0,32);

    TTAProgram::DataDefinition* dataDef = 
        new TTAProgram::DataDefinition(
            TTAProgram::Address(0, universalMachine.dataAddressSpace()), 1,
            false);

    dataMemory1->addDataDefinition(dataDef);

    TTAProgram::Instruction* loadInst = new TTAProgram::Instruction();

    TTAProgram::TerminalAddress* termAddr = 
        new TTAProgram::TerminalAddress(
            svAddress, dataMemory1->addressSpace());

    // move it to a register r12
    TTAProgram::TerminalRegister* dstTerminal =
        new TTAProgram::TerminalRegister(
            *universalMachine.integerRegisterFile().port(1), 12);


    loadInst->addMove(
        std::make_shared<TTAProgram::Move>(
            termAddr, dstTerminal, universalMachine.universalBus()));


    TTAProgram::Procedure* proc = 
        new TTAProgram::Procedure(
            "main", universalMachine.instructionAddressSpace());

    program1.addProcedure(proc);

    program1.addInstruction(loadInst);

    ProgramWriter* writer = NULL;
    TS_ASSERT_THROWS_NOTHING(writer = new ProgramWriter(program1));
    Binary* binary = NULL;  
    TS_ASSERT_THROWS_NOTHING(binary = writer->createBinary());
    
    BinaryStream* output = new BinaryStream(NEW_SEQUENTIAL2);

    TS_ASSERT_THROWS_NOTHING(
        TPEFWriter::instance().writeBinary(*output, binary));
}

/**
 * Tests that instruction annotations are written and read to/from
 * TPEF correctly in the TPEF to POM to TPEF conversion chain.
 */
void
ProgramWriterTest::testAnnotationReadingAndWriting() {

    typedef TTAProgram::ProgramAnnotation::Id annotationId;
    UniversalMachine* universalMachine = &UniversalMachine::instance();

    TTAMachine::AddressSpace& dataAddressSpace =
        universalMachine->dataAddressSpace();

    DataMemory* dataMemory1 = new TTAProgram::DataMemory(dataAddressSpace);

    Program program1(universalMachine->instructionAddressSpace());

    program1.addDataMemory(dataMemory1);

    SimValue svAddress(0,32);

    TTAProgram::DataDefinition* dataDef = 
        new TTAProgram::DataDefinition(
            TTAProgram::Address(0, universalMachine->dataAddressSpace()), 1,
            false);

    dataMemory1->addDataDefinition(dataDef);

    TTAProgram::Instruction* loadInst = new TTAProgram::Instruction();


    TTAProgram::TerminalAddress* termAddr = 
        new TTAProgram::TerminalAddress(
            svAddress, dataMemory1->addressSpace());

    // test the immediate annotation support
    TTAProgram::ProgramAnnotation immediateAnnotation1(
        (annotationId)0xFF0000, "hello annotation world!");

    TTAProgram::ProgramAnnotation immediateAnnotation2(
        (annotationId)0xFF0000, "hello annotation world again!");

    TTAProgram::ProgramAnnotation immediateAnnotation3(
        (annotationId)0xFF0001, "hello again with different Id");

    termAddr->addAnnotation(immediateAnnotation1);
    termAddr->addAnnotation(immediateAnnotation2);
    termAddr->addAnnotation(immediateAnnotation3);

    TS_ASSERT_EQUALS(
        termAddr->annotation(0, (annotationId)0xFF0001).stringValue(),
        "hello again with different Id");

    TS_ASSERT_EQUALS(termAddr->annotationCount(), 3);
    TS_ASSERT_EQUALS(termAddr->annotationCount((annotationId)0xFF0000), 2);
    TS_ASSERT_EQUALS(termAddr->annotationCount((annotationId)0xFF0001), 1);
    TS_ASSERT(termAddr->hasAnnotations((annotationId)0xFF0000));
    TS_ASSERT(!termAddr->hasAnnotations((annotationId)0xEFEFEF));

    // move it to a register r12
    TTAProgram::TerminalRegister* dstTerminal =
        new TTAProgram::TerminalRegister(
            *universalMachine->integerRegisterFile().port(1), 12);

    auto aMove = std::make_shared<TTAProgram::Move>(
        termAddr, dstTerminal, universalMachine->universalBus());

    // test the move annotation support
    TTAProgram::ProgramAnnotation moveAnnotation1(
        (annotationId)0xFF00FF, "hello move annotation world!");

    TTAProgram::ProgramAnnotation moveAnnotation2(
        (annotationId)0xFF00FF, "hello move annotation world again!");

    TTAProgram::ProgramAnnotation moveAnnotation3(
        (annotationId)0xFFFF01, "hello again with different Id");

    aMove->addAnnotation(moveAnnotation1);
    aMove->addAnnotation(moveAnnotation2);
    aMove->addAnnotation(moveAnnotation3);

    TS_ASSERT_EQUALS(aMove->annotationCount(), 3);
    TS_ASSERT_EQUALS(aMove->annotationCount((annotationId)0xFF00FF), 2);
    TS_ASSERT_EQUALS(aMove->annotationCount((annotationId)0xFFFF01), 1);
    TS_ASSERT(aMove->hasAnnotations((annotationId)0xFF00FF));
    TS_ASSERT(!aMove->hasAnnotations((annotationId)0xEFEFEF));

    loadInst->addMove(aMove);

    TTAProgram::Procedure* proc = 
        new TTAProgram::Procedure(
            "main", universalMachine->instructionAddressSpace());

    program1.addProcedure(proc);

    program1.addInstruction(loadInst);

    ProgramWriter* writer = NULL;
    TS_ASSERT_THROWS_NOTHING(writer = new ProgramWriter(program1));
    Binary* binary = NULL;  
    TS_ASSERT_THROWS_NOTHING(binary = writer->createBinary());
    
    // check that the annotations got into the TPEF::Binary object
    unsigned int result = 3;
    TS_ASSERT_EQUALS(
        dynamic_cast<CodeSection*>(binary->section(Section::ST_CODE, 0))->
        element(0)->annotationCount(), result);

    BinaryStream* output = new BinaryStream(NEW_ANNOTATED_TPEF);

    TS_ASSERT_THROWS_NOTHING(
        TPEFWriter::instance().writeBinary(*output, binary));

    delete output; // force flush of the BinaryStream
    output = NULL;

    // load the program back and check that the annotations are still
    // there

    BinaryStream input(NEW_ANNOTATED_TPEF);
    Binary* writtenBinary = BinaryReader::readBinary(input);   

    TPEFProgramFactory progFactory(*writtenBinary, universalMachine);
    Program* loadedProg = progFactory.build();

    TTAProgram::Move& theMove = 
        loadedProg->procedure(0).instructionAt(0).move(0);

    TTAProgram::TerminalAddress& term = 
        dynamic_cast<TerminalAddress&>(theMove.source());
    
    TS_ASSERT_EQUALS(term.annotationCount(), 3);
    TS_ASSERT_EQUALS(term.annotationCount((annotationId)0xFF0000), 2);
    TS_ASSERT_EQUALS(term.annotationCount((annotationId)0xFF0001), 1);
    TS_ASSERT(term.hasAnnotations((annotationId)0xFF0000));
    TS_ASSERT(!term.hasAnnotations((annotationId)0xEFEFEF));

    TS_ASSERT_EQUALS(theMove.annotationCount(), 3);
    TS_ASSERT_EQUALS(theMove.annotationCount((annotationId)0xFF00FF), 2);
    TS_ASSERT_EQUALS(theMove.annotationCount((annotationId)0xFFFF01), 1);
    TS_ASSERT(theMove.hasAnnotations((annotationId)0xFF00FF));
    TS_ASSERT(!theMove.hasAnnotations((annotationId)0xEFEFEF));
}

#endif
