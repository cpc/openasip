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
 * @file Unit.cc
 *
 * Implementation of Unit class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 * @note reviewed 22 Jun 2004 by ao, ml, vpj, ll
 */

#include "Unit.hh"
#include "Machine.hh"
#include "Port.hh"
#include "FUPort.hh"
#include "RFPort.hh"
#include "SpecialRegisterPort.hh"
#include "FunctionUnit.hh"
#include "Application.hh"
#include "ContainerTools.hh"
#include "AssocTools.hh"
#include "ObjectState.hh"

using std::string;

namespace TTAMachine {

// initialization of static data members
const string Unit::OSNAME_UNIT = "unit";

/**
 * Constructor.
 *
 * @param name The name of the unit.
 * @exception InvalidName If the given name is not a valid component name.
 */
Unit::Unit(const std::string& name)
    throw (InvalidName) :
    Component(name) {
}


/**
 * Constructor.
 *
 * Loads the state of the unit from the given ObjectState instance. Does not
 * load connections to other components.
 *
 * @param state The ObjectState instance from which the name is taken.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
Unit::Unit(const ObjectState* state)
    throw (ObjectStateLoadingException) :
    Component(state) {

    try {
        loadStateWithoutReferences(state);
    } catch (const Exception&) {
        // delete the ports that were loaded
        deleteAllPorts();
        throw;
    }
}


/**
 * Destructor.
 */
Unit::~Unit() {
    deleteAllPorts();
}


/**
 * Returns true if the requested port is found, otherwise false.
 *
 * @param name Name of the port.
 * @return Tru if the port is found, otherwise false.
 */
bool
Unit::hasPort(const std::string& name) const {
    PortTable::const_iterator iter = ports_.begin();
    while (iter != ports_.end()) {
        if ((*iter)->name() == name) {
            return true;
        }
        iter++;
    }
    return false;
}


/**
 * Returns the requested port.
 *
 * @param name Name of the port.
 * @return The requested port.
 * @exception InstanceNotFound If a port is not found by the given name.
 */
Port*
Unit::port(const std::string& name) const
    throw (InstanceNotFound) {

    PortTable::const_iterator iter = ports_.begin();
    while (iter != ports_.end()) {
        if ((*iter)->name() == name) {
            return *iter;
        }
        iter++;
    }

    string procName = "Unit::port";
    throw InstanceNotFound(__FILE__, __LINE__, procName);
}


/**
 * Returns the number of ports in the unit.
 *
 * @return The number of ports in the unit.
 */
int
Unit::portCount() const {
    return ports_.size();
}


/**
 * Returns a port by the given index.
 *
 * The index must be greater or equal to 0 and smaller than the number of
 * ports in the unit.
 *
 * @param index Index.
 * @return The port by the given index.
 * @exception OutOfRange If the given index is out of range.
 */
Port*
Unit::port(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= portCount()) {
        string procName = "Unit::port";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    return ports_[index];
}


/**
 * Adds a port to the unit.
 *
 * This method can be called from Port constructor only.
 *
 * @param port Port to be added.
 * @exception ComponentAlreadyExists If another port with the same name
 *                                   exists.
 */
void
Unit::addPort(Port& port)
    throw (ComponentAlreadyExists) {

    // check that this method is called from Port constructor only
    assert(port.parentUnit() == NULL);

    // check that a port with same name does not exist
    if (!hasPort(port.name())) {
        ports_.push_back(&port);
        return;
    }

    string procName = "Unit::addPort";
    throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
}


/**
 * Removes the given port.
 *
 * This method should only be called by Port destructor.
 *
 * @param port Port to be removed.
 */
void
Unit::removePort(Port& port) {

    // sanity check to verify that this is called from Port's destructor
    // only
    assert(port.parentUnit() == NULL);
    bool removed = ContainerTools::removeValueIfExists(ports_, &port);
    assert(removed);
}


/**
 * Registers the unit to a machine.
 *
 * @param mach Machine to which the unit is to be registered.
 * @exception ComponentAlreadyExists If there is another unit by the same
 *                                   name and type in the machine.
 */
void
Unit::setMachine(Machine& mach)
    throw (ComponentAlreadyExists) {

    internalSetMachine(mach);
    mach.addUnit(*this);
}


/**
 * Removes registration of the unit from its current machine.
 */
void
Unit::unsetMachine() {

    if (machine() == NULL) {
        return;
    }

    internalUnsetMachine();

    // detach all ports from sockets
    int ports = portCount();
    for (int i = 0; i < ports; i++) {
        Port* unitPort = port(i);
        unitPort->detachAllSockets();
    }
}


/**
 * Saves the state of the object to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
Unit::saveState() const {

    ObjectState* state = Component::saveState();
    state->setName(OSNAME_UNIT);

    // add ports
    for (int i = 0; i < portCount(); i++) {
        Port* port = this->port(i);
        state->addChild(port->saveState());
    }

    return state;
}


/**
 * Loads the state of the unit from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid or if references to
 *                                        sockets cannot be made.
 */
void
Unit::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    loadStateWithoutReferences(state);

