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
 * @file DataObject.hh
 *
 * Declaration of DataObject class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
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
