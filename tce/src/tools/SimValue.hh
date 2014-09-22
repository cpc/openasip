/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file SimValue.hh
 *
 * Declaration of SimValue class.
 *
 * @author Pekka J‰‰skel‰inen 2004,2010 (pjaaskel-no.spam-cs.tut.fi)
 * @author Mikko Jarvela 2013, 2014 (mikko.jarvela-no.spam-.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */

#ifndef TTA_SIM_VALUE_HH
#define TTA_SIM_VALUE_HH

#include "BaseType.hh"
#include "HalfFloatWord.hh"
#include <string.h>

#define SIMD_WORD_WIDTH 1024
#define SIMVALUE_MAX_BYTE_SIZE (SIMD_WORD_WIDTH / BYTE_BITWIDTH)

class TCEString;

//////////////////////////////////////////////////////////////////////////////
// SimValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Class that represents values in simulation.
 *
 * This class represents any data type that can be manipulated by operations
 * of the target architecture template, and provides the interface to access
 * the data in predefined types.
 *
 * Values are always stored in big-endian convention in the rawData_ byte 
 * array and the array is filled from the array's "right end" to left. For
 * instance, let's assume the array is 128 bytes wide and a 32-bit UIntWord
 * 305419896 (0x12345678) value is assigned to the SimValue. The value is 
 * laid out in the array as follows:
 * [0] = ...
 * [1] = ... 
 * ...
 * [124] = 0x12
 * [125] = 0x34
 * [126] = 0x56
 * [127] = 0x78
 *
 * When a user wants to interpret SimValue as any primitive value 
 * (FloatWord, UIntWord, etc.), depending on the user's machine endianness
 * the interpreted bytes are swapped correctly to be either in big-endian
 * or little-endian convention. For instance, if the user has a little-endian
 * machine and calls the uIntWordValue() function for a SimValue, which has
 * the above value, a new value with the 0x78563412 byte order is returned.
 *
 * The same swapping convention also occurs when a primitive value is
 * assigned to SimValue. If the value to be assigned is in little-endian 
 * and its bytes are 0xabcd0000, the last four bytes in the SimValue are
 * as 0x0000cdab.
 *
 * SimValue users don't need to worry about this possible byte swapping
 * since it is automatic and is done only if the user's machine is a
 * little-endian machine. However, users shouldn't access the public 
 * rawData_ member directly unless they know exactly what they are doing.
 */

class SimValue {
public:
    SimValue();
    explicit SimValue(int width);
    explicit SimValue(int value, int width);
    SimValue(const SimValue& source);
    ~SimValue() {}

    int width() const;
    void setBitWidth(int width);

    SimValue& operator=(const SIntWord& source);
    SimValue& operator=(const UIntWord& source);
    SimValue& operator=(const HalfFloatWord& source);
    SimValue& operator=(const FloatWord& source);
    SimValue& operator=(const DoubleWord& source);
    SimValue& operator=(const SimValue& source);
    void deepCopy(const SimValue& source);

    const SimValue operator+(const SIntWord& rightHand);
    const SimValue operator+(const UIntWord& rightHand);
    const SimValue operator+(const HalfFloatWord& rightHand);
    const SimValue operator+(const FloatWord& rightHand);
    const SimValue operator+(const DoubleWord& rightHand);

    const SimValue operator-(const SIntWord& rightHand);
    const SimValue operator-(const UIntWord& rightHand);
    const SimValue operator-(const HalfFloatWord& rightHand);
    const SimValue operator-(const FloatWord& rightHand);
    const SimValue operator-(const DoubleWord& rightHand);

    const SimValue operator/(const SIntWord& rightHand);
    const SimValue operator/(const UIntWord& rightHand);
    const SimValue operator/(const HalfFloatWord& rightHand);
    const SimValue operator/(const FloatWord& rightHand);
    const SimValue operator/(const DoubleWord& rightHand);

    const SimValue operator*(const SIntWord& rightHand);
    const SimValue operator*(const UIntWord& rightHand);
    const SimValue operator*(const HalfFloatWord& rightHand);
    const SimValue operator*(const FloatWord& rightHand);
    const SimValue operator*(const DoubleWord& rightHand);

    int operator==(const SimValue& rightHand) const;
    int operator==(const SIntWord& rightHand) const;
    int operator==(const UIntWord& rightHand) const;
    int operator==(const HalfFloatWord& rightHand) const;
    int operator==(const FloatWord& rightHand) const;
    int operator==(const DoubleWord& rightHand) const;

    int intValue() const;
    unsigned int unsignedValue() const;

    SIntWord sIntWordValue() const;
    UIntWord uIntWordValue() const;
    DoubleWord doubleWordValue() const;
    FloatWord floatWordValue() const;
    HalfFloatWord halfFloatWordValue() const;

    TCEString binaryValue() const;
    TCEString hexValue() const;

    void setValue(TCEString hexValue);
    void clearToZero(int bitWidth);

    /// Array that contains SimValue's underlaying bytes in big endian.
    Byte rawData_[SIMVALUE_MAX_BYTE_SIZE];

    /// The bitwidth of the value.
    int bitWidth_;

private:
    /// @todo Create more optimal 4-byte and 2-byte swapper functions for
    /// 2 and 4 byte values. The more optimal swapper would load all bytes
    /// to int or short int and shift the values to their correct places,
    /// which would reduce memory accesses.
    void swapByteOrder(const Byte* from, size_t byteCount, Byte* to) const;

    /// Mask for masking extra bits when returning unsigned value.
    UIntWord mask_;

};

//////////////////////////////////////////////////////////////////////////////
// NullSimValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Singleton class that is used to represent a null SimValue.
 *
 * All methods cause program abort with an error log message.
 *
 */
class NullSimValue {
public:
    static SimValue& instance();

private:
    NullSimValue();

    static SimValue instance_;

};

#define SIMULATOR_MAX_INTWORD_BITWIDTH 32

#endif
