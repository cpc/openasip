/**
 * @file EntryKeyData.cc
 *
 * Implementation of EntryKeyData, EntryKeyDataInt, EntryKeyDataDouble,
 * EntryKeyDataOperationSet, EntryKeyDataBool and EntryKeyDataParameterSet
 * classes.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include <string>
#include <sstream>
#include <typeinfo>
#include "EntryKeyData.hh"
#include "Conversion.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"

using std::string;
using std::set;
using std::vector;
using std::pair;
using namespace TTAMachine;

///////////////////////////////////////////////////////////////////////////////
// EntryKeyData
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor.
 */
EntryKeyData::EntryKeyData() {
}

/**
 * Destructor.
 */
EntryKeyData::~EntryKeyData() {
}

///////////////////////////////////////////////////////////////////////////////
// EntryKeyDataInt
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor.
 */
EntryKeyDataInt::EntryKeyDataInt() : data_(0) {
}

/**
 * Constructor.
 *
 * @param fieldData An integer.
 */
EntryKeyDataInt::EntryKeyDataInt(int fieldData) : data_(fieldData) {
}

/**
 * Destructor.
 */
EntryKeyDataInt::~EntryKeyDataInt() {
}

/**
 * Copies integer.
 *
 * Client is responsible of deallocating the memory reserved for the
 * returned object.
 *
 * @return A copy of the integer.
 */
EntryKeyData*
EntryKeyDataInt::copy() const {
    return new EntryKeyDataInt(data_);
}

/**
 * Checks if two integers are equal.
 *
 * Cannot compare integers to other data types.
 *
 * @param fieldData An integer.
 * @return True if two integers are equal.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataInt::isEqual(const EntryKeyData* fieldData) const 
    throw (WrongSubclass) {
    
    EntryKeyDataInt* integer = dynamic_cast<EntryKeyDataInt*>(
	                       const_cast<EntryKeyData*>(fieldData));
    if (integer == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataInt::isEqual");
    }
    return data_ == integer->data_;
}

/**
 * Checks if this integer is greater than another integer.
 *
 * Cannot compare integers to other data types.
 *
 * @param fieldData An integer.
 * @return True if this integer is greater than another integer,
 *         otherwise false.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataInt::isGreater(const EntryKeyData* fieldData) const 
    throw (WrongSubclass) {
    EntryKeyDataInt* integer = dynamic_cast<EntryKeyDataInt*>(
	                       const_cast<EntryKeyData*>(fieldData));
    if (integer == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataInt::isGreater");
    }
    return data_ > integer->data_;
}

/**
 * Checks if this integer is smaller than another integer.
 *
 * Cannot compare integers to other data types.
 *
 * @param fieldData An integer.
 * @return True if this integer is smaller than another integer,
 *         otherwise false.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataInt::isSmaller(const EntryKeyData* fieldData) const 
    throw (WrongSubclass) {
    EntryKeyDataInt* integer = dynamic_cast<EntryKeyDataInt*>(
	                       const_cast<EntryKeyData*>(fieldData));
    if (integer == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataInt::isSmaller");
    }
    return data_ < integer->data_;
}

/**
 * Returns the relative position between two integers.
 *
 * Returns the integer's relative position to the first integer
 * compared to the second. For example, if this integer is 14, the
 * first integer is 10 and the second 20, relative position would be
 * 0.4.
 *
 * @param data1 First integer.
 * @param data2 Second integer.
 * @return The relative position between two integers.
 * @exception WrongSubclass Given data type was illegal.
 */
double
EntryKeyDataInt::coefficient(
    const EntryKeyData* data1,
    const EntryKeyData* data2) const 
    throw (WrongSubclass) {
    EntryKeyDataInt* int1 = dynamic_cast<EntryKeyDataInt*>(
	                    const_cast<EntryKeyData*>(data1));
    EntryKeyDataInt* int2 = dynamic_cast<EntryKeyDataInt*>(
	                    const_cast<EntryKeyData*>(data2));
    if (int1 == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataInt::coefficient param1");
    }
    if (int2 == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataInt::coefficient param2");
    }
    
    return static_cast<double>(data_ - int1->data_) /
	static_cast<double>(int2->data_ - int1->data_);
}

/**
 * Converts the integer into a string.
 *
 * @return integer as a string
 */
