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
 * @file SectionReaderTest.hh
 *
 * Tests for SectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 */

#ifndef TTA_SECTION_READER_HH
#define TTA_SECTION_READER_HH

#include <TestSuite.h>

#include "BaseType.hh"
#include "SectionReader.hh"
#include "AOutSectionReader.hh"
#include "Section.hh"
#include "BinaryStream.hh"
#include "AOutReader.hh"

using namespace TPEF;

///////////////////////////////////////////////////////////////////////////////
// BOutSectionReader declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for BOutSectionReaders.
 */
class BOutSectionReader : public SectionReader {
protected:
    virtual BinaryReader* parent() const;
};

///////////////////////////////////////////////////////////////////////////////
// BOutTextReader declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Text section reader for b.out type file.
 */
class BOutTextReader : public BOutSectionReader {
protected:
    BOutTextReader();

    Section::SectionType type() const;

    void readData(
	BinaryStream &stream,
	Section* section) const 
	throw (UnreachableStream, KeyAlreadyExists, EndOfFile, 
	       OutOfRange, WrongSubclass, UnexpectedValue);

private:
    
    /// Prototype of class.
    static BOutTextReader proto_;
};

// initialization of prototype
BOutTextReader BOutTextReader::proto_;

///////////////////////////////////////////////////////////////////////////////
// AOutTextReader declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Text section reader for a.out type file.
 */
class AOutTextReader : public AOutSectionReader {
protected:
    AOutTextReader();

    Section::SectionType type() const;

    // Just for overriding original parent() method, that would
    // create real instance of AOutReader class..
    virtual BinaryReader* parent() const;

    void readData(
	BinaryStream &stream,
	Section* section) const
	throw (UnreachableStream, KeyAlreadyExists, EndOfFile, 
	       OutOfRange, WrongSubclass, UnexpectedValue);

private:
    
    /// Prototype of class.
    static AOutTextReader proto_;
};

// initialiaztion of prototype
AOutTextReader AOutTextReader::proto_;

///////////////////////////////////////////////////////////////////////////////
// TestSection declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Test section, empty implementation.
 */
class TestSection : public Section {
public:
    Section::SectionType type() const;

protected:
    Section* clone() const;
    TestSection(bool init);

private:
    /// Prototype of section.
    static TestSection proto_;
};

TestSection TestSection::proto_(true);

///////////////////////////////////////////////////////////////////////////////
// SectionReaderTest declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Tests functionality of SectionReader class.
 */
class SectionReaderTest : public CxxTest::TestSuite {
public:
    void testRegistration();

    void setUp();
    void tearDown();

    static void AOutReaderCalled();
    static void BOutReaderCalled();

private:
    /// Status variable for tests, declares if AOutReader was found.
    static bool aOutReaderFound;
    /// Status variable for tests, declares if BOutReader was found.
    static bool bOutReaderFound;
};

bool SectionReaderTest::aOutReaderFound = false;
bool SectionReaderTest::bOutReaderFound = false;

///////////////////////////////////////////////////////////////////////////////
// BOutSectionReader implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Returns binary reader(parent) of section reader class.
 *
 * In this test, there is no BinaryReader instances so we just 
 * return imaginary id.
 *
 * @return Non existing pointer of BinaryReader* that is used as an id.
 */
BinaryReader* 
BOutSectionReader::parent() const {
    return reinterpret_cast<BinaryReader*>(12);
}

///////////////////////////////////////////////////////////////////////////////
// BOutTextReader implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
BOutTextReader::BOutTextReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Returns type of section reader.
 *
 * @return Type of SectionReader.
 */
Section::SectionType 
BOutTextReader::type() const {
    return Section::ST_DUMMY;
}

/**
 * Test implementation of readData. 
 *
 * Calls SectionReaderTest::BOutReaderCalled() to inform test that this method
 * was runned.
 *
 * @param stream Not used.
 * @param section Not used.
 * @param bReader Id of BinaryReader which for SectionReader is registered to.
 * @param offset Not used.
 * @param length Not used.
 */
