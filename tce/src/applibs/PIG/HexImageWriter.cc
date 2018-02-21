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
 * @file HexImageWriter.cc
 *
 * Implementation of HexImageWriter class.
 *
 * @author Alexander Kobler 2018
 * @note rating: red
 */

#include <cmath>
#include <iostream>
#include <iomanip>
#include <numeric>

#include "AsciiImageWriter.hh"
#include "HexImageWriter.hh"
#include "BitVector.hh"

using namespace std;

/**s
 * The constructor.
 *
 * @param bits The bits to write.
 * @param rowLength Length of the rows to write.
 */
HexImageWriter::HexImageWriter(const BitVector& bits, int rowLength) :
    AsciiImageWriter(bits, rowLength) {
}

/**
 * The destructor.
 */
HexImageWriter::~HexImageWriter() {
}

/**
 * Writes the bit image to the given stream.
 *
 * @param stream The stream to write.
 */
void HexImageWriter::writeImage(std::ostream& stream) const {
    int lineCount =  static_cast<int>(
        ceil(static_cast<double>(bits().size()) / rowLength()));

    int nibbleCount = static_cast<int>(
    	ceil(static_cast<double>(rowLength() / 4)));

    if (lineCount == 0) {
    	stream << std::hex << std::setfill('0') << std::setw(nibbleCount) << 0 << std::endl;
    } else {

    }
}



std::vector<std::uint8_t> split_uint8_t(const BitVector& bits, int lineCount, int nibbleCount) {

	std::vector<std::uint8_t> result;

// TODO: std::accumulate()
	int i;
	i = accumulate(bits.rbegin(), bits.rend(), 0, [](int x, int y) { return (x << 1) + y; });

	result.push_back(i);

    return result ;
}

