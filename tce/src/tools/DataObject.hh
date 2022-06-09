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
 * @file DataObject.hh
 *
 * Declaration of DataObject class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_DATA_OBJECT_HH
#define TTA_DATA_OBJECT_HH

#include <string>

#include "Exception.hh"
#include "BaseType.hh"
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
    explicit DataObject(SLongWord value);
    explicit DataObject(double value);
    explicit DataObject(const std::string value);
    virtual ~DataObject();

    virtual int integerValue() const;
    virtual void setInteger(int value);
    virtual void setLong(SLongWord value);

    virtual SLongWord longValue() const;

    virtual std::string stringValue() const;
    virtual void setString(std::string value);

    virtual double doubleValue() const;
    virtual void setDouble(double value);

    virtual float floatValue() const;
    virtual void setFloat(float value);

    virtual bool boolValue() const;
    virtual void setBool(bool value);

    virtual bool isNull() const;
    virtual void setNull();

    virtual bool operator!=(const DataObject& object) const;

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
    mutable SLongWord intValue_;
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

    virtual int integerValue() const override;
    virtual SLongWord longValue() const override;

    virtual void setInteger(int value) override;
    virtual void setLong(SLongWord value) override;

    virtual std::string stringValue() const;
    virtual void setString(std::string value) override;

    virtual double doubleValue() const;
    virtual void setDouble(double value) override;

    virtual float floatValue() const;
    virtual void setFloat(float value) override;

    virtual bool isNull() const;
    virtual void setNull() override;
private:

    NullDataObject();
    virtual ~NullDataObject();

    NullDataObject(const DataObject& obj);

    static NullDataObject* instance_;
};

#include "DataObject.icc"

#endif
