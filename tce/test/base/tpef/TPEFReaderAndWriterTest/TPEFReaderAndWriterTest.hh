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
 * @file TPEFReaderAndWriterTest.hh
 *
 * A test suite for all TPEFReader and TPEFWriter classes.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 */

#ifndef TTA_TPEF_READER_AND_WRITER_TEST_HH
#define TTA_TPEF_READER_AND_WRITER_TEST_HH

#include <list>
#include <TestSuite.h>

#include "BinaryStream.hh"
#include "Binary.hh"
#include "Exception.hh"

// Used sections...
#include "Section.hh"
#include "CodeSection.hh"
#include "DataSection.hh"
#include "UDataSection.hh"
#include "StringSection.hh"
#include "ASpaceSection.hh"
#include "RelocSection.hh"
#include "NullSection.hh"
#include "SymbolSection.hh"
#include "ResourceSection.hh"
#include "LineNumSection.hh"

// and elements...
#include "ASpaceElement.hh"
#include "MoveElement.hh"
#include "ImmediateElement.hh"
#include "RelocElement.hh"
#include "ResourceElement.hh"
#include "LineNumElement.hh"
#include "LineNumProcedure.hh"
#include "SymbolElement.hh"
#include "NoTypeSymElement.hh"
#include "CodeSymElement.hh"
#include "DataSymElement.hh"
#include "FileSymElement.hh"
#include "SectionSymElement.hh"

#include "BinaryReader.hh"
#include "BinaryWriter.hh"
#include "TPEFWriter.hh"

using namespace TPEF;
using ReferenceManager::SafePointer;

///////////////////////////////////////////////////////////////////////////////
// TPEFReaderAndWriterTest declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Tests for all TPEFReader and TPEFWriter classes.
 */
class TPEFReaderAndWriterTest : public CxxTest::TestSuite {
public:
    void testWritingAndReadingClassHierarchy();

    void setUp();
    void tearDown();

private:
    void initHierarchy();

    void compareSections(Section* first, Section* second);
  
    void compareInstructionElements(InstructionElement *instr1,
                                    InstructionElement *instr2);
  
    void compareSymbolElements(SymbolElement *sym1,
                               SymbolElement *sym2);

    void compareASpaceElements(ASpaceElement *aSpace1,
                               ASpaceElement *aSpace2);    

    void compareRelocElements(RelocElement *reloc1,
                              RelocElement *reloc2);    

    /// Test binary.
    static Binary* origBinary_;
};


///////////////////////////////////////////////////////////////////////////////
// TPEFReaderAndWriterTest implementation
///////////////////////////////////////////////////////////////////////////////

Binary* TPEFReaderAndWriterTest::origBinary_ = NULL;

/**
 * Reads test stream and checks that it was read right..
 */
void
TPEFReaderAndWriterTest::testWritingAndReadingClassHierarchy() {

    BinaryStream stream("data/temp.bin");

    TPEFWriter::instance().writeBinary(stream, origBinary_);
    
    stream.setReadPosition(0);
    Binary* readBinary = BinaryReader::readBinary(stream);

    TS_ASSERT_EQUALS(origBinary_->type(), readBinary->type());

    TS_ASSERT_EQUALS(origBinary_->arch(), readBinary->arch());

    // name field was used as an id of section
    TS_ASSERT_EQUALS(
        origBinary_->strings()->name()->offset(),
        readBinary->strings()->name()->offset());

    for (Word origIter = 0; origIter < origBinary_->sectionCount(); 
         origIter++) {

        Word readIter = 0;

        // name field was used as an identification for sections
        while (readIter < readBinary->sectionCount()) {

            if (origBinary_->section(origIter)->name()->offset() ==
                readBinary->section(readIter)->name()->offset()) {
		
                compareSections(
                    origBinary_->section(origIter), 
                    readBinary->section(readIter));
                break;
            }
            readIter++;
        }
        
        // there was section found for each original section
        TS_ASSERT_DIFFERS(readIter, readBinary->sectionCount());
    }
    delete readBinary;
    readBinary = NULL;
    delete origBinary_;
    origBinary_ = NULL;
}

/**
 * Initialization code of each test
 */
void
TPEFReaderAndWriterTest::setUp() {
    initHierarchy();
}

/**
 * Cleanup code of each test.
 */
void
TPEFReaderAndWriterTest::tearDown() {
}

/**
 * Creates initial class hierarchy for test to write and read.
 */
