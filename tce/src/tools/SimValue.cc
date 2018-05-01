/*
    Copyright (c) 2002-2015 Tampere University of Technology.

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
 * @author Pekka Jääskeläinen 2004,2014-2015 (pjaaskel-no.spam-cs.tut.fi)
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
 * width of SIMULATOR_MAX_INTWORD_BITWIDTH bits.
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
 * @param value The numeric value of this SimValue (in host endianness).
 * @param width The bit width of the created SimValue.
 */
SimValue::SimValue(int value, int width) :
    mask_(~UIntWord(0)) {

    setBitWidth(width);

    const int BYTE_COUNT = (width + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;

#if HOST_BIGENDIAN == 1
    swapByteOrder(((const Byte*)&value), BYTE_COUNT, rawData_);
#else
    memcpy(rawData_, &value, BYTE_COUNT);
#endif
}


/**
 * Copy constructor.
 *
 * @param source The source object from which to copy data.
 */
SimValue::SimValue(const SimValue& source) {
    deepCopy(source);
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

/**
 * Sets SimValue's bitwidth and clears bytes to 0 for the whole width.
 *
 * @param width The new bit width.
 */
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
 * Assignment operator for source value of type SIntWord.
 *
 * @param source The source value.
 * @return Reference to itself.
 */
SimValue&
SimValue::operator=(const SIntWord& source) {

    const size_t BYTE_COUNT = sizeof(SIntWord);

#if HOST_BIGENDIAN == 1
    swapByteOrder((const Byte*)&source, BYTE_COUNT, rawData_);
#else
    memcpy(rawData_, &source, BYTE_COUNT);
#endif
    return (*this);
}

/**
 * Assignment operator for source value of type UIntWord.
 *
 * @param source The source value.
 * @return Reference to itself.
 */
SimValue&
SimValue::operator=(const UIntWord& source) {

    const size_t BYTE_COUNT = sizeof(UIntWord);

#if HOST_BIGENDIAN == 1
    swapByteOrder((const Byte*)&source, BYTE_COUNT, rawData_);
#else
    memcpy(rawData_, &source, BYTE_COUNT);
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
    uint16_t data = source.getBinaryRep();

    setBitWidth(BYTE_COUNT * BYTE_BITWIDTH);

#if HOST_BIGENDIAN == 1
    swapByteOrder((const Byte*)&data, BYTE_COUNT, rawData_);
#else
    memcpy(rawData_, &data, BYTE_COUNT);
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

    setBitWidth(BYTE_COUNT * BYTE_BITWIDTH);

#if HOST_BIGENDIAN == 1
    swapByteOrder((const Byte*)&source, BYTE_COUNT, rawData_);
#else
    memcpy(rawData_, &source, BYTE_COUNT);
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

    setBitWidth(BYTE_COUNT * BYTE_BITWIDTH);

#if HOST_BIGENDIAN == 1
    swapByteOrder((const Byte*)&source, BYTE_COUNT, rawData_);
#else
    memcpy(rawData_, &source, BYTE_COUNT);
#endif
    return (*this);
}

/**
 * Assignment operator for source value of type SimValue.
 *
 * @note No sign extension is done in case
 * the destination width differs from the source width.
 *
 * @param source The source value.
 * @return Reference to itself.
 */
SimValue&
SimValue::operator=(const SimValue& source) {

    const size_t DST_BYTE_COUNT =
        (bitWidth_ + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;
    const size_t SRC_BYTE_COUNT =
        (source.bitWidth_ + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;

    memcpy(rawData_, source.rawData_, SRC_BYTE_COUNT);
    if (SRC_BYTE_COUNT < DST_BYTE_COUNT) {
        memset(rawData_+SRC_BYTE_COUNT, 0, DST_BYTE_COUNT-SRC_BYTE_COUNT);
    } else if (bitWidth_ % BYTE_BITWIDTH) {
        const unsigned bitsInMSB = bitWidth_ % BYTE_BITWIDTH;
        const Byte msbBitMask = static_cast<Byte>((1 << bitsInMSB) - 1);
        rawData_[DST_BYTE_COUNT-1] &= msbBitMask;
    }

    return (*this);
}

/**
 * Copies the source SimValue completely.
 *
 * @param source The source value.
 */
void
SimValue::deepCopy(const SimValue& source) {

    const size_t BYTE_COUNT =
        (source.bitWidth_ + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;

    memcpy(rawData_, source.rawData_, BYTE_COUNT);
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
 * @note This compares only the first value as a 32bit uintword, thus
 *       does not work for values that exceed that width.
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

/**
 * Returns SimValue as a sign extended integer.
 *
 * @return Sign extended integer value.
 */
int
SimValue::intValue() const {

    const size_t BYTE_COUNT = sizeof(int);

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        int value;
    };

    CastUnion cast;

#if HOST_BIGENDIAN == 1
    swapByteOrder(rawData_ , BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_, BYTE_COUNT);
#endif
    int bitWidth = (bitWidth_ > 32) ? 32 : bitWidth_;
    return MathTools::fastSignExtendTo(cast.value, bitWidth);
}

/**
 * Returns SimValue as a zero extended unsigned integer.
 *
 * @return Zero extended unsigned integer value.
 */
unsigned int
SimValue::unsignedValue() const {
    const size_t BYTE_COUNT = sizeof(unsigned int);

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        unsigned int value;
    };

    CastUnion cast;

#if HOST_BIGENDIAN == 1
    swapByteOrder(rawData_, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_, BYTE_COUNT);
#endif

    int bitWidth = (bitWidth_ > 32) ? 32 : bitWidth_;
    return MathTools::fastZeroExtendTo(cast.value, bitWidth);
}

/**
 * Returns the SimValue as SIntWord value.
 *
 * @return SIntWord value.
 */
SIntWord
SimValue::sIntWordValue() const {

    const size_t BYTE_COUNT = sizeof(SIntWord);

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        SIntWord value;
    };

    CastUnion cast;

#if HOST_BIGENDIAN == 1
    swapByteOrder(rawData_, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_, BYTE_COUNT);
#endif

    if ((unsigned)bitWidth_ >= sizeof(SIntWord) * BYTE_BITWIDTH) {
        return cast.value;
    } else {
        return MathTools::fastSignExtendTo(cast.value, bitWidth_);
    }
}

/**
 * Returns the SimValue as UIntWord value.
 *
 * @return UIntWord value.
 */
UIntWord
SimValue::uIntWordValue() const {

    const size_t BYTE_COUNT = sizeof(UIntWord);

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        UIntWord value;
    };

    CastUnion cast;

#if HOST_BIGENDIAN == 1
    swapByteOrder(rawData_, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_, BYTE_COUNT);
#endif
    return cast.value & mask_;
}

/**
 * Returns the SimValue as a host endian DoubleWord value.
 *
 * @return DoubleWord value.
 */
DoubleWord
SimValue::doubleWordValue() const {

    const size_t BYTE_COUNT = sizeof(DoubleWord);

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        DoubleWord value;
    };

    CastUnion cast;

#if HOST_BIGENDIAN == 1
    swapByteOrder(rawData_, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_, BYTE_COUNT);
#endif
    return cast.value;
}

/**
 * Returns the SimValue as FloatWord value.
 *
 * @return the SimValue as a host endian FloatWord value.
 */
FloatWord
SimValue::floatWordValue() const {

    const size_t BYTE_COUNT = sizeof(FloatWord);

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        FloatWord value;
    };

    CastUnion cast;

#if HOST_BIGENDIAN == 1
    swapByteOrder(rawData_, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_, BYTE_COUNT);
#endif
    return cast.value;
}

