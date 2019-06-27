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
 * @file ImmediateControlField.cc
 *
 * Implementation of ImmediateControlField class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "ImmediateControlField.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "NullInstructionField.hh"
#include "MathTools.hh"
#include "MapTools.hh"
#include "Application.hh"
#include "ObjectState.hh"

using std::string;
using std::pair;

const std::string ImmediateControlField::OSNAME_IMM_CONTROL_FIELD = "ic_field";
const std::string ImmediateControlField::OSNAME_TEMPLATE_MAP = "temp_map";
const std::string ImmediateControlField::OSKEY_TEMPLATE_NAME = "temp_name";
const std::string ImmediateControlField::OSKEY_ENCODING = "encoding";

/**
 * The constructor.
 *
 * Registers the immediate control field to the parent BinaryEncoding
 * automatically. The field is added as the leftmost field of the TTA
 * instruction.
 *
 * @param parent The parent encoding map.
 * @exception ObjectAlreadyExists If the given encoding map has an immediate
 *                                control field already.
 */
ImmediateControlField::ImmediateControlField(BinaryEncoding& parent)
    : InstructionField(&parent) {
    setParent(NULL);
    parent.setImmediateControlField(*this);
    setParent(&parent);
}

/**
 * The constructor.
 *
 * Loads the state of the object from the given ObjectState tree.
 *
 * @param state The ObjectState tree.
 * @param parent The parent encoding map.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 * @exception ObjectAlreadyExists If the given encoding map has an immediate
 *                                control field already.
 */
ImmediateControlField::ImmediateControlField(
    const ObjectState* state, BinaryEncoding& parent)
    : InstructionField(state, &parent) {
    loadState(state);
    setParent(NULL);
    parent.setImmediateControlField(*this);
    setParent(&parent);
}

/**
 * The destructor.
 */
ImmediateControlField::~ImmediateControlField() {
    BinaryEncoding* oldParent = parent();
    assert(oldParent != NULL);
    setParent(NULL);
    oldParent->unsetImmediateControlField();
}


/**
 * Returns the parent encoding map.
 *
 * @return The parent encoding map.
 */
BinaryEncoding*
ImmediateControlField::parent() const {
    InstructionField* parent = InstructionField::parent();
    if (parent == NULL) {
	return NULL;
    } else {
	BinaryEncoding* encoding = dynamic_cast<BinaryEncoding*>(parent);
	assert(encoding != NULL);
	return encoding;
    }
}


/**
 * Returns the number of instruction templates with a binary encoding assigned.
 *
 * @return The number of instruction templates.
 */
int
ImmediateControlField::templateCount() const {
    return templates_.size();
}


/**
 * Returns the name of the instruction template encoded in this field at the
 * given position.
 *
 * @param index The position index.
 * @exception OutOfRange If the given index is negative or not smaller than the
 *                       number of encodings of instruction templates.
 */
std::string
ImmediateControlField::instructionTemplate(int index) const {
    if (index < 0 || index >= templateCount()) {
	const string procName = "ImmediateControlField::instructionTemplate";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }

    int counter(0);
    for (EncodingMap::const_iterator iter = templates_.begin();
	 iter != templates_.end(); iter++) {

	if (counter == index) {
	    return iter->first;
	} else {
	    counter++;
	}
    }

    assert(false);
    return "";
}

/**
 * Tells whether the instruction template with the given name has has a binary
 * encoding in this field.
 *
 * @param name Name of the instruction template.
 * @return True if the template has a binary encoding, otherwise false.
 */
bool
ImmediateControlField::hasTemplateEncoding(const std::string& name) const {
    return MapTools::containsKey(templates_, name);
}


/**
 * Returns the code that identifies the instruction template with the given
 * name.
 *
 * @param name Name of the instruction template.
 * @return The code that identifies the given instruction template.
 * @exception InstanceNotFound If the given instruction template does not have
 *                             a binary encoding in this field.
 */
