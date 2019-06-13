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
 * @file EntryKeyData.hh
 *
 * Declaration of EntryKeyData, EntryKeyDataInt, EntryKeyDataDouble,
 * EntryKeyDataOperationSet, EntryKeyDataBool and EntryKeyDataParameterSet
 * classes.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_ENTRY_KEY_DATA_HH
#define TTA_ENTRY_KEY_DATA_HH


#include <string>
#include <set>
#include <vector>

#include "Exception.hh"

namespace TTAMachine {
    class FunctionUnit;
}

/**
 * Interface for the data of the entry field.
 */
class EntryKeyData {
public:
    EntryKeyData();
    virtual ~EntryKeyData();
    /// Copies data.
    virtual EntryKeyData* copy() const = 0;

    /// Checks if two data are equal.
    virtual bool isEqual(const EntryKeyData*) const = 0;
    /// Checks if this data is greater than another data.
    virtual bool isGreater(const EntryKeyData*) const = 0;
    /// Checks if this data is smaller than another data.
    virtual bool isSmaller(const EntryKeyData*) const = 0;
    /// Returns the relative position between two data.
    virtual double coefficient(
        const EntryKeyData*, const EntryKeyData*) const = 0;
    /// Converts the data into a string.
    virtual std::string toString() const = 0;
    
private:
    /// Copying not allowed.
    EntryKeyData(const EntryKeyData&);
    /// Assignment not allowed.
    EntryKeyData& operator=(const EntryKeyData&);
};


/**
 * Implementation for integer type of data.
 */
class EntryKeyDataInt : public EntryKeyData {
public:
    EntryKeyDataInt();
    EntryKeyDataInt(int fieldData);
    virtual ~EntryKeyDataInt();
    EntryKeyData* copy() const;

    bool isEqual(const EntryKeyData* fieldData) const;
    bool isGreater(const EntryKeyData* fieldData) const;
    bool isSmaller(const EntryKeyData* fieldData) const;
    double coefficient(
        const EntryKeyData* data1, const EntryKeyData* data2) const;
    std::string toString() const;

private:
    /// Integer data.
    int data_;

    /// Copying not allowed.
    EntryKeyDataInt(const EntryKeyDataInt&);
    /// Assignment not allowed.
    EntryKeyDataInt& operator=(const EntryKeyDataInt&);
};


/**
 * Implementation for double type of data.
 */
class EntryKeyDataDouble : public EntryKeyData {
public:
    EntryKeyDataDouble();
    EntryKeyDataDouble(double fieldData);
    virtual ~EntryKeyDataDouble();
    EntryKeyData* copy() const;

    bool isEqual(const EntryKeyData* fieldData) const;
    bool isGreater(const EntryKeyData* fieldData) const;
    bool isSmaller(const EntryKeyData* fieldData) const;
    double coefficient(
        const EntryKeyData* data1, const EntryKeyData* data2) const;
    std::string toString() const;

private:
    /// Double data.
    double data_;

    /// Copying not allowed.
    EntryKeyDataDouble(const EntryKeyDataDouble&);
    /// Assignment not allowed.
    EntryKeyDataDouble& operator=(const EntryKeyDataDouble&);
};


/**
 * Implementation for operation set type of data.
 */
class EntryKeyDataOperationSet : public EntryKeyData {
public:
    EntryKeyDataOperationSet();
    EntryKeyDataOperationSet(std::set<std::string> fieldData);
    virtual ~EntryKeyDataOperationSet();
    EntryKeyData* copy() const;

    bool isEqual(const EntryKeyData* fieldData) const;
    bool isGreater(const EntryKeyData* fieldData) const;
    bool isSmaller(const EntryKeyData* fieldData) const;
    double coefficient(
        const EntryKeyData* data1, const EntryKeyData* data2) const;
    std::string toString() const;

private:
    /// Operation set data.
    std::set<std::string> data_;
    
    /// Copying not allowed.
    EntryKeyDataOperationSet(const EntryKeyDataOperationSet&);
    /// Assignment not allowed.
    EntryKeyDataOperationSet& operator=(const EntryKeyDataOperationSet&);
};

/**
 * Implementation for boolean type of data.
 */
class EntryKeyDataBool : public EntryKeyData {
public:
    EntryKeyDataBool();
    EntryKeyDataBool(bool fieldData);
    virtual ~EntryKeyDataBool();
    EntryKeyData* copy() const;

    bool isEqual(const EntryKeyData* fieldData) const;
    bool isGreater(const EntryKeyData* fieldData) const;
    bool isSmaller(const EntryKeyData* fieldData) const;
    double coefficient(
        const EntryKeyData* data1, const EntryKeyData* data2) const;
    std::string toString() const;

private:
    /// Boolean data.
    bool data_;
    
    /// Copying not allowed.
    EntryKeyDataBool(const EntryKeyDataBool&);
    /// Assignment not allowed.
    EntryKeyDataBool& operator=(const EntryKeyDataBool&);
};

/**
 * Implementation for FunctionUnit type of data.
 */
class EntryKeyDataFunctionUnit : public EntryKeyData {
public:
    EntryKeyDataFunctionUnit();
    EntryKeyDataFunctionUnit(const TTAMachine::FunctionUnit* fieldData);
    virtual ~EntryKeyDataFunctionUnit();
    EntryKeyData* copy() const;

    bool isEqual(const EntryKeyData* fieldData) const;
    bool isGreater(const EntryKeyData* fieldData) const;
    bool isSmaller(const EntryKeyData* fieldData) const;
    double coefficient(
        const EntryKeyData* data1, const EntryKeyData* data2) const;
    std::string toString() const;

private:
    /// FunctionUnit* data.
    const TTAMachine::FunctionUnit* data_;
    
    /// Copying not allowed.
    EntryKeyDataFunctionUnit(const EntryKeyDataFunctionUnit&);
    /// Assignment not allowed.
    EntryKeyDataFunctionUnit& operator=(const EntryKeyDataFunctionUnit&);
};

#endif
