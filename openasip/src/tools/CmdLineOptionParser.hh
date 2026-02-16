/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file CmdLineOptionParser.hh
 *
 * Declaration of CmdLineOptionParser classes.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
 * @note reviewed 3 December 2003 by jn, kl, ao
 */

#ifndef TTA_CMD_LINE_OPTION_PARSER_HH
#define TTA_CMD_LINE_OPTION_PARSER_HH

#include <string>
#include <vector>
#include "Exception.hh"

class OptionValue;

//////////////////////////////////////////////////////////////////////////////
// CmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Abstract base class for modeling command line options and their built-in
 * mini-parser.
 *
 * All option types have a set of properties that identify the option: name,
 * description and short name (alias). Possible concrete types of options
 * are: Boolean, real, integer, and character string.
 */
class CmdLineOptionParser {
public:

    CmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias,
        bool hidden = false);
    virtual ~CmdLineOptionParser();

    virtual OptionValue* copy() const = 0;

    std::string longName() const;
    std::string shortName() const;
    std::string description() const;

    /// Pure virtual function that parses the value of option.
    virtual bool parseValue(std::string arguments, std::string prefix) = 0;

    bool isHidden() { return hidden_; }
    bool isDefined();

    virtual int integer(int index = 0) const;
    virtual unsigned unsignedInteger(int index = 0) const;
    virtual std::string String(int index = 0) const;
    virtual double real() const;
    virtual bool isFlagOn() const;
    virtual bool isFlagOff() const;
    virtual int listSize() const;

    CmdLineOptionParser(const CmdLineOptionParser&) = delete;
    CmdLineOptionParser& operator=(const CmdLineOptionParser&) = delete;

protected:
    void setDefined();

private:
    /// The full name of an option.
    std::string longName_;
    /// The optional alias (shorter name).
    std::string shortName_;
    /// The description of option.
    std::string desc_;
    /// The hidden flag. If set, no entry is printed in the normal help text
    bool hidden_ = false;

    /// Is the value of this option set in the parsed command line?
    bool defined_;
};


//////////////////////////////////////////////////////////////////////////////
// IntegerCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Models an option that has an integer value.
 */
class IntegerCmdLineOptionParser : public CmdLineOptionParser {
public:
    IntegerCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~IntegerCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix);
    virtual int integer(int index = 0) const;

    IntegerCmdLineOptionParser(const IntegerCmdLineOptionParser&) = delete;
    IntegerCmdLineOptionParser&
    operator=(const IntegerCmdLineOptionParser&) = delete;

private:
    /// The value of option.
    int value_;
};

//////////////////////////////////////////////////////////////////////////////
// UnsignedIntegerCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Models an option that has an unsigned integer value.
 */
class UnsignedIntegerCmdLineOptionParser : public CmdLineOptionParser {
public:
    UnsignedIntegerCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~UnsignedIntegerCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix);
    virtual unsigned unsignedInteger(int index = 0) const;

    UnsignedIntegerCmdLineOptionParser(
        const UnsignedIntegerCmdLineOptionParser&) = delete;
    UnsignedIntegerCmdLineOptionParser&
    operator=(const UnsignedIntegerCmdLineOptionParser&) = delete;

private:
    /// The value of option.
    unsigned value_;
};


//////////////////////////////////////////////////////////////////////////////
// StringCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that has a string as a value.
 */
class StringCmdLineOptionParser : public CmdLineOptionParser {
public:
    StringCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~StringCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix);
    virtual std::string String(int index = 0) const;

    StringCmdLineOptionParser(const StringCmdLineOptionParser&) = delete;
    StringCmdLineOptionParser&
    operator=(const StringCmdLineOptionParser&) = delete;

private:
    /// The value of the option.
    std::string value_;
};

//////////////////////////////////////////////////////////////////////////////
// OptionalStringCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that acts as flag with optional string value.
 */
class OptionalStringCmdLineOptionParser : public CmdLineOptionParser {
public:
    OptionalStringCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~OptionalStringCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix);
    virtual std::string String(int index = 0) const;
    virtual bool isFlagOn() const;
    virtual bool isFlagOff() const;

    OptionalStringCmdLineOptionParser(
        const OptionalStringCmdLineOptionParser&) = delete;
    OptionalStringCmdLineOptionParser&
    operator=(const OptionalStringCmdLineOptionParser&) = delete;

private:
    /// The value of the option.
    std::string value_;
    /// The flag status.
    bool flag_;
};

//////////////////////////////////////////////////////////////////////////////
// RealCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that has a real value.
 */
class RealCmdLineOptionParser : public CmdLineOptionParser {
public:
    RealCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~RealCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix);
    virtual double real() const;

    RealCmdLineOptionParser(const RealCmdLineOptionParser&) = delete;
    RealCmdLineOptionParser&
    operator=(const RealCmdLineOptionParser&) = delete;

private:
    /// The value of the option.
    double value_;
};

//////////////////////////////////////////////////////////////////////////////
// BoolCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that has a boolean value.
 *
 * This option is also called 'flag'.
 */
class BoolCmdLineOptionParser : public CmdLineOptionParser {
public:
    BoolCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "",
        bool hidden = false);
    virtual ~BoolCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix);
    virtual bool isFlagOn() const;
    virtual bool isFlagOff() const;

    BoolCmdLineOptionParser(const BoolCmdLineOptionParser&) = delete;
    BoolCmdLineOptionParser&
    operator=(const BoolCmdLineOptionParser&) = delete;

private:
    /// The value of option.
    bool value_;
};

//////////////////////////////////////////////////////////////////////////////
// IntegerListCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that has a list of integers as value.
 */
class IntegerListCmdLineOptionParser : public CmdLineOptionParser {
public:
    IntegerListCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~IntegerListCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix);

    virtual int integer(int index = 0) const;
    virtual int listSize() const;

    IntegerListCmdLineOptionParser(
        const IntegerListCmdLineOptionParser&) = delete;
    IntegerListCmdLineOptionParser&
    operator=(const IntegerListCmdLineOptionParser&) = delete;

private:
    /// The values in integer list.
    std::vector<int> values_;
};


//////////////////////////////////////////////////////////////////////////////
// StringListCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * CmdLineOptionParser that has a list of strings as value.
 */
class StringListCmdLineOptionParser : public CmdLineOptionParser {
public:
    StringListCmdLineOptionParser(
        std::string name,
        std::string desc,
        std::string alias = "");
    virtual ~StringListCmdLineOptionParser();

    virtual OptionValue* copy() const;

    virtual bool parseValue(std::string arguments, std::string prefix);

    virtual std::string String(int index = 0) const;
    virtual int listSize() const;

    StringListCmdLineOptionParser(
        const StringListCmdLineOptionParser&) = delete;
    StringListCmdLineOptionParser&
    operator=(const StringListCmdLineOptionParser&) = delete;

private:
    /// The values in string list.
    std::vector<std::string> values_;
};

#include "CmdLineOptionParser.icc"

#endif
