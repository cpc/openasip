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
 * Format files.
 */
class BinaryStream {
public:
    BinaryStream(std::ostream &stream);
    BinaryStream(std::string name);
    virtual ~BinaryStream();

    Byte readByte() throw (UnreachableStream, EndOfFile);
    HalfWord readHalfWord() throw (UnreachableStream, EndOfFile);
    Word readWord() throw (UnreachableStream, EndOfFile);

    void readByteBlock(Byte* buffer, unsigned int howmany)
        throw (UnreachableStream, EndOfFile);
    void readHalfWordBlock(HalfWord* buffer, unsigned int howmany)
        throw (UnreachableStream, EndOfFile);
    void readWordBlock(Word* buffer, unsigned int howmany)
        throw (UnreachableStream, EndOfFile);

    void writeByte(Byte byte) throw (UnreachableStream, WritePastEOF);
    void writeHalfWord(HalfWord halfword)
        throw (UnreachableStream, WritePastEOF);
    void writeWord(Word word) throw (UnreachableStream, WritePastEOF);

    void writeByteBlock(Byte* bytes, unsigned int howmany)
        throw (UnreachableStream, WritePastEOF);
    void writeHalfWordBlock(HalfWord* hwords, unsigned int howmany)
        throw (UnreachableStream, WritePastEOF);
    void writeWordBlock(Word* words, unsigned int howmany)
        throw (UnreachableStream, WritePastEOF);

    unsigned int readPosition() throw (UnreachableStream);
    unsigned int writePosition() throw (UnreachableStream);
    void setReadPosition(unsigned int position) throw (UnreachableStream);
    void setWritePosition(unsigned int position) throw (UnreachableStream);
    bool endOfFile() throw (UnreachableStream);
    unsigned int sizeOfFile() throw (UnreachableStream);

private:
    
    /// The input stream.
    std::ifstream iStream_;
    /// The output stream.
    std::ofstream oStream_;
    /// The name of the stream.
    std::string fileName_;

    /// Externally given output stream.
    std::ostream* extOStream_;

    /// Assignment not allowed.
    BinaryStream& operator=(BinaryStream& old);
    /// Copying not allowed.
    BinaryStream(BinaryStream& old);

    void openInput(std::string name) throw (UnreachableStream);
    void openOutput(std::string name) throw (UnreachableStream);
    void close();
    Byte getByte() throw (UnreachableStream, EndOfFile);
    void putByte(Byte byte) throw (UnreachableStream, WritePastEOF);

};
}

#include "BinaryStream.icc"

#endif