void
TPEFReaderAndWriterTest::initHierarchy() {
    
    static bool alreadyInitialized = false;
    if (!alreadyInitialized) {
        // unique id for recognizing sections after writing and reading
        Word kindOfSectionId = 1;

        Section* newSection = NULL;

        // null section
        newSection = Section::createSection(Section::ST_NULL);
        NullSection* nullSection = 
            dynamic_cast<NullSection*>(newSection);
        TS_ASSERT_DIFFERS(nullSection, static_cast<Section*>(NULL));

        // create aspace section with three elements *************************
        newSection = Section::createSection(Section::ST_ADDRSP);
        ASpaceSection* aSpaceSection =
            dynamic_cast<ASpaceSection*>(newSection);
        TS_ASSERT_DIFFERS(aSpaceSection, static_cast<Section*>(NULL));
	
        ASpaceElement* undefASpace = new ASpaceElement();

        aSpaceSection->setUndefinedASpace(undefASpace);

        ASpaceElement* aSpaceElement1 = new ASpaceElement();
        aSpaceElement1->setMAU(8);
        aSpaceElement1->setAlign(1);
        aSpaceElement1->setWordSize(4);
        
        ASpaceElement* aSpaceElement2 = new ASpaceElement();
        aSpaceElement2->setMAU(4);
        aSpaceElement2->setAlign(1);
        aSpaceElement2->setWordSize(2);

        ASpaceElement* aSpaceElement3 = new ASpaceElement();
        aSpaceElement3->setMAU(16);
        aSpaceElement3->setAlign(100);
        aSpaceElement3->setWordSize(1);

        aSpaceSection->addElement(undefASpace);
        aSpaceSection->addElement(aSpaceElement1);
        aSpaceSection->addElement(aSpaceElement2);
        aSpaceSection->addElement(aSpaceElement3);

        // create at least 2 string sections. *******************************
        newSection = Section::createSection(Section::ST_STRTAB);
        StringSection* stringSection1 =
            dynamic_cast<StringSection*>(newSection);
        TS_ASSERT_DIFFERS(stringSection1, static_cast<Section*>(NULL));

        stringSection1->addByte(0);
        stringSection1->addByte('a');
        stringSection1->addByte('b');
        stringSection1->addByte('c');
        stringSection1->addByte('d');
        stringSection1->addByte('e');
        stringSection1->addByte('f');
        stringSection1->addByte('g');
        stringSection1->addByte('h');
        stringSection1->addByte('i');
        stringSection1->addByte('j');
        stringSection1->addByte(0);
        stringSection1->addByte('k');
        stringSection1->addByte('l');
        stringSection1->addByte('m');
        stringSection1->addByte('n');
        stringSection1->addByte('o');
        stringSection1->addByte(0);

        newSection = Section::createSection(Section::ST_STRTAB);
        StringSection* stringSection2 =
            dynamic_cast<StringSection*>(newSection);
        TS_ASSERT_DIFFERS(stringSection2, static_cast<Section*>(NULL));

        stringSection2->addByte(0);
        stringSection2->addByte('J');
        stringSection2->addByte('u');
        stringSection2->addByte('s');
        stringSection2->addByte('t');
        stringSection2->addByte('-');
        stringSection2->addByte('s');
        stringSection2->addByte('o');
        stringSection2->addByte('m');
        stringSection2->addByte('e');
        stringSection2->addByte('-');
        stringSection2->addByte('t');
        stringSection2->addByte('e');
        stringSection2->addByte('x');
        stringSection2->addByte('t');
        stringSection2->addByte('-');
        stringSection2->addByte('f');
        stringSection2->addByte('o');
        stringSection2->addByte('r');
        stringSection2->addByte('-');
        stringSection2->addByte('t');
        stringSection2->addByte('e');
        stringSection2->addByte('s');
        stringSection2->addByte('t');
        stringSection2->addByte('i');
        stringSection2->addByte('n');
        stringSection2->addByte('g');
        stringSection2->addByte('-');
        stringSection2->addByte('m');
        stringSection2->addByte('u');
        stringSection2->addByte('s');
        stringSection2->addByte('t');
        stringSection2->addByte('-');
        stringSection2->addByte('b');
        stringSection2->addByte('e');
        stringSection2->addByte('-');
        stringSection2->addByte('e');
        stringSection2->addByte('n');
        stringSection2->addByte('o');
        stringSection2->addByte('u');
        stringSection2->addByte('g');
        stringSection2->addByte('h');
        stringSection2->addByte('-');
        stringSection2->addByte('l');
        stringSection2->addByte('o');
        stringSection2->addByte('n');
        stringSection2->addByte('g');
        stringSection2->addByte(0);
	
        newSection = Section::createSection(Section::ST_STRTAB);
        StringSection* strSymSection1 =
            dynamic_cast<StringSection*>(newSection);
        TS_ASSERT_DIFFERS(strSymSection1, static_cast<Section*>(NULL));
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('0');
        strSymSection1->addByte('1');
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('0');
        strSymSection1->addByte('2');
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('0');
        strSymSection1->addByte('3');
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('0');
        strSymSection1->addByte('4');
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('0');
        strSymSection1->addByte('5');
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('0');
        strSymSection1->addByte('6');
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('0');
        strSymSection1->addByte('7');
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('0');
        strSymSection1->addByte('8');
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('0');
        strSymSection1->addByte('9');
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('1');
        strSymSection1->addByte('0');
        strSymSection1->addByte(0);
        strSymSection1->addByte('s');
        strSymSection1->addByte('y');
        strSymSection1->addByte('m');
        strSymSection1->addByte('1');
        strSymSection1->addByte('1');
        strSymSection1->addByte(0);
	
        newSection = Section::createSection(Section::ST_STRTAB);
        StringSection* strSymSection2 =
            dynamic_cast<StringSection*>(newSection);
        TS_ASSERT_DIFFERS(strSymSection2, static_cast<Section*>(NULL));
        strSymSection2->addByte(0);
        strSymSection2->addByte('s');
        strSymSection2->addByte('y');
        strSymSection2->addByte('m');
        strSymSection2->addByte('2');
        strSymSection2->addByte('1');
        strSymSection2->addByte(0);
        strSymSection2->addByte('s');
        strSymSection2->addByte('y');
        strSymSection2->addByte('m');
        strSymSection2->addByte('2');
        strSymSection2->addByte('2');
        strSymSection2->addByte(0);
        strSymSection2->addByte('s');
        strSymSection2->addByte('y');
        strSymSection2->addByte('m');
        strSymSection2->addByte('2');
        strSymSection2->addByte('3');
        strSymSection2->addByte(0);
        strSymSection2->addByte('s');
        strSymSection2->addByte('y');
        strSymSection2->addByte('m');
        strSymSection2->addByte('2');
        strSymSection2->addByte('4');
        strSymSection2->addByte(0);
        strSymSection2->addByte('s');
        strSymSection2->addByte('y');
        strSymSection2->addByte('m');
        strSymSection2->addByte('2');
        strSymSection2->addByte('5');
        strSymSection2->addByte(0);
        strSymSection2->addByte('s');
        strSymSection2->addByte('y');
        strSymSection2->addByte('m');
        strSymSection2->addByte('2');
        strSymSection2->addByte('6');
        strSymSection2->addByte(0);
        strSymSection2->addByte('s');
        strSymSection2->addByte('y');
        strSymSection2->addByte('m');
        strSymSection2->addByte('2');
        strSymSection2->addByte('7');
        strSymSection2->addByte(0);
        strSymSection2->addByte('s');
        strSymSection2->addByte('y');
        strSymSection2->addByte('m');
        strSymSection2->addByte('2');
        strSymSection2->addByte('8');
        strSymSection2->addByte(0);
        strSymSection2->addByte('s');
        strSymSection2->addByte('y');
        strSymSection2->addByte('m');
        strSymSection2->addByte('2');
        strSymSection2->addByte('9');
        strSymSection2->addByte(0);
        strSymSection2->addByte('s');
        strSymSection2->addByte('y');
        strSymSection2->addByte('m');
        strSymSection2->addByte('3');
        strSymSection2->addByte('0');
        strSymSection2->addByte(0);

        // create at least 2 udata sections.*********************************
        newSection = Section::createSection(Section::ST_UDATA);
        UDataSection* uDataSection1 =
            dynamic_cast<UDataSection*>(newSection);
        TS_ASSERT_DIFFERS(uDataSection1, static_cast<Section*>(NULL));
	
        uDataSection1->setDataLength(1507);
	
	
        newSection = Section::createSection(Section::ST_UDATA);
        UDataSection* uDataSection2 =
            dynamic_cast<UDataSection*>(newSection);
        TS_ASSERT_DIFFERS(uDataSection2, static_cast<Section*>(NULL));
	
        uDataSection2->setDataLength(715517);
	
        // create at least 2 data sections.**********************************
        newSection = Section::createSection(Section::ST_DATA);
        DataSection* dataSection1 =
            dynamic_cast<DataSection*>(newSection);
        TS_ASSERT_DIFFERS(dataSection1, static_cast<Section*>(NULL));
	
        newSection->setStartingAddress(0);
       
        // values of data section might change 
        // during writing, because containing of
        // chunks referred from relocation section 
        // might change
        dataSection1->addByte(0x00);
        dataSection1->addByte(0x01);
        dataSection1->addByte(0x02);
        dataSection1->addByte(0x03);
        dataSection1->addByte(0x04);
        dataSection1->addByte(0x05);
        dataSection1->addByte(0x06);
        dataSection1->addByte(0x07);
        dataSection1->addByte(0x08);
        dataSection1->addByte(0x09);
        dataSection1->addByte(0x0a);
        dataSection1->addByte(0x0b);
        dataSection1->addByte(0x0c);
        dataSection1->addByte(0x0d);
        dataSection1->addByte(0x0e);
        dataSection1->addByte(0x0f);
        dataSection1->addByte(0x10);
        dataSection1->addByte(0x11);
        dataSection1->addByte(0x12);
        dataSection1->addByte(0x13);
        dataSection1->addByte(0x15);
        dataSection1->addByte(0x16);
        dataSection1->addByte(0x17);
        dataSection1->addByte(0x18);
        dataSection1->addByte(0x19);
        dataSection1->addByte(0x1a);
        dataSection1->addByte(0x1b);
        dataSection1->addByte(0x1c);
        dataSection1->addByte(0x1d);

        newSection = Section::createSection(Section::ST_DATA);
        DataSection* dataSection2 =
            dynamic_cast<DataSection*>(newSection);
        TS_ASSERT_DIFFERS(dataSection2, static_cast<Section*>(NULL));

        newSection->setStartingAddress(0);

        dataSection2->addByte(0x50);
        dataSection2->addByte(0x60);
        dataSection2->addByte(0x70);
        dataSection2->addByte(0x80);
        dataSection2->addByte(0x90);
        dataSection2->addByte(0xa0);
        dataSection2->addByte(0xb0);
	
	
        // create at least 2 text sections.**********************************
        newSection = Section::createSection(Section::ST_CODE);
        CodeSection* codeSection1 =
            dynamic_cast<CodeSection*>(newSection);
        TS_ASSERT_DIFFERS(codeSection1, static_cast<Section*>(NULL));
	
        newSection->setStartingAddress(0);
	
        MoveElement* move1 = new MoveElement();       
        MoveElement* move2 = new MoveElement();
        MoveElement* move3 = new MoveElement();
        MoveElement* move4 = new MoveElement();
        MoveElement* move5 = new MoveElement();

        move1->setGuardType(MoveElement::MF_RF);      
        move2->setGuardType(MoveElement::MF_UNIT);      
        move3->setGuardType(MoveElement::MF_RF);      
        move4->setGuardType(MoveElement::MF_UNIT);      
        move5->setGuardType(MoveElement::MF_RF);      
	
        move1->setSourceType(MoveElement::MF_IMM);      
        move2->setSourceType(MoveElement::MF_IMM);      
        move3->setSourceType(MoveElement::MF_RF);      
        move4->setSourceType(MoveElement::MF_UNIT);      
        move5->setSourceType(MoveElement::MF_RF);      

        move1->setDestinationType(MoveElement::MF_RF);      
        move2->setDestinationType(MoveElement::MF_UNIT);      
        move3->setDestinationType(MoveElement::MF_RF);      
        move4->setDestinationType(MoveElement::MF_UNIT);      
        move5->setDestinationType(MoveElement::MF_RF);      

        ImmediateElement* imm1 = new ImmediateElement();
        imm1->addByte('F');
        imm1->addByte('i');
        imm1->addByte('r');
        imm1->addByte('s');
        ImmediateElement* imm2 = new ImmediateElement();
        imm2->addByte('1');
        ImmediateElement* imm3 = new ImmediateElement();
        imm3->addByte('1');
        imm3->addByte('2');
        // referenced from relocation section must be 4 bytes
        ImmediateElement* imm4 = new ImmediateElement();
        imm4->addByte('1');
        imm4->addByte('2');
        imm4->addByte('3');
        imm4->addByte('4');
        // referenced from relocation section must be 4 bytes
        ImmediateElement* imm5 = new ImmediateElement();
        imm5->addByte('4');
        imm5->addByte('3');
        imm5->addByte('2');
        imm5->addByte('1');
        // referenced from relocation section must be 4 bytes
        ImmediateElement* imm6 = new ImmediateElement();
        imm6->addByte('0');
        imm6->addByte('0');
        imm6->addByte('0');
        imm6->addByte('0');
        ImmediateElement* imm7 = new ImmediateElement();
        imm7->addByte('L');
        imm7->addByte('a');
        imm7->addByte('a');
        imm7->addByte('a');
        imm7->addByte('a');
        imm7->addByte('a');
        imm7->addByte('a');
        imm7->addByte('a');
        imm7->addByte('a');
        imm7->addByte('S');
        imm7->addByte('S');
        imm7->addByte('S');
        imm7->addByte('S');
        imm7->addByte('S');
        imm7->addByte('t');
        ImmediateElement* imm8 = new ImmediateElement();
        imm8->addByte('L');
        imm8->addByte('a');
        imm8->addByte('S');
        imm8->addByte('t');

        imm1->setBegin(true);
        codeSection1->addElement(imm1);
        codeSection1->addElement(move1);


        newSection = Section::createSection(Section::ST_CODE);
        CodeSection* codeSection2 =
            dynamic_cast<CodeSection*>(newSection);
        TS_ASSERT_DIFFERS(codeSection2, static_cast<Section*>(NULL));

        newSection->setStartingAddress(0);
	
        imm2->setBegin(true);
        codeSection2->addElement(imm2);
        codeSection2->addElement(imm3);
        codeSection2->addElement(move2);
        codeSection2->addElement(imm4);
        move4->setBegin(true);
        codeSection2->addElement(move4);
        codeSection2->addElement(imm5);
        codeSection2->addElement(move3);
        codeSection2->addElement(imm8);
        codeSection2->addElement(imm6);
        codeSection2->addElement(move5);
        codeSection2->addElement(imm7);


        InstructionAnnotation *annotation1 = new InstructionAnnotation(10);
        annotation1->addByte(0);

        InstructionAnnotation *annotation2 = new InstructionAnnotation(3);
        annotation2->addByte(2);
        annotation2->addByte(3);

        InstructionAnnotation *annotation3 = new InstructionAnnotation(1);
        annotation3->addByte(2);
        annotation3->addByte(3);
        annotation3->addByte(2);
        annotation3->addByte(4);
        annotation3->addByte(6);
        annotation3->addByte(3);
        annotation3->addByte(1);
        annotation3->addByte(3);

        InstructionAnnotation *annotation4 = new InstructionAnnotation(5);
	
        imm1->addAnnotation(annotation1);
        move2->addAnnotation(annotation3);
        move1->addAnnotation(annotation2);
        move1->addAnnotation(annotation4);
	
        // create at least 2 symbol table sections.**************************
	
        newSection = Section::createSection(Section::ST_SYMTAB);
        SymbolSection* symbolSection1 =
            dynamic_cast<SymbolSection*>(newSection);
        TS_ASSERT_DIFFERS(symbolSection1, static_cast<Section*>(NULL));	

        newSection = Section::createSection(Section::ST_SYMTAB);
        SymbolSection* symbolSection2 =
            dynamic_cast<SymbolSection*>(newSection);
        TS_ASSERT_DIFFERS(symbolSection2, static_cast<Section*>(NULL));

        newSection = Section::createSection(Section::ST_SYMTAB);
        SymbolSection* symbolSection3 =
            dynamic_cast<SymbolSection*>(newSection);
        TS_ASSERT_DIFFERS(symbolSection3, static_cast<Section*>(NULL));	
        symbolSection3->setFlagNoBits();
        symbolSection3->setLink(strSymSection1);
	

        // to string section which contains symbol strings
        symbolSection1->setLink(strSymSection1);

        // to string section which contains symbol strings
        symbolSection2->setLink(strSymSection2);
	
        NoTypeSymElement *zeroElement1 = new NoTypeSymElement();
        zeroElement1->setName(strSymSection1->chunk(0));
        zeroElement1->setSection(nullSection);

        NoTypeSymElement *zeroElement2 = new NoTypeSymElement();
        zeroElement2->setName(strSymSection1->chunk(0));
        zeroElement2->setSection(nullSection);

        NoTypeSymElement *zeroElement3 = new NoTypeSymElement();
        zeroElement3->setName(strSymSection1->chunk(0));

        NoTypeSymElement *noTypeSym1 = new NoTypeSymElement();
        noTypeSym1->setName(strSymSection1->chunk(1));
        noTypeSym1->setBinding(SymbolElement::STB_WEAK);
        noTypeSym1->setSection(nullSection);
	
        NoTypeSymElement *noTypeSym2 = new NoTypeSymElement();
        noTypeSym2->setName(strSymSection1->chunk(7));
        noTypeSym2->setBinding(SymbolElement::STB_WEAK);
        noTypeSym2->setSection(nullSection);
	
        NoTypeSymElement *noTypeSym3 = new NoTypeSymElement();
        noTypeSym3->setName(strSymSection2->chunk(1));
        noTypeSym3->setBinding(SymbolElement::STB_GLOBAL);
        noTypeSym3->setSection(nullSection);

        NoTypeSymElement *noTypeSym4 = new NoTypeSymElement();
        noTypeSym4->setName(strSymSection2->chunk(7));
        noTypeSym4->setBinding(SymbolElement::STB_LOCAL);
        noTypeSym4->setSection(nullSection);
	
	
        CodeSymElement *codeSym1 = new CodeSymElement();
        codeSym1->setReference(imm1);
        codeSym1->setName(strSymSection1->chunk(13));
        codeSym1->setBinding(SymbolElement::STB_GLOBAL);
        codeSym1->setSection(codeSection1);

        CodeSymElement *codeSym2 = new CodeSymElement();
        codeSym2->setReference(move4);
        codeSym2->setName(strSymSection1->chunk(19));
        codeSym2->setBinding(SymbolElement::STB_WEAK);
        codeSym2->setSection(codeSection2);
	
        CodeSymElement *codeSym3 = new CodeSymElement();
        codeSym3->setReference(imm2);
        codeSym3->setName(strSymSection2->chunk(13));
        codeSym3->setBinding(SymbolElement::STB_GLOBAL);
        codeSym3->setSection(codeSection2);
	
        CodeSymElement *codeSym4 = new CodeSymElement();
        codeSym4->setReference(move3);
        codeSym4->setName(strSymSection2->chunk(19));
        codeSym4->setBinding(SymbolElement::STB_LOCAL);
        codeSym4->setSection(codeSection2);
	
        DataSymElement *dataSym1 = new DataSymElement();
        dataSym1->setReference(dataSection2->chunk(0));
        dataSym1->setSize(2);
        dataSym1->setName(strSymSection1->chunk(25));
        dataSym1->setBinding(SymbolElement::STB_WEAK);
        dataSym1->setSection(dataSection2);

        DataSymElement *dataSym2 = new DataSymElement();
        dataSym2->setReference(dataSection2->chunk(2));
        dataSym2->setSize(1);
        dataSym2->setName(strSymSection1->chunk(31));
        dataSym2->setBinding(SymbolElement::STB_LOCAL);
        dataSym2->setSection(dataSection2);

        DataSymElement *dataSym3 = new DataSymElement();
        dataSym3->setReference(dataSection1->chunk(1));
        dataSym3->setSize(4);
        dataSym3->setName(strSymSection2->chunk(25));
        dataSym3->setBinding(SymbolElement::STB_LOCAL);
        dataSym3->setSection(dataSection1);

        DataSymElement *dataSym4 = new DataSymElement();
        dataSym4->setReference(dataSection2->chunk(4));
        dataSym4->setSize(3);
        dataSym4->setName(strSymSection2->chunk(31));
        dataSym4->setBinding(SymbolElement::STB_GLOBAL);
        dataSym4->setSection(dataSection2);
	
        FileSymElement *fileSym1 = new FileSymElement();
        fileSym1->setName(strSymSection1->chunk(37));
        fileSym1->setBinding(SymbolElement::STB_GLOBAL);
        fileSym1->setSection(nullSection);

        FileSymElement *fileSym2 = new FileSymElement();
        fileSym2->setName(strSymSection1->chunk(43));
        fileSym2->setBinding(SymbolElement::STB_WEAK);
        fileSym2->setSection(nullSection);

        FileSymElement *fileSym3 = new FileSymElement();
        fileSym3->setName(strSymSection2->chunk(37));
        fileSym3->setBinding(SymbolElement::STB_WEAK);
        fileSym3->setSection(nullSection);

        FileSymElement *fileSym4 = new FileSymElement();
        fileSym4->setName(strSymSection2->chunk(43));
        fileSym4->setBinding(SymbolElement::STB_LOCAL);
        fileSym4->setSection(nullSection);
	
        SectionSymElement *sectionSym1 = new SectionSymElement();
        sectionSym1->setValue(100);
        sectionSym1->setName(strSymSection1->chunk(49));
        sectionSym1->setBinding(SymbolElement::STB_LOCAL);
        sectionSym1->setSection(codeSection1);

        SectionSymElement *sectionSym2 = new SectionSymElement();
        sectionSym2->setValue(200);
        sectionSym2->setName(strSymSection1->chunk(55));
        sectionSym2->setBinding(SymbolElement::STB_GLOBAL);
        sectionSym2->setSection(codeSection2);

        SectionSymElement *sectionSym3 = new SectionSymElement();
        sectionSym3->setValue(300);
        sectionSym3->setName(strSymSection2->chunk(49));
        sectionSym3->setBinding(SymbolElement::STB_WEAK);
        sectionSym3->setSection(dataSection1);

        SectionSymElement *sectionSym4 = new SectionSymElement();
        sectionSym4->setValue(400);
        sectionSym4->setName(strSymSection2->chunk(55));
        sectionSym4->setBinding(SymbolElement::STB_WEAK);
        sectionSym4->setSection(dataSection2);

        symbolSection1->addElement(zeroElement1);
        symbolSection1->addElement(codeSym1);
        symbolSection1->addElement(fileSym1);
        symbolSection1->addElement(sectionSym1);
        symbolSection1->addElement(fileSym2);
        symbolSection1->addElement(dataSym1);
        symbolSection1->addElement(dataSym2);
        symbolSection1->addElement(noTypeSym1);
        symbolSection1->addElement(sectionSym2);
        symbolSection1->addElement(noTypeSym2);

        symbolSection2->addElement(zeroElement2);
        symbolSection2->addElement(codeSym3);
        symbolSection2->addElement(fileSym3);
        symbolSection2->addElement(sectionSym3);
        symbolSection2->addElement(fileSym4);
        symbolSection2->addElement(dataSym3);
        symbolSection2->addElement(codeSym2);
        symbolSection2->addElement(codeSym4);
        symbolSection2->addElement(dataSym4);
        symbolSection2->addElement(noTypeSym3);
        symbolSection2->addElement(sectionSym4);
        symbolSection2->addElement(noTypeSym4);

        symbolSection3->addElement(zeroElement3);

        // create at least 2 reloc sections.*********************************

        // not anymore, it would be too difficult to update memory
        // references manually.	
        // tests of relocation sections are now elsewhere.

        // create at least 2 processor resource table sections.***************

        newSection = Section::createSection(Section::ST_MR);
        ResourceSection* resourceSection1 = 
            dynamic_cast<ResourceSection*>(newSection);
        TS_ASSERT_DIFFERS(resourceSection1, static_cast<Section*>(NULL));

        ResourceElement* res1 = new ResourceElement();
        ResourceElement* res2 = new ResourceElement();
        ResourceElement* res3 = new ResourceElement();
        ResourceElement* res4 = new ResourceElement();
        ResourceElement* res5 = new ResourceElement();
	
        res1->setType(ResourceElement::MRT_NULL);
        res1->setName(stringSection1->chunk(2));
        res1->setInfo(10);

        res2->setType(ResourceElement::MRT_BUS);
        res2->setName(stringSection1->chunk(5));
        res2->setInfo(1000);

        res3->setType(ResourceElement::MRT_UNIT);
        res3->setName(stringSection2->chunk(9));
        res3->setInfo(100000);

        res4->setType(ResourceElement::MRT_RF);
        res4->setName(stringSection2->chunk(12));
        res4->setInfo(10000000);

        res5->setType(ResourceElement::MRT_OP);
        res5->setName(stringSection2->chunk(15));
        res5->setInfo(1000000000);
	
        resourceSection1->setLink(stringSection1);
        resourceSection1->addElement(res1);
        resourceSection1->addElement(res2);
       	resourceSection1->addElement(res3);
        resourceSection1->addElement(res4);
        resourceSection1->addElement(res5);	
	

        // create at least 2 line number sections.**************************

        newSection = Section::createSection(Section::ST_LINENO);
        LineNumSection* lineNumSection1 =
            dynamic_cast<LineNumSection*>(newSection);
        TS_ASSERT_DIFFERS(lineNumSection1, static_cast<Section*>(NULL));
	
        newSection = Section::createSection(Section::ST_LINENO);
        LineNumSection* lineNumSection2 =
            dynamic_cast<LineNumSection*>(newSection);
        TS_ASSERT_DIFFERS(lineNumSection2, static_cast<Section*>(NULL));

        LineNumElement *lineNum1  = new LineNumElement();
        lineNum1->setLineNumber(1);
        lineNum1->setInstruction(imm1);

        LineNumElement *lineNum2  = new LineNumElement();
        lineNum2->setLineNumber(3);
        lineNum2->setInstruction(move1);

        LineNumElement *lineNum3  = new LineNumElement();
        lineNum3->setLineNumber(1);
        lineNum3->setInstruction(move3);

        LineNumElement *lineNum4  = new LineNumElement();
        lineNum4->setLineNumber(2);
        lineNum4->setInstruction(imm2);

        LineNumElement *lineNum5  = new LineNumElement();
        lineNum5->setLineNumber(3);
        lineNum5->setInstruction(move4);

        LineNumElement *lineNum6  = new LineNumElement();
        lineNum6->setLineNumber(5);
        lineNum6->setInstruction(imm5);

        LineNumElement *lineNum7  = new LineNumElement();
        lineNum7->setLineNumber(1);
        lineNum7->setInstruction(move5);

        LineNumElement *lineNum8  = new LineNumElement();
        lineNum8->setLineNumber(101);
        lineNum8->setInstruction(imm6);

        LineNumElement *lineNum9  = new LineNumElement();
        lineNum9->setLineNumber(1);
        lineNum9->setInstruction(move3);

        LineNumElement *lineNum10  = new LineNumElement();
        lineNum10->setLineNumber(100);
        lineNum10->setInstruction(move4);

        LineNumElement *lineNum11  = new LineNumElement();
        lineNum11->setLineNumber(1000);
        lineNum11->setInstruction(move5);
	
        LineNumProcedure *proced1 = new LineNumProcedure();
        proced1->setProcedureSymbol(codeSym1);
        proced1->addLine(lineNum1);
        proced1->addLine(lineNum2);

        LineNumProcedure *proced2 = new LineNumProcedure();
        proced2->setProcedureSymbol(codeSym2);
        proced2->addLine(lineNum3);
        proced2->addLine(lineNum4);
        proced2->addLine(lineNum5);
        proced2->addLine(lineNum6);

        LineNumProcedure *proced3 = new LineNumProcedure();
        proced3->setProcedureSymbol(codeSym3);
        proced3->addLine(lineNum7);
        proced3->addLine(lineNum8);

        LineNumProcedure *proced4 = new LineNumProcedure();
        proced4->setProcedureSymbol(codeSym4);
        proced4->addLine(lineNum9);
        proced4->addLine(lineNum10);
        proced4->addLine(lineNum11);

        lineNumSection1->setCodeSection(codeSection1);
        // should be symbol table where symbols can be found
        lineNumSection1->setLink(symbolSection1); 
        lineNumSection1->addElement(proced1);

        lineNumSection2->setCodeSection(codeSection2);
        // should be symbol table where symbols can be found
        lineNumSection2->setLink(symbolSection2); 
        lineNumSection2->addElement(proced2);
        lineNumSection2->addElement(proced3);
        lineNumSection2->addElement(proced4);

        // set binary's properties and links********************************
        origBinary_ = new Binary();

        origBinary_->setStrings(stringSection2);
        origBinary_->setArch(Binary::FA_TTA_TUT);
        origBinary_->setType(Binary::FT_OBJSEQ);

        aSpaceSection->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        aSpaceSection->setASpace(undefASpace);

        Chunk *nullString = origBinary_->strings()->chunk(0);
        aSpaceElement1->setName(nullString);
        aSpaceElement2->setName(nullString);
        aSpaceElement3->setName(nullString);
        undefASpace->setName(nullString);
        aSpaceSection->setLink(origBinary_->strings());
        origBinary_->addSection(aSpaceSection);

        stringSection1->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        stringSection1->setASpace(undefASpace);
        stringSection1->setLink(nullSection);
        origBinary_->addSection(stringSection1);

        stringSection2->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        stringSection2->setASpace(undefASpace);
        stringSection2->setLink(nullSection);
        origBinary_->addSection(stringSection2);

        uDataSection1->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        uDataSection1->setASpace(aSpaceElement1);
        uDataSection1->setLink(nullSection);
        origBinary_->addSection(uDataSection1);

        uDataSection2->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        uDataSection2->setASpace(aSpaceElement1);
        uDataSection2->setLink(nullSection);
        origBinary_->addSection(uDataSection2);

        strSymSection2->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        strSymSection2->setASpace(undefASpace);
        strSymSection2->setLink(nullSection);
        origBinary_->addSection(strSymSection2);

        dataSection1->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        dataSection1->setASpace(aSpaceElement1);
        dataSection1->setLink(nullSection);
        origBinary_->addSection(dataSection1);

        dataSection2->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        dataSection2->setASpace(aSpaceElement1);
        dataSection2->setLink(nullSection);
        origBinary_->addSection(dataSection2);

        strSymSection1->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        strSymSection1->setASpace(undefASpace);
        strSymSection1->setLink(nullSection);
        origBinary_->addSection(strSymSection1);

        codeSection1->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        codeSection1->setASpace(aSpaceElement1);
        codeSection1->setLink(resourceSection1);
        origBinary_->addSection(codeSection1);

        codeSection2->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        codeSection2->setASpace(aSpaceElement1);
        codeSection2->setLink(resourceSection1);
        origBinary_->addSection(codeSection2);

        nullSection->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        nullSection->setASpace(undefASpace);
        nullSection->setLink(nullSection);
        origBinary_->addSection(nullSection);

        resourceSection1->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        resourceSection1->setASpace(undefASpace);
        origBinary_->addSection(resourceSection1);

        symbolSection1->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        symbolSection1->setASpace(undefASpace);
        origBinary_->addSection(symbolSection1);

        symbolSection2->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        symbolSection2->setASpace(undefASpace);
        origBinary_->addSection(symbolSection2);

        symbolSection3->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        symbolSection3->setASpace(undefASpace);
        origBinary_->addSection(symbolSection3);

        lineNumSection1->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        lineNumSection1->setASpace(undefASpace);
        origBinary_->addSection(lineNumSection1);

        lineNumSection2->setName(
            origBinary_->strings()->chunk(kindOfSectionId++));
        lineNumSection2->setASpace(undefASpace);
        origBinary_->addSection(lineNumSection2);

        alreadyInitialized = true;
    }
}

