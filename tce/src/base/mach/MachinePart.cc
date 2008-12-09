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
 * @file MachinePart.cc
 *
 * Implementation of MachinePart class and derived Component and SubComponent
 * classes.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 17 Jun 2003 by jn, pj, jm, ll
 * @note rating: red
 */

#include <string>

#include "MachinePart.hh"
#include "MachineTester.hh"
#include "MOMTextGenerator.hh"
#include "Application.hh"

using std::string;
using boost::format;

namespace TTAMachine {

/////////////////////////////////////////////////////////////////////////////
// MachinePart
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
MachinePart::MachinePart() : id_(idCounter_++){
}


/**
 * Destructor.
 */
MachinePart::~MachinePart() {
}

int MachinePart::idCounter_ = 0;

/**
   Comparator
*/
bool
MachinePart::Comparator::operator() (
    const MachinePart* mp1, const MachinePart* mp2) const {
    if (mp1 == NULL) {
        return false;
    }
    if (mp2 == NULL) {
        return true;
    }
    return mp1->id_ > mp2->id_;
}

/////////////////////////////////////////////////////////////////////////////
// Component
/////////////////////////////////////////////////////////////////////////////

const string Component::OSNAME_COMPONENT = "component";
const string Component::OSKEY_NAME = "name";

/**
 * Constructor.
 *
 * @param name Name of the component.
 * @exception InvalidName If the given name is not a valid name for a
 *                        component.
 */
Component::Component(const std::string& name)
    throw (InvalidName) :
    MachinePart(), name_(name), machine_(NULL) {

    if (!MachineTester::isValidComponentName(name)) {
        const string procName = "Component::Component";
        MOMTextGenerator textGen;
        format errorMsg = textGen.text(MOMTextGenerator::TXT_INVALID_NAME);
        errorMsg % name;
        throw InvalidName(__FILE__, __LINE__, procName, errorMsg.str());
    }
}


/**
 * Constructor.
 *
 * Loads the name from the given ObjectState object.
 *
 * @param state The ObjectState instance from which the name is loaded.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
Component::Component(const ObjectState* state)
    throw (ObjectStateLoadingException) :
    MachinePart(), machine_(NULL) {

    try {
        setName(state->stringAttribute(OSKEY_NAME));
    } catch (const Exception& e) {
        string procName = "Component::Component";
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, e.errorMessage());
    }
}

/**
 * Destructor.
 */
Component::~Component() {
}


/**
 * Returns the name of the component.
 *
 * @return Name of the component.
 */
std::string
Component::name() const {
    return name_;
}


/**
 * Sets the name of the component.
 *
 * @param name The new name.
 * @exception ComponentAlreadExists Sub class implementations may throw the
 *                                  exception if there exists another
 *                                  component by the same name in the machine
 *                                  already.
 * @exception InvalidName If the given name is not a valid name for
 *                        a component.
 */
void
Component::setName(const std::string& name)
    throw (ComponentAlreadyExists, InvalidName) {

    if (!MachineTester::isValidComponentName(name)) {
        const string procName = "Component::setName";
        MOMTextGenerator textGen;
        format errorMsg = textGen.text(MOMTextGenerator::TXT_INVALID_NAME);
        errorMsg % name;
        throw InvalidName(__FILE__, __LINE__, procName, errorMsg.str());
    }

    name_ = name;
}


/**
 * Returns the machine which is set for this component.
 *
 * @return The machine which is set for this component.
 */
Machine*
Component::machine() const {
    return machine_;
}


/**
 * Ensures that the component is registered to the same machine as the
 * given component.
 *
 * @param component The component.
 * @exception IllegalRegistration If the components are not registered to the
 *                                same machine.
 */
void
Component::ensureRegistration(const Component& component) const
    throw (IllegalRegistration) {

    if (machine() == NULL || machine() != component.machine()) {
        const string procName = "Component::ensureRegistration";
        throw IllegalRegistration(__FILE__, __LINE__, procName);
    }
}


/**
 * Returns true if the component is registered to a machine, otherwise false.
 *
 * @return True if the component is registered to a machine, otherwise
 *         false.
 */
bool
Component::isRegistered() const {
    return (machine_ != NULL);
}


/**
 * Creates a new ObjectState instance and saves the name of the component
 * into it.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
Component::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_COMPONENT);
    state->setAttribute(OSKEY_NAME, name());
    return state;
}


/**
 * Loads the name of the component from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the machine already contains
 *                                        same type of component with the
 *                                        same name.
 */
void
Component::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "Component::loadState";

    try {
        string name = state->stringAttribute(OSKEY_NAME);
        setName(name);
    } catch (const KeyNotFound& e) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    } catch (const ComponentAlreadyExists& e) {
        MOMTextGenerator textGenerator;
        format text = textGenerator.text(
            MOMTextGenerator::TXT_SAME_NAME);
        text % state->stringAttribute(OSKEY_NAME);
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, text.str());
    } catch (const InvalidName& e) {
        MOMTextGenerator textGenerator;
        format text = textGenerator.text(
            MOMTextGenerator::TXT_INVALID_NAME);
        text % state->stringAttribute(OSKEY_NAME);
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, text.str());
    }
}


/////////////////////////////////////////////////////////////////////////////
// SubComponent
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
SubComponent::SubComponent() : MachinePart() {
}


/**
 * Destructor.
 */
SubComponent::~SubComponent() {
}

}