std::string
EntryKeyDataInt::toString() const {
    return Conversion::toString(data_);
}


///////////////////////////////////////////////////////////////////////////////
// EntryKeyDataDouble
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor.
 */
EntryKeyDataDouble::EntryKeyDataDouble() : data_(0) {
}

/**
 * Constructor.
 *
 * @param fieldData A double.
 */
EntryKeyDataDouble::EntryKeyDataDouble(double fieldData) : data_(fieldData) {
}

/**
 * Destructor.
 */
EntryKeyDataDouble::~EntryKeyDataDouble() {
}

/**
 * Copies the double.
 *
 * Client is responsible of deallocating the memory reserved for the
 * returned object.
 *
 * @return A copy of the double.
 */
EntryKeyData*
EntryKeyDataDouble::copy() const {
    return new EntryKeyDataDouble(data_);
}

/**
 * Checks if two doubles are equal.
 *
 * Cannot compare double to other data types.
 *
 * @param fieldData A double.
 * @return True if two doubles are equal.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataDouble::isEqual(const EntryKeyData* fieldData) const 
    throw (WrongSubclass) {
    
    EntryKeyDataDouble* data = dynamic_cast<EntryKeyDataDouble*>(
        const_cast<EntryKeyData*>(fieldData));
    if (data == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataDouble::isEqual");	
    }
    return data_ == data->data_;
}

/**
 * Checks if another double is greater.
 *
 * Cannot compare double to other data types.
 *
 * @param fieldData A double.
 * @return True if this double is greater than another double,
 *         otherwise false.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataDouble::isGreater(const EntryKeyData* fieldData) const 
    throw (WrongSubclass) {

    EntryKeyDataDouble* data = dynamic_cast<EntryKeyDataDouble*>(
        const_cast<EntryKeyData*>(fieldData));
    if (data == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataDouble::isGreater");		
    }
    return data_ > data->data_;
}

/**
 * Checks if this double is smaller than another double.
 *
 * Cannot compare double to other data types.
 *
 * @param fieldData A double.
 * @return True if this double is smaller than another double,
 *         otherwise false.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataDouble::isSmaller(const EntryKeyData* fieldData) const 
    throw (WrongSubclass) {

    EntryKeyDataDouble* data = dynamic_cast<EntryKeyDataDouble*>(
        const_cast<EntryKeyData*>(fieldData));
    if (data == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataDouble::isSmaller");
    }
    return data_ < data->data_;
}

/**
 * Returns the relative position between two doubles.
 *
 * Returns the double's relative position to the first double
 * compared to the second. For example, if this double is 14, the
 * first double is 10 and the second 20, relative position would be
 * 0.4.
 *
 * @param data1 First double.
 * @param data2 Second double.
 * @return The relative position between two doubles.
 * @exception WrongSubclass Given data type was illegal.
 */
double
EntryKeyDataDouble::coefficient(
    const EntryKeyData* data1,
    const EntryKeyData* data2) const
    throw (WrongSubclass) {
    
    EntryKeyDataDouble* double1 = dynamic_cast<EntryKeyDataDouble*>(
        const_cast<EntryKeyData*>(data1));
    EntryKeyDataDouble* double2 = dynamic_cast<EntryKeyDataDouble*>(
        const_cast<EntryKeyData*>(data2));
    if (double1 == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataDouble::coefficient param1");	
    }
    if(double2 == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataDouble::coefficient param2");
    }

    return (data_ - double1->data_) / (double2->data_ - double1->data_);
}

/**
 * Converts the double into a string.
 *
 * @return Double as a string.
 */
std::string
EntryKeyDataDouble::toString() const {
    return Conversion::toString(data_);
}

///////////////////////////////////////////////////////////////////////////////
// EntryKeyDataOperationSet
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor.
 */
EntryKeyDataOperationSet::EntryKeyDataOperationSet() : data_() {
}

/**
 * Constructor.
 *
 * @param fieldData A set of operations.
 */
EntryKeyDataOperationSet::EntryKeyDataOperationSet(
    std::set<std::string> fieldData) : data_(fieldData) {
}

/**
 * Destructor.
 */
EntryKeyDataOperationSet::~EntryKeyDataOperationSet() {
}

/**
 * Copies the operation set.
 *
 * Client is responsible of deallocating the memory reserved for the
 * returned object.
 *
 * @return A copy of the operation set.
 */
