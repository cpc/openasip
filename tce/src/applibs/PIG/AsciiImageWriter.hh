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
 * @file AsciiImageWriter.hh
 *
 * Declaration of AsciiBinaryWriter class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_ASCII_IMAGE_WRITER_HH
#define TTA_ASCII_IMAGE_WRITER_HH

#include "BitImageWriter.hh"
#include "Exception.hh"

class BitVector;

/**
 * Writes the bit image with ASCII 1's and 0's.
 */
class AsciiImageWriter : public BitImageWriter {
public:
    AsciiImageWriter(const BitVector& bits, int rowLength);
    virtual ~AsciiImageWriter();

    virtual void writeImage(std::ostream& stream) const;


protected:
    const BitVector& bits() const;
    int rowLength() const;
    void writeSequence(
        std::ostream& stream, int length, bool padEnd = false) const;

    void writeHexSequence(
        std::ostream& stream, int length, bool padEnd = false) const;

    void
    writeHexSequence(std::ostream& stream, int length, bool padEnd = false) const
        throw (OutOfRange);

private:
    /// The bits to be written.
    const BitVector& bits_;
    /// The length of a row in the output.
    int rowLength_;
    /// The index of the next bit to be written.
    mutable unsigned int nextBitIndex_;
};

#endif
