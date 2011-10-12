/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file BinaryStreamTest.hh
 *
 * A test suite for BinaryStream.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note reviewed 8 August 2003 by pj, am, jn, tr, rl
 */

#ifndef TTA_BINARYSTREAMTEST_HH
#define TTA_BINARYSTREAMTEST_HH

#include <iomanip>
using std::hex;
#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <TestSuite.h>
#include "BaseType.hh"
#include "BinaryStream.hh"
#include "Swapper.hh"
#include "Exception.hh"
#include "Conversion.hh"

using namespace TPEF;

/**
 * Implements the tests needed to verify correctness of BinaryStream.
 */
class BinaryStreamTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testBadFilename();
    void testByteOrder();
    void testReadSingle();
    void testReadBlock();
    void testWriteSingle();
    void testWriteBlock();
    void testReadPos();
    void testWritePos();
    void testSeekBeyondEof();
    void testFileSize();
    
private:
    /// Test binary stream.
    BinaryStream* stream_;

    /// Input file for testing reading.
    static const string readTestFile_;
    /// Output file to test writing.
    static const string writeTestFile_;

    /// Used to tell if additional cleanup is needed.
    bool writeTest_;

    /// Last cursor position in des3.seq
    static const unsigned int eofPos_;

    /// Correct byte values from des3.seq.
    static const Byte des3byte_[4];
    /// Correct half-word values from des3.seq.
    static const HalfWord des3halfword_[4];
    /// Correct word values from des3.seq.
    static const Word des3word_[4];
};


const string BinaryStreamTest::readTestFile_ =
"data/des3.seq";
const string BinaryStreamTest::writeTestFile_ =
"data/writetest";

const unsigned int BinaryStreamTest::eofPos_ = 229612;

const Byte BinaryStreamTest::des3byte_[4] = {
    0x00, 0x00, 0x01, 0x07,
};

const HalfWord BinaryStreamTest::des3halfword_[4] = {
    0x0001, 0x0e28, 0x0000, 0x0d30
};

const Word BinaryStreamTest::des3word_[4] = {
    0x000006b0, 0x00010278, 0x00000000, 0x0000318c
};

/**
 * Called before each test.
 */
inline void
BinaryStreamTest::setUp() {
    writeTest_ = false;
}


/**
 * Cleans up memory allocations and created files after each test.
 */
inline void
BinaryStreamTest::tearDown() {
    if (stream_ != 0) {
        delete stream_;
    }
    if (writeTest_) {
        string clean = "rm -f "+writeTestFile_;
        if (system(clean.c_str()) == -1)
            abort();
    }
}


/**
 * Tests that the correct exception is thrown when reading from
 * file fails because of a bad filename.
 */
inline void
BinaryStreamTest::testBadFilename() {
    Byte byteBuff[10];
    HalfWord hwBuff[10];
    Word wBuff[10];

    stream_ = new BinaryStream("badfilename");
    TS_ASSERT_THROWS(stream_->readByte(), UnreachableStream);
    TS_ASSERT_THROWS(stream_->readHalfWord(), UnreachableStream);
    TS_ASSERT_THROWS(stream_->readWord(), UnreachableStream);

    TS_ASSERT_THROWS(stream_->readByteBlock(byteBuff, 10), 
		     UnreachableStream);

    TS_ASSERT_THROWS(stream_->readHalfWordBlock(hwBuff, 10), 
		     UnreachableStream);

    TS_ASSERT_THROWS(stream_->readWordBlock(wBuff, 10), 
		     UnreachableStream);       
}

/**
 * Tests that the byte order is right when writing 
 * words and half words.
 */
inline void
BinaryStreamTest::testByteOrder() {
    Word wordToWrite = 0x11223344;
    HalfWord halfWordToWrite = 0x1122;

    stream_ = new BinaryStream("data/byteordertest.bin");

    stream_->writeWord(wordToWrite);
    stream_->writeHalfWord(halfWordToWrite);
    
    TS_ASSERT_EQUALS(stream_->readByte(), 0x11);
    TS_ASSERT_EQUALS(stream_->readByte(), 0x22);
    TS_ASSERT_EQUALS(stream_->readByte(), 0x33);
    TS_ASSERT_EQUALS(stream_->readByte(), 0x44);
   
    TS_ASSERT_EQUALS(stream_->readByte(), 0x11);
    TS_ASSERT_EQUALS(stream_->readByte(), 0x22);
}


