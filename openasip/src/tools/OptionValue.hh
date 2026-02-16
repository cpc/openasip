/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file OptionValue.hh
 *
 * Declaration of OptionValue classes.
 *
 * @author Jari Mäntyneva (jari.mantyneva-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2014
 * @note rating: red
 */

#ifndef TTA_OPTION_VALUE_HH
#define TTA_OPTION_VALUE_HH

#include <string>
#include <vector>
#include "Exception.hh"

//////////////////////////////////////////////////////////////////////////////
// OptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * An abstract base class for modeling all kinds of options.
 *
 * Possible option types are bool, real, int, and string. Each
 * option knows its name and description.
 */
class OptionValue {
public:

    OptionValue();
    virtual ~OptionValue();

    virtual void setStringValue(const std::string&);
    virtual void setIntegerValue(int);
    virtual void setUnsignedIntegerValue(unsigned);
    virtual void setRealValue(double);
    virtual void setBoolValue(bool);
    virtual void setIntegerListValue(std::vector<int> values);
    virtual std::string stringValue(int index = 0) const;
    virtual int integerValue(int index = 0) const;
    virtual double realValue() const;
    virtual bool isFlagOn() const;
    virtual bool isFlagOff() const;
    virtual int listSize() const;

    OptionValue(const OptionValue&) = delete;
    OptionValue& operator=(const OptionValue&) = delete;
};


//////////////////////////////////////////////////////////////////////////////
// IntegerOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Models an option that has an integer value.
 */
class IntegerOptionValue : public OptionValue {
public:
    IntegerOptionValue(int value);
    virtual ~IntegerOptionValue();
    virtual int integerValue(int index = 0) const;
    virtual void setIntegerValue(int value);

    IntegerOptionValue(const IntegerOptionValue&) = delete;
    IntegerOptionValue& operator=(const IntegerOptionValue&) = delete;

private:
    /// The value of option.
    int value_;
};

//////////////////////////////////////////////////////////////////////////////
// UnsignedIntegerOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Models an option that has an unsigned integer value.
 */
class UnsignedIntegerOptionValue : public OptionValue {
public:
    UnsignedIntegerOptionValue(unsigned value);
    virtual ~UnsignedIntegerOptionValue();
    virtual unsigned unsignedIntegerValue(int index = 0) const;
    virtual void setUnsignedIntegerValue(unsigned value);

    UnsignedIntegerOptionValue(const UnsignedIntegerOptionValue&) = delete;
    UnsignedIntegerOptionValue&
    operator=(const UnsignedIntegerOptionValue&) = delete;

private:
    /// The value of option.
    unsigned value_;
};

//////////////////////////////////////////////////////////////////////////////
// StringOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * OptionValue that has a string as a value.
 */
class StringOptionValue : public OptionValue {
public:
    StringOptionValue(const std::string value);
    virtual ~StringOptionValue();
    virtual std::string stringValue(int index = 0) const;
    virtual void setStringValue(const std::string& value);

    StringOptionValue(const StringOptionValue&) = delete;
    StringOptionValue& operator=(const StringOptionValue&) = delete;

private:
    /// The value of the option.
    std::string value_;
};

//////////////////////////////////////////////////////////////////////////////
// RealOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * OptionValue that has a real value.
 */
class RealOptionValue : public OptionValue {
public:
    RealOptionValue(double value);
    virtual ~RealOptionValue();

    virtual double realValue() const;
    virtual void setRealValue(double value);

    RealOptionValue(const RealOptionValue&) = delete;
    RealOptionValue& operator=(const RealOptionValue&) = delete;

private:
    /// The value of the option.
    double value_;
};

//////////////////////////////////////////////////////////////////////////////
// BoolOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * OptionValue that has a boolean value.
 *
 * This option is also called 'flag'.
 */
class BoolOptionValue : public OptionValue {
public:
    BoolOptionValue(bool value);
    virtual ~BoolOptionValue();

    virtual bool isFlagOn() const;
    virtual bool isFlagOff() const;
    virtual void setBoolValue(bool value);

    BoolOptionValue(const BoolOptionValue&) = delete;
    BoolOptionValue& operator=(const BoolOptionValue&) = delete;

private:
    /// The value of option.
    bool value_;
};

//////////////////////////////////////////////////////////////////////////////
// IntegerListOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * OptionValue that has list of integers as value.
 */
class IntegerListOptionValue : public OptionValue {
public:
    IntegerListOptionValue(std::vector<int> values);
    virtual ~IntegerListOptionValue();

    virtual int integerValue(int index = 0) const;
    virtual int listSize() const;
    virtual void setIntegerListValue(std::vector<int> values);

    IntegerListOptionValue(const IntegerListOptionValue&) = delete;
    IntegerListOptionValue& operator=(const IntegerListOptionValue&) = delete;

private:
    /// The value of option.
    std::vector<int> values_;
};


//////////////////////////////////////////////////////////////////////////////
// StringListOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * OptionValue that has list of strings as value.
 */
class StringListOptionValue : public OptionValue {
public:
    StringListOptionValue(std::vector<std::string> values);
    virtual ~StringListOptionValue();

    virtual std::string stringValue(int index = 0) const;
    virtual int listSize() const;
    virtual void setStringListValue(std::vector<std::string> values);

    StringListOptionValue(const StringListOptionValue&) = delete;
    StringListOptionValue& operator=(const StringListOptionValue&) = delete;

private:
    /// The value of option.
    std::vector<std::string> values_;
};

#endif
