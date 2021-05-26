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
 * @file Operand.cc
 *
 * Definition of Operand class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2007 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 17 August 2004 by jn, ll, tr, pj
 */

#include <vector>
#include <string>
#include <boost/format.hpp>

#include "Operand.hh"
#include "Conversion.hh"
#include "Application.hh"
#include "ContainerTools.hh"
#include "ObjectState.hh"

using std::vector;
using std::string;
using std::set;

// const declarations used in Operation property file
const string Operand::OPRND_ID = "id";
const string Operand::OPRND_TYPE = "type";
const string Operand::OPRND_MEM_ADDRESS = "mem-address";
const string Operand::OPRND_MEM_UNITS = "mem-units";
const string Operand::OPRND_MEM_DATA = "mem-data";
const string Operand::OPRND_CAN_SWAP = "can-swap";
const string Operand::OPRND_IN = "in";
const string Operand::OPRND_OUT = "out";
const string Operand::OPRND_ELEM_WIDTH = "element-width";
const string Operand::OPRND_ELEM_COUNT = "element-count";

const std::string Operand::SLONG_WORD_STRING = "SLongWord";
const std::string Operand::ULONG_WORD_STRING = "ULongWord";
const std::string Operand::SINT_WORD_STRING = "SIntWord";
const std::string Operand::UINT_WORD_STRING = "UIntWord";
const std::string Operand::FLOAT_WORD_STRING = "FloatWord";
const std::string Operand::HALF_FLOAT_WORD_STRING = "HalfFloatWord";
const std::string Operand::DOUBLE_WORD_STRING = "DoubleWord";
const std::string Operand::BOOL_STRING = "Bool";
const std::string Operand::RAW_DATA_STRING = "RawData";
const std::string Operand::UNKNOWN_TYPE_STRING = "InvalidValue";

/**
 * Default constructor.
 */
Operand::Operand(bool isInput) : 
    Serializable(), index_(0), isInput_(isInput), type_(SINT_WORD), 
    elementCount_(1), isAddress_(false), addressUnits_(0), 
    isMemoryData_(false) {
    elementWidth_ = defaultElementWidth(type_);
}

/**
 * Constructor.
 *
 * @param index Index of operand.
 */ 
Operand::Operand(bool isInput, int index, OperandType type) : 
    Serializable(), index_(index), isInput_(isInput), type_(type),
    elementCount_(1), isAddress_(false), addressUnits_(0), 
    isMemoryData_(false) {
    elementWidth_ = defaultElementWidth(type_);
}

/**
 * Copy constructor.
 *
 * @param op Operand to be copied. 
 */
Operand::Operand(const Operand& op) : 
    Serializable(), index_(op.index()), isInput_(op.isInput()),
    type_(op.type()), elementWidth_(op.elementWidth()), 
    elementCount_(op.elementCount()), isAddress_(op.isAddress()), 
    addressUnits_(op.addressUnits_), isMemoryData_(op.isMemoryData()),
    swap_(op.swap()) {
}

/**
 * Destructor.
 */
Operand::~Operand() {
}

/**
 * Clears the operand.
 *
 * This is used when operand state is reloaded from ObjectState tree. Old
 * must be cleared first.
 */
void
Operand::clear() {
	index_ = 0;
    elementWidth_ = 0;
    elementCount_ = 0;
	isAddress_ = false;
	isMemoryData_ = false;
	swap_.clear();
}

/**
 * Returns the index of the Operand.
 *
 * @return The index of the Operand.
 */
int
Operand::index() const {
    return index_;
}

/**
 * Returns true if operand is input.
 *
 * @return True if operand is input.
 */
bool
Operand::isInput() const {
    return isInput_;
}

/**
 * Returns true if operand is output.
 *
 * @return True if operand is output.
 */
bool
Operand::isOutput() const {
    return !isInput_;
}

/**
 * Returns the type of the Operand.
 *
 * @return The type of the Operand.
 */
Operand::OperandType 
Operand::type() const {
    return type_;
}

/**
 * Sets the operand type.
 *
 * @param type The new type of the operand.
 */
