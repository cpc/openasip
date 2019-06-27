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
 * @file ArrayImageWriter.cc
 *
 * Implementation of ArrayImageWriter class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <cmath>
#include <iostream>
#include "ArrayImageWriter.hh"
#include "BitVector.hh"

/**
 * The constructor.
 *
 * @param bits The bits to write.
 * @param rowLength Length of the rows to write.
 */
ArrayImageWriter::ArrayImageWriter(const BitVector& bits, int rowLength) :
    AsciiImageWriter(bits, rowLength) {
}


/**
 * The destructor.
 */
ArrayImageWriter::~ArrayImageWriter() {
}


/**
 * Writes the bit image to the given stream.
 *
 * @param stream The stream to write.
 */
void
ArrayImageWriter::writeImage(std::ostream& stream) const {
    int lineCount = 
        static_cast<int>(
            ceil(static_cast<double>(bits().size()) / rowLength()));

    for (int line = 1; line < lineCount; line++) {
        stream << "\"";
        writeSequence(stream, rowLength());
        stream << "\"," << std::endl;
    }
    stream << "\"";
    bool padRemainingBits = true;
    writeSequence(stream, rowLength(), padRemainingBits);
    stream << "\"";
}
