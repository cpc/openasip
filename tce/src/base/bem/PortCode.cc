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
 * @file PortCode.cc
 *
 * Implementation of PortCode class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "PortCode.hh"
#include "ObjectState.hh"
#include "MathTools.hh"

const std::string PortCode::OSNAME_PORT_CODE = "port_code";
const std::string PortCode::OSKEY_UNIT_NAME = "unit_name";
const std::string PortCode::OSKEY_ENCODING = "encoding";
const std::string PortCode::OSKEY_EXTRA_BITS = "extra_bits";
const std::string PortCode::OSKEY_INDEX_WIDTH = "index_width";


/**
 * The constructor.
 *
 * Creates a port code with port ID.
 *
 * @param unitName Name of the unit.
 * @param encoding The encoding.
 * @param extraBits The number of extra bits in the encoding.
 * @param indexWidth The width of the register index.
 * @exception OutOfRange If the some of the given values is negative.
 */
PortCode::PortCode(
    const std::string& unitName, 
    unsigned int encoding,
    unsigned int extraBits,
    int indexWidth)
    throw (OutOfRange) :
    unitName_(unitName), encoding_(encoding),
    extraBits_(extraBits), indexWidth_(indexWidth), hasEncoding_(true),
    parent_(NULL) {

    if (indexWidth_ < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
}


/**
 * The constructor.
 *
 * Creates a port code without port ID.
 *
 * @param unitName Name of the unit.
 * @param indexWidth The width of the register index.
 * @exception OutOfRange If the some of the given values is negative.
 */
PortCode::PortCode(
    const std::string& unitName, 
    int indexWidth)
    throw (OutOfRange) :
    unitName_(unitName), encoding_(0), extraBits_(0),
    indexWidth_(indexWidth), hasEncoding_(false), parent_(NULL) {

    if (indexWidth_ < 0) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is erroneous.
 */
PortCode::PortCode(const ObjectState* state)
    throw (ObjectStateLoadingException) : 
    unitName_(""), encoding_(0), extraBits_(0), indexWidth_(0), 
    hasEncoding_(false), parent_(NULL) {        

    try {
        unitName_ = state->stringAttribute(OSKEY_UNIT_NAME);
        if (state->hasAttribute(OSKEY_ENCODING)) {
            encoding_ = state->intAttribute(OSKEY_ENCODING);
            extraBits_ = state->intAttribute(OSKEY_EXTRA_BITS);
            hasEncoding_ = true;
        }
        if (state->hasAttribute(OSKEY_INDEX_WIDTH)) {
            indexWidth_ = state->intAttribute(OSKEY_INDEX_WIDTH);
        }
    } catch (const Exception&) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, __func__);
    }
}


/**
 * The destructor.
 */
PortCode::~PortCode() {
}


/**
 * Returns the name of the unit.
 *
 * @return The name of the unit.
 */
std::string
PortCode::unitName() const {
    return unitName_;
}


/**
 * Tells whether the port code has an encoding (port ID).
 *
 * @return True if the port code has an encoding, otherwise false.
 */
bool
PortCode::hasEncoding() const {
    return hasEncoding_;
}


/**
 * Returns the encoding.
 *
 * @return The encoding.
 * @exception NotAvailable If the port code does not have an encoding 
 *                         (port ID).
 */
unsigned int
PortCode::encoding() const 
    throw (NotAvailable) {

    if (!hasEncoding()) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }
    return encoding_;
}


/**
 * Returns the number of extra bits in the encoding.
 *
 * @return The number of extra bits.
 */
unsigned int
PortCode::extraBits() const {
    return extraBits_;
}


/**
 * Returns the width of the whole port code (portID + opcode).
 *
 * @return The width.
 */
int
PortCode::width() const {
    if (hasEncoding()) {
        return MathTools::requiredBits(encoding()) + extraBits() + 
            indexWidth();
    } else {
        return indexWidth();
    }
}


/**
 * Returns the width of the encoding (port ID).
 *
 * @return The width of the encoding.
 */
int
PortCode::encodingWidth() const {
    return width() - indexWidth();
}


/**
 * Returns the width of the register index (opcode).
 *
 * @return The width of the register index.
 */
int
PortCode::indexWidth() const {
    return indexWidth_;
}


/**
 * Returns the parent socket code table.
 *
 * @return The parent socket code table.
 */
SocketCodeTable*
PortCode::parent() const {
    return parent_;
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
PortCode::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_PORT_CODE);
    state->setAttribute(OSKEY_UNIT_NAME, unitName());
    if (hasEncoding()) {
        state->setAttribute(OSKEY_ENCODING, encoding());
        state->setAttribute(OSKEY_EXTRA_BITS, extraBits());
    }
    state->setAttribute(OSKEY_INDEX_WIDTH, indexWidth());
    return state;
}


/**
 * Sets the parent socket code table pointer.
 *
 * @param parent The parent pointer.
 */
void
PortCode::setParent(SocketCodeTable* parent) {
    parent_ = parent;
}
