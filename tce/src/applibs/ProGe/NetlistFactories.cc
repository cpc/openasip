/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file NetlistFactories.cc
 *
 * Implementation of NetlistFactories class.
 *
 * Created on: 25.5.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "NetlistFactories.hh"

#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"
#include "ProGeTypes.hh"

#include "AssocTools.hh"

namespace ProGe {

PortFactory* PortFactory::instance_ = NULL;
const TTAMachine::Machine* PortFactory::staticMachine_ = NULL;
const IDF::MachineImplementation* PortFactory::staticImplementation_ = NULL;

PortFactory::PortFactory()
//    : machine_(NULL),
//      implementation_(NULL)
{}

PortFactory::PortFactory(
    const TTAMachine::Machine& /*machine*/,
    const IDF::MachineImplementation& /*impl*/)
//    : machine_(&machine),
//      implementation_(&impl)
{}

PortFactory::~PortFactory() {
    PortPrototypeContainer::iterator port_it = portPrototypes_.begin();
    while (port_it != portPrototypes_.end()) {
        delete port_it->second;
        port_it->second = NULL;
        port_it++;
    }
    PortGroupPrototypeContainer::iterator portGroup_it =
        portGroupPrototypes_.begin();
    while (portGroup_it != portGroupPrototypes_.end()) {
        delete portGroup_it->second;
        portGroup_it->second = NULL;
        portGroup_it++;
    }
}

void
PortFactory::registerPort(SignalType type, const NetlistPort* port) {
    assert(!AssocTools::containsKey(portPrototypes_, type));
    portPrototypes_.insert(std::make_pair(type, port));
}

void
PortFactory::registerPort(const NetlistPort* port) {
    assert(port->assignedSignal().type() != SignalType::UNDEFINED);
    registerPort(port->assignedSignal().type(), port);
}

void
PortFactory::registerPorts() {
    registerPort(new InBitPort("clk", Signal(SignalType::CLOCK)));
    registerPort(
        new InBitPort("rstx", Signal(SignalType::RESET, ActiveState::LOW)));
}

void
PortFactory::registerPortGroup(
    SignalGroupType type, const NetlistPortGroup* portGroup) {
    assert(!AssocTools::containsKey(portGroupPrototypes_, type));
    portGroupPrototypes_.insert(std::make_pair(type, portGroup));
}

void
PortFactory::registerPortGroup(const NetlistPortGroup* portGroup) {
    assert(
        portGroup->assignedSignalGroup().type() !=
        SignalGroupType::UNDEFINED);

    registerPortGroup(portGroup->assignedSignalGroup().type(), portGroup);
}

void
PortFactory::registerPortGroups() {
    registerPortGroup(new NetlistPortGroup(
        SignalGroup(SignalGroupType::INSTRUCTION_LINE),
        new OutBitPort(
            "imem_en_x", Signal(SignalType::READ_REQUEST, ActiveState::LOW)),
        new OutPort(
            "imem_addr", "IMEMADDRWIDTH", BIT_VECTOR,
            Signal(SignalType::ADDRESS)),
        new InPort(
            "imem_data", "IMEMWIDTHINMAUS*IMEMMAUWIDTH", BIT_VECTOR,
            Signal(SignalType::FETCHBLOCK)),
        new InBitPort("busy", Signal(SignalType::STALL))));
}

/**
 * Creates new NetlistPort for the given signal type.
 *
 * @param type The signal type.
 * @return Pointer to newly created NetlistPort. Returns nullptr if there is
 *         no prototype for the given signal type.
 */
NetlistPort*
PortFactory::createPort(SignalType type, Direction direction) const {
    if (portPrototypes_.count(type)) {
        NetlistPort* found = portPrototypes_.find(type)->second->clone();
        if (found->direction() != direction) {
            return NetlistTools::mirror(found);
        } else {
            return found;
        }
    }
    return NULL;
}

NetlistPort*
PortFactory::create(SignalType type, Direction direction) {
    return instance()->createPort(type, direction);
}

NetlistPortGroup*
PortFactory::createPortGroup(SignalGroupType type) const {
    if (portGroupPrototypes_.count(type)) {
        return portGroupPrototypes_.find(type)->second->clone();
    }
    return NULL;
}

NetlistPortGroup*
PortFactory::create(SignalGroupType type) {
    return instance()->createPortGroup(type);
}

/**
 * Initializes factory context for the singleton factory. This function may
 * be called only once.
 */
void
PortFactory::initializeContext(
    const TTAMachine::Machine& machine,
    const IDF::MachineImplementation& impl) {
    assert(
        staticMachine_ == NULL && staticImplementation_ == NULL &&
        "Attempted to initialize twice.");
    staticMachine_ = &machine;
    staticImplementation_ = &impl;
}

/**
 * Global access to the factory as singleton. initializeContext() must be
 * called once before this.
 */
PortFactory*
PortFactory::instance() {
    if (instance_) {
        return instance_;
    } else {
        assert(staticMachine_ != NULL && staticImplementation_ != NULL);
        return (
            instance_ =
                new PortFactory(*staticMachine_, *staticImplementation_));
    }
}

/**
 * Creates default clock port.
 *
 */
NetlistPort*
PortFactory::clockPort(Direction direction) {
    static const InBitPort clkPortPrototype("clk", Signal(SignalType::CLOCK));
    return clkPortPrototype.clone(direction != IN);
}

/**
 * Creates default active-low reset port.
 */
NetlistPort*
PortFactory::resetPort(Direction direction) {
    static const InBitPort rstxPortPrototype(
        "rstx", Signal(SignalType::RESET, ActiveState::LOW));
    return rstxPortPrototype.clone(direction != IN);
}

} /* namespace ProGe */