/**
 * Returns the SimValue as HalfFloatWord value.
 *
 * @return the SimValue as a host endian HalfFloatWord value.
 */
HalfFloatWord
SimValue::halfFloatWordValue() const {
    const size_t BYTE_COUNT = sizeof(uint16_t);

    union CastUnion {
        Byte bytes[BYTE_COUNT];
        uint16_t value;
    };

    CastUnion cast;

#if HOST_BIGENDIAN == 1
    swapByteOrder(rawData_, BYTE_COUNT, cast.bytes);
#else
    memcpy(cast.bytes, rawData_, BYTE_COUNT);
#endif
    return HalfFloatWord(cast.value);
}

/**
 * Returns the value as a binary string.
 *
 * @return the value as a 2's complement (MSB left) binary string in ascii.
 */
TCEString
SimValue::binaryValue() const {

    const size_t BYTE_COUNT = bitWidth_ / BYTE_BITWIDTH;

    int remainBits = bitWidth_ % BYTE_BITWIDTH;
    TCEString binaryStr = "";

    if (remainBits > 0) {
        binaryStr += Conversion::toBinary(
            static_cast<unsigned int>(rawData_[BYTE_COUNT]),
            remainBits);
    }

    for (int i = BYTE_COUNT - 1; i >= 0; --i) {
        binaryStr += Conversion::toBinary(
            static_cast<unsigned int>(rawData_[i]), 8);
    }

    return binaryStr;
}

