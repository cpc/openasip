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
 * @file Bus.cc
 *
 * Implementation of Bus class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 11 Jun 2004 by am, pj, jn, ll
 * @note rating: red
 */

#include <string>

#include "Bus.hh"
#include "Guard.hh"
#include "Segment.hh"
#include "Socket.hh"
#include "Bridge.hh"
#include "InstructionTemplate.hh"
#include "MOMTextGenerator.hh"
#include "Application.hh"
#include "ContainerTools.hh"
#include "ObjectState.hh"

using std::vector;
using std::string;
using boost::format;

namespace TTAMachine {

// declaration of constants used in ObjectStates
const string Bus::OSNAME_BUS = "bus";
const string Bus::OSKEY_WIDTH = "width";
const string Bus::OSKEY_IMMWIDTH = "immwidth";
const string Bus::OSKEY_EXTENSION = "extension";
const string Bus::OSVALUE_SIGN = "sign";
const string Bus::OSVALUE_ZERO = "zero";


/**
 * Constructor.
 *
 * @param name Name of the bus.
 * @param width Bit width of the bus.
 * @param immWidth Bit width of the inline immediate word.
 * @param extensionMode Extension mode applied to the inline immediate word
 *                      when it is narrower than the bus on which it is
 *                      transported.
 * @exception OutOfRange If the given width or immediate width is invalid.
 * @exception InvalidName If the given name is not a valid component name.
 */
Bus::Bus(
    const std::string& name, int width, int immWidth,
    Machine::Extension extensionMode)
    : Component(name),
      width_(width),
      immWidth_(immWidth),
      extensionMode_(extensionMode) {
    if (width <= 0 || immWidth < 0 || immWidth > width) {
        string procName = "Bus::Bus";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
}

/**
 * Constructor.
 *
 * Loads the state of the bus from the given ObjectState instance. Does not
 * load connections to sockets.
 *
 * @param state Root node of the ObjectState tree from which the state is
 *              loaded.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
Bus::Bus(const ObjectState* state) : Component(state) {
    try {
        loadStateWithoutReferences(state);
    } catch (Exception&) {
        deleteAllSegments();
        deleteAllGuards();
        throw;
    }
}

/**
 * Destructor.
 *
 * Before destruction detaches all sockets from the bus.
 */
Bus::~Bus() {
    unsetMachine();
    deleteAllSegments();
    deleteAllGuards();
}


/**
 * Returns the position of the bus in move slot order.
 *
 * @return The position (0 is the first position).
 * @exception NotAvailable If the bus is not registered to a machine.
 */
int
Bus::position() const {
    if (!isRegistered()) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }

    Machine::BusNavigator busNav = machine()->busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        if (busNav.item(i) == this) {
            return i;
        }
    }

    assert(false);
    return 0;
}

/**
 * Returns the bit width of the bus.
 *
 * @return Bit width of the bus.
 */
int
Bus::width() const {
    return width_;
}


/**
 * Returns the bit width of the inline immediate word.
 *
 * @return Bit width of the inline immediate word.
 */
int
Bus::immediateWidth() const {
    return immWidth_;
}


/**
 * Returns true if the bus uses sign extension.
 *
 * @return True if the bus uses sign extension.
 */
bool
Bus::signExtends() const {
    return extensionMode_ == Machine::SIGN;
}


/**
 * Returns true if the bus uses zero extension.
 *
 * @return True if the bus uses zero extension.
 */
bool
Bus::zeroExtends() const {
    return extensionMode_ == Machine::ZERO;
}


/**
 * Sets the name of the bus.
 *
 * @param name Name of the bus.
 * @exception ComponentAlreadyExists If a bus with the given name is
 *                                   already in the same machine.
 * @exception InvalidName If the given name is not a valid component name.
 */
void
Bus::setName(const std::string& name) {
    if (name == this->name()) {
        return;
    }

    if (machine() != NULL) {
        if (machine()->busNavigator().hasItem(name) || 
            machine()->immediateSlotNavigator().hasItem(name)) {
            string procName = "Bus::setName";
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        } else {
            Component::setName(name);
        }
    } else {
        Component::setName(name);
    }
}

