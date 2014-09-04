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
 * @file SimValue.cc
 *
 * Non-inline definitions of SimValue class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pjaaskel-no.spam-cs.tut.fi)
 * @author Mikko Jarvela 2013, 2014 (mikko.jarvela-no.spam-.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */

#include "SimValue.hh"
#include "MathTools.hh"
#include "Conversion.hh"
#include "TCEString.hh"


/**
 * Default constructor.
 *
 * To allow creation of SimValue arrays. Constructs a SimValue with
 * width of SIMULATOR_MAX_INTWORD_BITWIDTH (32) bits.
 */
SimValue::SimValue() :
    mask_(~UIntWord(0)) {

    setBitWidth(SIMULATOR_MAX_INTWORD_BITWIDTH);
}

/**
 * Constructor.
 *
 * @param width The bit width of the created SimValue.
 */
SimValue::SimValue(int width) :
    mask_(~UIntWord(0)) {

    setBitWidth(width);
}


/**
 * Constructor.
 *
 * @param value The numeric value of this SimValue.
 * @param width The bit width of the created SimValue.
 */
SimValue::SimValue(int value, int width) :
    mask_(~UIntWord(0)) {

    setBitWidth(width);

    const int BYTE_COUNT = (width + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;
    const int LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

#if WORDS_BIGENDIAN != 1
    swapByteOrder(((const Byte*)&value), BYTE_COUNT, rawData_ + LEFT_BYTE_POS);
#else
    memcpy(rawData_ + LEFT_BYTE_POS, &value, BYTE_COUNT);
#endif
}


/**
 * Copy constructor.
 *
 * @param source The source object from which to copy data.
 */
SimValue::SimValue(const SimValue& source) :
    mask_(source.mask_) {

    setBitWidth(source.bitWidth_);

    const int BYTE_COUNT = (bitWidth_ + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;
    const int LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;
    
    memcpy(
        rawData_ + LEFT_BYTE_POS, source.rawData_ + LEFT_BYTE_POS, BYTE_COUNT);
}

/**
 * Returns the bit width of the SimValue.
 *
 * @return The bit width.
 */
int
SimValue::width() const {
    return bitWidth_;
}

void
SimValue::setBitWidth(int width) {
    assert(width <= SIMD_WORD_WIDTH);

    bitWidth_ = width;
    if (BYTE_BITWIDTH * sizeof(mask_) > static_cast<size_t>(width)) {
        mask_ = ~((~UIntWord(0)) << bitWidth_);
    }

    const int BYTE_COUNT = (width + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;

    if (static_cast<size_t>(BYTE_COUNT) > sizeof(DoubleWord)) {
        clearToZero(width);
    } else {
        clearToZero(BYTE_BITWIDTH * sizeof(DoubleWord));
    }
}


/**
 * Assignment operator for source value of type int.
 *
 * @param source The source value.
 * @return Reference to itself.
 */
SimValue&
SimValue::operator=(const SIntWord& source) {
    const size_t BYTE_COUNT = sizeof(SIntWord);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

#if WORDS_BIGENDIAN != 1
    swapByteOrder((const Byte*)&source, BYTE_COUNT, rawData_ + LEFT_BYTE_POS);
#else
    memcpy(rawData_ + LEFT_BYTE_POS, &source, BYTE_COUNT);
#endif
    return (*this);
}

/**
 * Assignment operator for source value of type IntWord.
 *
 * @param source The source value.
 * @return Reference to itself.
 */
SimValue&
SimValue::operator=(const UIntWord& source) {
    const size_t BYTE_COUNT = sizeof(UIntWord);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

#if WORDS_BIGENDIAN != 1
    swapByteOrder((const Byte*)&source, BYTE_COUNT, rawData_ + LEFT_BYTE_POS);
#else
    memcpy(rawData_ + LEFT_BYTE_POS, &source, BYTE_COUNT);
#endif
    return (*this);
}

/**
 * Assignment operator for source value of type HalfFloatWord.
 *
 * @param source The source value.
 * @return Reference to itself.
 */
SimValue&
SimValue::operator=(const HalfFloatWord& source) {
    const size_t BYTE_COUNT = sizeof(uint16_t);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;
    uint16_t data = source.getBinaryRep();

    setBitWidth(BYTE_COUNT * BYTE_BITWIDTH);

#if WORDS_BIGENDIAN != 1
    swapByteOrder((const Byte*)&data, BYTE_COUNT, rawData_ + LEFT_BYTE_POS);
#else
    memcpy(rawData_ + LEFT_BYTE_POS, &data, BYTE_COUNT);
#endif
    return (*this);
}

/**
 * Assignment operator for source value of type FloatWord.
 *
 * @param source The source value.
 * @return Reference to itself.
 */
SimValue&
SimValue::operator=(const FloatWord& source) {
    const size_t BYTE_COUNT = sizeof(FloatWord);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    setBitWidth(BYTE_COUNT * BYTE_BITWIDTH);

#if WORDS_BIGENDIAN != 1
    swapByteOrder((const Byte*)&source, BYTE_COUNT, rawData_ + LEFT_BYTE_POS);
#else
    memcpy(rawData_ + LEFT_BYTE_POS, &source, BYTE_COUNT);
#endif
    return (*this);
}

/**
 * Assignment operator for source value of type DoubleWord.
 *
 * @param source The source value.
 * @return Reference to itself.
 */
SimValue&
SimValue::operator=(const DoubleWord& source) {
    const size_t BYTE_COUNT = sizeof(DoubleWord);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    setBitWidth(BYTE_COUNT * BYTE_BITWIDTH);

#if WORDS_BIGENDIAN != 1
    swapByteOrder((const Byte*)&source, BYTE_COUNT, rawData_ + LEFT_BYTE_POS);
#else
    memcpy(rawData_ + LEFT_BYTE_POS, &source, BYTE_COUNT);
#endif
    return (*this);
}

/**
 * Assignment operator for source value of type SimValue.
 *
 * In case the bit widths don't match, sign extension is done to the target
 * at the highest bit of the narrower value. This models dropping the extra
 * most significant bits of the target value in case the widths don't match
 * in a way  that signed values keep their original meaning in signed
 * calculations. That is, a 1-bit "-1" is still "-1" when written to a
 * 4-bit SimValue, and vice versa.
 *
 * @param source The source value.
 * @return Reference to itself.
 */
SimValue&
SimValue::operator=(const SimValue& source) {
    const size_t BYTE_COUNT = 
        (static_cast<size_t>(bitWidth_) + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    memcpy(
        rawData_ + LEFT_BYTE_POS, source.rawData_ + LEFT_BYTE_POS, BYTE_COUNT);
    return (*this);
}

/**
 * Copies the source SimValue completely, along with the bitwidth info.
 *
 * @param source The source value.
 */
void
SimValue::deepCopy(const SimValue& source) {
    // Choose the thinner bit width, which is used to copy the bytes. This
    // copy style may clip away bytes from a larger source SimValue.
    int bitWidth = bitWidth_;
    if (source.bitWidth_ < bitWidth_) {
        bitWidth = source.bitWidth_;
    }

    const size_t BYTE_COUNT = 
        (source.bitWidth_ + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    memcpy(
        rawData_ + LEFT_BYTE_POS, source.rawData_ + LEFT_BYTE_POS, BYTE_COUNT);
    bitWidth_ = source.bitWidth_;
    mask_ = source.mask_;
}

/**
 * Explicit addition operator to SIntWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator+(const SIntWord& rightHand) {
    SimValue copy(*this);
    copy = sIntWordValue() + rightHand;
    return copy;
}

/**
 * Explicit addition operator to UIntWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator+(const UIntWord& rightHand) {
    SimValue copy(*this);
    copy = uIntWordValue() + rightHand;
    return copy;
}

/**
 * Explicit addition operator to HalfFloatWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator+(const HalfFloatWord& rightHand) {
    SimValue copy(*this);
    copy = halfFloatWordValue() + rightHand;
    return copy;
}

/**
 * Explicit addition operator to FloatWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator+(const FloatWord& rightHand) {
    SimValue copy(*this);
    copy = floatWordValue() + rightHand;
    return copy;
}

/**
 * Explicit addition operator to DoubleWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator+(const DoubleWord& rightHand) {
    SimValue copy(*this);
    copy = doubleWordValue() + rightHand;
    return copy;
}


/**
 * Explicit subtraction operator to HalfFloatWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator-(const HalfFloatWord& rightHand) {
    SimValue copy(*this);
    copy = halfFloatWordValue() - rightHand;
    return copy;
}

/**
 * Explicit subtraction operator to SIntWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator-(const SIntWord& rightHand) {
    SimValue copy(*this);
    copy = sIntWordValue() - rightHand;
    return copy;
}

/**
 * Explicit subtraction operator to UIntWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator-(const UIntWord& rightHand) {
    SimValue copy(*this);
    copy = uIntWordValue() - rightHand;
    return copy;
}

/**
 * Explicit subtraction operator to FloatWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator-(const FloatWord& rightHand) {
    SimValue copy(*this);
    copy = floatWordValue() - rightHand;
    return copy;
}

/**
 * Explicit subtraction operator to DoubleWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator-(const DoubleWord& rightHand) {
    SimValue copy(*this);
    copy = doubleWordValue() - rightHand;
    return copy;
}

/**
 * Explicit division operator to HalfFloatWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator/(const HalfFloatWord& rightHand) {
    SimValue copy(*this);
    copy = halfFloatWordValue() / rightHand;
    return copy;
}

/**
 * Explicit division operator to SIntWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator/(const SIntWord& rightHand) {
    SimValue copy(*this);
    copy = sIntWordValue() / rightHand;
    return copy;
}

/**
 * Explicit division operator to UIntWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator/(const UIntWord& rightHand) {
    SimValue copy(*this);
    copy = uIntWordValue() / rightHand;
    return copy;
}

/**
 * Explicit division operator to FloatWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator/(const FloatWord& rightHand) {
    SimValue copy(*this);
    copy = floatWordValue() / rightHand;
    return copy;
}

/**
 * Explicit division operator to DoubleWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator/(const DoubleWord& rightHand) {
    SimValue copy(*this);
    copy = doubleWordValue() / rightHand;
    return copy;
}

/**
 * Explicit multiply operator to HalfFloatWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator*(const HalfFloatWord& rightHand) {
    SimValue copy(*this);
    copy = halfFloatWordValue() * rightHand;
    return copy;
}

/**
 * Explicit multiplication operator to SIntWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator*(const SIntWord& rightHand) {
    SimValue copy(*this);
    copy = sIntWordValue() * rightHand;
    return copy;
}

/**
 * Explicit multiplication operator to UIntWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator*(const UIntWord& rightHand) {
    SimValue copy(*this);
    copy = uIntWordValue() * rightHand;
    return copy;
}

/**
 * Explicit multiplication operator to FloatWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator*(const FloatWord& rightHand) {
    SimValue copy(*this);
    copy = floatWordValue() * rightHand;
    return copy;
}

/**
 * Explicit multiplication operator to DoubleWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the addition.
 * @return The SimValue with the result of the operation.
 */
const SimValue
SimValue::operator*(const DoubleWord& rightHand) {
    SimValue copy(*this);
    copy = doubleWordValue() * rightHand;
    return copy;
}

/**
 * Explicit equality operator for SimValue type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the comparison.
 * @return Reference to itself.
 *
 */
int
SimValue::operator==(const SimValue& rightHand) const {
    /// @todo Should this be changed to comparison between bytes from the
    /// whole bitwidth?
    return uIntWordValue() == rightHand.uIntWordValue();
}

/**
 * Explicit equality operator for SIntWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the comparison.
 * @return Reference to itself.
 *
 */
int
SimValue::operator==(const SIntWord& rightHand) const {
    return sIntWordValue() == rightHand;
}

/**
 * Explicit equality operator for UIntWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the comparison.
 * @return Reference to itself.
 *
 */
int
SimValue::operator==(const UIntWord& rightHand) const {
    return uIntWordValue() == rightHand;
}

/**
 * Explicit equality operator for HalfFloatWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the comparison.
 * @return Reference to itself.
 *
 */
int
SimValue::operator==(const HalfFloatWord& rightHand) const {
    return halfFloatWordValue().getBinaryRep() == rightHand.getBinaryRep();
}

/**
 * Explicit equality operator for FloatWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the comparison.
 * @return Reference to itself.
 *
 */
int
SimValue::operator==(const FloatWord& rightHand) const {
    return floatWordValue() == rightHand;
}

/**
 * Explicit equality operator for DoubleWord type.
 *
 * These operators are defined to avoid ambiguous overload because of built-in
 * operators.
 *
 * @param rightHand The right hand side of the comparison.
 * @return Reference to itself.
 *
 */
int
SimValue::operator==(const DoubleWord& rightHand) const {
    return doubleWordValue() == rightHand;
}

int 
SimValue::intValue() const {
    const size_t BYTE_COUNT = sizeof(int);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        int value;
    };

    CastUnion cast;
    
#if WORDS_BIGENDIAN != 1
    swapByteOrder(rawData_ + LEFT_BYTE_POS, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_ + LEFT_BYTE_POS, BYTE_COUNT);
#endif
    int bitWidth = (bitWidth_ > 32) ? 32 : bitWidth_;
    return MathTools::fastSignExtendTo(cast.value, bitWidth);
}

unsigned int 
SimValue::unsignedValue() const {
    const size_t BYTE_COUNT = sizeof(unsigned int);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        unsigned int value;
    };

    CastUnion cast;
    
#if WORDS_BIGENDIAN != 1
    swapByteOrder(rawData_ + LEFT_BYTE_POS, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_ + LEFT_BYTE_POS, BYTE_COUNT);
#endif
    
    int bitWidth = (bitWidth_ > 32) ? 32 : bitWidth_;
    return MathTools::fastZeroExtendTo(cast.value, bitWidth);
}

SIntWord 
SimValue::sIntWordValue() const {
    const size_t BYTE_COUNT = sizeof(SIntWord);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        SIntWord value;
    };

    CastUnion cast;
    
#if WORDS_BIGENDIAN != 1
    swapByteOrder(rawData_ + LEFT_BYTE_POS, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_ + LEFT_BYTE_POS, BYTE_COUNT);
#endif
    return cast.value & mask_;
}
 
UIntWord 
SimValue::uIntWordValue() const {
    const size_t BYTE_COUNT = sizeof(UIntWord);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        UIntWord value;
    };

    CastUnion cast;
    
#if WORDS_BIGENDIAN != 1
    swapByteOrder(rawData_ + LEFT_BYTE_POS, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_ + LEFT_BYTE_POS, BYTE_COUNT);
#endif
    return cast.value & mask_;
}

DoubleWord
SimValue::doubleWordValue() const {
    const size_t BYTE_COUNT = sizeof(DoubleWord);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        DoubleWord value;
    };

    CastUnion cast;
    
#if WORDS_BIGENDIAN != 1
    swapByteOrder(rawData_ + LEFT_BYTE_POS, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_ + LEFT_BYTE_POS, BYTE_COUNT);
#endif
    return cast.value;
}

FloatWord 
SimValue::floatWordValue() const {
    const size_t BYTE_COUNT = sizeof(FloatWord);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        FloatWord value;
    };

    CastUnion cast;
    
#if WORDS_BIGENDIAN != 1
    swapByteOrder(rawData_ + LEFT_BYTE_POS, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_ + LEFT_BYTE_POS, BYTE_COUNT);
#endif
    return cast.value;
}