/**
 * Compares if given sections are equal.
 *
 * @param first Section to compare.
 * @param second Section to compare.
 */
void
TPEFReaderAndWriterTest::compareSections(Section* first, Section* second) {
  
    // common data for all sections
    TS_ASSERT_EQUALS(first->type(), second->type());    
    TS_ASSERT_EQUALS(first->flags(), second->flags());
    TS_ASSERT_EQUALS(first->startingAddress(), second->startingAddress());

    if (first->link() != NULL && second->link() != NULL) {
        TS_ASSERT_EQUALS(first->link()->name()->offset(),
                         second->link()->name()->offset());
    } else {
        TS_ASSERT_EQUALS(first->link(), second->link());
    }

    if (first->aSpace() != NULL && second->aSpace() != NULL) {
        compareASpaceElements(first->aSpace(), second->aSpace());       
    } else {
        TS_ASSERT_EQUALS(first->aSpace(), second->aSpace());
    }
 
    if (first->name() != NULL && second->name() != NULL) {
        TS_ASSERT_EQUALS(first->name()->offset(),second->name()->offset());
    } else {
        TS_ASSERT_EQUALS(first->name(), second->name());
    }

    TS_ASSERT_EQUALS(first->elementCount(),second->elementCount());

    Word firstElements = 0;
    Word secondElements = 0;

    // Section specific parts.
    switch (first->type()) {

    case Section::ST_NULL:	
        // no special stuff to check.
        break;

    case Section::ST_STRTAB: {
        TS_ASSERT_EQUALS(dynamic_cast<DataSection*>(first)->length(),
                         dynamic_cast<DataSection*>(second)->length());

        for (Word i = 0; i < dynamic_cast<DataSection*>(first)->length(); i++) {
            Chunk offset(i);
            TS_ASSERT_EQUALS(
                dynamic_cast<DataSection*>(first)->byte(&offset),
                dynamic_cast<DataSection*>(second)->byte(&offset));
        }

        TS_ASSERT_DIFFERS(dynamic_cast<StringSection*>(first), 
                          static_cast<Section*>(NULL));

        TS_ASSERT_DIFFERS(dynamic_cast<StringSection*>(second), 
                          static_cast<Section*>(NULL));
        break;

    case Section::ST_SYMTAB:
        // elements
        while (firstElements < first->elementCount()) {
            compareSymbolElements(
                dynamic_cast<SymbolElement*>
                (first->element(firstElements)),
                dynamic_cast<SymbolElement*>
                (second->element(secondElements)));

            firstElements++;
            secondElements++;
        }
        break;
    }

    case Section::ST_DEBUG:
        TS_FAIL("Debug section checking is not implemented!");
        break;

    case Section::ST_RELOC: {
        RelocSection *relocSect1 = dynamic_cast<RelocSection*>(first);
        RelocSection *relocSect2 = dynamic_cast<RelocSection*>(second);

        // sections referencedSection field
        if (relocSect1->referencedSection() != NULL &&
            relocSect2->referencedSection() != NULL) {

            TS_ASSERT_EQUALS(
                relocSect1->referencedSection()->name()->offset(),
                relocSect2->referencedSection()->name()->offset());
        } else {
            TS_ASSERT_EQUALS(
                relocSect1->referencedSection(),
                relocSect2->referencedSection());
        }

        // elements
        while (firstElements < first->elementCount()) {
            compareRelocElements(
                dynamic_cast<RelocElement*>
                (first->element(firstElements)),
                dynamic_cast<RelocElement*>
                (second->element(secondElements)));
            
            firstElements++;
            secondElements++;
        }
        break;
    }

    case Section::ST_LINENO: {
        LineNumSection *lineNumSect1 = dynamic_cast<LineNumSection*>(first);
        LineNumSection *lineNumSect2 = dynamic_cast<LineNumSection*>(second);

        // sections codeSection field
        compareSections(
            lineNumSect1->codeSection(), lineNumSect2->codeSection());
	
        // check elements
        while (firstElements != first->elementCount() &&
               secondElements != second->elementCount()) {
            LineNumProcedure* proced1 = dynamic_cast<LineNumProcedure*>
                (first->element(firstElements));
	    
            LineNumProcedure* proced2 = dynamic_cast<LineNumProcedure*>
                (second->element(secondElements));
	    
            // symbol field of procedure
            compareSymbolElements(
                proced1->procedureSymbol(), proced2->procedureSymbol());
	    
            TS_ASSERT_EQUALS(proced1->lineCount(), proced2->lineCount());
	    
            HalfWord lineIndex1 = 0;
            HalfWord lineIndex2 = 0;

            while (lineIndex1 < proced1->lineCount() && 
                   lineIndex2 < proced2->lineCount()) {
	      
                const LineNumElement *elem1 = proced1->line(lineIndex1);
                const LineNumElement *elem2 = proced2->line(lineIndex2);
		
                TS_ASSERT_EQUALS(elem1->lineNumber(), elem2->lineNumber());
		
                InstructionElement *instr1 = elem1->instruction();
                InstructionElement *instr2 = elem2->instruction();

                compareInstructionElements(instr1, instr2);  

                lineIndex1++;
                lineIndex2++;
            }
	    
            firstElements++;
            secondElements++;
        }   

    	break;
    }

    case Section::ST_NOTE:
        TS_FAIL("Note section checking is not implemented!");
        break;

    case Section::ST_ADDRSP:
        // check elements
        while (firstElements < first->elementCount()) {

            ASpaceElement* elem1 = dynamic_cast<ASpaceElement*>
                (first->element(firstElements));
            ASpaceElement* elem2 = dynamic_cast<ASpaceElement*>
                (second->element(secondElements));
	    
            compareASpaceElements(elem1, elem2);
	    
            firstElements++;
            secondElements++;
        }
        break;

    case Section::ST_MR:
        // check elements
        while (firstElements < first->elementCount()) {

            ResourceElement* elem1 = dynamic_cast<ResourceElement*>
                (first->element(firstElements));
            ResourceElement* elem2 = dynamic_cast<ResourceElement*>
                (second->element(secondElements));
 
            TS_ASSERT_EQUALS(elem1->info(), elem2->info());
            TS_ASSERT_EQUALS(elem1->type(), elem2->type());

            if (elem1->name() != NULL && elem2->name() != NULL) {
                TS_ASSERT_EQUALS(elem1->name()->offset(), 
                                 elem2->name()->offset());

            } else {
                TS_ASSERT_EQUALS(elem1->name(), elem2->name());
            }

            firstElements++;
            secondElements++;
        }
        break;

    case Section::ST_CODE:
        while (firstElements < first->elementCount() &&
               secondElements < second->elementCount()) {
	  
            compareInstructionElements(
                dynamic_cast<InstructionElement*>
                (first->element(firstElements)), 
                dynamic_cast<InstructionElement*>
                (second->element(secondElements)));

            firstElements++;
            secondElements++;
        }
        break;

    case Section::ST_DATA:
        TS_ASSERT_EQUALS(dynamic_cast<DataSection*>(first)->length(),
                         dynamic_cast<DataSection*>(second)->length());

        for (Word i = 0; i < dynamic_cast<DataSection*>(first)->length(); i++) {
            Chunk offset(i);
            TS_ASSERT_EQUALS(dynamic_cast<DataSection*>(first)->byte(&offset),
                             dynamic_cast<DataSection*>(second)->byte(&offset));
        }

        break;

    case Section::ST_UDATA:
        TS_ASSERT_EQUALS(dynamic_cast<UDataSection*>(first)->length(),
                         dynamic_cast<UDataSection*>(second)->length());
        break;

    default:
        TS_FAIL("Unknown section.");
    }
}


