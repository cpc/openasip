/**
 * @file AOutReaderTest.hh
 *
 * Tests for AOutReader class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 */

#ifndef AOUT_READER_TEST_HH
#define AOUT_READER_TEST_HH

#include <list>
#include <string>
#include <sstream>
#include <cstddef>
#include <iostream>
#include <TestSuite.h>
#include <iomanip>

#include "BinaryStream.hh"
#include "Binary.hh"
#include "Exception.hh"
#include "BinaryReader.hh"
#include "BinaryWriter.hh"
#include "TPEFWriter.hh"

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
#include "DebugSection.hh"

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
#include "DebugElement.hh"
#include "DebugStabElem.hh"

using std::list;
using std::string;
using std::hex;
using std::dec;

using namespace TPEF;
using ReferenceManager::SafePointer;

/**
 * Test class for AOutReader.
 *
 * Reads test binaries and checks that reading is done correctly by 
 * comparing results to reference data.
 * 
 * Test also creates new text file for every compared binary to data 
 * directory. File contains symbol and relocation tables of readed file
 * and dissassembled code of text section and everything alse that we can 
 * find out about binary. (functions from here will be used probably in tpef dumper)
 */
class AOutReaderTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testLinkedBinary();
    void testMainBinaryObject();
    void testFunk1BinaryObject();
    void testFunk2BinaryObject();
    void testEmptyBinaryObject();
       
private:
    /// Contains the test.out after it is read from a.out.
    Binary* linkedBinary_;
    /// Contains the test.out after it is read from TPEF.
    Binary* linkedBinaryFromTPEF_;
    /// Contains test_main.o.after it is read from a.out.
    Binary* mainBinary_;
    /// Contains the test_main.o after it is read from TPEF.
    Binary* mainBinaryFromTPEF_;
    /// Contains test_funk1.o after it is read from a.out.
    Binary* funk1Binary_;
    /// Contains the test_funk1.o after it is read from TPEF.
    Binary* funk1BinaryFromTPEF_;
    /// Contains test_funk2.o after it is read from a.out.
    Binary* funk2Binary_;    
    /// Contains the test_funk2.o after it is read from TPEF.
    Binary* funk2BinaryFromTPEF_;
    /// Contains test_empty.o after it is read from a.out.
    Binary* emptyBinary_;    
    /// Contains the test_empty.o after it is read from TPEF.
    Binary* emptyBinaryFromTPEF_;

    void compareBinaries(Binary *first, Binary *second);

    void compareSections(Section* first, Section *second);
    	
    void compareChunks(Chunk *chunk1, Chunk *chunk2);
	       
    void compareInstructionElements(InstructionElement *instr1,
				    InstructionElement *instr2);

    void compareSymbolElements(SymbolElement *sym1,
			       SymbolElement *sym2);

    void compareASpaceElements(ASpaceElement *aSpace1,
			       ASpaceElement *aSpace2);    

    void compareRelocElements(RelocElement *reloc1,
			      RelocElement *reloc2);    

    void compareDebugElements(DebugElement *elem1,
                              DebugElement *elem2);
};


#include <iostream>
using std::cerr;
using std::endl;

const string linkedBinaryName = "data/test.out";
const string mainBinaryName   = "data/test_main.o";
const string funk1BinaryName  = "data/test_funk1.o";
const string funk2BinaryName  = "data/test_funk2.o";
const string emptyBinaryName  = "data/test_empty.o";

const string tpefBinaryName = "data/temp.tpef";

/**
 * Sets the test environment before each test.
 */