HalfFloatWord 
SimValue::halfFloatWordValue() const {
    const size_t BYTE_COUNT = sizeof(uint16_t);
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        uint16_t value;
    };

    CastUnion cast;
    
#if WORDS_BIGENDIAN != 1
    swapByteOrder(rawData_ + LEFT_BYTE_POS, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_ + LEFT_BYTE_POS, BYTE_COUNT);
#endif
    return HalfFloatWord(cast.value);
}

/**
 * Returns the value as a binary string.
 *
 * @return SimValue bytes in binary format.
 */
TCEString
SimValue::binaryValue() const {
    const size_t BYTE_COUNT = bitWidth_ / BYTE_BITWIDTH;
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    int remainBits = bitWidth_ % BYTE_BITWIDTH;
    TCEString binaryStr = "";

    if (remainBits > 0) {
        binaryStr += Conversion::toBinary(
            static_cast<unsigned int>(rawData_[LEFT_BYTE_POS - 1]), 
            remainBits);
    }

    for (size_t i = 0; i < BYTE_COUNT; ++i) {
        binaryStr += Conversion::toBinary(
            static_cast<unsigned int>(rawData_[LEFT_BYTE_POS + i]), 8);
    }

    return binaryStr;
}

/**
 * Returns the value as a hex string.
 *
 * @return SimValue bytes in hex format.
 */
