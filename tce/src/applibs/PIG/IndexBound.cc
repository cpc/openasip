/*
    Copyright (c) 2002-2013 Tampere University of Technology.

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
 * @file IndexBound.cc
 *
 * Implementation of IndexBound class.
 *
 * @author Otto Esko 2013 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include "IndexBound.hh"

/**
 * Constructor for moveslot bound indices
 */
IndexBound::IndexBound(unsigned int startIndex, unsigned int endIndex):
    slotStartIndex_(startIndex), slotEndIndex_(endIndex), usesLimm_(false), 
    limmWidth_(0), limmValueLeftBitIndex_(0), limmValueRightBitIndex_(0) {
}

/**
 * Constructor for moveslot bounds and limm slot bound indices
 */
IndexBound::IndexBound(
    unsigned int startIndex, 
    unsigned int endIndex,
    int limmWidth,
    int limmLeftIndex,
    int limmRightIndex):
    slotStartIndex_(startIndex), slotEndIndex_(endIndex), usesLimm_(true),
    limmWidth_(limmWidth), limmValueLeftBitIndex_(limmLeftIndex), 
    limmValueRightBitIndex_(limmRightIndex) {
}

/**
 * The destructor.
 */
IndexBound::~IndexBound() {
}


/**
 * Move slot start index
 *
 * @return Starting bit index of the move slot
 */
unsigned int 
IndexBound::slotStartIndex() const {

    return slotStartIndex_;
}

/**
 * Move slot end index
 *
 * @return End bit index of the move slot
 */
unsigned int 
IndexBound::slotEndIndex() const {

    return slotEndIndex_;
}

/**
 * Increment start bit index.
 *
 * Useful for converting the move slot index to absolute position inside
 * the instruction bit stream.
 *
 * @param increment Value to be added to the start index
 */
void 
IndexBound::incrStartIndex(unsigned int increment) {

    slotStartIndex_ += increment;
}

/**
 * Increment end bit index.
 *
 * Useful for converting the move slot index to absolute position inside
 * the instruction bit stream
 *
 * @param increment Value to be added to the end index
 */
void 
IndexBound::incrEndIndex(unsigned int increment) {
    
    slotEndIndex_ += increment;
}

/**
 * Is long immediate encoding utilized in this move slot
 *
 * @return Is long immediate utilized in this move slot
 */
bool 
IndexBound::isLimmEncoded() const {

    return usesLimm_;
}

/**
 * Query the width of the limm slot
 *
 * @return Width of the limm slot. 0 if limm is not used
 */
int
IndexBound::limmWidth() const {

    return limmWidth_;
}

/**
 * Query the leftmost (MSB) index of the long immediate value in this slot
 *
 * This tells the leftmost bit of the long immediate *value* this
 * slot is going to encode, not the position of the limm slot inside
 * the instruction. Notice that the indexing starts from zero.
 *
 * @return Leftmost bit of the LIMM value this slot is encoding
 */
int 
IndexBound::limmLeftIndex() const {

    return limmValueLeftBitIndex_;
}

/**
 * Query the rightmost (LSB) index of the long immediate value in this slot
 *
 * This tells the rightmost bit of the long immediate *value* this
 * slot is going to encode, not the position of the limm slot inside
 * the instruction. Notice that the indexing starts from zero.
 *
 * @return Rightmost bit of the LIMM value this slot is encoding
 */
int 
IndexBound::limmRightIndex() const {
    
    return limmValueRightBitIndex_;
}