/**
 * Sets the bit width of the bus.
 *
 * @param width The new bit width of the bus.
 * @exception OutOfRange If the given width is less or equal to zero or if
 *                       there is an instruction template that has greater
 *                       bit width for this slot.
 */
void
Bus::setWidth(int width) {
    const string procName = "Bus::setWidth";

    if (width <= 0) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    width_ = width;
}

/**
 * Sets the number of bits of inline immediates.
 *
 * @param width The bit width of inline immediates.
 * @exception OutOfRange If the given width is negative or
                         greater than the bit width of the bus.
 */
void
Bus::setImmediateWidth(int width) {
    if (width < 0 || width > this->width()) {
        string procName = "Bus::setImmediateWidth";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    immWidth_ = width;
}

/**
 * Sets the zero extension mode.
 */
void
Bus::setZeroExtends() {
    extensionMode_ = Machine::ZERO;
}


/**
 * Sets the sign extension mode.
 */
void
Bus::setSignExtends() {
    extensionMode_ = Machine::SIGN;
}


/**
 * Sets the extension mode.
 */
void
Bus::setExtensionMode(const Machine::Extension extension) {
    extensionMode_ = extension;
}


/**
 * Returns true if the bus has a segment with the given name.
 *
 * @param name Name of the segment.
 * @return True if the bus has a segment with the given name, otherwise
 *         false.
 */
bool
Bus::hasSegment(const std::string& name) const {
    SegmentTable::const_iterator iter = segments_.begin();
    while (iter != segments_.end()) {
        if ((*iter)->name() == name) {
            return true;
        }
        iter++;
    }
    return false;
}


/**
 * Returns true if the bus is connected to the given socket, otherwise false.
 *
 * @param socket Socket.
 * @return True if the bus is connected to the given socket, otherwise false.
 */
bool
Bus::isConnectedTo(const Socket& socket) const {

    SegmentTable::const_iterator iter = segments_.begin();
    while (iter != segments_.end()) {
        if ((*iter)->isConnectedTo(socket)) {
            return true;
        }
        iter++;
    }
    return false;
}


/**
 * Returns the bus segment at a given index.
 *
 * The index must be greater or equal to 0 and less than the number of
 * segments. The segments are returned in the correct order. Thus, index 0
 * returns the first segment in the segment chain, index 1 returns its
 * destination segment and so on.
 *
 * @param index Index.
 * @return The segment found on the given index.
 * @exception OutOfRange If the given index is out of range.
 */
Segment*
Bus::segment(int index) const {
    if (index < 0 || index >= segmentCount()) {
        string procName = "Bus::segment";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    // find the first segment of the chain
    Segment* firstSegment = NULL;
    SegmentTable::const_iterator iter = segments_.begin();
    while (iter != segments_.end()) {
        Segment* segment = *iter;
        if (!segment->hasSourceSegment()) {
            firstSegment = segment;
            break;
        }
        iter++;
    }

    // follow the segment chain until the requested segment is reached
    Segment* nextSegment = firstSegment;
    for (int i = 0; i < index; i++) {
        nextSegment = nextSegment->destinationSegment();
    }

    return nextSegment;
}

/**
 * Returns the segment which has the given name.
 *
 * @param name Name of the segment.
 * @return Segment which has the given name.
 * @exception InstanceNotFound If the bus does not contain a segment by the
 *                             given name.
 */
Segment*
Bus::segment(const std::string& name) const {
    SegmentTable::const_iterator iter = segments_.begin();
    while (iter != segments_.end()) {
        if ((*iter)->name() == name) {
            return *iter;
        }
        iter++;
    }

    // requested segment not found
    string procName = "Bus::segment";
    throw InstanceNotFound(__FILE__, __LINE__, procName);
}

/**
 * Returns the number of segments in this bus.
 *
 * @return Number of segments in this bus.
 */
int
Bus::segmentCount() const {
    return segments_.size();
}

/**
 * Checks whether the bus has the given guard.
 */
bool 
Bus::hasGuard(const Guard& guard) const {
    for (GuardTable::const_iterator iter = guards_.begin();
         iter != guards_.end(); iter++) {
        if ((*iter)->isEqual(guard)) {
            return true;
        }
    }
    return false;
}

/**
 * Adds guard to the bus.
 *
 * @param guard Pointer to the Guard object to be added.
 * @exception ComponentAlreadyExists If there is an equal guard already.
 */
void
Bus::addGuard(Guard& guard) {
    if (hasGuard(guard)) {
        string procName = "Bus::addGuard";
        throw ComponentAlreadyExists(__FILE__, __LINE__, __func__,
                                     "Bus already has the given guard!");
    }

    guards_.push_back(&guard);
}

/**
 * Removes the given guard from the bus.
 *
 * This method should be called by Guard destructor only.
 *
 * @param guard Guard to be removed.
 */
void
Bus::removeGuard(Guard& guard) {
    // run time check: can be called from Guard destructor only
    assert(guard.parentBus() == NULL);
    ContainerTools::removeValueIfExists(guards_, &guard);
}


/**
 * Returns the number of guards in the bus.
 *
 * @return The number of guards in the bus.
 */
int
Bus::guardCount() const {
    return guards_.size();
}


/**
 * Returns the guard by the given index.
 *
 * The index must be between 0 and the return value of numberOfGuards() - 1.
 *
 * @param index Index.
 * @return The guard by the given index.
 * @exception OutOfRange If the given index is out of range.
 */
Guard*
Bus::guard(int index) const {
    if (index < 0 || static_cast<size_t>(index) >= guards_.size()) {
        string procName = "Bus::guard";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    return guards_[index];
}

/**
 * Checks if the bus has previous bus.
 *
 * Previous does not mean source in that case but the previous bus from
 * location point of view.
 *
 * @return True if the bus has previous bus, otherwise false.
 */
bool
Bus::hasPreviousBus() const {
    Bridge* prevBridge = previousBridge();
    return prevBridge != NULL;
}


/**
 * Checks if the bus has next bus.
 *
 * Next does not mean destination bus in that case but the next bus from
 * location point of view.
 *
 * @return True if the bus has next bus, otherwise false.
 */
bool
Bus::hasNextBus() const {
    Bridge* nextBridge = this->nextBridge();
    return nextBridge != NULL;
}


/**
 * Returns the next bus from the location point of view.
 *
 * @return The next bus.
 * @exception InstanceNotFound If the bus is the last in the chain.
 */
Bus*
Bus::nextBus() const {
    Bridge* nextBridge = this->nextBridge();
    if (nextBridge != NULL) {
        return nextBridge->nextBus();
    } else {
        string procName = "Bus::nextBus";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }
}

/**
 * Returns the previous bus from the location point of view.
 *
 * @return The previous bus.
 * @exception InstanceNotFound If the bus is the first in the chain.
 */
Bus*
Bus::previousBus() const {
    Bridge* prevBridge = previousBridge();
    if (prevBridge != NULL) {
        return prevBridge->previousBus();
    } else {
        string procName = "Bus::previousBus";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }
}

/**
 * Checks if the bus can read from the given bus.
 *
 * That is, checks if the given bus is source bus of this bus.
 *
 * @param bus The bus.
 * @return True if this bus can read from the given bus.
 */
bool
Bus::canRead(const Bus& bus) const {
    BridgeTable::const_iterator iter = sourceBridges_.begin();
    while (iter != sourceBridges_.end()) {
        if ((*iter)->sourceBus() == &bus) {
            return true;
        }
        iter++;
    }
    return false;
}


/**
 * Checks if the bus can write to the given bus.
 *
 * That is, checks if the given bus is destination bus of this bus.
 *
 * @param bus The bus.
 * @return True if this bus can write to the given bus.
 */
bool
Bus::canWrite(const Bus& bus) const {
    BridgeTable::const_iterator iter = destinationBridges_.begin();
    while (iter != destinationBridges_.end()) {
        if ((*iter)->destinationBus() == &bus) {
            return true;
        }
        iter++;
    }
    return false;
}


/**
 * Check if the bus can both read from and write to the given bus.
 *
 * That is, checks if the given bus is joined by a bidirectional bridge to
 * this bus.
 *
 * @param bus The bus.
 * @return True if this bus can both read from and write to the given bus.
 */
bool
Bus::canReadWrite(const Bus& bus) const {
    return canWrite(bus) && canRead(bus);
}


/**
 * Adds a source bridge for the bus.
 *
 * This method can be called from methods of Bridge only. Do not call this
 * method.
 *
 * @param bridge The source bridge to add.
 */
void
Bus::setSourceBridge(Bridge& bridge) {

    // run time check: can be called from Bridge methods only
    assert(bridge.destinationBus() == NULL);

    assert(sourceBridges_.size() < 2);
    sourceBridges_.push_back(&bridge);
}


/**
 * Adds a destination bridge for the bus.
 *
 * This method can be called from methods of Bridge only. Do not call this
 * method.
 *
 * @param bridge The destination bridge to add.
 */
void
Bus::setDestinationBridge(Bridge& bridge) {

    // run time check: can be called from Bridge methods only
    assert(bridge.sourceBus() == NULL);

    assert(destinationBridges_.size() < 2);
    destinationBridges_.push_back(&bridge);
}


/**
 * Removes a source bridge from the bus.
 *
 * This method can be called from Bridge destructor only. Do not call this
 * method.
 *
 * @param bridge The source bridge to remove.
 */
void
Bus::clearSourceBridge(Bridge& bridge) {

    // run time check: can be called from Bridge destructor only
    assert(bridge.destinationBus() == NULL);

    bool removed = ContainerTools::removeValueIfExists(
        sourceBridges_, &bridge);
    assert(removed);
}


/**
 * Removes a destination bridge from the bus.
 *
 * This method can be called from Bridge destructor only. Do not call this
 * method.
 *
 * @param bridge The destination bridge to remove.
 */
void
Bus::clearDestinationBridge(Bridge& bridge) {

    // run time check: can be called from Bridge destructor only
    assert(bridge.sourceBus() == NULL);

    bool removed = ContainerTools::removeValueIfExists(
        destinationBridges_, &bridge);
    assert(removed);
}


/**
 * Registers the bus to a machine.
 *
 * @param mach Machine to which the bus is going to be registered.
 * @exception ComponentAlreadyExists If the given machine already has another
 *                                   bus by the same name.
 */
void
Bus::setMachine(Machine& mach) {
    internalSetMachine(mach);
    mach.addBus(*this);
}

/**
 * Removes registration of the bus from its current machine.
 *
 * Detaches all the sockets attached to the bus and deletes all the guards and
 * modifies instruction templates to not use this bus.
 */
void
Bus::unsetMachine() {

    Machine* mach = machine();

    if (mach == NULL) {
        return;
    }

    internalUnsetMachine();

    // detach all sockets
    SegmentTable::iterator iter = segments_.begin();
    while (iter != segments_.end()) {
        (*iter)->detachAllSockets();
        iter++;
    }

    // delete bridges
    Machine::BridgeNavigator bNavigator = mach->bridgeNavigator();
    for (int i = 0; i < bNavigator.count();) {
        if (bNavigator.item(i)->sourceBus() == this ||
            bNavigator.item(i)->destinationBus() == this) {
            // deleting bridge removes it automatically from the container,
            // no iterator increment needed
            delete bNavigator.item(i);
        } else {
            i++;
        }
    }

    // delete the template slot that uses the bus
    Machine::InstructionTemplateNavigator itNav =
        mach->instructionTemplateNavigator();
    for (int i = 0; i < itNav.count(); i++) {
        InstructionTemplate* it = itNav.item(i);
        it->removeSlot(name());
    }

    mach->removeBus(*this);
}



/**
 * Adds the given segment to this bus.
 *
 * This method should be called by Segment constructor only.
 *
 * @param segment Segment to be added.
 * @exception ComponentAlreadyExists If a segment with the same name as
 *                                   the given segment is already existing
 *                                   in this bus.
 */
void
Bus::addSegment(Segment& segment) {
    // run time check: can be called from Segment constructor only
    assert(segment.parentBus() == NULL);

    if (hasSegment(segment.name())) {
        string procname = "Bus::addSegment";
        throw ComponentAlreadyExists(__FILE__, __LINE__, procname);
    } else {
        segments_.push_back(&segment);
    }
}

/**
 * Removes the given segment from the bus.
 *
 * This method should be called by Segment destructor only.
 *
 * @param segment Segment to be removed.
 */
void
Bus::removeSegment(Segment& segment) {

    // run time check: can be called from Segment destructor only
    assert(segment.parentBus() == NULL);

    bool removed = ContainerTools::removeValueIfExists(segments_, &segment);
    assert(removed);
}


/**
 * Saves the contents to an ObjectState tree.
 *
 * @return Root of the created tree.
 */
ObjectState*
Bus::saveState() const {

    ObjectState* busState = Component::saveState();

    // change the name of the ObjectState
    busState->setName(OSNAME_BUS);

    // set attributes
    busState->setAttribute(OSKEY_WIDTH, width_);
    busState->setAttribute(OSKEY_IMMWIDTH, immWidth_);
    if (extensionMode_ == Machine::SIGN) {
        busState->setAttribute(OSKEY_EXTENSION, OSVALUE_SIGN);
    } else if (extensionMode_ == Machine::ZERO) {
        busState->setAttribute(OSKEY_EXTENSION, OSVALUE_ZERO);
    }

    // add segments
    for (int i = 0; i < segmentCount(); i++) {
        Segment* seg = segment(i);
        busState->addChild(seg->saveState());
    }

    // save guards
    for (int i = 0; i < guardCount(); i++) {
        Guard* currentGuard = guard(i);
        busState->addChild(currentGuard->saveState());
    }

    return busState;
}


/**
 * Loads the state of the object from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the name of this bus becomes the
 *                                        same as an existing bus in the
 *                                        same machine or if generating
 *                                        references to other components did
 *                                        not succeed.
 */
void
Bus::loadState(const ObjectState* state) {
    deleteAllSegments();
    deleteAllGuards();

    // load state without references to other machine parts
    try {
        loadStateWithoutReferences(state);

        // create guards and load states of segments
        for (int i = 0; i < state->childCount(); i++) {
            ObjectState* childState = state->child(i);
            if (childState->name() == PortGuard::OSNAME_PORT_GUARD) {
                // guard is registered automatically
                new PortGuard(childState, *this);
            } else if (childState->name() ==
                       RegisterGuard::OSNAME_REGISTER_GUARD) {
                // guard is registered automatically
                new RegisterGuard(childState, *this);
            } else if (childState->name() ==
                       UnconditionalGuard::OSNAME_UNCONDITIONAL_GUARD) {
                // guard is registered automatically.
                new UnconditionalGuard(childState, *this);
            } else if (childState->name() == Segment::OSNAME_SEGMENT) {
                string segmentName =
                    childState->stringAttribute(Segment::OSKEY_NAME);
                Segment* segment = this->segment(segmentName);
                segment->loadState(childState);
            }
        }
    } catch (Exception& exception) {
        const string procName = "Bus::loadState";
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}

/**
 * Loads the state of the object from the given ObjectState tree without
 * references to other components/subcomponents.
 *
 * @param state Root node of the ObjectState tree.
 * @exception ObjectStateLoadingException If a bus by the same name is
 *                                        already registered to the same
 *                                        machine or if the given ObjectState
 *                                        tree is invalid.
 */
void
Bus::loadStateWithoutReferences(const ObjectState* state) {
    const string procName = "Bus::loadStateWithoutReferences";

    if (state->name() != OSNAME_BUS) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    Component::loadState(state);

    // read attributes
    try {
        setWidth(state->intAttribute(OSKEY_WIDTH));
        setImmediateWidth(state->intAttribute(OSKEY_IMMWIDTH));
        string extensionMode = state->stringAttribute(OSKEY_EXTENSION);
        if (extensionMode == OSVALUE_SIGN) {
            setSignExtends();
        } else if (extensionMode == OSVALUE_ZERO) {
            setZeroExtends();
        } else {
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
        }
    } catch (Exception& e) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName,
                                          e.errorMessage());
    }

    // create all segments, guards are not created because they are
    // completely useless without references
    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* child = state->child(i);
        if (child->name() == Segment::OSNAME_SEGMENT) {
            try {
                // segment is attached automatically
                new Segment(child, *this);
            } catch (const ComponentAlreadyExists&) {
                MOMTextGenerator textGen;
                format errorMsg = textGen.text(
                    MOMTextGenerator::TXT_SEGMENT_WITH_SAME_NAME);
                errorMsg % child->stringAttribute(Segment::OSKEY_NAME) %
                    name();
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName, errorMsg.str());
            }
        }
    }

    adjustSegmentChain(state);
}