/**
 * Compares if given instructions are equal.
 *
 * Asserts if they are not.
 *
 * @param instr1 Instruction to compare.
 * @param instr2 Instruction to compare.
 */
void
TPEFReaderAndWriterTest::compareInstructionElements(
    InstructionElement *instr1,
    InstructionElement *instr2) {

    TS_ASSERT_EQUALS(instr1->isMove(), instr2->isMove());
    TS_ASSERT_EQUALS(instr1->begin(), instr2->begin());
    TS_ASSERT_EQUALS(instr1->annotationCount(), instr2->annotationCount());

    // annotation order doesn't have to be preserved... 
    // so this test is more strict that it should be
    for (Word i = 0; i < instr1->annotationCount(); i++) {
        InstructionAnnotation *ann1, *ann2;
        ann1 = instr1->annotation(i);
        ann2 = instr2->annotation(i);
		
        TS_ASSERT_EQUALS(ann1->id(), ann2->id());
        TS_ASSERT_EQUALS(ann1->size(), ann2->size());

        for (Word j = 0; j < ann1->size(); j++) {
            TS_ASSERT_EQUALS(ann1->byte(j), ann2->byte(j));
        }
    }

    if (instr1->isMove()) {
        MoveElement *move1 = 
            dynamic_cast<MoveElement*>(instr1);
        MoveElement *move2 = 
            dynamic_cast<MoveElement*>(instr2);
			
        TS_ASSERT_EQUALS(move1->bus(), move2->bus());

        TS_ASSERT_EQUALS(move1->sourceType(), 
                         move2->sourceType());

        TS_ASSERT_EQUALS(move1->destinationType(), 
                         move2->destinationType());
	
        TS_ASSERT_EQUALS(move1->sourceUnit(), move2->sourceUnit());	
        TS_ASSERT_EQUALS(move1->sourceIndex(), move2->sourceIndex());

        TS_ASSERT_EQUALS(move1->destinationUnit(), 
                         move2->destinationUnit());

        TS_ASSERT_EQUALS(move1->destinationIndex(), 
                         move2->destinationIndex());

        TS_ASSERT_EQUALS(move1->guardUnit(), move2->guardUnit());
        TS_ASSERT_EQUALS(move1->guardIndex(), move2->guardIndex());
        TS_ASSERT_EQUALS(move1->isGuarded(), move2->isGuarded());
        TS_ASSERT_EQUALS(move1->isGuardInverted(), move2->isGuardInverted());
	
    } else {
        ImmediateElement *imm1 = 
            dynamic_cast<ImmediateElement*>(instr1);
        ImmediateElement *imm2 = 
            dynamic_cast<ImmediateElement*>(instr2);
			
        TS_ASSERT_EQUALS(imm1->destinationUnit(),
                         imm2->destinationUnit());
	
        TS_ASSERT_EQUALS(imm1->destinationIndex(),
                         imm2->destinationIndex());
	
        TS_ASSERT_EQUALS(imm1->length(), imm2->length());
	
        for (unsigned int i = 0; i < imm1->length(); i++) {
            TS_ASSERT_EQUALS(imm1->byte(i), imm2->byte(i));
        }			
    }
}