unsigned int
ImmediateControlField::templateEncoding(const std::string& name) const {
    if (!hasTemplateEncoding(name)) {
	const string procName = "ImmediateControlField::templateEncoding";
	throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    EncodingMap::const_iterator iter = templates_.find(name);
    assert(iter != templates_.end());
    return iter->second;
}

/**
 * Assings the given code to the instruction template identified by the given
 * name.
 *
 * If the given instruction template has an encoding already, replaces the
 * encoding with the given one.
 *
 * @param name Name of the instruction template.
 * @param encoding The code to be assigned.
 * @exception ObjectAlreadyExists If the given code is already assigned to
 *                                another instruction template.
 */
void
ImmediateControlField::addTemplateEncoding(
    const std::string& name, unsigned int encoding) {
    if (MapTools::containsValue(templates_, encoding) &&
	MapTools::keyForValue<string>(templates_, encoding) != name) {
	const string procName = "ImmediateControlField::addTemplateEncoding";
	throw ObjectAlreadyExists(__FILE__, __LINE__, procName);
    }


    pair<EncodingMap::iterator, bool> result =
	templates_.insert(pair<string, int>(name, encoding));

    if (!result.second) {
	// if there is a code for the template already
	EncodingMap::iterator iter = templates_.find(name);
	assert(iter != templates_.end());
	iter->second = encoding;
    }
}

/**
 * Removes the code of the instruction template with the given name.
 *
 * @param name Name of the instruction template.
 */
void
ImmediateControlField::removeTemplateEncoding(const std::string& name) {
    templates_.erase(name);
}


/**
 * Returns the bit width of the immediate control field.
 *
 * @return The bit width of the field.
 */
int
ImmediateControlField::width() const {
    int minWidth(0);
    for (EncodingMap::const_iterator iter = templates_.begin();
	 iter != templates_.end(); iter++) {
	int encoding = iter->second;
	int requiredBits = MathTools::bitLength(encoding);
	if (requiredBits > minWidth) {
	    minWidth = requiredBits;
	}
    }
    return minWidth + extraBits();
}


/**
 * Always returns 0.
 *
 * @return 0.
 */
int
ImmediateControlField::childFieldCount() const {
    return 0;
}


/**
 * Always throws OutOfRange exception because immediate control field does
 * not have any child fields.
 *
 * @return Never returns.
 * @exception OutOfRange Always thrown.
 */
InstructionField&
ImmediateControlField::childField(int) const {
    const string procName = "ImmediateControlField::childField";
    throw OutOfRange(__FILE__, __LINE__, procName);
}

/**
 * Loads the state of the immediate control field from the given ObjectState
 * tree.
 *
 * @param state The ObjectState tree.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
ImmediateControlField::loadState(const ObjectState* state) {
    clearTemplateEncodings();
    InstructionField::loadState(state);
    const string procName = "ImmediateControlField::loadState";

    if (state->name() != OSNAME_IMM_CONTROL_FIELD) {
	throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
	for (int i = 0; i < state->childCount(); i++) {
	    ObjectState* child = state->child(i);
	    if (child->name() != OSNAME_TEMPLATE_MAP) {
		throw ObjectStateLoadingException(
		    __FILE__, __LINE__, procName);
	    }
	    string tempName = child->stringAttribute(OSKEY_TEMPLATE_NAME);
	    int encoding = child->intAttribute(OSKEY_ENCODING);
	    addTemplateEncoding(tempName, encoding);
	}

    } catch (const Exception& exception) {
	throw ObjectStateLoadingException(
	    __FILE__, __LINE__, procName, exception.errorMessage());
    }
}

/**
 * Saves the state of the immediate control field to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
ImmediateControlField::saveState() const {

    ObjectState* state = InstructionField::saveState();
    state->setName(OSNAME_IMM_CONTROL_FIELD);
    state->setAttribute(OSKEY_POSITION, relativePosition());

    for (int i = 0; i < templateCount(); i++) {
	string temp = instructionTemplate(i);
	int encoding = templateEncoding(temp);
	ObjectState* tempMap = new ObjectState(OSNAME_TEMPLATE_MAP);
	state->addChild(tempMap);
	tempMap->setAttribute(OSKEY_TEMPLATE_NAME, temp);
	tempMap->setAttribute(OSKEY_ENCODING, encoding);
    }

    return state;
}


/**
 * Clears all the template encodings from the immediate control field.
 */
void
ImmediateControlField::clearTemplateEncodings() {
    templates_.clear();
}
