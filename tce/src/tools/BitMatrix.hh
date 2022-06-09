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
 * @file BitMatrix.hh
 *
 * Declaration of BitMatrix class
 *
 * @author Pekka Jääskeläinen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BIT_MATRIX_HH
#define TTA_BIT_MATRIX_HH

#include <string>

/**
 * Models a bit matrix.
 *
 * The implementation should be quite efficient. It uses natural words of
 * the machine to store the bits to provide fast logical operations on
 * larger bit chunks at the same time. Especially functions such as fully ORing
 * and ANDing two bit matrices should be quite efficient. Checking a single
 * matrix element is couple of cycles slower than the naive implementation
 * of storing one bit per word. The main methods are inline non-virtual
 * methods for fastest possible access.
 */
class BitMatrix {
public:
    BitMatrix(int width, int height, bool initialValue);
    BitMatrix(const BitMatrix& another);
    virtual ~BitMatrix();

    bool bitAt(int column, int row) const;
    void setBit(int column, int row, bool value);
    void setAllToZero();
    void setAllToOne();
    void shiftLeft();
    void orWith(const BitMatrix& another);
    bool conflictsWith(const BitMatrix& another) const;
    
    int rowCount() const;
    int columnCount() const;

    std::string toString() const;
    std::string toDotString() const;

    bool operator==(const BitMatrix& other) const;
    bool operator<(const BitMatrix& rightHand) const;
private:
    // Storage for bits of a row in the reservation table.
    typedef unsigned RowWord;
    // The contents of the matrix.
    RowWord* matrix_;
    // Count of rows in the matrix.
    const int rows_;
    // Count of columns in the matrix.
    const int columns_;
    // Count of words per row to store the bits.
    const int wordsPerRow_;   
};

#include "BitMatrix.icc"

#endif