void 
AOutReaderTest::setUp() {
    static bool allReadySetUp = false;

    if (!allReadySetUp) {
	// open binary stream
	BinaryStream tpefBinaryStream(tpefBinaryName);

	// read a.out object file
	BinaryStream mainBinaryStream(mainBinaryName);
	mainBinary_ = BinaryReader::readBinary(mainBinaryStream);

	// write it to TPEF and read again
	tpefBinaryStream.setWritePosition(0);
	tpefBinaryStream.setReadPosition(0);
	TPEFWriter::instance().writeBinary(tpefBinaryStream, mainBinary_);
	mainBinaryFromTPEF_ = BinaryReader::readBinary(tpefBinaryStream);

	// read a.out object file
	BinaryStream funk1BinaryStream(funk1BinaryName);
	funk1Binary_ = BinaryReader::readBinary(funk1BinaryStream);

	// write it to TPEF and read again
	tpefBinaryStream.setWritePosition(0);
	tpefBinaryStream.setReadPosition(0);
	TPEFWriter::instance().writeBinary(tpefBinaryStream, funk1Binary_);
	funk1BinaryFromTPEF_ = BinaryReader::readBinary(tpefBinaryStream);

	// read a.out object file
	BinaryStream funk2BinaryStream(funk2BinaryName);
	funk2Binary_ = BinaryReader::readBinary(funk2BinaryStream);

	// write it to TPEF and read again
	tpefBinaryStream.setWritePosition(0);
	tpefBinaryStream.setReadPosition(0);
	TPEFWriter::instance().writeBinary(tpefBinaryStream, funk2Binary_);
	funk2BinaryFromTPEF_ = BinaryReader::readBinary(tpefBinaryStream);

	// read a.out object file
	BinaryStream emptyBinaryStream(emptyBinaryName);
	emptyBinary_ = BinaryReader::readBinary(emptyBinaryStream);

	// write it to TPEF and read again
	tpefBinaryStream.setWritePosition(0);
	tpefBinaryStream.setReadPosition(0);
	TPEFWriter::instance().writeBinary(tpefBinaryStream, emptyBinary_);
	emptyBinaryFromTPEF_ = BinaryReader::readBinary(tpefBinaryStream);

	// read linked binary to new stream
	BinaryStream linkedBinaryStream(linkedBinaryName);
	linkedBinary_ = BinaryReader::readBinary(linkedBinaryStream);

	// write it to TPEF stream and read again to class structure
	tpefBinaryStream.setWritePosition(0);
	tpefBinaryStream.setReadPosition(0);
	TPEFWriter::instance().writeBinary(tpefBinaryStream, linkedBinary_);
	linkedBinaryFromTPEF_ = BinaryReader::readBinary(tpefBinaryStream);

	allReadySetUp = true;
    }
}

/**
 * Clears test environment after each test.
 */
void 
AOutReaderTest::tearDown() {
    
}

void 
AOutReaderTest::testLinkedBinary() {
    compareBinaries(linkedBinary_, linkedBinaryFromTPEF_);
}

void 
AOutReaderTest::testMainBinaryObject() {
    compareBinaries(mainBinary_, mainBinaryFromTPEF_);
}

void 
AOutReaderTest::testFunk1BinaryObject() {
    compareBinaries(funk1Binary_, funk1BinaryFromTPEF_);
}

void 
AOutReaderTest::testFunk2BinaryObject() {
    compareBinaries(funk2Binary_, funk2BinaryFromTPEF_);
}

void 
AOutReaderTest::testEmptyBinaryObject() {
    compareBinaries(emptyBinary_, emptyBinaryFromTPEF_);
}

/**
 * Compares if given binaries are equal.
 *
 * @param first Binary to compare.
 * @param second Binary to compare.
 */
void
AOutReaderTest::compareBinaries(Binary* first, Binary* second) {

    TS_ASSERT_EQUALS(first->type(), second->type());
    TS_ASSERT_EQUALS(first->arch(), second->arch());
   
    compareSections(first->strings(), second->strings());

    // section ordering and section id:s should not make difference, 
    // but now it does. Good thing is that there is only one of each 
    // section in a.out so this will work for now.

    for (Word firstIter = 0; firstIter < first->sectionCount(); 
	 firstIter++) {
	
        Word secondIter = 0;

	// We have just one of each section so we can search by 
	// section type and size here. In more generic version we should compare 
	// every section with all others to find out that there is one 
	// match for each section 
	// (or if one section matches many times there should be many instances
	// of that section too in both of compared binaries).
	
        for (; secondIter < second->sectionCount(); secondIter++) {

            if (first->section(firstIter)->type() == 
		second->section(secondIter)->type() &&
		first->section(firstIter)->elementCount() == 
		second->section(secondIter)->elementCount()) {
		
		// and still some extra checking for relocation sections
		if (first->section(firstIter)->type() == Section::ST_RELOC &&

		    dynamic_cast<RelocSection*>
		    (first->section(
			firstIter))->referencedSection()->type() ==
		    dynamic_cast<RelocSection*>
		    (second->section(
			secondIter))->referencedSection()->type() ||
		    
		    first->section(firstIter)->type() != Section::ST_RELOC) {
		    
		    compareSections(first->section(firstIter), 
				    second->section(secondIter));
		    break;
		}
            }
        }
	
        TS_ASSERT_DIFFERS(secondIter, second->sectionCount());
    }
}