/**
 * Compares if given symbols are equal.
 *
 * Asserts if they are not.
 *
 * @param sym1 Symbol to compare.
 * @param sym2 Symbol to compare.
 */
void
TPEFReaderAndWriterTest::compareSymbolElements(SymbolElement *sym1,
                                               SymbolElement *sym2) {

    TS_ASSERT_EQUALS(sym1->type(), sym2->type());
    TS_ASSERT_EQUALS(sym1->absolute(), sym2->absolute());
    TS_ASSERT_EQUALS(sym1->binding(), sym2->binding());
    
    // name field
    if (sym1->name() != NULL && sym2->name() != NULL) {
        TS_ASSERT_EQUALS(sym1->name()->offset(), sym2->name()->offset());
    } else {
        TS_ASSERT_EQUALS(sym1->name(), sym2->name());
    }

    // section field
    if (sym1->section() != NULL && sym2->section() != NULL) {
        TS_ASSERT_EQUALS(sym1->section()->name()->offset(),
                         sym2->section()->name()->offset());
    } else {
        TS_ASSERT_EQUALS(sym1->section(), sym2->section());
    }

    // test value and size....
    switch(sym1->type()) {
    case SymbolElement::STT_NOTYPE:
        break;

    case SymbolElement::STT_CODE: {	
        CodeSymElement *codeSym1 = dynamic_cast<CodeSymElement*>(sym1);
        CodeSymElement *codeSym2 = dynamic_cast<CodeSymElement*>(sym2);

        InstructionElement *instr1 = codeSym1->reference();
        InstructionElement *instr2 = codeSym2->reference();

        if (instr1 != NULL && instr2 != NULL) {
            compareInstructionElements(instr1, instr2);
        } else {
            TS_ASSERT_EQUALS(instr1, instr2);
        }
	
        TS_ASSERT_EQUALS(codeSym1->size(), codeSym2->size());

        break;
    }

    case SymbolElement::STT_DATA: {
        DataSymElement* dataSym1 = dynamic_cast<DataSymElement*>(sym1);
        DataSymElement* dataSym2 = dynamic_cast<DataSymElement*>(sym2);
	
        Chunk* chunk1 = dataSym1->reference();
        Chunk* chunk2 = dataSym2->reference();

        if (chunk1 != NULL && chunk2 != NULL) {
            TS_ASSERT_EQUALS(chunk1->offset(), chunk2->offset());
        } else {
            TS_ASSERT_EQUALS(chunk1, chunk2);
        }
	
        TS_ASSERT_EQUALS(dataSym1->size(), dataSym2->size());

        break;
    }

    case SymbolElement::STT_SECTION: {
        SectionSymElement *sectSym1 = dynamic_cast<SectionSymElement*>(sym1);
        SectionSymElement *sectSym2 = dynamic_cast<SectionSymElement*>(sym2);

        TS_ASSERT_EQUALS(sectSym1->value(), sectSym2->value());
        TS_ASSERT_EQUALS(sectSym1->size(), sectSym2->size());

        break;
    }

    case SymbolElement::STT_FILE: {
        FileSymElement *fileSym1 = dynamic_cast<FileSymElement*>(sym1);
        FileSymElement *fileSym2 = dynamic_cast<FileSymElement*>(sym2);

        TS_ASSERT_EQUALS(fileSym1->value(), fileSym2->value());

        break;
    }

    default:
        TS_FAIL("Unknown symbol type");
    }
}

