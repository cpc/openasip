/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file EntryKeyData.hh
 *
 * Declaration of EntryKeyData, EntryKeyDataInt, EntryKeyDataDouble,
 * EntryKeyDataOperationSet, EntryKeyDataBool and EntryKeyDataParameterSet
 * classes.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
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
    virtual bool isEqual(const EntryKeyData*) const 
        throw (WrongSubclass) = 0;
    /// Checks if this data is greater than another data.
    virtual bool isGreater(const EntryKeyData*) const
        throw (WrongSubclass) = 0;
    /// Checks if this data is smaller than another data.
    virtual bool isSmaller(const EntryKeyData*) const 
        throw (WrongSubclass) = 0;
    /// Returns the relative position between two data.
    virtual double coefficient(const EntryKeyData*, const EntryKeyData*) const 
        throw (WrongSubclass) = 0;
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

    bool isEqual(const EntryKeyData* fieldData) const 
        throw (WrongSubclass);
    bool isGreater(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    bool isSmaller(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    double coefficient(const EntryKeyData* data1,
                       const EntryKeyData* data2) const
        throw (WrongSubclass);
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

    bool isEqual(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    bool isGreater(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    bool isSmaller(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    double coefficient(const EntryKeyData* data1,
                       const EntryKeyData* data2) const
        throw (WrongSubclass);
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

    bool isEqual(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    bool isGreater(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    bool isSmaller(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    double coefficient(const EntryKeyData* data1,
                       const EntryKeyData* data2) const
        throw (WrongSubclass);
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

    bool isEqual(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    bool isGreater(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    bool isSmaller(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    double coefficient(const EntryKeyData* data1,
                       const EntryKeyData* data2) const
        throw (WrongSubclass);
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

    bool isEqual(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    bool isGreater(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    bool isSmaller(const EntryKeyData* fieldData) const
        throw (WrongSubclass);
    double coefficient(const EntryKeyData* data1,
                       const EntryKeyData* data2) const
        throw (WrongSubclass);
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