EntryKeyData*
EntryKeyDataOperationSet::copy() const {
    return new EntryKeyDataOperationSet(data_);
}

/**
 * Checks if two operation sets are equal.
 *
 * Cannot compare to other data types.
 *
 * @param fieldData Operation set.
 * @return True if two operation sets are equal.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataOperationSet::isEqual(const EntryKeyData* fieldData) const 
    throw (WrongSubclass) {
    
    EntryKeyDataOperationSet* data = dynamic_cast<EntryKeyDataOperationSet*>(
        const_cast<EntryKeyData*>(fieldData));
    if (data == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataOperationSet::isEqual");	
    }
    return data_ == data->data_;
}

/**
 * Checks if another operation set is greater.
 *
 * Cannot compare to other data types. Cannot compare to other operation sets
 * and because of that returns always false.
 *
 * @param fieldData Operation set.
 * @return Always false.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataOperationSet::isGreater(const EntryKeyData*) const 
    throw (WrongSubclass) {

    return false;
}

/**
 * Checks if this operation set is smaller than another set.
 *
 * Cannot compare to other data types. Cannot compare to other operation sets
 * and because of that returns always true.
 *
 * @param fieldData Operation set.
 * @return Always true.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataOperationSet::isSmaller(const EntryKeyData*) const 
    throw (WrongSubclass) {

    return true;
}

/**
 * Cannot be called for EntryKeyDataOperationSet.
 *
 * Operation sets cannot be compared and no coefficient can be counted.
 *
 * @param data1 Nothing.
 * @param data2 Nothing.
 * @return Nothing.
 * @exception WrongSubclass Given data type was illegal.
 */
double
EntryKeyDataOperationSet::coefficient(
    const EntryKeyData*,
    const EntryKeyData*) const
    throw (WrongSubclass) {
 
    throw WrongSubclass(__FILE__, __LINE__,
                        "EntryKeyDataOperationSet::coefficient");
    return 0.0; // stupid return statement to keep compiler quiet
}

/**
 * Converts the operation set into a string.
 *
 * @return Operation set as a string.
 */