void 
BOutTextReader::readData(
    BinaryStream& /*stream*/,
    Section* /*section*/) const 
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile, 
	   OutOfRange, WrongSubclass, UnexpectedValue) {

    SectionReaderTest::BOutReaderCalled();
}

///////////////////////////////////////////////////////////////////////////////
// AOutTextReader implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
AOutTextReader::AOutTextReader() {
    SectionReader::registerSectionReader(this);
}

/**
 * Returns type of reader.
 *
 * @return Type of reader.
 */
Section::SectionType 
AOutTextReader::type() const {
    return Section::ST_DUMMY;
}

/**
 * Returns binary reader of section reader class.
 *
 * In this test, there is no BinaryReader instances so we just 
 * return imaginary id.
 *
 * @return Non existing pointer of BinaryReader* that is used as an id.
 */
BinaryReader* 
AOutTextReader::parent() const {
    return reinterpret_cast<BinaryReader*>(10);
}

/**
 * Test implementation of readData. 
 *
 * Calls SectionReaderTest::AOutReaderCalled() to inform test, that this
 * method was runned.
 *
 * @param stream Not used.
 * @param bReader Id of BinaryReader which for SectionReader is registered to.
 * @param section Not used.
 * @param offset Not used.
 * @param length Not used.
 */
void 
AOutTextReader::readData(
    BinaryStream &/*stream*/,
    Section* /*section*/) const 
    throw (UnreachableStream, KeyAlreadyExists, EndOfFile, 
	   OutOfRange, WrongSubclass, UnexpectedValue) {
    
    SectionReaderTest::AOutReaderCalled();
}

///////////////////////////////////////////////////////////////////////////////
// TestSection implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */ 
TestSection::TestSection(bool init) {
    if (init) {
	Section::registerSection(this);
    }
}

/**
 * Returns type of section.
 *
 * @return Type of Section.
 */
Section::SectionType 
TestSection::type() const {
    return Section::ST_DUMMY;
}

/**
 * Returns clone of section prototype.
 *
 * @return Newly created section.
 */
Section* 
TestSection::clone() const {
    return new TestSection(false);
}

///////////////////////////////////////////////////////////////////////////////
// SectionReaderTest implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Tests that right type reader is found for each type of section.
 */
void 
SectionReaderTest::testRegistration() {	

    Section* sect = Section::createSection(Section::ST_DUMMY);
    BinaryStream stream("data/emptyfileforopeningstream");
    try {
	SectionReader::readSection(stream, sect, 
				   reinterpret_cast<BinaryReader*>(10));
    } catch ( ... ) {
	bool readSectionThrewAnException = false;
	assert(readSectionThrewAnException);
    }
    
    TS_ASSERT(aOutReaderFound);
    TS_ASSERT(!bOutReaderFound);
    aOutReaderFound = false;
    bOutReaderFound = false;
    
    try {
	SectionReader::readSection(stream, sect, 
				   reinterpret_cast<BinaryReader*>(12));
    } catch ( ... ) {
	bool readSectionThrewAnException = false;
	assert(readSectionThrewAnException);
    }
    
    TS_ASSERT(!aOutReaderFound);
    TS_ASSERT(bOutReaderFound);
    aOutReaderFound = false;
    bOutReaderFound = false;
    
    try {
	SectionReader::readSection(stream, sect, 
				   reinterpret_cast<BinaryReader*>(12));

	SectionReader::readSection(stream, sect, 
				   reinterpret_cast<BinaryReader*>(10));
    } catch ( ... ) {
	bool readSectionThrewAnException = false;
	assert(readSectionThrewAnException);
    }
    
    TS_ASSERT(aOutReaderFound);
    TS_ASSERT(bOutReaderFound);
    
    delete sect;
}

/**
 * Setup method of test suite.
 */
void 
SectionReaderTest::setUp() {
}

/**
 * Cleanup method of test suite.
 */
void 
SectionReaderTest::tearDown() {
}

/**
 * Sets aOutReaderCalled variable true.
 */
void 
SectionReaderTest::AOutReaderCalled() {
    aOutReaderFound = true;
}

/**
 * Sets bOutReaderCalled variable true.
 */
void 
SectionReaderTest::BOutReaderCalled() {
    bOutReaderFound = true;
}

#endif
