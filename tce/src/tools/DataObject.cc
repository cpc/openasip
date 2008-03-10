/**
 * @file DataObject.cc
 *
 * Definition of DataObject class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 */

#include <string>
using std::string;

#include "DataObject.hh"
#include "Conversion.hh"
#include "Application.hh"
#include "StringTools.hh"

/**
 * Constructor.
 *
 * Creates an DataObject which represents an empty string.
 */
DataObject::DataObject() :
    type_(TYPE_STRING), intFresh_(false), stringFresh_(true),
    doubleFresh_(false), floatFresh_(false), intValue_(0), stringValue_(""),
    doubleValue_(0), floatValue_(0) {
}

/**
 * Constructor for integer data.
 *
 * @param value Value as integer.
 */
DataObject::DataObject(int value) {
    setInteger(value);
}

/**
 * Constructor for double data.
 *
 * @param value Value as double.
 */
DataObject::DataObject(double value) {
    setDouble(value);
}

/**
 * Constructor for string data.
 *
 * @param value Value as string.
 */
DataObject::DataObject(const std::string value) {
    setString(value);
}

/**
 * Destructor.
 */
DataObject::~DataObject() {
}

/**
 * Sets the integer value of DataObject.
 *
 * @param value The integer value.
 */
void
DataObject::setInteger(int value) {
    type_ = TYPE_INT;
    intFresh_ = true;
    stringFresh_ = false;
    doubleFresh_ = false;
    floatFresh_ = false;
    intValue_ = value;
}

/**
 * Sets the string value of DataObject.
 *
 * @param value The string value.
 */
void
DataObject::setString(std::string value) {
    type_ = TYPE_STRING;
    stringFresh_ = true;
    intFresh_ = false;
    doubleFresh_ = false;
    floatFresh_ = false;
    stringValue_ = value;
}

/**
 * Sets the double value of DataObject.
 *
 * @param value The double value of DataObject.
 */
void
DataObject::setDouble(double value) {
    type_ = TYPE_DOUBLE;
    doubleFresh_ = true;
    intFresh_ = false;
    stringFresh_ = false;
    floatFresh_ = false;
    doubleValue_ = value;
}

/**
 * Sets the float value of DataObject.
 *
 * @param value The float value of DataObject.
 */
void
DataObject::setFloat(float value) {
    type_ = TYPE_FLOAT;
    floatFresh_ = true;
    intFresh_ = false;
    stringFresh_ = false;
    doubleFresh_ = false;
    floatValue_ = value;
}

/**
 * Sets the bool value of DataObject.
 *
 * @param value The bool value of DataObject.
 */
void
DataObject::setBool(bool value) {
    setInteger(value);
}

/**
 * Sets the DataObject to null.
 *
 */
void
DataObject::setNull() {
    type_ = TYPE_NULL;
    intFresh_ = true;
    intValue_ = 0;
    stringFresh_ = true;
    stringValue_ = "";
}


/**
 * Returns the integer value of DataObject.
 *
 * If integer value is not available, the conversion is done from the
 * original value type. Note that a NULL DataObject is converted to 0.
 *
 * @return The integer value of DataObject.
 * @exception NumberFormatException If conversion fails or if DataObject is
 *            not initialized.
 */
int
DataObject::integerValue() const
    throw (NumberFormatException) {

    if (intFresh_) {
        return intValue_;
    } else {
        try {
            switch (type_) {
            case TYPE_STRING:
                intValue_ = Conversion::toInt(stringValue_);
                break;
            case TYPE_DOUBLE:
                intValue_ = Conversion::toInt(doubleValue_);
                break;
            case TYPE_FLOAT:
                intValue_ = Conversion::toInt(floatValue_);
                break;
            case TYPE_NOTYPE: {
                string method = "DataObject::integerValue()";
                string message = "DataObject not initialized.";
                throw NumberFormatException(
                    __FILE__, __LINE__, method, message);
                break;
            }
            case TYPE_NULL: 
                intValue_ = 0;
                break;
            default:
                break;
            }
        } catch (const NumberFormatException&) {

            // it can be an unsigned int, let's try to convert to
            // unsigned int first
            switch (type_) {
            case TYPE_STRING:
                intValue_ = Conversion::toUnsignedInt(stringValue_);
                break;
            case TYPE_DOUBLE:
                intValue_ = Conversion::toUnsignedInt(doubleValue_);
                break;
            case TYPE_FLOAT:
                intValue_ = Conversion::toUnsignedInt(floatValue_);
                break;
            case TYPE_NOTYPE: {
                string method = "DataObject::integerValue()";
                string message = "DataObject not initialized.";
                throw NumberFormatException(
                    __FILE__, __LINE__, method, message);
                break;
            }
            default:
                break;
            }
        }
        intFresh_ = true;
        return intValue_;
    }
}

/**
 * Returns the string value of DataObject.
 *
 * If string value is not available, the conversion is done from the original
 * data type. Note that a NULL DataObject is converted to an empty string.
 *
 * @return The string value of DataObject.
 * @exception NumberFormatException If conversion fails or if DataObject is
 *            not initialized.
 */
string
DataObject::stringValue() const
    throw (NumberFormatException) {

    if (stringFresh_) {
        return stringValue_;
    } else {
        switch (type_) {
        case TYPE_INT:
            stringValue_ = Conversion::toString(intValue_);
            break;
        case TYPE_DOUBLE:
            stringValue_ = Conversion::toString(doubleValue_);
            break;
        case TYPE_FLOAT:
            stringValue_ = Conversion::toString(floatValue_);
            break;
        case TYPE_NOTYPE: {
            string method = "DataObject::stringValue()";
            string message = "DataObject not initialized.";
            throw NumberFormatException(__FILE__, __LINE__, method, message);
            break;
        }
        case TYPE_NULL: 
            stringValue_ = "";
            break;
        default:
            break;
        }
        stringFresh_ = true;
        return stringValue_;
    }
}

