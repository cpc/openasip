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
 * @file OptionValue.hh
 *
 * Declaration of OptionValue classes.
 *
 * @author Jari Mäntyneva (jari.mantyneva-no.spam-tut.fi)
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

    virtual void setStringValue(const std::string&)
        throw (WrongSubclass);
    virtual void setIntegerValue(int)
        throw (WrongSubclass);
    virtual void setRealValue(double)
        throw (WrongSubclass);
    virtual void setBoolValue(bool)
	throw (WrongSubclass);
    virtual void setIntegerListValue(std::vector<int> values)
        throw (WrongSubclass);
    virtual std::string stringValue(int index = 0) const
	throw (WrongSubclass, OutOfRange);
    virtual int integerValue(int index = 0) const
        throw (WrongSubclass, OutOfRange);
    virtual double realValue() const
        throw (WrongSubclass);
    virtual bool isFlagOn() const
        throw (WrongSubclass);
    virtual bool isFlagOff() const
        throw (WrongSubclass);
    virtual int listSize() const
	throw (WrongSubclass);

private:

    /// Copying not allowed.
    OptionValue(const OptionValue&);
    /// Assignment not allowed.
    OptionValue& operator=(const OptionValue&);
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
    virtual int integerValue(int index = 0) const
        throw (WrongSubclass, OutOfRange);
    virtual void setIntegerValue(int value)
        throw (WrongSubclass);

private:
    /// Copying not allowed.
    IntegerOptionValue(const IntegerOptionValue&);
    /// Assignment not allowed.
    IntegerOptionValue& operator=(const IntegerOptionValue&);

    /// The value of option.
    int value_;
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
    virtual std::string stringValue(int index = 0) const
        throw (WrongSubclass, OutOfRange);
    virtual void setStringValue(const std::string& value)
        throw (WrongSubclass);

private:
    /// Copying not allowed.
    StringOptionValue(const StringOptionValue&);
    /// Assignment not allowed.
    StringOptionValue& operator=(const StringOptionValue&);

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

    virtual double realValue() const
        throw (WrongSubclass);
    virtual void setRealValue(double value)
        throw (WrongSubclass);

private:
    /// Copying not allowed.
    RealOptionValue(const RealOptionValue&);
    /// Assignment not allowed.
    RealOptionValue& operator=(const RealOptionValue&);

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

    virtual bool isFlagOn() const
        throw (WrongSubclass);
    virtual bool isFlagOff() const
        throw (WrongSubclass);
    virtual void setBoolValue(bool value)
        throw (WrongSubclass);

private:
    /// Copying not allowed.
    BoolOptionValue(const BoolOptionValue&);
    /// Assignment not allowed.
    BoolOptionValue& operator=(const BoolOptionValue&);

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

    virtual int integerValue(int index = 0) const
	throw (WrongSubclass, OutOfRange);
    virtual int listSize() const
	throw (WrongSubclass);
    virtual void setIntegerListValue(std::vector<int> values)
        throw (WrongSubclass);

private:
    /// Copying not allowed.
    IntegerListOptionValue(const IntegerListOptionValue&);
    /// Assignment not allowed.
    IntegerListOptionValue& operator=(const IntegerListOptionValue&);

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

    virtual std::string stringValue(int index = 0) const
	throw (WrongSubclass, OutOfRange);
    virtual int listSize() const
	throw (WrongSubclass);
    virtual void setStringListValue(std::vector<std::string> values)
        throw (WrongSubclass);

private:
    /// Copying not allowed.
    StringListOptionValue(const StringListOptionValue&);
    /// Assignment not allowed.
    StringListOptionValue& operator=(const StringListOptionValue&);

    /// The value of option.
    std::vector<std::string> values_;
};

#endif