/**
 * Tests that correct data is read from the input file by using
 * read-functions.
 */
inline void
BinaryStreamTest::testReadSingle() {
    try {
        stream_ = new BinaryStream(readTestFile_);

        const unsigned int BLOCK_SIZE = 4;
        
        for (unsigned i = 0; i < BLOCK_SIZE; i++) {
            Byte byte = stream_->readByte();
            TS_ASSERT_EQUALS(byte, des3byte_[i]);
        }
        
        for (unsigned i = 0; i < BLOCK_SIZE; i++) {
            HalfWord halfWord = stream_->readHalfWord();
            TS_ASSERT_EQUALS(halfWord, des3halfword_[i]);
        }
        
        for (unsigned i = 0; i < BLOCK_SIZE; i++) {
            Word word = stream_->readWord();
            TS_ASSERT_EQUALS(word, des3word_[i]);
        }
        
    } catch (const UnreachableStream& error) {
        TS_FAIL(error.fileName() + ":" +
                Conversion::toString(error.lineNum()) + ":" +
                error.procedureName() + ":" + "Error reading file: " +
                error.errorMessage());
    }
    
}


/**
 * Tests that correct data is read from the input file by using
 * readBlock-functions.
 */
inline void
BinaryStreamTest::testReadBlock() {
    try {
        stream_ = new BinaryStream(readTestFile_);
        
        const unsigned int BLOCK_SIZE = 4;

        Byte bytes[BLOCK_SIZE];
        stream_->readByteBlock(bytes, BLOCK_SIZE);
        for (unsigned int i = 0; i < BLOCK_SIZE; i++) {
            TS_ASSERT_EQUALS(bytes[i], des3byte_[i]);
        }
        
        HalfWord halfWords[BLOCK_SIZE];;
        stream_->readHalfWordBlock(halfWords, BLOCK_SIZE);
        for (unsigned int i = 0; i < BLOCK_SIZE; i++) {
            TS_ASSERT_EQUALS(halfWords[i], des3halfword_[i]);
        }
        
        Word words[BLOCK_SIZE];
        stream_->readWordBlock(words, BLOCK_SIZE);
        for (unsigned int i = 0; i < BLOCK_SIZE; i++) {
            TS_ASSERT_EQUALS(words[i], des3word_[i]);
        }
        
    } catch (const UnreachableStream& error) {
        TS_FAIL(error.fileName() + ":" +
                Conversion::toString(error.lineNum()) + ":" +
                error.procedureName() + ":" + "Error reading file: " +
                error.errorMessage());
    }
}


/**
 * Writes single data items to file, reads them and compares the
 * results.
 */
inline void
BinaryStreamTest::testWriteSingle() {
    writeTest_ = true;
    try {
        Byte byte = 0x43;
        HalfWord halfWord = 0xAB34;
        Word word = 0x3B1285FB;
        
        stream_ = new BinaryStream(writeTestFile_);
        
        stream_->writeByte(byte);
        stream_->writeHalfWord(halfWord);
        stream_->writeWord(word);

        // close the stream in between to make sure the data is really
        // there

        delete stream_;
        stream_ = new BinaryStream(writeTestFile_);

        TS_ASSERT_EQUALS(stream_->readByte(), byte);
        TS_ASSERT_EQUALS(stream_->readHalfWord(), halfWord);
        TS_ASSERT_EQUALS(stream_->readWord(), word);
        
    } catch (const UnreachableStream& error) {
        TS_FAIL(error.fileName() + ":" +
                Conversion::toString(error.lineNum()) + ":" +
                error.procedureName() + ":" + "Error accessing file: " +
                error.errorMessage());
    }
}


/**
 * Writes data to a file using writeBlock-functions, reads it and
 * compares the results.
 */
