/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
