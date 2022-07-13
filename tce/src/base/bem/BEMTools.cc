/*
 Copyright (c) 2002-2016 Tampere University.

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
/*
 * @file BEMTools.cc
 *
 * Implementation of BEMTools class.
 *
 * Created on: 21.1.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "BEMTools.hh"

#include <cassert>

#include "MathTools.hh"

/**
 * Returns bit width required by the encoding and extrabits combination.
 *
 * @note encoding value of zero translates to width of zero.
 */
unsigned
BEMTools::encodingWidth(unsigned encoding, unsigned extrabits) {
    return MathTools::bitLength(encoding) + extrabits;
}

/**
 * Calculates the extra bits (zeroes) of the given encoding.
 *
 * For example:
 * @verbatim
 *  (encoding, width) = (3, 5) ->  |00011| -> return 3.
 * @endverbatim
 *
 * @param encoding The encoding value.
 * @param width The number of bits the encoding occupies.
 *              Must be at least or more than required bits needed for the
 *              value. Must greater than 0.
 */
unsigned
BEMTools::extrabits(unsigned encoding, unsigned width) {
    assert(width >= MathTools::bitLength(encoding));
    assert(width > 0);
    return width - MathTools::bitLength(encoding);
}