void
Operand::setType(OperandType type) {
    type_ = type;
}

/**
 * Returns the string type of the Operand.
 *
 * @return The string type of the Operand.
 */
const std::string&
Operand::typeString() const {
    switch (type_)
    {
        case SINT_WORD:
            return SINT_WORD_STRING;
            break;
        case UINT_WORD:
            return UINT_WORD_STRING;
            break;
        case SLONG_WORD:
            return SLONG_WORD_STRING;
            break;
        case ULONG_WORD:
            return ULONG_WORD_STRING;
            break;
        case FLOAT_WORD:
            return FLOAT_WORD_STRING;
            break;
        case DOUBLE_WORD:
            return DOUBLE_WORD_STRING;
            break;
        case HALF_FLOAT_WORD:
            return HALF_FLOAT_WORD_STRING;
            break;
        case BOOL:
            return BOOL_STRING;
            break;
        case RAW_DATA:
            return RAW_DATA_STRING;
            break;
        default:
            return UNKNOWN_TYPE_STRING;
            break;
    }
}

TCEString 
Operand::CTypeString() const {
    if (isVector()) {
        return "";
    }
    TCEString type;
    switch (this->type()) {
    case Operand::SINT_WORD:
        type = "signed";
        break;
    case Operand::UINT_WORD:
        type = "unsigned";
        break;
    case Operand::FLOAT_WORD:
        return "float";
        break;
    case Operand::DOUBLE_WORD:
        return "double";
        break;
    case Operand::RAW_DATA:
        return "";
        break;
    default:
    // TODO: how do halffloats work?
    // they should not need any casts.
    // cannot return half as it's only supported in opencl.
        return "";
    }
    if (elementWidth() <= 8) {
        return type + " char";
    }
    if (elementWidth() <= 16) {
        return type + " short";
    }
    if (elementWidth() <= 64) {
        return type + " long";
    }
    return type + " int";
}


/**
 * Tells if the operand is a vector operand, in other words, has subwords.
 *
 * @return True, if the operand is a vector operand.
 */
bool
Operand::isVector() const {
    return (elementCount_ > 1);
}

/**
 * Returns width of an operand element in bits.
 *
 * @return Bit width of an element.
 */
int
Operand::elementWidth() const {
    return elementWidth_;
}

/**
 * Sets the element bit width.
 *
 * @param New element bit width.
 */
void
Operand::setElementWidth(int elementWidth) {
    elementWidth_ = elementWidth;
}

/**
 * Returns total number of elements.
 *
 * @return Number of elements.
 */
int
Operand::elementCount() const {
    return elementCount_;
}

/**
 * Sets the number of elements.
 *
 * @param New element count.
 */
void
Operand::setElementCount(int elementCount) {
    elementCount_ = elementCount;
}

/**
 * Returns width of the operand in bits.
 *
 * @return Bit width of the operand.
 */
int
Operand::width() const {
    return elementWidth_ * elementCount_;
}

/**
 * Returns true if Operand is address.
 *
 * @return True if Operand is address, false otherwise.
 */
bool
Operand::isAddress() const {
    return isAddress_;
}

/**
 * Returns the count of MAU's which this operand addresses.
 *
 * returns 0 if the operand is not a memory address,
 * or the size of the memory access int MAU's this operation accesses.
 *
 * @return count of MAUs addressed by this operand.
 */
int
Operand::memoryUnits() const {
    return addressUnits_;
}

/**
 * Returns true if Operand is memory data.
 *
 * @return True if Operand is memory data, false otherwise.
 */
bool
Operand::isMemoryData() const {
    return isMemoryData_;
}

/**
 * Returns the set of operands that can be swapped with this operand.
 *
 * @return The set of operands that can be swapped with this operand.
 */
const std::set<int>&
Operand::swap() const {
    return swap_;
}

/**
 * Returns true if Operand can be swapped with op.
 *
 * @param op Operand being investigated.
 * @return True if Operand can be swapped with op, false otherwise.
 */
