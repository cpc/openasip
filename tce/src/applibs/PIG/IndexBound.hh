/*
    Copyright (c) 2002-2013 Tampere University.

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
 * @file IndexBound.hh
 *
 * Declaration of Index class.
 *
 * @author Otto Esko 2013 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_INDEX_BOUND_HH
#define TTA_INDEX_BOUND_HH

/**
 * Class for storing index information needed for fixing instruction addresses
 * in InstructionBitVector
 */
class IndexBound {
public:
    IndexBound(unsigned int startIndex, unsigned int endIndex);

    IndexBound(
        unsigned int startIndex, 
        unsigned int endIndex,
        int limmWidth,
        int limmLeftIndex,
        int limmRightIndex);

    ~IndexBound();

    unsigned int slotStartIndex() const;

    unsigned int slotEndIndex() const;

    void incrStartIndex(unsigned int increment);

    void incrEndIndex(unsigned int increment);

    bool isLimmEncoded() const;

    int limmWidth() const;

    int limmLeftIndex() const;

    int limmRightIndex() const;

private:

    IndexBound();

    /// Start index of the moveslot in instruction
    unsigned int slotStartIndex_;
    /// End index of the moveslot in instruction
    unsigned int slotEndIndex_;
    /// Is address encoded in long immediate
    bool usesLimm_;
    /// Width of the long immediate chunk in this moveslot
    int limmWidth_;
    /// Index of the left most bit (MSB) of the value to be encoded in
    /// this slot
    int limmValueLeftBitIndex_;
    /// Index of the right most bit (LSB) of the value to be encoded
    /// in this slot
    int limmValueRightBitIndex_;
};

#endif
