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
 * @file Bridge.cc
 *
 * Implementation of Bridge class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 10 Jun 2004 by vpj, am, tr, ll
 * @note rating: red
 */

#include <set>

#include "Bridge.hh"
#include "Machine.hh"
#include "Bus.hh"
#include "MachineTester.hh"
#include "MachineTestReporter.hh"
#include "MOMTextGenerator.hh"
#include "Application.hh"
#include "AssocTools.hh"
#include "ObjectState.hh"

using std::string;
using std::set;
using boost::format;

namespace TTAMachine {

// initialization of static data members
const string Bridge::OSNAME_BRIDGE = "bridge";
const string Bridge::OSKEY_SOURCE_BUS = "source";
const string Bridge::OSKEY_DESTINATION_BUS = "destination";


/**
 * Constructor.
 *
 * @param name Name of the bridge.
 * @param sourceBus Bus from which the bridge reads data.
 * @param destinationBus Bus to which the bridge writes data.
 * @exception IllegalRegistration If the given source and destination buses
 *                                are not registered to the same machine.
 * @exception ComponentAlreadyExists If there is another bridge by the same
 *                                   name in the machine which contains the
 *                                   source and destination buses.
 * @exception IllegalConnectivity If the bridge cannot be created because it
 *                                would violate a connectivity constraint.
 * @exception IllegalParameters If the given buses are the same instance.
 * @exception InvalidName If the given name is not a valid component name.
 */
Bridge::Bridge(const string& name, Bus& sourceBus, Bus& destinationBus)
    : Component(name), sourceBus_(NULL), destinationBus_(NULL) {
    const string procName = "Bridge::Bridge";

    if (&sourceBus == &destinationBus) {
        throw IllegalParameters(__FILE__, __LINE__, procName);
    }

    sourceBus.ensureRegistration(destinationBus);
    MachineTester& tester = sourceBus.machine()->machineTester();
    if (!tester.canBridge(sourceBus, destinationBus)) {
        string errorMsg = MachineTestReporter::bridgingError(
            sourceBus, destinationBus, tester);
        throw IllegalConnectivity(__FILE__, __LINE__, procName, errorMsg);
    }

    setMachine(*sourceBus.machine());
    setSourceAndDestination(sourceBus, destinationBus);
}

/**
 * Constructor.
 *
 * Creates a skeleton objects with name only. This constructor should be used
 * by Machine::loadState only. Do not use this constructor.
 *
 * @param state The ObjectState instance.
 * @param mach The machine to which the bridge belongs.
 * @exception ObjectStateLoadingException If the machine already has a bridge
 *                                        by the same name as the coming name
 *                                        of this bridge or if the
 *                                        ObjectState instance is invalid.
 */
Bridge::Bridge(const ObjectState* state, Machine& mach)
    : Component(state), sourceBus_(NULL), destinationBus_(NULL) {
    Machine::BridgeNavigator bridgeNav = mach.bridgeNavigator();

    if (!bridgeNav.hasItem(name())) {
        setMachine(mach);
    } else {
        MOMTextGenerator textGenerator;
        format errorMsg = textGenerator.text(MOMTextGenerator::
                                             TXT_BRIDGE_EXISTS_BY_SAME_NAME);
        errorMsg % name();
        string procName = "Bridge::Bridge";
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          errorMsg.str());
    }
}

/**
 * Destructor.
 */
Bridge::~Bridge() {
    unsetMachine();
}


/**
 * Sets the name of the bridge.
 *
 * @param name Name of the bridge.
 * @exception ComponentAlreadyExists If a bridge with the given name is
 *                                   already in the same machine.
 * @exception InvalidName If the given name is not a valid component name.
 */
void
Bridge::setName(const string& name) {
    if (name == this->name()) {
        return;
    }

    if (machine() != NULL) {
        if (machine()->bridgeNavigator().hasItem(name)) {
            string procName = "Bridge::setName";
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        } else {
            Component::setName(name);
        }
    } else {
        Component::setName(name);
    }
}