/**
 * Returns the value as a big endian ordered (C-literal style) hex
 * ascii string.
 *
 * @param noHexIdentifier Leaves "0x" prefix out if set to true.
 * @return SimValue bytes in hex format.
 */
TCEString
SimValue::hexValue(bool noHexIdentifier) const {
    size_t hexNumbers = (bitWidth_ + 3) / 4;
    if (bitWidth_ <= 32) {
        if (noHexIdentifier) {
            return Conversion::toHexString(
                uIntWordValue(), hexNumbers).substr(2);
        } else {
            return Conversion::toHexString(uIntWordValue(), hexNumbers);
        }
    }

    const size_t BYTE_COUNT =
        (bitWidth_ + (BYTE_BITWIDTH - 1)) / BYTE_BITWIDTH;
    const unsigned MSB_MASK = ~(0xffffffffu << (8-(BYTE_COUNT*8-bitWidth_)));

    TCEString hexStr;
    // Convert the raw data buffer to hex string values one byte at a time.
    // Also, remove "0x" from the front of the hex string for each hex value.
    for (int i = BYTE_COUNT - 1; i >= 0; --i) {
        unsigned int value = static_cast<unsigned int>(rawData_[i]);
        if (i == static_cast<int>(BYTE_COUNT - 1)) {

            value &= MSB_MASK;
        }
        hexStr += Conversion::toHexString(value, 2).substr(2);
    }

    // Remove extraneous zero digit from the front.
    hexStr = hexStr.substr(hexStr.size() - hexNumbers);

    if (!noHexIdentifier) hexStr.insert(0, "0x");

    return hexStr;
}

/**
 * Sets SimValue to correspond the hex value.
 *
 * Given hex string must be in big-endian order when it is given. For
 * instance, if the user wants to set integer value 5 through this function,
 * the function should be called "setValue("0x00000005");". Add leading
 * zeroes if you want to clear bytes before the byte that has value 5.
 *
 * @param hexValue New value in hex format.
 */
void
SimValue::setValue(TCEString hexValue) {
    if (hexValue.size() > 2 && hexValue[0] == '0' && hexValue[1] == 'x') {
        hexValue = hexValue.substr(2); // Remove "0x."
    }

    const size_t VALUE_BITWIDTH = hexValue.size() * 4;

    // stretch the SimValue to the hex value bit width in case
    // this is an initialization
    if (bitWidth_ == 0) bitWidth_ = VALUE_BITWIDTH;

    size_t paddingBytes = 0;
    // Check the hex string value is legal.
    if (VALUE_BITWIDTH > SIMD_WORD_WIDTH) {
        throw NumberFormatException(
            __FILE__, __LINE__, __func__, "Too wide value.");
    } else if (VALUE_BITWIDTH == 0) {
        throw NumberFormatException(
            __FILE__, __LINE__, __func__, "Input value is empty.");
    } else if (VALUE_BITWIDTH > (size_t)bitWidth_) {
        // Add padding zero bytes in case the hexValue defines less
        // bytes than the width of the value.
        paddingBytes = (VALUE_BITWIDTH - bitWidth_) / 8;
        for (size_t i = 0; i < paddingBytes; ++i)
            rawData_[VALUE_BITWIDTH / 8 + i] = 0;
    }
    Byte bigEndianData[SIMVALUE_MAX_BYTE_SIZE];
    Conversion::toRawData(hexValue, bigEndianData);

    // because the hexValues are 4bits each they might not fill exact
    // bytes, thus we need to round up to consume an extra byte for
    // remaining 4bits
    int byteWidth = VALUE_BITWIDTH / 8;
    if (VALUE_BITWIDTH % 8 != 0) ++byteWidth;

    swapByteOrder(bigEndianData, byteWidth, rawData_);
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

    memset(rawData_, 0, BYTE_COUNT);
}

/**
 * Sets all SimValue bytes to 0.
 */
void
SimValue::clearToZero() {
    clearToZero(SIMD_WORD_WIDTH);
}

/**
 * Dumps raw data encoded in the SimValue in hexadecimal format.
 */
TCEString
SimValue::dump() const {
    TCEString result = "width=";
    result += Conversion::toString(width());
    result += " mask=";
    result += Conversion::toBinary(mask_, 32);
    result += " data=0x";

    // Convert the raw data buffer to hex string values one byte at a time.
    // Also, remove "0x" from the front of the hex string for each hex value.
    for (int i = SIMVALUE_MAX_BYTE_SIZE - 1; i >= 0; --i) {
        unsigned int value =
            static_cast<unsigned int>(rawData_[i]);
        result += Conversion::toHexString(value, 2).substr(2);
    }

    return result;
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

    assert (from != to);
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