/**
 * Compares if given address space elements are equal.
 *
 * Asserts if they are not.
 *
 * @param aSpace1 Address space to compare.
 * @param aSpace2 Address space to compare.
 */
void
TPEFReaderAndWriterTest::compareASpaceElements(ASpaceElement *aSpace1,
                                               ASpaceElement *aSpace2) {

    TS_ASSERT_EQUALS(aSpace1->MAU(), aSpace2->MAU());
    TS_ASSERT_EQUALS(aSpace1->align(), aSpace2->align());
    TS_ASSERT_EQUALS(aSpace1->wordSize(), aSpace2->wordSize());

    if (aSpace1->name() != aSpace2->name()) {
        TS_ASSERT_EQUALS(aSpace1->name()->offset(), 
                         aSpace2->name()->offset());
    } else {
        TS_ASSERT(aSpace1->name() == NULL);	
    }
}

/**
 * Compares if given relocation elements are equal.
 *
 * Asserts if they are not.
 *
 * @param reloc1 Element to compare.
 * @param reloc2 Element to compare.
 */
void
TPEFReaderAndWriterTest::compareRelocElements(RelocElement *reloc1,
                                              RelocElement *reloc2) {
    
    TS_ASSERT_EQUALS(reloc1->type(), reloc2->type());
    TS_ASSERT_EQUALS(reloc1->size(), reloc2->size());
    
    if (reloc1->location() != NULL && reloc2->location() != NULL) {
	
        // which type of elements are being relocated immediate or data chunk
        InstructionElement *isInstruction = 
            dynamic_cast<InstructionElement*>(reloc1->location());

        Chunk *isChunk = 
            dynamic_cast<Chunk*>(reloc1->location());

        if (isInstruction != NULL) {
            compareInstructionElements(
                dynamic_cast<InstructionElement*>(reloc1->location()),
                dynamic_cast<InstructionElement*>(reloc2->location()));
	    
        } else if (isChunk != NULL) {
            TS_ASSERT_EQUALS(
                dynamic_cast<Chunk*>(reloc1->location())->offset(),
                dynamic_cast<Chunk*>(reloc2->location())->offset());

        } else {
            TS_FAIL("Wrong type of elements to be relocated\n");
        }

        isInstruction = 
            dynamic_cast<InstructionElement*>(reloc1->destination());

        isChunk = dynamic_cast<Chunk*>(reloc1->destination());

        if (isInstruction != NULL) {
            compareInstructionElements(
                dynamic_cast<InstructionElement*>(reloc1->destination()),
                dynamic_cast<InstructionElement*>(reloc2->destination()));
	    
        } else if (isChunk != NULL) {
            TS_ASSERT_EQUALS(
                dynamic_cast<Chunk*>(reloc1->destination())->offset(),
                dynamic_cast<Chunk*>(reloc2->destination())->offset());

        } else {
            TS_FAIL("Wrong type of destination elements\n");
        }
	
        // and symbol and rest of stuff
	

    } else {
        TS_ASSERT_EQUALS(reloc1->location(), reloc2->location());
    }

    if (reloc1->aSpace() != NULL && reloc2->aSpace() != NULL) {
        compareASpaceElements(reloc1->aSpace(), reloc2->aSpace());
    } else {
        TS_ASSERT_EQUALS(reloc1->aSpace(), reloc2->aSpace());
    }

    if (reloc1->symbol() != NULL && reloc2->symbol() != NULL) {
        compareSymbolElements(reloc1->symbol(), reloc2->symbol());
    } else {
        TS_ASSERT_EQUALS(reloc1->symbol(), reloc2->symbol());
    }
}

#endif