bool
Operand::canSwap(const Operand& op) const {
    return ContainerTools::containsValue(swap_, op.index());
}

/**
 * Loads Operand state from an ObjectState object.
 *
 * @param state State containing operand properties.
 * @exception ObjectStateLoadingException If loading fails for some reason.
 */
void
Operand::loadState(const ObjectState* state) {
    clear();

    string method = "Operand::loadState()";
    
    try {
        index_ = state->intAttribute(OPRND_ID);        
        std::string typeString = state->stringAttribute(OPRND_TYPE);
            
        if (typeString.compare(SINT_WORD_STRING) == 0) {
            type_ = SINT_WORD;
        } else if (typeString.compare(UINT_WORD_STRING) == 0) {
            type_ = UINT_WORD;
        } else if (typeString.compare(SLONG_WORD_STRING) == 0) {
            type_ = SLONG_WORD;
        } else if (typeString.compare(ULONG_WORD_STRING) == 0) {
            type_ = ULONG_WORD;
        } else if (typeString.compare(FLOAT_WORD_STRING) == 0) {
            type_ = FLOAT_WORD;
        } else if (typeString.compare(DOUBLE_WORD_STRING) == 0) {
            type_ = DOUBLE_WORD;
        } else if (typeString.compare(HALF_FLOAT_WORD_STRING) == 0) {
            type_ = HALF_FLOAT_WORD;
        } else if (typeString.compare(BOOL_STRING) == 0) {
            type_ = BOOL;
        } else if (typeString.compare(RAW_DATA_STRING) == 0) {
            type_ = RAW_DATA;
        } else {
            string msg = "Invalid operand type: \"" + typeString + "\""; 
            throw Exception(__FILE__, __LINE__, method, msg);
        }

        if (state->hasAttribute(OPRND_ELEM_WIDTH)) {
            elementWidth_ = state->intAttribute(OPRND_ELEM_WIDTH);
        } else {
            elementWidth_ = defaultElementWidth(type_);
        }

        if (state->hasAttribute(OPRND_ELEM_COUNT)) {
            elementCount_ = state->intAttribute(OPRND_ELEM_COUNT);
        } else {
            elementCount_ = 1;
        }
    
        isAddress_ = state->boolAttribute(OPRND_MEM_ADDRESS);
        isMemoryData_ = state->boolAttribute(OPRND_MEM_DATA);

        if (state->hasAttribute(OPRND_MEM_UNITS)) {
            addressUnits_ = state->intAttribute(OPRND_MEM_UNITS);
        }

        if (state->childCount() > 1) {
            string msg = "Erronous number of children";
            throw Exception(__FILE__, __LINE__, method, msg);
        }
     
        for (int j = 0; j < state->childCount(); j++) {
            ObjectState* child = state->child(j);
            if (child->name() == OPRND_CAN_SWAP) {
                for (int i = 0; i < child->childCount(); i++) {
                    ObjectState* canSwap = child->child(i);
                    int id = 
                        Conversion::toInt(canSwap->stringAttribute(OPRND_ID));
                    swap_.insert(id);
                }
            } else {
                string msg = "Unknown attribute: " + child->name();
                throw Exception(__FILE__, __LINE__, method, msg);
            }
        }
        
    } catch (const NumberFormatException& n) {
        string message = "Conversion failed: " + n.errorMessage();
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, method, message);

    } catch (const KeyNotFound& k) {
        string message = "Attribute not found: " + k.errorMessage();
        ObjectStateLoadingException error(
            __FILE__, __LINE__, method, message);
        error.setCause(k);
        throw error;
    } catch (Exception& e) {
        string message = 
            "Error while loading operation operand settings. "
            "An error in the operation format?" + e.errorMessageStack();
        ObjectStateLoadingException newe(
            __FILE__, __LINE__, __func__, message);
        newe.setCause(e);
        throw newe;
    }
    catch (...) {
        string message = 
            "Error while loading operation operand settings. "
            "An error in the operation format?";
        ObjectStateLoadingException newe(
            __FILE__, __LINE__, __func__, message);
        throw newe;
    }
}