/**
 * Deletes all the segments.
 */
void
Bus::deleteAllSegments() {
    // segment destructor removes the pointer from the table
    while (segments_.size() > 0) {
        delete segments_[0];
    }
}


/**
 * Deletes all the guards.
 */
void
Bus::deleteAllGuards() {
    // guard destructor removes the pointer from the table
    while (guards_.size() > 0) {
        delete guards_[0];
    }
}


/**
 * Returns a bridge that joins this bus and the previous bus of this bus.
 *
 * If there is two such bridges, returns one of them. If there is no such a
 * bridge, returns NULL.
 *
 * @return Previous bridge or NULL.
 */
Bridge*
Bus::previousBridge() const {
    Bridge* prevBridge = previousBridge(sourceBridges_);
    if (prevBridge == NULL) {
        prevBridge = previousBridge(destinationBridges_);
    }
    return prevBridge;
}


/**
 * Returns a bridge that joins this bus and the next bus of this bus.
 *
 * If there is two such bridges, returns one of them. If there is no such a
 * bridge, returns NULL.
 *
 * @return Next bridge or NULL.
 */
Bridge*
Bus::nextBridge() const {
    Bridge* nextBridge = this->nextBridge(sourceBridges_);
    if (nextBridge == NULL) {
        nextBridge = this->nextBridge(destinationBridges_);
    }
    return nextBridge;
}


