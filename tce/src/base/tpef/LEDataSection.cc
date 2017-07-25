/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file DataSection.cc
 *
 * Non-inline definitions of DataSection class.
 *
 * @author Heikki Kultala 2014(hkultala-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <cmath>

#include "LEDataSection.hh"
#include "Application.hh"
#include "Swapper.hh"
#include "MathTools.hh"

namespace TPEF {

LEDataSection LEDataSection::proto_(true);

/**
 * Constructor.
 *
 * @param init true if registeration is wanted
 */
LEDataSection::LEDataSection(bool init) : DataSection(false) {
    
    if (init) {
        Section::registerSection(this);
    }

    unsetFlagVLen();
    unsetFlagNoBits();
}

/**
 * Destructor.
 */
LEDataSection::~LEDataSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
LEDataSection::type() const {
    return ST_LEDATA;
}

/**
 * Creates an instance of DataSection.
 *
 * @return Newly created section.
 */
Section*
LEDataSection::clone() const {
    
    return new LEDataSection(false);
}


/**
 * Writes unsigned value to data section.
 *
 * Value is aligned to field as normal big endian value
 * least significant bit is stored to last bit of last MAU.
 *
 * If we call writeValue(0, 4, 3) and MAU is 2bit
 * start of data section will be 00 00 00 11.
 *
 * However the MAUs are stored in data section like this:
 * 00000000|000000000|00000000|00000011 (one MAU per byte)
 *    MAU1 |   MAU2  |  MAU3  | MAU4
 *
 * @param index MAU offset to section where to we write value.
 * @param numOfMAUs Number of MAUs that we use for storing value.
 * @param value Value to write.
 */
void
LEDataSection::writeValue(Word index, Word numOfMAUs, unsigned long value) {

    int mauInBytes = MAUsToBytes(1);    
    
    MinimumAddressableUnit mauMask = 
        static_cast<unsigned int>(-1) >> 
        (sizeof(mauMask)*BYTE_BITWIDTH - aSpace()->MAU());
    
    int shiftCount = 0;
    // start writing from beginning of area..
    for (unsigned int i = 0; i < numOfMAUs; i++) {
        int byteOffset = MAUsToBytes(index + i);
        
        MinimumAddressableUnit currentMAU = 0;
        
        if (shiftCount < static_cast<int>(sizeof(value)*BYTE_BITWIDTH)) {
            // I tried math tools... system tests went broken :(
            // ssooo... if it's not broken.... 
            currentMAU = (value >> shiftCount) & mauMask;
        }
        
        shiftCount += aSpace()->MAU();
        
        // write current MAU :)
        for (int j = mauInBytes-1; j >= 0; j--) {
            Byte currentByte = static_cast<Byte>(currentMAU);
            setByte(byteOffset + j, currentByte);
            currentMAU = currentMAU >> BYTE_BITWIDTH;
        }
    }
}

/**
 * Writes signed value to data section.
 *
 * For example, when we call writeValue(0, 4, -3) and MAU is 2bit
 * start of data section will be 11 11 11 01.
 * 
 * MAUs are stored in data section like this:
 * 00000011 000000011 00000011 00000001 (one MAU per byte)
 *
 * If we call writeValue(0, 4, -3) and MAU is 10bit
 * start of data section will be 1111111111 1111111111 1111111111 1111111101.
 *
 * However the MAUs are stored in data section like this:
 *
 * 00000011 11111111|00000011 11111111|00000011 11111111|00000011 11111101|
 *       MAU1       |      MAU2       |      MAU3       |      MAU4       |
 *
 * @param index MAU offset to section where to we write value.
 * @param numOfMAUs Number of MAUs that we use for storing value.
 * @param value Value to write.
 */
void
LEDataSection::writeValue(Word index, Word numOfMAUs, signed long value) { 

    int mauInBytes = MAUsToBytes(1);    
    
    MinimumAddressableUnit mauMask = 
        static_cast<unsigned int>(-1) >> 
        (sizeof(mauMask)*BYTE_BITWIDTH - aSpace()->MAU());
    
    int shiftCount = 0;
    // start writing from beginning of area..
    for (unsigned int i = 0; i < numOfMAUs; i++) {
        int byteOffset = MAUsToBytes(index + i);
        
        MinimumAddressableUnit currentMAU = 
            static_cast<MinimumAddressableUnit>(~0);
        
        if (shiftCount < static_cast<int>(sizeof(value)*BYTE_BITWIDTH)) {
            // I tried math tools... system tests went broken :(
            // ssooo... if it's not broken.... 
            currentMAU = (value >> shiftCount) & mauMask;
            shiftCount += aSpace()->MAU();
        }
        
        // write current MAU :)
        for (int j = mauInBytes-1; j >= 0; j--) {
            Byte currentByte = static_cast<Byte>(currentMAU);
            setByte(byteOffset + j,currentByte);
            currentMAU = currentMAU >> BYTE_BITWIDTH;
        }
    }
}

}
