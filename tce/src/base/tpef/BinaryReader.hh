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
 * @file BinaryReader.hh
 *
 * Declaration of BinaryReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_BINARYREADER_HH
#define TTA_BINARYREADER_HH

#include <set>

#include "Exception.hh"

namespace TPEF {

    class BinaryStream;
    class Binary;
/**
 * Abstract base class for each actual BinaryReader class.
 *
 * Handles registrations and storing of every BinaryReader instance.
 * BinaryReaders implement Singleton design pattern.
 */
class BinaryReader {
public:
    typedef unsigned long Length;

    virtual ~BinaryReader();

    static Binary* readBinary(BinaryStream& stream);

protected:
    BinaryReader();

    static void registerBinaryReader(BinaryReader* reader);

    /// Does actual reading and constructing part.
    virtual Binary* readData(BinaryStream& stream) const = 0;

    /// Checks if given stream can be read with this BinaryReader class.
    virtual bool isMyStreamType(BinaryStream& stream) const = 0;

private:
    BinaryReader(const BinaryReader&);

    /// Contains instances of concrete BinaryReaders.
    static std::set<BinaryReader*>* prototypes_;
};
}

#endif
