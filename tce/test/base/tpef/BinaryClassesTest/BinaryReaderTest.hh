/**
 * @file BinaryReaderTest.hh
 *
 * Tests for BinaryReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 */

#ifndef TTA_BINARY_READER_TEST_HH
#define TTA_BINARY_READER_TEST_HH

#include <string>
#include <TestSuite.h>

#include "BinaryReader.hh"
#include "BinaryStream.hh"
#include "Binary.hh"
#include "Exception.hh"

using namespace TPEF;

///////////////////////////////////////////////////////////////////////////////
// MIReader declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Test binary reader. 
 * 
 * Reads files which starts with ox7f byte and word MI which ends with 
 * non printable character.
 */
class MIReader : public BinaryReader {
protected:
    MIReader();

    virtual Binary* readData(BinaryStream& stream) const 
	throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists, 
	       EndOfFile, OutOfRange, WrongSubclass, UnexpectedValue);

    virtual bool isMyStreamType(BinaryStream& stream) const
	throw (UnreachableStream);

private:
    /// Prototype of reader.
    static MIReader proto_;
};

MIReader MIReader::proto_;

///////////////////////////////////////////////////////////////////////////////
// MIKULIReader declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Test binary reader. 
 * 
 * Reads files which starts with ox7f byte and word MIKULI which ends with 
 * non printable character.
 */
class MIKULIReader : public BinaryReader {
protected:
    MIKULIReader();

    virtual Binary* readData(BinaryStream& stream) const 
	throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists, 
	       EndOfFile, OutOfRange, WrongSubclass, UnexpectedValue);

    virtual bool isMyStreamType(BinaryStream& stream) const
	throw (UnreachableStream);

private:
    /// Prototype of reader.
    static MIKULIReader proto_;

};

MIKULIReader MIKULIReader::proto_;

///////////////////////////////////////////////////////////////////////////////
// BinaryReaderTest declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Tests for BinaryReader class.
 */
class BinaryReaderTest : public CxxTest::TestSuite {
public:
    void testMagicRegocnition();

    void setUp();
    void tearDown();
};

Binary *MIBinary = NULL;
Binary *MIKULIBinary = NULL;

///////////////////////////////////////////////////////////////////////////////
// MIReader implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
MIReader::MIReader() {
    BinaryReader::registerBinaryReader(this);
}

/**
 * Reads BinaryStream and returns which kind of binary were read.
 * 
 * @param stream Stream that should be readed.
 * @return Non existing Binary* that is used as an id in test. 
 */
Binary* 
MIReader::readData(BinaryStream& /*stream*/) const 
    throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists, 
	   EndOfFile, OutOfRange, WrongSubclass, UnexpectedValue) {
     
    static Binary* bin = new Binary();
    MIBinary = bin;
    return bin;
}

/**
 * Checks if given stream is readable with this class.
 *
 * @param stream Stream that should be readed.
 * @return True if object is able to read stream.
 */
bool 
MIReader::isMyStreamType(BinaryStream& stream) const
    throw (UnreachableStream) {

    unsigned long streamStart = 0;

    try {
	streamStart = stream.readPosition();
	
	// check magic 
	if(stream.readByte() == 0x7f && 
	   stream.readByte() == 'M' &&
	   stream.readByte() == 'I' &&
	   !isprint(stream.readByte())){
	    
	    stream.setReadPosition(streamStart);
	    return true;
	}
    } catch (EndOfFile& e) {
	return false;
    }
    
    stream.setReadPosition(streamStart);
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// MIKULIReader implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
MIKULIReader::MIKULIReader() {
    BinaryReader::registerBinaryReader(this);
}

/**
 * Reads BinaryStream and returns which kind of binary were read.
 * 
 * @param stream Stream that should be readed.
 * @return Non existing Binary* that is used as an id in test. 
 */
Binary* 
MIKULIReader::readData(BinaryStream& /*stream*/) const 
    throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists, 
	   EndOfFile, OutOfRange, WrongSubclass, UnexpectedValue) {
    
    static Binary* bin = new Binary();
    MIKULIBinary = bin;
    return bin;
}

/**
 * Checks if given stream is readable with this class.
 *
 * @param stream Stream that should be readed.
 * @return True if object is able to read stream.
 */
bool 
MIKULIReader::isMyStreamType(BinaryStream& stream) const
    throw (UnreachableStream) {

    unsigned long streamStart = 0;
    
    try {
	streamStart = stream.readPosition();
	// check magic 
	if(stream.readByte() == 0x7f && 
	   stream.readByte() == 'M' &&
	   stream.readByte() == 'I' &&
	   stream.readByte() == 'K' &&
	   stream.readByte() == 'U' &&
	   stream.readByte() == 'L' &&
	   stream.readByte() == 'I' &&
	   !isprint(stream.readByte())){
	    
	    stream.setReadPosition(streamStart);
	    return true;
	}
    } catch (EndOfFile& e) {
	return false;
    }

    stream.setReadPosition(streamStart);
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// BinaryReaderTest implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Tests that right BinaryReader is found when trying to read binary file.
 *
 * In this test case there is registered two different readers
 * MI and MIKULI readers. Test tries to read MIKULI, MI and POKS
 * type of files. Test also tries to read file that doesn't have
 * magic number defined (no 7f byte in start of file that marks start of 
 * magic).
 */
void 
BinaryReaderTest::testMagicRegocnition() {
    
    BinaryStream stream1("data/type.POKS");
    TS_ASSERT_THROWS(BinaryReader::readBinary(stream1),
		     InstanceNotFound);
    
    BinaryStream stream2("data/type.MI");
    TS_ASSERT(BinaryReader::readBinary(stream2) == MIBinary);
    
    BinaryStream stream3("data/type.MIKULI");
    TS_ASSERT(BinaryReader::readBinary(stream3) == MIKULIBinary);
    
    BinaryStream stream4("data/type.NOMAGIC");
    TS_ASSERT_THROWS(BinaryReader::readBinary(stream4),
		     InstanceNotFound);
    
    delete MIBinary;
    delete MIKULIBinary;
}

/**
 * Initialization code of test suite.
 */
void 
BinaryReaderTest::setUp() {
}

/**
 * Cleanup code of test suite.
 */
void 
BinaryReaderTest::tearDown() {
}

#endif
