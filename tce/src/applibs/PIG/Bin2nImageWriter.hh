/*
  Copyright (c) 2002-2019 Tampere University of Technology.

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
 * @file Bin2nImageWriter.hh
 *
 * Declaration of Bin2nImageWriter class.
 *
 * @author Topi Leppanen 2019 (topi.leppanen-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef TTA_BIN2N_IMAGE_WRITER_HH
#define TTA_BIN2N_IMAGE_WRITER_HH

#include "BitImageWriter.hh"

class BitVector;

/**
 * Writes the data image with binary 1's and 0's. Pads from the left to
 * next multiple of 2 of rowLength_ and after
 * that reverses the byte endianness.
 * Used for programming the FPGA with Jupyter through AlmaIF
 */
class Bin2nImageWriter : public BitImageWriter {
public:
    Bin2nImageWriter(const BitVector& bits, int rowLength);
    virtual ~Bin2nImageWriter();

    virtual void writeImage(std::ostream& stream) const;

    const BitVector& bits() const;

    void writeSequence(std::ostream& stream, unsigned int length) const;

private:
    static char character(const BitVector& bits, unsigned int startIndex, int length);

    unsigned int nextPowerOf2(unsigned int n) const;

    /// The bits to be written.
    const BitVector& bits_;
    /// The length of a row in the output
    int rowLength_;
    /// The index of the next bit to be written
    mutable unsigned int nextBitIndex_;

};

#endif
