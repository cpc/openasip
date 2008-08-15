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
 * @file DataObject.hh
 *
 * Declaration of DataObject class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 */

#ifndef TTA_DATA_OBJECT_HH
#define TTA_DATA_OBJECT_HH

#include <string>

#include "Exception.hh"

/**
 * Class that represents data in different formats.
 *
 * Data can be represented as a string, integer, double, or float. The
 * idea behind the data object is to cache the conversion results and
 * to minimize conversions. The DataObject's value can be null, it is
 * indicated with isNull() returning true. This should not be confused
 * with NullDataObject, which is just there to indicate a DataObject
 * reference which is null (a null reference, compares to a null pointer).
 */
class DataObject {
public:

    /**
     * Models the type with which DataObject was originally initialized.
     */
    enum OrigType {
        TYPE_INT,
        TYPE_STRING,
        TYPE_DOUBLE,
        TYPE_FLOAT,
        TYPE_NULL,
        TYPE_NOTYPE
    };

    DataObject();
    explicit DataObject(int value);
    explicit DataObject(double value);
    explicit DataObject(const std::string value);
    virtual ~DataObject();

    virtual int integerValue() const throw (NumberFormatException);
    virtual void setInteger(int value);

    virtual std::string stringValue() const throw (NumberFormatException);
    virtual void setString(std::string value);

    virtual double doubleValue() const throw (NumberFormatException);
    virtual void setDouble(double value);

    virtual float floatValue() const throw (NumberFormatException);
    virtual void setFloat(float value);

    virtual bool boolValue() const throw (NumberFormatException);
    virtual void setBool(bool value);

    virtual bool isNull() const throw (NumberFormatException);
    virtual void setNull();

    virtual bool operator!=(const DataObject& object) 
        const throw (NumberFormatException);

protected:
    OrigType type() const;
    bool intFresh() const;
    bool stringFresh() const;
    bool doubleFresh() const;
    bool floatFresh() const;

private:
    /// The type of value in which DataObject was last assigned.
    OrigType type_;
    /// Flag indicating that the value of integer is up-to-date.
    mutable bool intFresh_;
    /// Flag indicating that the value of string is up-to-date.
    mutable bool stringFresh_;
    /// Flag indicating that the value of double is up-to-date.
    mutable bool doubleFresh_;
    /// Flag indicating that the value of float is up-to-date.
    mutable bool floatFresh_;
    /// Value as integer.
    mutable int intValue_;
    /// Value as string.
    mutable std::string stringValue_;
    /// Value as double.
    mutable double doubleValue_;
    /// Value as float.
    mutable float floatValue_;
};

/**
 * Singleton class that represents a null DataObject.
 *
 * All methods write to error log and abort the program.
 */
class NullDataObject : public DataObject {
public:
    static NullDataObject& instance();
    virtual int integerValue() const throw (NumberFormatException);
    virtual void setInteger(int value);

    virtual std::string stringValue() const throw (NumberFormatException);
    virtual void setString(std::string value);

    virtual double doubleValue() const throw (NumberFormatException);
    virtual void setDouble(double value);

    virtual float floatValue() const throw (NumberFormatException);
    virtual void setFloat(float value);

    virtual bool isNull() const throw (NumberFormatException);
    virtual void setNull();

private:

    NullDataObject();
    virtual ~NullDataObject();

    NullDataObject(const DataObject& obj);

    static NullDataObject* instance_;
};

#include "DataObject.icc"

#endif