/**
 * Saves the state of the Operation to ObjectState object.
 *
 * @return The created ObjectState tree.
 */
ObjectState*
Operand::saveState() const {
    
    ObjectState* root = new ObjectState("");
    root->setAttribute(OPRND_ID, index_);
    
    switch(type_) {
    case SINT_WORD:
        root->setAttribute(OPRND_TYPE, SINT_WORD_STRING);
        break;
    case UINT_WORD:
        root->setAttribute(OPRND_TYPE, UINT_WORD_STRING);
        break;
    case SLONG_WORD:
        root->setAttribute(OPRND_TYPE, SLONG_WORD_STRING);
        break;
    case ULONG_WORD:
        root->setAttribute(OPRND_TYPE, ULONG_WORD_STRING);
        break;
    case FLOAT_WORD:
        root->setAttribute(OPRND_TYPE, FLOAT_WORD_STRING);
        break;
    case DOUBLE_WORD:
        root->setAttribute(OPRND_TYPE, DOUBLE_WORD_STRING);
        break;
    case HALF_FLOAT_WORD:
        root->setAttribute(OPRND_TYPE, HALF_FLOAT_WORD_STRING);
        break;
    case BOOL:
        root->setAttribute(OPRND_TYPE, BOOL_STRING);
        break;
    case RAW_DATA:
        root->setAttribute(OPRND_TYPE, RAW_DATA_STRING);
        break;
    default:
        root->setAttribute(OPRND_TYPE, UNKNOWN_TYPE_STRING);        
    }
    
    root->setAttribute(OPRND_ELEM_WIDTH, elementWidth_);
    root->setAttribute(OPRND_ELEM_COUNT, elementCount_);

    root->setAttribute(OPRND_MEM_ADDRESS, isAddress_);
    root->setAttribute(OPRND_MEM_DATA, isMemoryData_);
    if (addressUnits_ != 0) {
        root->setAttribute(OPRND_MEM_UNITS, addressUnits_);
    }

    if (swap_.size() > 0) {
        ObjectState* canSwap = new ObjectState(OPRND_CAN_SWAP);
        set<int>::const_iterator it = swap_.begin();
        while (it != swap_.end()) {
            ObjectState* canSwapChild = new ObjectState(OPRND_IN);
            canSwapChild->setAttribute(OPRND_ID, *it);
            canSwap->addChild(canSwapChild);
            it++;
        }
        root->addChild(canSwap);
    }
    return root;
}

/**
 * Returns default element width depending on operand's type.
 *
 * @param type Operand type.
 * @return Element bit width of the operand.
 */
int
Operand::defaultElementWidth(OperandType type) {
    if (type == DOUBLE_WORD || type == SLONG_WORD || type == ULONG_WORD) {
        return 64;
    } else if (type == HALF_FLOAT_WORD) {
        return 16;
    } else if (type == BOOL) {
        return 1;
    } else {
        return 32;
    }
}

//////////////////////////////////////////////////////////////////////////////
// NullOperand
//////////////////////////////////////////////////////////////////////////////

NullOperand* NullOperand::instance_ = NULL;

/**
 * Constructor.
 */
NullOperand::NullOperand() : Operand(false) {
}

/**
 * Destructor.
 */
NullOperand::~NullOperand() {
}

/**
 * Aborts program with error log message.
 *
 * @return 0.
 */
int
NullOperand::index() const {
    abortWithError("index()");
    return 0;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperand::isInput() const {
    abortWithError("isInput()");
    return 0;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperand::isOutput() const {
    abortWithError("isOutput()");
    return 0;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperand::isAddress() const {
    abortWithError("isAddress()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperand::isMemoryData() const {
    abortWithError("isMemoryData()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return An empty vector.
 */
const set<int>&
NullOperand::swap() const {
    abortWithError("swap()");
    return swap_;
}

/**
 * Aborts the program with error message.
 *
 * @return false.
 */
bool
NullOperand::canSwap(const Operand&) const {
    abortWithError("canSwap()");
    return false;
}

