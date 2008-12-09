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
 * @file BinaryStream.cc
 *
 * Implementation of BinaryStream class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @author Mikael Lepistö 2005 (mikael.lepisto-no.spam-tut.fi)
 * @note reviewed 7 August 2003 by pj, am, jn, ao, rl
 *
 * @note rating: yellow
 */

#include <string>
#include <fstream>
#include <cassert>
#include <boost/format.hpp>

#include "BinaryStream.hh"
#include "Swapper.hh"

using std::fstream;
using std::ios;

namespace TPEF {

BinaryStream::BinaryStream(std::ostream& stream): 
    fileName_(""), extOStream_(&stream) {
}

/**
 * Prepares the binary stream so that it can be read from / written to.
 *
 * The input string is used as the name of the file to open (if
 * existing) or to create (if not existing).
 *
 * @param name is the name of the input file.
 * @note The initial read and write positions of the stream are 0.
 */
BinaryStream::BinaryStream(std::string name): 
    fileName_(name), extOStream_(NULL) {
}


/**
 * Closes the stream.
 *
 * No other cleanup or deallocation activity is required.
 * 
 * @note Only way of closing and flushing the actual output stream can be done
 * be calling the destructor (through deletion of the object).
 */
BinaryStream::~BinaryStream() {
    close();
}


/**
 * Reads one Byte from the binary stream.
 *
 * @return The next 8-bit byte from the stream.
 * @exception UnreachableStream If reading from the stream fails.
 * @exception EndOfFile If end of file were reached.
 */
Byte
BinaryStream::readByte()
    throw (UnreachableStream, EndOfFile) {

    Byte value;

    try {
        value = getByte();

    } catch (const EndOfFile& error) {
        EndOfFile newException =
            EndOfFile(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }

    return value;
}


/**
 * Reads one HalfWord from the binary stream.
 *
 * @return The next 16-bit word (half-word) from the stream.
 * @exception UnreachableStream If reading from the stream fails.
 * @exception EndOfFile If end of file were reached.
 */
HalfWord
BinaryStream::readHalfWord()
        throw (UnreachableStream, EndOfFile) {

    Byte buffer[sizeof(HalfWord)];

    try {
        readByteBlock(buffer, sizeof(HalfWord));

    } catch (const EndOfFile& error) {
        EndOfFile newException =
            EndOfFile(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }

    // convert half-word to host endianess
    HalfWord result;
    Swapper::fromBigEndianToHostByteOrder(buffer, result);

    return result;
}


/**
 * Reads one Word from the binary stream.
 *
 * @return The next 32-bit word from the stream.
 * @exception UnreachableStream If reading from the stream fails.
 * @exception EndOfFile If end of file were reached.
 */
Word
BinaryStream::readWord()
    throw (UnreachableStream, EndOfFile) {

    Byte buffer[sizeof(Word)];

    try {
        readByteBlock(buffer, sizeof(Word));

    } catch (const EndOfFile& error) {
        EndOfFile newException =
            EndOfFile(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }

    // convert word to host endianess
    Word result;
    Swapper::fromBigEndianToHostByteOrder(buffer, result);

    return result;
}


/**
 * Reads a block of Bytes from the binary stream.
 *
 * @param buffer is where the bytes are returned.
 * @param howmany is how many bytes are supposed to be read.
 * @exception UnreachableStream If reading from the stream fails.
 * @exception EndOfFile If end of file were reached.
 */
void
BinaryStream::readByteBlock(Byte* buffer, unsigned int howmany)
    throw (UnreachableStream, EndOfFile) {

    try {
        for (unsigned int i = 0; i < howmany; i++) {
            buffer[i] = getByte();
        }

    } catch (const EndOfFile& error) {
        EndOfFile newException =
            EndOfFile(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }
}


/**
 * Reads a block of HalfWords from the binary stream.
 *
 * @param buffer is where the halfwords are returned.
 * @param howmany is how many halfwords are supposed to be read.
 * @exception UnreachableStream If reading from the stream fails.
 * @exception EndOfFile If end of file were reached.
 */
void
BinaryStream::readHalfWordBlock(HalfWord* buffer, unsigned int howmany)
    throw (UnreachableStream, EndOfFile) {

    try {
        for (unsigned int i = 0; i < howmany; i++) {
            buffer[i] = readHalfWord();
        }

    } catch (const EndOfFile& error) {
        EndOfFile newException =
            EndOfFile(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }
}


/**
 * Reads a block of Words from the binary stream.
 *
 * @param buffer is where the words are returned.
 * @param howmany is how many words are supposed to be read.
 * @exception UnreachableStream If reading from the stream fails.
 * @exception EndOfFile If end of file were reached.
 */
void
BinaryStream::readWordBlock(Word* buffer, unsigned int howmany)
    throw (UnreachableStream, EndOfFile) {

    try {
        for (unsigned int i = 0; i < howmany; i++) {
            buffer[i] = readWord();
        }
    } catch (const EndOfFile& error) {
        EndOfFile newException =
            EndOfFile(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }
}


/**
 * Writes one Byte to the binary stream.
 *
 * @param byte The Byte to write to the stream.
 * @exception UnreachableStream If writing to stream fails.
 * @exception WritePastEOF If tried to write past end of file.
 */
void
BinaryStream::writeByte(Byte byte)
    throw (UnreachableStream, WritePastEOF) {

    try {
        putByte(byte);

    } catch (const WritePastEOF& error) {
        WritePastEOF newException =
            WritePastEOF(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }
}


/**
 * Writes one HalfWord to the binary stream.
 *
 * @param halfword The HalfWord to write to the stream.
 * @exception UnreachableStream If writing to stream fails.
 * @exception WritePastEOF If tried to write past end of file.
 */
void
BinaryStream::writeHalfWord(HalfWord halfword)
    throw (UnreachableStream, WritePastEOF) {

    Byte buffer[sizeof(HalfWord)];

    // convert HalfWord to big-endian byte order.
    Swapper::fromHostToBigEndianByteOrder(halfword, buffer);

    try {
        writeByteBlock(buffer, sizeof(HalfWord));

    } catch (const WritePastEOF& error) {
        WritePastEOF newException =
            WritePastEOF(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }
}


/**
 * Writes one Word to the binary stream.
 *
 * @param word The Word to write to the stream.
 * @exception UnreachableStream If writing to stream fails.
 * @exception WritePastEOF If tried to write past end of file.
 */
void
BinaryStream::writeWord(Word word)
    throw (UnreachableStream, WritePastEOF) {

    Byte buffer[sizeof(Word)];

    // convert Word to big-endian byte order.
    Swapper::fromHostToBigEndianByteOrder(word, buffer);

    try {
        writeByteBlock(buffer, sizeof(Word));

    } catch (const WritePastEOF& error) {
        WritePastEOF newException =
            WritePastEOF(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }
}


/**
 * Writes a block of Bytes to the binary stream.
 *
 * @param bytes The block of Bytes to be written.
 * @param howmany How many Bytes to write.
 * @exception UnreachableStream If writing to stream fails.
 * @exception WritePastEOF If tried to write past end of file.
 */
void
BinaryStream::writeByteBlock(Byte* bytes, unsigned int howmany)
    throw (UnreachableStream, WritePastEOF) {

    try {
        for (unsigned int i = 0; i < howmany; i++) {
            putByte(bytes[i]);
        }
    } catch (const WritePastEOF& error) {
        WritePastEOF newException =
            WritePastEOF(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }
}


/**
 * Writes a block of HalfWords to the binary stream.
 *
 * @param halfwords The block of HalfWords to be written.
 * @param howmany How many HalfWords to write.
 * @exception UnreachableStream If writing to stream fails.
 * @exception WritePastEOF If tried to write past end of file.
 */
void
BinaryStream::writeHalfWordBlock(HalfWord* halfwords, unsigned int howmany)
    throw (UnreachableStream, WritePastEOF) {

    try {
        for (unsigned int i = 0; i < howmany; i++) {
            writeHalfWord(halfwords[i]);
        }
    } catch (const WritePastEOF& error) {
        WritePastEOF newException =
            WritePastEOF(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }
}


/**
 * Writes a block of Words to the binary stream.
 *
 * @param words The block of Words to be written.
 * @param howmany How many Words to write.
 * @exception UnreachableStream If writing to stream fails.
 * @exception WritePastEOF If tried to write past end of file.
 */
void
BinaryStream::writeWordBlock(Word* words, unsigned int howmany)
    throw (UnreachableStream, WritePastEOF) {

    try {
        for (unsigned int i = 0; i < howmany; i++) {
            writeWord(words[i]);
        }
    } catch (const WritePastEOF& error) {
        WritePastEOF newException =
            WritePastEOF(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;

    } catch (const UnreachableStream& error) {
        UnreachableStream newException =
            UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
        newException.setCause(error);
        throw newException;
    }
}


/**
 * Opens the binary file for input.
 *
 * @param name Name of the input file.
 * @exception UnreachableStream If file is not found or is unreadable.
 * @note The initial read position is 0.
 */
void
BinaryStream::openInput(std::string name)
    throw (UnreachableStream) {

    if (extOStream_ != NULL) {
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, "External stream is write-only.");
    }

    iStream_.open(name.c_str());

    if (!iStream_.is_open()) {
        const std::string error = (boost::format(
            "File '%s' could not be opened for input.") % name).str();
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, error);
    }
    if (oStream_.is_open()) {
        oStream_.flush();
    }
    iStream_.tie(&oStream_);
}


/**
 * Opens the binary file for output.
 *
 * If a file doesn't exist, a new empty file will be created.
 *
 * @param name Name of the output file.
 * @exception UnreachableStream If file cannot be opened.
 * @note The initial write position is 0.
 */
void
BinaryStream::openOutput(std::string name)
    throw (UnreachableStream) {
    
    if (extOStream_ != NULL) {
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, 
            "External stream should be always open.");
    }

    oStream_.open(name.c_str(), fstream::out);

    // With some versions of STL a non-existing file is not
    // automatically created when using only 'out'-flag. In that case,
    // we have to open the stream in truncate mode to force the creating
    // of the empty file.
    if (!oStream_.is_open()) {
        oStream_.clear();
        oStream_.open(name.c_str(), fstream::out | fstream::trunc);
    }

    if (!oStream_.is_open()) {
        const std::string error = (boost::format(
             "File '%s' could not be opened for output.") % fileName_).str();        
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, error);
    }
}


/**
 * Closes the stream.
 */
void
BinaryStream::close() {
    if (iStream_.is_open()) {
        iStream_.close();
    }
    if (oStream_.is_open()) {
        oStream_.close();
    }
}


/**
 * Returns the current position of the read cursor.
 *
 * @exception UnreachableStream If stream is bad or otherwise
 *            unreachable.
 */
unsigned int
BinaryStream::readPosition()
    throw (UnreachableStream) {
    
    if (extOStream_ != NULL) {
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, "External stream is write-only.");
    }

    if (!iStream_.is_open()) {
        try {
            openInput(fileName_);
        } catch (const UnreachableStream& error) {
            UnreachableStream newException =
                UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
            newException.setCause(error);
            throw newException;
        }
    }

    if (iStream_.bad()) {
        throw UnreachableStream(__FILE__, __LINE__,
                                "BinaryStream::readPosition", fileName_);
    }
    return iStream_.tellg();
}


/**
 * Returns the current position of the write cursor.
 *
 * @exception UnreachableStream If stream is bad or otherwise
 *            unreachable.
 */
unsigned int
BinaryStream::writePosition()
    throw (UnreachableStream) {

    if (extOStream_ != NULL) {
        return extOStream_->tellp();
    }

    if (!oStream_.is_open()) {
        try {
            openOutput(fileName_);

        } catch (const UnreachableStream& error) {
            UnreachableStream newException =
                UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
            newException.setCause(error);
            throw newException;
        }
    }

    if (oStream_.bad()) {
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, fileName_);
    }
    return oStream_.tellp();
}


/**
 * Sets the read cursor position in the stream.
 *
 * If the stream has reached end-of-file, and read position is then
 * set before eof, eof-status is automatically cleared and reading is
 * again possible. Setting position beyond eof doesn't immediately set
 * eof-status, instead, it is only set after trying to read past eof.
 *
 * @param position New read cursor position.
 * @exception UnreachableStream If stream is bad or otherwise
 *            unreachable.
 */
void
BinaryStream::setReadPosition(unsigned int position)
    throw (UnreachableStream) {

    if (extOStream_ != NULL) {
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, "External stream is write-only.");
    }

    if (!iStream_.is_open()) {
        try {
            openInput(fileName_);

        } catch (const UnreachableStream& error) {
            UnreachableStream newException =
                UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
            newException.setCause(error);
            throw newException;
        }
    }
    if (iStream_.bad()) {
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, fileName_);
    }

    // if eof is already reached, the stream must be cleared to make it
    // accessible again
    bool eof = false;
    if (iStream_.eof()) {
        eof = true;
        iStream_.clear();
    }

    // possible eof-status is cleared if the position is set before eof
    iStream_.seekg(0, ios::end);
    unsigned int fileSize = iStream_.tellg();
    if (position <= fileSize) {
        iStream_.clear();
        eof = false;
    }

    iStream_.seekg(position);

    // return the eof status if it has been cleared earlier to access the
    // stream and read position is not set before eof
    if (eof) {
        iStream_.setstate(ios::eofbit);
    }
}


/**
 * Sets the write cursor position in the stream.
 *
 * If write position is located past the end of the file and writing
 * to the stream is attempted, WritePastEOF exception will be
 * thrown.
 *
 * @see putByte
 * @param position New write cursor position.
 * @exception UnreachableStream If stream is bad or otherwise
 *            unreachable.
 */
void
BinaryStream::setWritePosition(unsigned int position)
    throw (UnreachableStream) {

    if (extOStream_ != NULL) {
        extOStream_->seekp(position);    
        return;
    }

    if (!oStream_.is_open()) {
        try {
            openOutput(fileName_);

        } catch (const UnreachableStream& error) {
            UnreachableStream newException =
                UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
            newException.setCause(error);
            throw newException;
        }
    }

    if (oStream_.bad()) {
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, fileName_);
    }

    oStream_.seekp(position);
}


/**
 * Returns true if read position is at the end of file.
 *
 * @exception UnreachableStream If stream is bad or otherwise
 *            unreachable.
 */
bool
BinaryStream::endOfFile()
    throw (UnreachableStream) {

    if (extOStream_ != NULL) {
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, "External stream is write-only.");
    }

    if (!iStream_.is_open()) {
        try {
            openInput(fileName_);

        } catch (const UnreachableStream& error) {
            UnreachableStream newException =
                UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
            newException.setCause(error);
            throw newException;
        }
    }

    if (iStream_.bad()) {
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, fileName_);
    }

    return iStream_.eof();
}


/**
 * Checks the size of the file being handled.
 *
 * First tries to check the size from output stream, if it's not opened,
 * tries to check from input stream.
 *
 * @exception UnreachableStream If stream is bad or file does not exist.
 *
 * @note UnreachableStream is also generated if file is existing with
 *       only write rights and it is not yet opened for writing. This is
 *       because of restricted file handling abilities of C++ standard
 *       library.
 *
 * @return The size of the file.
 */
unsigned int
BinaryStream::sizeOfFile()
    throw (UnreachableStream) {
    
    if (extOStream_ != NULL) {
        unsigned int currentPos =  extOStream_->tellp();
        extOStream_->seekp(0, ios::end);
        unsigned int fileSize = extOStream_->tellp();
        extOStream_->seekp(currentPos, ios::end);
        return fileSize;
    }

    if (oStream_.is_open()) {
        unsigned int currentPos = writePosition();
        oStream_.seekp(0, ios::end);
        unsigned int fileSize = oStream_.tellp();
        setWritePosition(currentPos);
        return fileSize;

    } else if (!iStream_.is_open()) {
        try {
            openInput(fileName_);

        } catch (const UnreachableStream& error) {
            UnreachableStream newException =
                UnreachableStream(__FILE__, __LINE__, __func__, fileName_);
            newException.setCause(error);
            throw newException;
        }
    }

    if (iStream_.bad()) {
        throw UnreachableStream(
            __FILE__, __LINE__, __func__, fileName_);
    }

    unsigned int currentPos = readPosition();
    iStream_.seekg(0, ios::end);
    unsigned int fileSize = iStream_.tellg();
    setReadPosition(currentPos);
    return fileSize;
}

}