/**
 * Returns a bridge that joins this bus and the previous bus of this bus.
 *
 * Returns NULL if such a bridge does not exist.
 *
 * @param bridge The bridge table to look for the bridge.
 * @return The previous bridge.
 */
Bridge*
Bus::previousBridge(const BridgeTable& bridges) const {
    BridgeTable::const_iterator iter = bridges.begin();
    while (iter != bridges.end()) {
        Bridge* bridge = *iter;
        if (bridge->nextBus() == this) {
            return bridge;
        }
        iter++;
    }
    return NULL;
}


/**
 * Returns a bridge that joins this bus and the next bus of this bus.
 *
 * Returns NULL if such a bridge does not exist.
 *
 * @param bridge The bridge table to look for the bridge.
 * @return The next bridge.
 */
Bridge*
Bus::nextBridge(const BridgeTable& bridges) const {
    BridgeTable::const_iterator iter = bridges.begin();
    while (iter != bridges.end()) {
        Bridge* bridge = *iter;
        if (bridge->previousBus() == this) {
            return bridge;
        }
        iter++;
    }
    return NULL;
}


/**
 * Adjusts the order of segments according to the order represented by the
 * given ObjectState tree.
 *
 * @param busState ObjectState tree representing the state of this bus.
 * @exception ObjectStateLoadingException If the segment chain is erroneus in
 *                                        the given ObjectState tree.
 */