    // create port-socket connections
    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* child = state->child(i);
        if (child->name() == FUPort::OSNAME_FUPORT ||
            child->name() == RFPort::OSNAME_RFPORT ||
            child->name() == SpecialRegisterPort::OSNAME_SPECIAL_REG_PORT) {
            string portName = child->stringAttribute(Port::OSKEY_NAME);
            Port* port = this->port(portName);
            port->loadState(child);
        }
    }
}


/**
 * Loads its state from the given ObjectState instance without references to
 * other components.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
Unit::loadStateWithoutReferences(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "Unit::loadStateWithoutReferences";

    // load ports
    try {
        // cannot delete all the ports because there might be guards
        // referencing to them
        NameSet newPortNames = portNames(state);
        deleteOtherPorts(newPortNames);

        for (int i = 0; i < state->childCount(); i++) {
            ObjectState* child = state->child(i);
            string portName = child->stringAttribute(Port::OSKEY_NAME);

            if (!hasPort(portName)) {
                if (child->name() == RFPort::OSNAME_RFPORT) {
                    // port is attached automatically
                    new RFPort(child, *this);
                } else if (child->name() == FUPort::OSNAME_FUPORT) {
                    // port is attached automatically
                    new FUPort(child, *this);
                } else if (child->name() ==
                           SpecialRegisterPort::OSNAME_SPECIAL_REG_PORT) {
                    // port is attached automatically
                    new SpecialRegisterPort(child, *this);
                }
            }
        }

    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * Deletes all the ports from the unit.
 */
void
Unit::deleteAllPorts() {
    while (ports_.size() > 0) {
        // the size of the vector is decreased when the code of Port
        // destructor is executed
        delete ports_[0];
    }
}


/**
 * Deletes all the ports which has a name that does not appear in the given
 * name set.
 *
 * @param portsToLeave A set of names of ports to leave.
 */
void
Unit::deleteOtherPorts(const NameSet& portsToLeave) {
    for (int i = 0; i < portCount();) {
        Port* port = this->port(i);
        if (!AssocTools::containsKey(portsToLeave, port->name())) {
            delete port;
        } else {
            i++;
        }
    }
}


/**
 * Creates a set of port names that exists in the given ObjectState tree.
 *
 * @param state An ObjectState instance representing an unit.
 * @return Set of port names.
 * @exception KeyNotFound If the given ObjectState instance is invalid.
 */
Unit::NameSet
Unit::portNames(const ObjectState* state)
    throw (KeyNotFound) {

    NameSet names;
    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* child = state->child(i);
        names.insert(child->stringAttribute(Port::OSKEY_NAME));
    }

    return names;
}

}
