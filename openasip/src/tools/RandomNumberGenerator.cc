/*
 Copyright (c) 2002-2015 Tampere University.

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
 * @file RandomNumberGenerator.cc
 *
 * Implementation of RandomNumberGenerator class.
 *
 * Created on: 25.2.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "RandomNumberGenerator.hh"
#include "LLVMIRTools.hh"

#include <ctime>

#include "Conversion.hh"

const RandomNumberGenerator::SeedType
RandomNumberGenerator::DEFAULTSEED = 12345;

RandomNumberGenerator::RandomNumberGenerator()
     : value_(DEFAULTSEED) {
}


RandomNumberGenerator::RandomNumberGenerator(
    RandomNumberGenerator::SeedType seed)
    : value_(seed) {
}

RandomNumberGenerator::~RandomNumberGenerator() {
}

RandomNumberGenerator::RNGValueType
RandomNumberGenerator::operator()() {
    value_ = 1103515245 * value_ + 12345;
    return value_;
}

void
RandomNumberGenerator::seed(RandomNumberGenerator::SeedType seed) {
    value_ = seed;
}

/**
 * Returns new random seed.
 */
RandomNumberGenerator::RNGValueType
RandomNumberGenerator::generateNewSeed() {
    return RandomNumberGenerator(time(0))();
}

/**
 * Returns non-NAN single precision floating point number from given value.
 */
FloatWord
RandomNumberGenerator::asFloat(RNGValueType seed) {
    union iIntFloatUnion {
        UIntWord i;
        float f;
    } converter;
    converter.i = static_cast<UIntWord>(seed);
    if ((converter.i & 0x7F800000) == 0x7F800000) { // If NaN, make non-NaN
        converter.i &= ~(0x800000);
    }
    return converter.f;
}

/**
 * Returns non-NAN half precision floating point number from given value.
 */
HalfFloatWord
RandomNumberGenerator::asHalfFloat(RNGValueType seed) {
    HalfWord hw = seed;
    if ((hw & 0x7C00) == 0x7C00) { // Avoid NaNs and infinities
        hw &= ~(0x400);
    }
    if (hw & 0x7C00) {
        // If exp is not zero ensure that msb of significant is one.
        hw |= 0x0200;
    }
    return HalfFloatWord(hw);
}

/**
 * Converts given string to value of SeedType.
 *
 * First attempts to interpret the string as numerical value. If that fails
 * new value is composed from characters of the string.
 */
RandomNumberGenerator::SeedType
RandomNumberGenerator::convertToSeed(const std::string& src) {
    try {
        return Conversion::toUnsignedInt(src);
    } catch (NumberFormatException&) {
        RandomNumberGenerator::SeedType seed = DEFAULTSEED;
        for (size_t i = 0; i < src.size(); i++) {
            seed += static_cast<
                RandomNumberGenerator::RNGValueType>(src.at(i));
        }
        return seed;
    }
}


RandomDoubleGenerator::RandomDoubleGenerator()
    : RandomNumberGenerator() {
}

RandomDoubleGenerator::RandomDoubleGenerator(SeedType seed)
    : RandomNumberGenerator(seed) {
}

RandomDoubleGenerator::RNGValueType
RandomDoubleGenerator::operator()() {
    SIntWord tmp;
    assert(sizeof(double) == 8 && "Unexpected size of double (!= 8).");
    assert(sizeof(SIntWord) == 4);
    union {
        SIntWord words[2];
        double result;
    } conversion;

    // Generate msb bits of double //
    tmp = RandomNumberGenerator::operator()();

    if ((tmp & 0x7FF00000) == 0x7FF00000) { // Avoid NaNs and infinities
        tmp &= ~(0x100000); // Change lowest exponent
    }

    if (tmp & 0x7FF00000) {
        // If exp is not zero ensure that msb of significant is one.
        tmp |= 0x80000;
    }

#if HOST_BIGENDIAN == 1
    conversion.words[0] = tmp;
#else
    conversion.words[1] = tmp;
#endif

    // Generate lsb bits of double //
    tmp =  RandomNumberGenerator::operator()();
#if HOST_BIGENDIAN == 1
    conversion.words[1] = tmp;
#else
    conversion.words[0] = tmp;
#endif

    return conversion.result;
}