/**
 * Compares if given sections are equal.
 *
 * This function would work better, if this would create table
 * of every  matched sections, so all section data shouldn't be always
 * compared and reference loops (if there are any) would be easier to 
 * recognize.
 *
 * This could be done much better, e.g., if we create some class or table to 
 * gather information of matched sections... this is needed if there is two 
 * exactly same sections and matching should be done by references to these 
 * sections from other sections...
 *
 * @param first Section to compare.
 * @param second Section to compare.
 */
void
AOutReaderTest::compareSections(Section* first, Section* second) {
  
    if (first == NULL || second == NULL) {       
	// sections were equally NULL
	if (first == second) {
	    return;
	} else {
	    TS_FAIL("Only one of the compared sections were NULL.");
	}
    } 

//cerr << " compareSecs-1" << ".0x" << hex << int(first->type()) << " " <<  dec;

    // common data for all sections
    TS_ASSERT_EQUALS(first->type(), second->type());    
    TS_ASSERT_EQUALS(first->flags(), second->flags());
    TS_ASSERT_EQUALS(first->startingAddress(), second->startingAddress());


//cerr << "compareSecs-2" << ".0x" << hex << int(first->type()) << " " <<  dec;
    if (first->link() != first) {
	compareSections(first->link(), second->link());
    }

//cerr << "compareSecs-3" << ".0x" << hex << int(first->type()) << " " <<  dec;
    compareASpaceElements(first->aSpace(), second->aSpace());       

//cerr << "compareSecs-4" << ".0x" << hex << int(first->type()) << " " <<  dec;
    compareChunks(first->name(), second->name());

//cerr << "compareSecs-5" << ".0x" << hex << int(first->type()) << " " <<  dec;
  
    // both have same amount of elements.
    TS_ASSERT_EQUALS(first->elementCount(),second->elementCount());

    Word firstElements = 0;
    Word secondElements = 0;

//cerr << "compareSecs-6" << ".0x" << hex << int(first->type()) <<  dec;

    // Section specific parts.
    switch (first->type()) {

    case Section::ST_NULL:	
//cerr << ".null\n";
        // no special stuff to check.
        break;

    case Section::ST_STRTAB: {
//cerr << ".strtab\n";

        TS_ASSERT_EQUALS(dynamic_cast<DataSection*>(first)->length(),
                         dynamic_cast<DataSection*>(second)->length());

        for (Word i = 0; 
	     i < dynamic_cast<DataSection*>(first)->length(); 
	     i++) {
            
            TS_ASSERT_EQUALS(
                dynamic_cast<DataSection*>(first)->byte(i),
                dynamic_cast<DataSection*>(second)->byte(i));
        }

        TS_ASSERT_DIFFERS(dynamic_cast<StringSection*>(first), 
			  static_cast<StringSection*>(NULL));

        TS_ASSERT_DIFFERS(dynamic_cast<StringSection*>(second), 
			  static_cast<StringSection*>(NULL));
        break;

    case Section::ST_SYMTAB:
//cerr << ".symtab\n";

        // elements
        while (firstElements != first->elementCount() &&
               secondElements != second->elementCount()) {

            compareSymbolElements(
		dynamic_cast<SymbolElement*>(first->element(firstElements)),
                dynamic_cast<SymbolElement*>(second->element(secondElements)));

            firstElements++;
            secondElements++;
        }
        break;
    }

    case Section::ST_DEBUG:
        // elements
        while (firstElements != first->elementCount() &&
               secondElements != second->elementCount()) {

            compareDebugElements(
                dynamic_cast<DebugElement*>(first->element(firstElements)),
                dynamic_cast<DebugElement*>(second->element(secondElements)));
            
            firstElements++;
            secondElements++;
        }
        break;
        

    case Section::ST_RELOC: {
//cerr << ".reloc\n";
        RelocSection *relocSect1 = dynamic_cast<RelocSection*>(first);
        RelocSection *relocSect2 = dynamic_cast<RelocSection*>(second);

        // sections referencedSection field
        compareSections(relocSect1->referencedSection(),
			relocSect2->referencedSection());

        // elements
        while (firstElements != first->elementCount() &&
               secondElements != second->elementCount()) {

	    compareRelocElements(
		dynamic_cast<RelocElement*>(first->element(firstElements)),
		dynamic_cast<RelocElement*>(second->element(secondElements)));
            
            firstElements++;
            secondElements++;
        }
        break;
    }

    case Section::ST_LINENO: {
//cerr << ".lineno\n";

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
//cerr << ".note\n";

        TS_FAIL("Note section checking is not implemented!");
        break;

    case Section::ST_ADDRSP:
//cerr << ".aspace\n";

        // check elements
        while (firstElements != first->elementCount() &&
               secondElements != second->elementCount()) {

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
//cerr << ".mr\n";

        // check elements
        while (firstElements != first->elementCount() &&
               secondElements != second->elementCount()) {

 	    ResourceElement* elem1 = dynamic_cast<ResourceElement*>
		(first->element(firstElements));
	    
	    ResourceElement* elem2 = dynamic_cast<ResourceElement*>
		(second->element(secondElements));
	    
	    TS_ASSERT_EQUALS(elem1->info(), elem2->info());
	    TS_ASSERT_EQUALS(elem1->type(), elem2->type());

	    compareChunks(elem1->name(), elem2->name());

            firstElements++;
            secondElements++;
        }
        break;

    case Section::ST_CODE:
//cerr << ".code\n";

        while (firstElements != first->elementCount() &&
               secondElements != second->elementCount()) {
	  
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
//cerr << ".data\n";

        TS_ASSERT_EQUALS(dynamic_cast<DataSection*>(first)->length(),
                         dynamic_cast<DataSection*>(second)->length());

        for (Word i = 0; i < dynamic_cast<DataSection*>(first)->length(); i++) {
            Chunk offset(i);
            TS_ASSERT_EQUALS(dynamic_cast<DataSection*>(first)->byte(&offset),
                             dynamic_cast<DataSection*>(second)->byte(&offset));
        }

        break;

    case Section::ST_UDATA:
//cerr << ".udata\n";

        TS_ASSERT_EQUALS(dynamic_cast<UDataSection*>(first)->length(),
                         dynamic_cast<UDataSection*>(second)->length());
        break;

    default:
        TS_FAIL("Unknown section.");
    }
}

/**
 * Compares if given chunks are equal.
 *
 * Asserts if they are not.
 *
 * @param chunk1 Chunk to compare.
 * @param chunk2 Chunk to compare.
 */
void 
AOutReaderTest::compareChunks(Chunk *chunk1, Chunk *chunk2) {
    if (chunk1 == static_cast<Chunk*>(NULL) || 
	chunk2 == static_cast<Chunk*>(NULL)) {
	if (chunk1 != chunk2) {
	    TS_FAIL("One of the compared chunks was NULL");
	    return;
	} else {
	    return; // both were equally NULL
	}
    }
    TS_ASSERT_EQUALS(chunk1->offset(), chunk2->offset());
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
AOutReaderTest::compareInstructionElements(
    InstructionElement *instr1,
    InstructionElement *instr2) {

    if (instr1 == static_cast<InstructionElement*>(NULL) || 
	instr2 == static_cast<InstructionElement*>(NULL)) {
	if (instr1 != instr2) {
	    TS_FAIL("One of the compared instructions was NULL");
	    return;
	} else {
	    return; // both were equally NULL
	}
    }

    TS_ASSERT_EQUALS(instr1->isMove(), instr2->isMove());
    TS_ASSERT_EQUALS(instr1->begin(), instr2->begin());

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
AOutReaderTest::compareSymbolElements(SymbolElement *sym1,
				      SymbolElement *sym2) {

    if (sym1 == static_cast<SymbolElement*>(NULL) || 
	sym2 == static_cast<SymbolElement*>(NULL)) {
	if (sym1 != sym2) {
	    TS_FAIL("One of the compared symbols was NULL");
	    return;
	} else {
	    return; // both were equally NULL
	}
    }

    TS_ASSERT_EQUALS(sym1->type(), sym2->type());
    TS_ASSERT_EQUALS(sym1->absolute(), sym2->absolute());
    TS_ASSERT_EQUALS(sym1->binding(), sym2->binding());
    
    // name field
    compareChunks(sym1->name(), sym2->name());

    // section field
    compareSections(sym1->section(), sym2->section());

    // test value and size....
    switch(sym1->type()) {
    case SymbolElement::STT_NOTYPE:
	break;

    case SymbolElement::STT_PROCEDURE:
    case SymbolElement::STT_CODE: {	
	CodeSymElement *codeSym1 = dynamic_cast<CodeSymElement*>(sym1);
	CodeSymElement *codeSym2 = dynamic_cast<CodeSymElement*>(sym2);

	InstructionElement *instr1 = codeSym1->reference();
	InstructionElement *instr2 = codeSym2->reference();

	TS_ASSERT_EQUALS(codeSym1->type(), codeSym2->type());

	compareInstructionElements(instr1, instr2);
	
	TS_ASSERT_EQUALS(codeSym1->size(), codeSym2->size());

	break;
    }

    case SymbolElement::STT_DATA: {
	DataSymElement* dataSym1 = dynamic_cast<DataSymElement*>(sym1);
	DataSymElement* dataSym2 = dynamic_cast<DataSymElement*>(sym2);
	
	Chunk* chunk1 = dataSym1->reference();
	Chunk* chunk2 = dataSym2->reference();

	compareChunks(chunk1, chunk2);
	
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
AOutReaderTest::compareASpaceElements(ASpaceElement *aSpace1,
				      ASpaceElement *aSpace2) {

    if (aSpace1 == static_cast<ASpaceElement*>(NULL) || 
	aSpace2 == static_cast<ASpaceElement*>(NULL)) {
	if (aSpace1 != aSpace2) {
	    TS_FAIL("One of the compared address spaces was NULL");
	} else {
	    return; // both were equally NULL
	}
    }

    TS_ASSERT_EQUALS(aSpace1->MAU(), aSpace2->MAU());
    TS_ASSERT_EQUALS(aSpace1->align(), aSpace2->align());
    TS_ASSERT_EQUALS(aSpace1->wordSize(), aSpace2->wordSize());    
    compareChunks(aSpace1->name(), aSpace2->name());
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
AOutReaderTest::compareRelocElements(RelocElement *reloc1,
				     RelocElement *reloc2) {
    
    if (reloc1 == static_cast<RelocElement*>(NULL) || 
	reloc2 == static_cast<RelocElement*>(NULL)) {
	if (reloc1 != reloc2) {
	    TS_FAIL("One of the compared relocations was NULL");
	    return;
	} else {
	    return; // both were equally NULL
	}
    }

    TS_ASSERT_EQUALS(reloc1->type(), reloc2->type());
    TS_ASSERT_EQUALS(reloc1->size(), reloc2->size());
    
    if (reloc1->location() != static_cast<SectionElement*>(NULL) && 
	reloc2->location() != static_cast<SectionElement*>(NULL)) {
	
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
	    compareChunks(
		dynamic_cast<Chunk*>(reloc1->location()),
		dynamic_cast<Chunk*>(reloc2->location()));

	} else {
	    TS_FAIL("Wrong type of elements to be relocated\n");
	}

	if (reloc1->destination() != NULL) {
	    isInstruction = 
		dynamic_cast<InstructionElement*>(reloc1->destination());
	    
	    isChunk = dynamic_cast<Chunk*>(reloc1->destination());
	    
	    if (isInstruction != NULL) {
		compareInstructionElements(
		    dynamic_cast<InstructionElement*>(reloc1->destination()),
		    dynamic_cast<InstructionElement*>(reloc2->destination()));
		
	    } else if (isChunk != NULL) {
		compareChunks(
		    dynamic_cast<Chunk*>(reloc1->destination()),
		    dynamic_cast<Chunk*>(reloc2->destination()));
		
	    } else {
		TS_FAIL("Wrong type of destination elements\n");
	    }
	} else {
	    TS_ASSERT_EQUALS(reloc1->destination(),
			     reloc2->destination());
	}
	
    } else {
	// if there was null in location
	TS_ASSERT_EQUALS(reloc1->location(), reloc2->location());
    }

    compareASpaceElements(reloc1->aSpace(), reloc2->aSpace());
    compareSymbolElements(reloc1->symbol(), reloc2->symbol());
}



/**
 * Compares if given debug element are equal.
 *
 * Asserts if they are not.
 *
 * @param elem1 Symbol to compare.
 * @param elem2 Symbol to compare.
 */
void 
AOutReaderTest::compareDebugElements(DebugElement *elem1,
                                     DebugElement *elem2) {

    if (elem1 == static_cast<DebugElement*>(NULL) || 
        elem2 == static_cast<DebugElement*>(NULL)) {
        
        if (elem1 != elem2) {
            TS_FAIL("One of the compared debug elements was NULL");
            return;
        } else {
            return; // both were equally NULL
        }
    }
   
    TS_ASSERT_EQUALS(elem1->type(), elem2->type());
    compareChunks(elem1->debugString(), elem2->debugString());
    TS_ASSERT_EQUALS(elem1->length(), elem2->length());
    
    for (unsigned int i = 0; i < elem1->length(); i++) {
        TS_ASSERT_EQUALS(elem1->byte(i), elem2->byte(i));
    }
}

 

#endif