/**
 * Returns the previous bus from location point of view.
 *
 * @return The previous bus.
 */
Bus*
Bridge::previousBus() const {
    if (sourcePrevious_) {
        return sourceBus_;
    } else {
        return destinationBus_;
    }
}


/**
 * Returns the next bus from location point of view.
 *
 * @return The next bus.
 */
Bus*
Bridge::nextBus() const {
    if (sourcePrevious_) {
        return destinationBus_;
    } else {
        return sourceBus_;
    }
}


/**
 * Registers the bridge to a machine.
 *
 * @param mach Machine to which the bridge is going to be registered.
 * @exception ComponentAlreadyExists If the given machine already has another
 *                                   bridge by the same name.
 */
void
Bridge::setMachine(Machine& mach) {
    mach.addBridge(*this);
    internalSetMachine(mach);
}

/**
 * Removes registration of the bridge from its current machine.
 *
 * The bridge is deleted because it cannot be unregistered from the machine.
 */
void
Bridge::unsetMachine() {

    assert(machine() != NULL);

    Bus* oldSource = sourceBus_;
    Bus* oldDestination = destinationBus_;
    sourceBus_ = NULL;
    destinationBus_ = NULL;

    if (oldSource != NULL) {
        // may be NULL if loading the state from ObjectState has failed
        oldSource->clearDestinationBridge(*this);
    }

    if (oldDestination != NULL) {
        // may be NULL if loading the state from ObjectState has failed
        oldDestination->clearSourceBridge(*this);
    }

    Machine* mach = machine();
    internalUnsetMachine();
    mach->deleteBridge(*this);
}


/**
 * Saves the contents to an ObjectState object.
 *
 * @return The newly created ObjectState object.
 */
ObjectState*
Bridge::saveState() const {
    ObjectState* bridge = Component::saveState();
    bridge->setName(OSNAME_BRIDGE);
    string sourceBusName = sourceBus_->name();
    string destinationBusName = destinationBus_->name();
    bridge->setAttribute(OSKEY_SOURCE_BUS, sourceBusName);
    bridge->setAttribute(OSKEY_DESTINATION_BUS, destinationBusName);
    return bridge;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * If the bridge is already joining two buses, the function will not work and
 * ObjectStateLoadingException is thrown. This method works only if the
 * bridge was created by the constructor with ObjectState argument.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the machine already a bridge by
 *                                        the same name as the coming name
 *                                        of this bridge or if creating
 *                                        references fails or if the given
 *                                        ObjectState instance is invalid.
 */
void
Bridge::loadState(const ObjectState* state) {
    const string procName = "Bridge::loadState";

    if (state->name() != OSNAME_BRIDGE) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }
    if (sourceBus() != NULL || destinationBus() != NULL) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    Component::loadState(state);

    try {
        string sourceBusName = state->stringAttribute(OSKEY_SOURCE_BUS);
        string destinationBusName =
            state->stringAttribute(OSKEY_DESTINATION_BUS);

        assert(isRegistered());
        Machine::BusNavigator busNav = machine()->busNavigator();
        Bus* source;
        Bus* destination;
        try {
            source = busNav.item(sourceBusName);
            destination = busNav.item(destinationBusName);
        } catch (InstanceNotFound& e) {
            MOMTextGenerator textGenerator;
            format errorMsg = textGenerator.text(
                MOMTextGenerator::TXT_BRIDGE_UNKNOWN_SRC_OR_DST);
            errorMsg % name();
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                              errorMsg.str());
        }
        MachineTester& tester = machine()->machineTester();
        if (tester.canBridge(*source, *destination)) {
            setSourceAndDestination(*source, *destination);
        } else {
            string errorMsg = MachineTestReporter::bridgingError(
                *source, *destination, tester);
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                              errorMsg);
        }

    } catch(Exception& e) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          e.errorMessage());
    }
}