/**
 * Returns the double value of DataObject.
 *
 * If double value is not available, the conversion is done from the original
 * data type. Note that a NULL DataObject is converted to 0.0.
 *
 * @return The double value of DataObject.
 * @exception NumberFormatException If conversion fails or if DataObject is
 *            not initialized.
 */
double
DataObject::doubleValue() const
    throw (NumberFormatException) {

    if (doubleFresh_) {
        return doubleValue_;
    } else {
        switch (type_) {
        case TYPE_STRING:
            doubleValue_ = Conversion::toDouble(stringValue_);
            break;
        case TYPE_INT:
            doubleValue_ = Conversion::toDouble(intValue_);
            break;
        case TYPE_FLOAT:
            doubleValue_ = Conversion::toDouble(floatValue_);
            break;
        case TYPE_NOTYPE: {
            string method = "DataObject::doubleValue()";
            string message = "DataObject not initialized.";
            throw NumberFormatException(__FILE__, __LINE__, method, message);
            break;
        }
        case TYPE_NULL: 
            doubleValue_ = 0.0;
            break;
        default:
            break;
        }
        doubleFresh_ = true;
        return doubleValue_;
    }
}

/**
 * Returns the float value of DataObject.
 *
 * If float value is not available, the conversion is done from the original
 * data type. Note that a NULL DataObject is converted to 0.0.
 *
 * @return The float value of DataObject.
 * @exception NumberFormatException If conversion fails or if DataObject is
 *            not initialized.
 */
float
DataObject::floatValue() const
    throw (NumberFormatException) {

    if (floatFresh_) {
        return floatValue_;
    } else {
        switch (type_) {
        case TYPE_STRING:
            floatValue_ = Conversion::toFloat(stringValue_);
            break;
        case TYPE_INT:
            floatValue_ = Conversion::toFloat(intValue_);
            break;
        case TYPE_DOUBLE:
            floatValue_ = Conversion::toFloat(doubleValue_);
            break;
        case TYPE_NOTYPE: {
            string method = "DataObject::floatValue()";
            string message = "DataObject not initialized.";
            throw NumberFormatException(__FILE__, __LINE__, method, message);
            break;
        }
        case TYPE_NULL: 
            floatValue_ = 0.0;
            break;
        default:
            break;
        }
        floatFresh_ = true;
        return floatValue_;
    }

}

/**
 * Returns the bool value of DataObject.
 *
 * The conversion is done from the original data type. The NULL value is
 * represented as "false".
 *
 * @return The bool value of DataObject.
 * @exception NumberFormatException If conversion fails or if DataObject is
 *                                  not initialized.
 */
bool
DataObject::boolValue() const
    throw (NumberFormatException) {

    if (type_ == TYPE_STRING) {
        std::string strValue = StringTools::stringToLower(stringValue());
        if (strValue == "true") {
            return true;
        } else if (strValue == "false") {
            return false;
        }
    } else if (type_ == TYPE_NULL) {
        return false;
    }

    return integerValue();
}

/**
 * Returns true in case the object represents a NULL value.
 *
 * @return True in case the object's value is NULL.
 */
bool
DataObject::isNull() const 
    throw (NumberFormatException) {
    return type_ == TYPE_NULL;
}


/**
 * Tests the inequality of two DataObjects.
 *
 * @param object The object this object is compared to.
 * @return True if the two objects are inequal.
 * @exception NumberFormatException If conversion fails.
 */
bool
DataObject::operator!=(const DataObject& object) const
    throw (NumberFormatException) {

    if (object.type_ != type_)
        return true;

    if (type_ == TYPE_NULL)
        return false; // the type_ is the value as itself

    string value1 = stringValue();
    string value2 = object.stringValue();
    return value1 != value2;
}

///////////////////////////////////////////////////////////////////////////////
// NullDataObject
///////////////////////////////////////////////////////////////////////////////

#include <iostream>

NullDataObject* NullDataObject::instance_ = NULL;

/**
 * Constructor.
 */
NullDataObject::NullDataObject() {
}

/**
 * Destructor.
 */
NullDataObject::~NullDataObject() {
}


/**
 * Returns an instance of NullDataObject (singleton)
 *
 * @return NullDataObject singleton instance.
 */
NullDataObject&
NullDataObject::instance() {
    if (instance_ == NULL) {
        instance_ = new NullDataObject();
    }
    return *instance_;
}


/**
 * All method implementations of NullDataObject write a message to error log
 * and abort the program.
 */
void
NullDataObject::setInteger(int) {
    abortWithError("setInteger()");
}

void
NullDataObject::setString(std::string) {
    abortWithError("setString()");
}

void
NullDataObject::setDouble(double) {
    abortWithError("setDouble()");
}

void
NullDataObject::setFloat(float) {
    abortWithError("setFloat()");
}

void
NullDataObject::setNull() {
    abortWithError("setNull()");
}

int
NullDataObject::integerValue() const
    throw (NumberFormatException) {
    abortWithError("integerValue()");
    return 0;
}

string
NullDataObject::stringValue() const
    throw (NumberFormatException) {
    abortWithError("stringValue()");
    return "";
}

double
NullDataObject::doubleValue() const
    throw (NumberFormatException) {
    abortWithError("doubleValue()");
    return 0.0;
}

float
NullDataObject::floatValue() const
    throw (NumberFormatException) {
    abortWithError("floatValue()");
    return 0.0;
}

bool
NullDataObject::isNull() const 
    throw (NumberFormatException) {
    abortWithError("isNull()");
    return false;
}