inline void
BinaryStreamTest::testWriteBlock() {
    writeTest_ = true;
    
    const unsigned int BLOCK_SIZE = 5;

    Byte wByteBlock[BLOCK_SIZE];
    Byte rByteBlock[BLOCK_SIZE];
    HalfWord wHalfWordBlock[BLOCK_SIZE];
    HalfWord rHalfWordBlock[BLOCK_SIZE];
    Word wWordBlock[BLOCK_SIZE];
    Word rWordBlock[BLOCK_SIZE];
    
    try {
        stream_ = new BinaryStream(writeTestFile_);
        
        wByteBlock[0] = 0x23;
        wByteBlock[1] = 0x10;
        wByteBlock[2] = 0x30;
        wByteBlock[3] = 0xAC;
        wByteBlock[4] = 0xFF;
	
        stream_->writeByteBlock(wByteBlock, BLOCK_SIZE);

        stream_->readByteBlock(rByteBlock, BLOCK_SIZE);
        for (unsigned int i = 0; i < BLOCK_SIZE; i++) {
            TS_ASSERT_EQUALS(rByteBlock[i], wByteBlock[i]);
        }

        wHalfWordBlock[0] = 0x04BA;
        wHalfWordBlock[1] = 0x12CD;
        wHalfWordBlock[2] = 0xABCD;
        wHalfWordBlock[3] = 0xFFFF;
        wHalfWordBlock[4] = 0x1223;

        stream_->writeHalfWordBlock(wHalfWordBlock, BLOCK_SIZE);

	unsigned int currentReadPos = stream_->readPosition();

	delete stream_;
        stream_ = new BinaryStream(writeTestFile_);

	stream_->setReadPosition(currentReadPos);

        stream_->readHalfWordBlock(rHalfWordBlock, BLOCK_SIZE);
        for (unsigned int i = 0; i < BLOCK_SIZE; i++) {
            TS_ASSERT_EQUALS(rHalfWordBlock[i], wHalfWordBlock[i]);
        }

	delete stream_;
        stream_ = new BinaryStream(writeTestFile_);

        wWordBlock[0] = 0x12345678;
        wWordBlock[1] = 0x1B01A844;
        wWordBlock[2] = 0x42000004;
        wWordBlock[3] = 0xFFFFFFFF;
        wWordBlock[4] = 0xDEADBEEF;
        
        stream_->writeWordBlock(wWordBlock, BLOCK_SIZE);

        stream_->readWordBlock(rWordBlock, BLOCK_SIZE);
        for (unsigned int i = 0; i < BLOCK_SIZE; i++) {
            TS_ASSERT_EQUALS(rWordBlock[i], wWordBlock[i]);
        }
        
    } catch (const UnreachableStream& error) {
        TS_FAIL(error.fileName() + ":" +
                Conversion::toString(error.lineNum()) + ":" +
                error.procedureName() + ":" + "Error accessing file: " +
                error.errorMessage());
    }
}


/**
 * Tests endOfFile, readPosition and setReadPosition.
 */
inline void
BinaryStreamTest::testReadPos() {
    try {
        stream_ = new BinaryStream(readTestFile_);
        
        TS_ASSERT(!stream_->endOfFile());
        TS_ASSERT_EQUALS(static_cast<int>(stream_->readPosition()), 0);

        const unsigned int TEST_POS = 20;

        stream_->setReadPosition(TEST_POS);
        TS_ASSERT_EQUALS(stream_->readPosition(), TEST_POS);
        stream_->readByte();
        TS_ASSERT_EQUALS(stream_->readPosition(), TEST_POS + 1);
        
        // check if end of file can be found by reading the stream
        // to the end and that UnreachableStream is thrown if
        // reading is continued

        const unsigned int EOF_OFFSET = 10;

        stream_->setReadPosition(eofPos_ - EOF_OFFSET);

        for (Word i = 0; i < EOF_OFFSET + 1; i++) {
            stream_->readByte();
        }
        TS_ASSERT(stream_->endOfFile());
        TS_ASSERT_THROWS(stream_->readByte(), EndOfFile);
        TS_ASSERT_THROWS(stream_->readHalfWord(), EndOfFile);
        TS_ASSERT_THROWS(stream_->readWord(), EndOfFile);

	Byte byteBuff[10];
	HalfWord hwBuff[10];
	Word wBuff[10];

        TS_ASSERT_THROWS(stream_->readByteBlock(byteBuff, 10), EndOfFile);
        TS_ASSERT_THROWS(stream_->readHalfWordBlock(hwBuff, 10), EndOfFile);
        TS_ASSERT_THROWS(stream_->readWordBlock(wBuff, 10), EndOfFile);
        
        // check that eof-flag is correctly changed when changing
        // read position

        stream_->setReadPosition(eofPos_ - EOF_OFFSET);
        TS_ASSERT(!stream_->endOfFile());
        stream_->setReadPosition(eofPos_ + EOF_OFFSET);
        TS_ASSERT(!stream_->endOfFile());
        stream_->readByte();
        TS_ASSERT(stream_->endOfFile());
        stream_->setReadPosition(eofPos_ - EOF_OFFSET);
        TS_ASSERT(!stream_->endOfFile());
        
    } catch (const UnreachableStream& error) {
        TS_FAIL(error.fileName() + ":" +
                Conversion::toString(error.lineNum()) + ":" +
                error.procedureName() + ":" + "Error reading file: " +
                error.errorMessage());
    }
}