TCEString
SimValue::hexValue() const {
    if (bitWidth_ <= 32) {
        size_t hexNumbers = (bitWidth_ + 3) / 4;
        return Conversion::toHexString(sIntWordValue(), hexNumbers); 
    }
    
    const size_t BYTE_COUNT =
        (bitWidth_ + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    TCEString hexStr = "0x";

    // Convert the raw data buffer to hex string values one byte at a time.
    // Also, remove "0x" from the front of the hex string for each hex value.
    for (size_t i = 0; i < BYTE_COUNT; ++i) {
        unsigned int value = 
            static_cast<unsigned int>(rawData_[LEFT_BYTE_POS + i]);
        hexStr += Conversion::toHexString(value, 2).substr(2);
    }

    return hexStr;
}

/**
 * Sets SimValue to correspond the hex value.
 *
 * @param hexValue New value in hex format.
 */
void
SimValue::setValue(TCEString hexValue) {
    if (hexValue.size() > 2 && hexValue[0] == '0' && hexValue[1] == 'x') {
        hexValue = hexValue.substr(2); // Remove "0x."
    }

    const size_t VALUE_BITWIDTH = hexValue.size() * 4;

    // Check the hex string value is legal.
    if (VALUE_BITWIDTH > SIMD_WORD_WIDTH) {
        throw NumberFormatException(
            __FILE__, __LINE__, __func__, "Too wide value.");
    } else if (VALUE_BITWIDTH == 0) {
        throw NumberFormatException(
            __FILE__, __LINE__, __func__, "Input value is empty.");
    }

    const size_t BYTE_COUNT = 
        (VALUE_BITWIDTH + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;    
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    /// @todo Use memset to init the SimValue to 0 for the whole bitwidth 
    /// before copying the bytes?
    Conversion::toRawData(hexValue, rawData_ + LEFT_BYTE_POS);
}

/**
 * Sets SimValue bytes to 0 for the given bitwidth.
 *
 * @param bitWidth The width that is to be nullified.
 */
void
SimValue::clearToZero(int bitWidth) {
    assert(bitWidth <= SIMD_WORD_WIDTH);

    const size_t BYTE_COUNT = (bitWidth + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;
    const size_t LEFT_BYTE_POS = SIMVALUE_MAX_BYTE_SIZE - BYTE_COUNT;

    memset(rawData_ + LEFT_BYTE_POS, 0, BYTE_COUNT);
}

/**
 * Copies the byte order from source array in opposite order to target array.
 *
 * @note Caller is responsible for making sure both input pointers have
 *       enough allocated memory.
 * @param from Array from which the bytes are copied.
 * @param byteCount How many bytes are copied.
 * @param to Array to which the bytes are copied in opposite order.
 */
void
SimValue::swapByteOrder(
    const Byte* from, size_t byteCount, Byte* to) const {
 
    for (size_t i = 0; i < byteCount; ++i) {
        to[byteCount - 1 - i] = from[i];
    }
}

//////////////////////////////////////////////////////////////////////////////
// NullSimValue
//////////////////////////////////////////////////////////////////////////////

SimValue NullSimValue::instance_(0);

/**
 * Returns an instance of NullSimValue class (singleton).
 *
 * @return Singleton instance of NullSimValue class.
 */
SimValue&
NullSimValue::instance() {
    return instance_;
}