std::string
EntryKeyDataOperationSet::toString() const {

    string result = "";
    for (std::set<string>::iterator i = data_.begin();
         i != data_.end();
         i++) {
        
        result += *i;
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// EntryKeyDataBool
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor.
 */
EntryKeyDataBool::EntryKeyDataBool() : data_() {
}

/**
 * Constructor.
 *
 * @param fieldData A boolean.
 */
EntryKeyDataBool::EntryKeyDataBool(bool fieldData) : data_(fieldData) {
}

/**
 * Destructor.
 */
EntryKeyDataBool::~EntryKeyDataBool() {
}

/**
 * Copies the boolean.
 *
 * Client is responsible of deallocating the memory reserved for the
 * returned object.
 *
 * @return A copy of the boolean.
 */
EntryKeyData*
EntryKeyDataBool::copy() const {
    return new EntryKeyDataBool(data_);
}

/**
 * Checks if two booleans are equal.
 *
 * Cannot compare to other data types.
 *
 * @param fieldData Boolean.
 * @return True if two booleans are equal.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataBool::isEqual(const EntryKeyData* fieldData) const 
    throw (WrongSubclass) {
    
    EntryKeyDataBool* data = dynamic_cast<EntryKeyDataBool*>(
        const_cast<EntryKeyData*>(fieldData));
    if (data == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataBool::isEqual");	
    }
    return data_ == data->data_;
}

/**
 * Checks if another operation set is greater.
 *
 * Cannot compare to other data types. You cannot say which is greater in 
 * case of booleans and false is returned always.
 *
 * @param fieldData Boolen.
 * @return Always false.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataBool::isGreater(const EntryKeyData*) const 
    throw (WrongSubclass) {

    return false;
}

/**
 * Checks if this operation set is smaller than another set.
 *
 * Cannot compare to other data types. You cannot say which is greater in 
 * case of booleans and true is returned always.
 *
 * @param fieldData Boolean.
 * @return Always true.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataBool::isSmaller(const EntryKeyData*) const 
    throw (WrongSubclass) {

    return true;
}

/**
 * Cannot be called for EntryKeyDataBool.
 *
 * Booleans cannot be compared with greater of smaller and no coefficient
 * can be counted.
 *
 * @param data1 Nothing.
 * @param data2 Nothing.
 * @return Nothing
 * @exception WrongSubclass Given data type was illegal.
 */
double
EntryKeyDataBool::coefficient(
    const EntryKeyData*,
    const EntryKeyData*) const
    throw (WrongSubclass) {
 
    throw WrongSubclass(__FILE__, __LINE__,
                        "EntryKeyDataBool::coefficient");
    return 0.0; // stupid return statement to keep compiler quiet
}

/**
 * Converts the boolean into a string.
 *
 * @return Boolean as a string.
 */
std::string
EntryKeyDataBool::toString() const {

    if (data_) {
        return "true";
    } else {
        return "false";
    }
}


///////////////////////////////////////////////////////////////////////////////
// EntryKeyDataFunctioUnit
///////////////////////////////////////////////////////////////////////////////

/**
 * Default constructor.
 */
EntryKeyDataFunctionUnit::EntryKeyDataFunctionUnit() : data_() {
}

/**
 * Constructor.
 *
 * @param fieldData A set of parameters.
 */
EntryKeyDataFunctionUnit::EntryKeyDataFunctionUnit(
    const FunctionUnit* fieldData) : data_(fieldData) {
}

/**
 * Destructor.
 */
EntryKeyDataFunctionUnit::~EntryKeyDataFunctionUnit() {
}

/**
 * Copies the set of parameters.
 *
 * Client is responsible of deallocating the memory reserved for the
 * returned object.
 *
 * @return A copy of the parameter set.
 */
EntryKeyData*
EntryKeyDataFunctionUnit::copy() const {
    return new EntryKeyDataFunctionUnit(data_);
}

/**
 * Checks if two FunctionUnits are equal
 *
 * Cannot compare to other data types.
 *
 * @param fieldData FunctionUnit pointer.
 * @return True if two FunctionUnits are equal.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataFunctionUnit::isEqual(const EntryKeyData* fieldData) const 
    throw (WrongSubclass) {
    
    EntryKeyDataFunctionUnit* data = dynamic_cast<EntryKeyDataFunctionUnit*>(
        const_cast<EntryKeyData*>(fieldData));
    if (data == NULL) {
        throw WrongSubclass(__FILE__, __LINE__,
                            "EntryKeyDataFunctionUnit::isEqual");	
    }

    return data_->isArchitectureEqual(data->data_, false);
}

/**
 * Checks if another EntryKeyDataFunctionUnit is greater.
 *
 * Cannot compare to other data types. Cannot say which is greater in 
 * case of FunctionUnits and false is returned always.
 *
 * @param fieldData FunctionUnit*.
 * @return Always false.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataFunctionUnit::isGreater(const EntryKeyData*) const 
    throw (WrongSubclass) {

    return false;
}

/**
 * Checks if another EntryKeyDataFunctionUnit is smaller.
 *
 * Cannot compare to other data types. Cannot say which is smaller in 
 * case of FunctionUnits and true is returned always.
 *
 * @param fieldData FunctionUnit*.
 * @return Always true.
 * @exception WrongSubclass Given data type was illegal.
 */
bool
EntryKeyDataFunctionUnit::isSmaller(const EntryKeyData*) const 
    throw (WrongSubclass) {

    return true;
}

/**
 * Cannot be called for EntryKeyDataFunctionUnit.
 *
 * FunctionUnits cannot be compared with greater of smaller and no coefficient
 * can be counted.
 *
 * @param data1 Nothing.
 * @param data2 Nothing.
 * @return Nothing
 * @exception WrongSubclass Given data type was illegal.
 */
double
EntryKeyDataFunctionUnit::coefficient(
    const EntryKeyData*,
    const EntryKeyData*) const
    throw (WrongSubclass) {
 
    throw WrongSubclass(__FILE__, __LINE__,
                        "EntryKeyDataFunctionUnit::coefficient");
    return 0.0; // stupid return statement to keep compiler quiet
}

/**
 * Converts the FunctionUnit name and operations into a string.
 *
 * @return FunctionUnit name and operations as a string.
 */
std::string
EntryKeyDataFunctionUnit::toString() const {
    
    string result = "";
    result += data_->name();
    result += ":";
    for (int i = 0; i < data_->operationCount(); i++) {
        result += data_->operation(i)->name();
        if ((i + 1) < data_->operationCount()) {
            result += "_";
        }
    }
    return result;
}
