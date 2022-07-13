/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file OperationTriggeredField.cc
 *
 * Implementation of OperationTriggeredField class.
 *
 * @author Kari Hepola 2022 (kari.hepola@tuni.fi)
 * @note rating: red
 */


#include "OperationTriggeredField.hh"
#include "OperationTriggeredEncoding.hh"
#include "ObjectState.hh"

const std::string OperationTriggeredField::OSNAME_FIELD = "ota-field";
const std::string OperationTriggeredField::OSKEY_PIECE_NAME = "piece";
const std::string OperationTriggeredField::OSKEY_START_NAME = "start";
const std::string OperationTriggeredField::OSKEY_WIDTH_NAME = "width";

/**
 * The constructor.
 * 
 * Registers the field encoding to the parent
 * binary encoding automatically.
 * 
 * @param parent The parent binaryEncoding.
 */

OperationTriggeredField::OperationTriggeredField(
    OperationTriggeredEncoding& parent, int piece, int start, int width)
    : piece_(piece), start_(start), width_(width) {
        parent.addField(*this);
}

/**
 * The constructor
 * 
 * Loads the state of the field encoding from
 * the given ObjectState tree
 * 
 * @param state The ObjectState tree
 * @param parent The parent operation triggered encoding
 * @exception ObjectStateLoadingException If an error occurs while loading
              the state.
 */

OperationTriggeredField::OperationTriggeredField(const ObjectState* state,
OperationTriggeredEncoding& parent) {
        parent.addField(*this);
        loadState(state);
}

/**
 * Returns the piece index that indicates the index of the encoding piece.
 * 
 * @return The encoding piece index.      
 */

int
OperationTriggeredField::piece() const {
    return piece_;
}

void 
OperationTriggeredField::setPiece(int piece) {
    piece_ = piece;
}

/**
 * Returns the piece index that indicates the index of the encoding piece.
 * 
 * @return The encoding piece index.      
 */

int
OperationTriggeredField::start() const {
    return start_;
}

void 
OperationTriggeredField::setStart(int start) {
    start_ = start;
}

/**
 *  Always returns 0 because field encoding does not
 *  have any child fields.
 * 
 * @return 0
 */

int
OperationTriggeredField::childFieldCount() const {
    return 0;
}

/**
 *  Returns the bit width of the field encoding
 * 
 * @return The bit width of the field encoding.  
 */

int
OperationTriggeredField::width() const {
    return width_;
}

void 
OperationTriggeredField::setWidth(int width) {
    width_ = width;
}

/**
 * Loads the state of the field
 * encoding from the given ObjectState tree. 
 * 
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *            the state. 
 */

void
OperationTriggeredField::loadState(const ObjectState* state) {
    ObjectState* newState = new ObjectState(*state);

    try {
        piece_ = newState->intAttribute(OSKEY_PIECE_NAME);
        start_ = newState->intAttribute(OSKEY_START_NAME);
        width_ = newState->intAttribute(OSKEY_WIDTH_NAME);
        
    } catch (const Exception& exception) {
     const std::string procName = "OperationTriggeredField::loadState";
     throw ObjectStateLoadingException(
         __FILE__, __LINE__, procName, exception.errorMessage());
    }
 
    delete newState;
}

/**
 * Saves the state of the field encoding
 * to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */

ObjectState*
OperationTriggeredField::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_FIELD);   
    state->setAttribute(OSKEY_PIECE_NAME, piece_);
    state->setAttribute(OSKEY_START_NAME, start_);
    state->setAttribute(OSKEY_WIDTH_NAME, width_);

return state;
}