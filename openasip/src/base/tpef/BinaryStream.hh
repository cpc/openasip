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
 * @file BinaryStream.hh
 *
 * Declaration of BinaryStream class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note reviewed 7 August 2003 by pj, am, jn, ao, rl
 *
 * @note rating: yellow
 */

#ifndef TTA_BINARYSTREAM_HH
#define TTA_BINARYSTREAM_HH

#include <fstream>
#include <string>
#include "TPEFBaseType.hh"
#include "Exception.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

/**
 * Abstracts the input/output binary stream used to read and write TTA
 * programs.
 *
 * It takes care of opening and closing the streams automatically, and
 * hides possible byte order mismatch.
 *
 * The bits read from the stream are converted to the byte order of
 * the host machine. Conversely, the bits written into the stream will
 * be converted to the standard byte order of TTA Program Exchange
 * Format files which adheres to the byte order of the ADF the TPEF
 * is associated with (by default big endian).
 */
class BinaryStream {
public:
    BinaryStream(std::ostream &stream, bool littleEndian=false);
    BinaryStream(std::string name, bool littleEndian=false);
    virtual ~BinaryStream();

    Byte readByte();
    HalfWord readHalfWord();
    Word readWord();

    void readByteBlock(Byte* buffer, unsigned int howmany);
    void readHalfWordBlock(HalfWord* buffer, unsigned int howmany);
    void readWordBlock(Word* buffer, unsigned int howmany);

    void writeByte(Byte byte);
    void writeHalfWord(HalfWord halfword);
    void writeWord(Word word);

    void writeByteBlock(Byte* bytes, unsigned int howmany);
    void writeHalfWordBlock(HalfWord* hwords, unsigned int howmany);
    void writeWordBlock(Word* words, unsigned int howmany);

    unsigned int readPosition();
    unsigned int writePosition();
    void setReadPosition(unsigned int position);
    void setWritePosition(unsigned int position);
    bool endOfFile();
    unsigned int sizeOfFile();

    void setTPEFVersion(TPEFHeaders::TPEFVersion version);
    TPEFHeaders::TPEFVersion TPEFVersion() const;

private:
    /// The input stream.
    std::ifstream iStream_;
    /// The output stream.
    std::ofstream oStream_;
    /// The name of the stream.
    std::string fileName_;

    /// Externally given output stream.
    std::ostream* extOStream_;

    /// In case we want to store the words in little endian order,
    /// big endian otherwise.
    bool littleEndianStorage_;

    /// Indicates TPEF format version used.
    TPEFHeaders::TPEFVersion tpefVersion_;

    /// Assignment not allowed.
    BinaryStream& operator=(BinaryStream& old);
    /// Copying not allowed.
    BinaryStream(BinaryStream& old);

    void openInput(std::string name);
    void openOutput(std::string name);
    void close();
    Byte getByte();
    void putByte(Byte byte);

    bool needsSwap() const;
};
}

#include "BinaryStream.icc"

#endif
