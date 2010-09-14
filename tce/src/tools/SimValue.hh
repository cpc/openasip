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
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */

#ifndef TTA_SIM_VALUE_HH
#define TTA_SIM_VALUE_HH

#include "BaseType.hh"

//////////////////////////////////////////////////////////////////////////////
// SimValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Class that represents values in simulation.
 *
 * This class represents any data type that can be manipulated by operations
 * of the target architecture template, and provides the interface to access
 * the data in three predefined types.
 *
 */
class SimValue {
public:
    inline SimValue();
    inline explicit SimValue(int width);
    inline explicit SimValue(int value, int width);

    inline ~SimValue() {}
    
    inline SimValue(const SimValue& source);

    int width() const;
    void setBitWidth(int width);

    SimValue& operator=(const SIntWord& source);
    SimValue& operator=(const UIntWord& source);
    SimValue& operator=(const FloatWord& source);
    SimValue& operator=(const DoubleWord& source);
    SimValue& operator=(const SimValue& source);

    const SimValue operator+(const SIntWord& rightHand);
    const SimValue operator+(const UIntWord& rightHand);
    const SimValue operator+(const FloatWord& rightHand);
    const SimValue operator+(const DoubleWord& rightHand);

    const SimValue operator-(const SIntWord& rightHand);
    const SimValue operator-(const UIntWord& rightHand);
    const SimValue operator-(const FloatWord& rightHand);
    const SimValue operator-(const DoubleWord& rightHand);

    const SimValue operator/(const SIntWord& rightHand);
    const SimValue operator/(const UIntWord& rightHand);
    const SimValue operator/(const FloatWord& rightHand);
    const SimValue operator/(const DoubleWord& rightHand);

    const SimValue operator*(const SIntWord& rightHand);
    const SimValue operator*(const UIntWord& rightHand);
    const SimValue operator*(const FloatWord& rightHand);
    const SimValue operator*(const DoubleWord& rightHand);

    int operator==(const SimValue& rightHand) const;
    int operator==(const SIntWord& rightHand) const;
    int operator==(const UIntWord& rightHand) const;
    int operator==(const FloatWord& rightHand) const;
    int operator==(const DoubleWord& rightHand) const;

    int intValue() const;
    unsigned int unsignedValue() const;

    SIntWord sIntWordValue() const;
    UIntWord uIntWordValue() const;
    DoubleWord doubleWordValue() const;
    FloatWord floatWordValue() const;

    /// These are public for fast access in the compiled simulation engine.
    union Value {
        UIntWord uIntWord;
        SIntWord sIntWord;
        FloatWord floatWord;
        DoubleWord doubleWord;
    };

    /// The value data.
    Value value_;

    /// The bitwidth of the value.
    int bitWidth_;

private:
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

#include "SimValue.icc"

#endif