/**
 * Tests writePosition and setWritePosition.
 */
inline void
BinaryStreamTest::testWritePos() {
    writeTest_ = true;

    const unsigned int BLOCK_SIZE = 3;
    Byte bytes[BLOCK_SIZE];
    bytes[0] = 0x3B;
    bytes[1] = 0x66;
    bytes[2] = 0x01;
        
    try {
        stream_ = new BinaryStream(writeTestFile_);
        const unsigned int startPos = 0;
        TS_ASSERT_EQUALS(stream_->writePosition(), startPos);
        stream_->writeByte(0);
        TS_ASSERT_EQUALS(stream_->writePosition(), startPos + 1);
        stream_->writeByteBlock(bytes, BLOCK_SIZE);
        TS_ASSERT_EQUALS(stream_->writePosition(), startPos + BLOCK_SIZE + 1);

    } catch (const UnreachableStream& error) {
        TS_FAIL(error.fileName() + ":" +
                Conversion::toString(error.lineNum()) + ":" +
                error.procedureName() + ":" + "Error accessing file: " +
                error.errorMessage());
    }
}


/**
 * Tests that writing beyond eof causes an exception.
 */
inline void
BinaryStreamTest::testSeekBeyondEof() {
    writeTest_ = true;

    try {
        stream_ = new BinaryStream(writeTestFile_);

        const unsigned int BLOCKS = 4;

        for (unsigned int i = 0; i < BLOCKS; i++) {
            stream_->writeByte(i);
        }

        const unsigned int EOF_POS = stream_->writePosition();

        stream_->setWritePosition(EOF_POS + 1);
        TS_ASSERT_THROWS(stream_->writeByte(0x69), WritePastEOF);
        TS_ASSERT_THROWS(stream_->writeByte(0x69), WritePastEOF);
        TS_ASSERT_THROWS(stream_->writeHalfWord(0x69), WritePastEOF);
        TS_ASSERT_THROWS(stream_->writeWord(0x69), WritePastEOF);

	Byte byteBuff[10];
	HalfWord hwBuff[10];
	Word wBuff[10];
        TS_ASSERT_THROWS(stream_->writeByteBlock(byteBuff, 10), WritePastEOF);
        TS_ASSERT_THROWS(
	    stream_->writeHalfWordBlock(hwBuff, 10), WritePastEOF);
        TS_ASSERT_THROWS(stream_->writeWordBlock(wBuff, 10), WritePastEOF);

    } catch (const UnreachableStream& error) {
        TS_FAIL(error.fileName() + ":" +
                Conversion::toString(error.lineNum()) + ":" +
                error.procedureName() + ":" + "Error accessing file: " +
                error.errorMessage());
    }
}


/**
 * Tests that the size of the file is read correctly, and that the
 * stream is in the same position than after asking the size of the
 * file. Also tests that asking the size of a file that doesn't exists
 * throws exception.
 */
inline void
BinaryStreamTest::testFileSize() {
    try {
        stream_ = new BinaryStream(readTestFile_);
        unsigned int pos = stream_->readPosition();
        TS_ASSERT_EQUALS(stream_->sizeOfFile(), eofPos_);
        TS_ASSERT_EQUALS(pos, stream_->readPosition());
	delete stream_;
	stream_ = new BinaryStream("filethatdoesntexist");
	TS_ASSERT_THROWS(stream_->sizeOfFile(), UnreachableStream);
    } catch (const UnreachableStream& error) {
        TS_FAIL(error.fileName() + ":" +
                Conversion::toString(error.lineNum()) + ":" +
                error.procedureName() + ":" + "Error reading file: " +
                error.errorMessage());
    }
} 

#endif