/**
 * Adjusts the direction of the bus chain (next/previous).
 *
 * When two bus chains are joined by a new bridge the direction of the
 * destination side chain may have to be changed to verify that the
 * direction of the new longer chain does not change in the middle of the
 * chain. That is, if the source bus of the new bridge is the last of its
 * chain and the destination bus of the new bridge is also the last, the
 * direction of the destination side chain has to be changed such that the
 * destination bus will become the first bus of its chain.
 *
 * @param sourceBus The new source side bus.
 * @param destinationBus The new destination side bus.
 */
void
Bridge::adjustChainDirection(
    const Bus& sourceBus,
    const Bus& destinationBus) {

    Machine* mach = sourceBus.machine();
    Machine::BridgeNavigator bridgeNav = mach->bridgeNavigator();

    // if another bridge joining the buses is found, see the direction
    // from that bridge
    for (int i = 0; i < bridgeNav.count(); i++) {
        Bridge* bridge = bridgeNav.item(i);
        if (bridge->sourceBus() == &destinationBus &&
            bridge->destinationBus() == &sourceBus) {
            if (bridge->previousBus() == &sourceBus) {
                sourcePrevious_ = true;
            } else {
                sourcePrevious_ = false;
            }
            return;
        }
    }

    // change the direction of the bus chain containing destinationBus if
    // needed
    if (!sourceBus.hasNextBus()) {
        sourcePrevious_ = true;
        setFirstOfChain(destinationBus);
    } else {
        sourcePrevious_ = false;
        // find the bus that should be the first bus of destination side
        // chain
        const Bus* otherEnd = &destinationBus;
        while (otherEnd->hasNextBus()) {
            otherEnd = otherEnd->nextBus();
        }
        if (otherEnd != &destinationBus) {
            setFirstOfChain(*otherEnd);
        }
    }
}


/**
 * Sets the source and destination buses of the bridge.
 *
 * The bridge must not have source and destination buses before calling this
 * method.
 *
 * @param sourceBus The source bus.
 * @param destinationBus The destination bus.
 */
void
Bridge::setSourceAndDestination(Bus& sourceBus, Bus& destinationBus) {
    assert(sourceBus_ == NULL && destinationBus_ == NULL);
    assert(!sourceBus.canWrite(destinationBus));
    assert(!destinationBus.canRead(sourceBus));
    adjustChainDirection(sourceBus, destinationBus);
    sourceBus.setDestinationBridge(*this);
    destinationBus.setSourceBridge(*this);
    sourceBus_ = &sourceBus;
    destinationBus_ = &destinationBus;
}


/**
 * Sets the given bus to the first bus of the bus chain.
 *
 * That is, changes the direction of the chain if the given bus is currently
 * the last bus of the chain. The given bus must be at the end of chain.
 *
 * @param bus The bus to be set to the first bus of the chain.
 */
void
Bridge::setFirstOfChain(const Bus& bus) {

    assert(!(bus.hasPreviousBus() && bus.hasNextBus()));

    if (bus.hasNextBus()) {
        // if the bus is already the first
        return;
    } else {
        const Bus* observable = &bus;

        // collect the buses of the chain to set
        set<const Bus*> busesInChain;
        busesInChain.insert(observable);
        while (observable->hasPreviousBus()) {
            observable = observable->previousBus();
            busesInChain.insert(observable);
        }

        Machine::BridgeNavigator bridgeNav =
            bus.machine()->bridgeNavigator();
        // change (next/previous) direction of each bridge that is joining
        // a bus in the set
        for (int i = 0; i < bridgeNav.count(); i++) {
            Bridge* bridge = bridgeNav.item(i);
            // Source bus or destination bus is NULL only if the bridge
            // was created from ObjectState. Then calling nextBus() is
            // illegal because sourcePrevious_ member is not initialized.
            if (bridge->sourceBus() != NULL &&
                bridge->destinationBus() != NULL &&
                AssocTools::containsKey(busesInChain, bridge->nextBus())) {
                bridge->sourcePrevious_ = !bridge->sourcePrevious_;
            }
        }
    }
}

}
