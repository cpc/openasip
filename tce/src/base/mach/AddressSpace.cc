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
 * @file AddressSpace.cc
 *
 * Implementation of AddressSpace class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 10 Jun 2004 by vpj, am, tr, ll
 * @note rating: red
 */

#include <string>

#include "AddressSpace.hh"
#include "ControlUnit.hh"
#include "MOMTextGenerator.hh"
#include "Application.hh"
#include "Machine.hh"

using std::string;
using boost::format;

namespace TTAMachine {

// initialization of static data members
const string AddressSpace::OSNAME_ADDRESS_SPACE = "adress_space";
const string AddressSpace::OSKEY_WIDTH = "width";
const string AddressSpace::OSKEY_MIN_ADDRESS = "min_a";
const string AddressSpace::OSKEY_MAX_ADDRESS = "max_a";

/**
 * Constructor.
 *
 * @param name Name of the address space.
 * @param width Bit width of the minimum addressable word.
 * @param minAddress Lowest address in the address space.
 * @param maxAddress Highest address in the address space.
 * @param owner The machine which owns the address space.
 * @exception ComponentAlreadyExists If another address space by the same
 *                                   name is already registered to the
 *                                   machine.
 * @exception OutOfRange If the some of the given parameters is out of range.
 * @exception InvalidName If the given name is not valid for a component.
 */
AddressSpace::AddressSpace(
    const string& name,
    int width,
    unsigned int minAddress,
    unsigned int maxAddress,
    Machine& owner)
    throw (ComponentAlreadyExists, OutOfRange, InvalidName) :
    Component(name) {

    if (width <= 0 || minAddress >= maxAddress) {
        string procName = "AddressSpace::AddressSpace";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    width_ = width;
    minAddress_ = minAddress;
    maxAddress_ = maxAddress;

    setMachine(owner);
}


/**
 * Constructor.
 *
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState from which the name is taken.
 * @param owner The machine which owns the address space.
 * @exception ObjectStateLoadingException If the machine already has an
 *                                        address space by the same name as
 *                                        the coming name of this or if
 *                                        the ObjectState instance is
 *                                        invalid.
 */
AddressSpace::AddressSpace(const ObjectState* state, Machine& owner)
    throw (ObjectStateLoadingException) :
    Component(state), width_(0), minAddress_(0), maxAddress_(0) {

    loadState(state);
    try {
        setMachine(owner);
    } catch (ComponentAlreadyExists&) {
        MOMTextGenerator textGenerator;
        format errorMsg = textGenerator.text(MOMTextGenerator::
                                         TXT_AS_EXISTS_BY_NAME);
        errorMsg % name();
        string procName = "AddressSpace::AddressSpace";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          errorMsg.str());
    }
}


/**
 * Destructor.
 */
AddressSpace::~AddressSpace() {
    unsetMachine();
}


/**
 * Returns the bit width of the minimum addressable word.
 *
 * @return The bit width of the minimum addressable word.
 */
int
AddressSpace::width() const {
    return width_;
}


/**
 * Returns the lowest memory address of the address space.
 *
 * @return The lowest memory address of the address space.
 */
unsigned int
AddressSpace::start() const {
    return minAddress_;
}


/**
 * Returns the highest memory address of the address space.
 *
 * @return The highest memory address of the address space.
 */
unsigned int
AddressSpace::end() const {
    return maxAddress_;
}


/**
 * Sets the name of the address space.
 *
 * @param name Name of the address space.
 * @exception ComponentAlreadyExists If an address space with the given name
 *                                   is already in the same machine.
 * @exception InvalidName If the given name is not valid for a component.
 */
void
AddressSpace::setName(const string& name)
    throw (ComponentAlreadyExists, InvalidName) {

    if (name == this->name()) {
        return;
    }

    if (machine() != NULL) {
        if (machine()->addressSpaceNavigator().hasItem(name)) {
            string procName = "AddressSpace::setName";
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        } else {
            Component::setName(name);
        }
    } else {
        Component::setName(name);
    }
}


/**
 * Sets the bit width of the minimum addressable word.
 *
 * @param width The bit width of the minimum addressable word.
 * @exception OutOfRange If the given width is illegal (<=0).
 */
void
AddressSpace::setWidth(int width)
    throw (OutOfRange) {

    if (width <= 0) {
        string procName = "AddressSpace::setWidth";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    width_ = width;
}


/**
 * Sets the memory address bounds of the address space.
 *
 * @param start The lowest memory address.
 * @param end The highest memory address.
 * @exception OutOfRange If the given start and end addresses are illegal.
 */
void
AddressSpace::setAddressBounds(unsigned int start, unsigned int end)
    throw (OutOfRange) {

    if (start >= end) {
        string procName = "AddressSpace::setAddressBounds";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    minAddress_ = start;
    maxAddress_ = end;
}


/**
 * Registers the address space to a machine. Do not use this method.
 *
 * @param mach Machine to which the address space is going to be registered.
 * @exception ComponentAlreadyExists If there is another address space by the
 *                                   same name in the given machine.
 */
void
AddressSpace::setMachine(TTAMachine::Machine& mach)
    throw (ComponentAlreadyExists) {

    assert(!isRegistered());
    mach.addAddressSpace(*this);
    internalSetMachine(mach);
}


/**
 * Removes registration of the address space from its current machine. The
 * address space is deleted too because it cannot be unregistered.
 */
void
AddressSpace::unsetMachine() {

    assert(isRegistered());
    Machine* mach = machine();

    // remove dangling pointers in function units
    Machine::FunctionUnitNavigator fuNav = mach->functionUnitNavigator();
    int units = fuNav.count();
    for (int i = 0; i < units; i++) {
        FunctionUnit* fu = fuNav.item(i);
        if (fu->addressSpace() == this) {
            fu->setAddressSpace(NULL);
        }
    }

    // remove dangling pointer in GCU
    ControlUnit* cu = mach->controlUnit();
    if (cu != NULL) {
        if (cu->addressSpace() == this) {
            cu->setAddressSpace(NULL);
        }
    }

    internalUnsetMachine();
    mach->deleteAddressSpace(*this);
}


/**
 * Saves the contents to an ObjectState object.
 *
 * @return The newly created ObjectState object.
 */
ObjectState*
AddressSpace::saveState() const {

    ObjectState* as = new ObjectState(OSNAME_ADDRESS_SPACE);
    as->setAttribute(OSKEY_NAME, name());

    // set width
    as->setAttribute(OSKEY_WIDTH, width_);

    // set min address
    as->setAttribute(OSKEY_MIN_ADDRESS, minAddress_);

    // set max address
    as->setAttribute(OSKEY_MAX_ADDRESS, maxAddress_);

    return as;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid or if the machine
 *                                        already has an address space by
 *                                        the same name as the coming name
 *                                        of this address space.
 */
void
AddressSpace::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "AddressSpace::loadState";

    if (state->name() != OSNAME_ADDRESS_SPACE) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    Component::loadState(state);

    try {
        setWidth(state->intAttribute(OSKEY_WIDTH));
        unsigned int minAddress = state->unsignedIntAttribute(
            OSKEY_MIN_ADDRESS);
        unsigned int maxAddress = state->unsignedIntAttribute(
            OSKEY_MAX_ADDRESS);
        setAddressBounds(minAddress, maxAddress);
    } catch (Exception& e) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          e.errorMessage());
    }
}

}