void
Bus::adjustSegmentChain(const ObjectState* busState) {
    const string procName = "Bus::adjustSegmentChain";

    if (segmentCount() == 0) {
        assert(!busState->hasChild(Segment::OSNAME_SEGMENT));
        return;
    }

    MOMTextGenerator textGenerator;

    try {
        // find the last segment, the segment that has no destination
        Segment* lastSegment = NULL;
        for (int i = 0; i < busState->childCount(); i++) {
            ObjectState* childState = busState->child(i);
            if (childState->name() == Segment::OSNAME_SEGMENT &&
                !childState->hasAttribute(Segment::OSKEY_DESTINATION)) {
                string lastSegmentName =
                    childState->stringAttribute(Segment::OSKEY_NAME);
                lastSegment = segment(lastSegmentName);
                break;
            }
        }

        // if last segment was not found, throw
        if (lastSegment == NULL) {
            format errorMsg = textGenerator.text(
                MOMTextGenerator::TXT_NO_LAST_SEGMENT);
            errorMsg % name();
            throw ObjectStateLoadingException(
                __FILE__, __LINE__, procName, errorMsg.str());
        }

        Segment* destSegment = lastSegment;
        int modifications = 0;

        // in the following loop, find a segment whose destination should be
        // lastSegment and set it in front of that segment
        while (modifications < segmentCount() - 1) {
            bool chainModified = false;
            for (int i = 0; i < busState->childCount(); i++) {
                ObjectState* childState = busState->child(i);
                if (childState->name() == Segment::OSNAME_SEGMENT &&
                    childState->hasAttribute(Segment::OSKEY_DESTINATION)) {
                    string destName = childState->stringAttribute(
                        Segment::OSKEY_DESTINATION);
                    if (destName == destSegment->name()) {
                        string thisName = childState->
                            stringAttribute(Segment::OSKEY_NAME);
                        Segment* thisSegment = segment(thisName);
                        thisSegment->moveBefore(*destSegment);
                        destSegment = thisSegment;
                        modifications++;
                        chainModified = true;
                    }
                }
            }

            // if the source segment was not found and the segment chain is not
            // set completely yet, throw
            if (!chainModified && modifications < segmentCount() - 1) {
                format errorMsg = textGenerator.text(
                    MOMTextGenerator::TXT_NO_SOURCE_SEGMENT);
                errorMsg % name() % destSegment->name();
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName, errorMsg.str());
            }
        }
    } catch (Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}

