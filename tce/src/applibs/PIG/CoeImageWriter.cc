/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file CoeImageWriter.cc
 *
 * Implementation of CoeImageWriter class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <cmath>
#include <iostream>
#include "AsciiImageWriter.hh"
#include "CoeImageWriter.hh"
#include "BitVector.hh"
using std::endl;

/**
 * The constructor.
 *
 * @param bits The bits to write.
 * @param rowLength Length of the rows to write.
 */
CoeImageWriter::CoeImageWriter(const BitVector& bits, int rowLength) :
    AsciiImageWriter(bits, rowLength) {
}

/**
 * The destructor.
 */
CoeImageWriter::~CoeImageWriter() {
}

/**
 * Writes the bit image to the given stream.
 *
 * @param stream The stream to write.
 */
void CoeImageWriter::writeImage(std::ostream& stream) const {
    int lineCount =  static_cast<int>(
        ceil(static_cast<double>(bits().size()) / rowLength()));
    writeHeader(stream);
    if (lineCount == 0) {
        for (int i = 0; i < rowLength(); i++) {
            stream << "0";
        }
    } else {
        bool padEndings = false;
        for (int i = 0; i < lineCount-1; i++) {
            writeSequence(stream, rowLength(), padEndings);
            stream << "," << endl;
        }
        // last line might need padding
        padEndings = true;
        writeSequence(stream, rowLength(), padEndings);
    }
    // format requires semicolon at the end
    stream << ";" << endl;
}

/**
 * Writes header to the given stream.
 *
 * @param stream The stream to write.
 */
void CoeImageWriter::writeHeader(std::ostream& stream) const {
    stream << "; Created by generatebits" << endl;
    stream << "memory_initialization_radix=2;" << endl
           << "memory_initialization_vector=" << endl;
}
