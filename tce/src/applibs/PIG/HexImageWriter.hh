/*
    Copyright (c) 2002-2018 Tampere University of Technology.

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
 * @file HexImageWriter.hh
 *
 * Declaration of HexImageWriter class
 *
 * @author Alexander Kobler 2018
 * @note rating: red
 */



#ifndef TTA_HEX_IMAGE_WRITER_HH
#define TTA_HEX_IMAGE_WRITER_HH

#include <cstdint>
#include <vector>
#include <iterator>

#include "AsciiImageWriter.hh"

/**
 * Writes the bit image in HEX format. It's used for both ASIC and FPGA memory models
 */
class HexImageWriter : public AsciiImageWriter {
public:
    HexImageWriter(const BitVector& bits, int rowLength);
    virtual ~HexImageWriter();

    virtual void writeImage(std::ostream& stream) const;
};

#endif /* TTA_HEX_IMAGE_WRITER_HH */
