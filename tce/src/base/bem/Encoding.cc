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
 * @file Encoding.cc
 *
 * Implementation of Encoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "Encoding.hh"
#include "ObjectState.hh"
#include "MathTools.hh"

using std::string;

const std::string Encoding::OSNAME_ENCODING = "encoding";
const std::string Encoding::OSKEY_ENCODING = "encoding";
const std::string Encoding::OSKEY_EXTRA_BITS = "extra_bits";

/**
 * The constructor.
 *
 * @param encoding The encoding.
 * @param extraBits The number of extra bits.
 * @param parent The parent pointer.
 */
Encoding::Encoding(
    unsigned int encoding,
    unsigned int extraBits,
    InstructionField* parent) : 
    encoding_(encoding), extraBits_(extraBits), parent_(parent) {
}


/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @param parent The parent pointer.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
Encoding::Encoding(const ObjectState* state, InstructionField* parent)
    throw (ObjectStateLoadingException) :
    encoding_(0), extraBits_(0), parent_(parent) {

    const string procName = "Encoding::Encoding";

    try {
        encoding_ = state->intAttribute(OSKEY_ENCODING);
        extraBits_ = state->intAttribute(OSKEY_EXTRA_BITS);
    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * The destructor.
 */
Encoding::~Encoding() {
}


/**
 * Returns the parent instruction field.
 *
 * @return The parent instruction field.
 */
InstructionField*
Encoding::parent() const {
    return parent_;
}


/**
 * Returns the encoding.
 *
 * @return The encoding.
 */
unsigned int
Encoding::encoding() const {
    return encoding_;
}


/**
 * Returns the number of extra zero bits in the encoding.
 *
 * @return The number of extra bits.
 */
unsigned int
Encoding::extraBits() const {
    return extraBits_;
}


/**
 * Returns the bit width required by the encoding.
 *
 * @return The bit width.
 */
int
Encoding::width() const {
    return MathTools::requiredBits(encoding()) + extraBits();
}


/**
 * Saves the state of the object to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
Encoding::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_ENCODING);
    state->setAttribute(OSKEY_ENCODING, encoding());
    state->setAttribute(OSKEY_EXTRA_BITS, extraBits());
    return state;
}


/**
 * Sets the parent pointer.
 *
 * @param parent The parent pointer.
 */
void
Encoding::setParent(InstructionField* parent) {
    parent_ = parent;
}
