/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file BitMatrix.hh
 *
 * Declaration of BitMatrix class
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
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