/**
 * Checks is this bus architecture equal with the given bus.
 *
 * Architecture equality means that buses have same widths, immediate widths,
 * extension modes, number of segments and same guards. Bridges are not checked.
 *
 * @param bus Bus to compare with.
 * @return True if the buses are architecture equal.
 */
bool
Bus::isArchitectureEqual(const Bus& bus) const {

    if (width() != bus.width() ||
        immediateWidth() != bus.immediateWidth() ||
        signExtends() != bus.signExtends() ||
        segmentCount() != bus.segmentCount()) {
        return false;
    }
    for (int i = 0; i < guardCount(); i++) {
        bool match = false;

        // check if there is matching guard in another bus
        for (int j = 0; j < bus.guardCount(); j++) {
            if (guard(i)->isEqual(*bus.guard(j))) {
                match = true;
                break;
            }
        }

        // if no matching guard was not found buses are not equal
        if (match == false) {
            return false;
        }
        // otherwise continue to next guard
    }
    return true;
}

/**
 * Copies the bus.
 *
 * @return Copy of the Bus.
 */
Bus*
Bus::copy() const {
    ObjectState* newBusState = saveState();
    Bus* newBus = new Bus(newBusState);

    delete newBusState;
    newBusState = NULL;
    return newBus;
}

void
Bus::copyGuardsTo(Bus& other) const {
    for (int i = 0; i < guardCount(); ++i) {
        guard(i)->copyTo(other);
    }        
}

}
