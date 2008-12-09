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
 * @file BinaryWriter.hh
 *
 * Declaration of BinaryWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_BINARY_WRITER_HH
#define TTA_BINARY_WRITER_HH

#include "Binary.hh"
#include "Section.hh"
#include "BinaryStream.hh"

namespace TPEF {

/**
 * Interface for actual binary writer classes and for clients
 * to write binary object representation.
 *
 * Provides a way for calling the concrete writers with better error checking.
 */
class BinaryWriter {
public:
    virtual ~BinaryWriter();

    void writeBinary(BinaryStream& stream, const Binary* bin) const;

protected:
    BinaryWriter();

    /// Does real writing of binary.
    virtual void actualWriteBinary(
        BinaryStream& stream,
        const Binary* bin) const = 0;

    void setWriter(const BinaryWriter* writer);

private:
    /// Contains concrete writer instance that is used for writing binary.
    const BinaryWriter* writerToUse_;
};
}

#endif
