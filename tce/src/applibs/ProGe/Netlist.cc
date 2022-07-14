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
/**
 * @file Netlist.cc
 *
 * Implementation of the Netlist class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include <cstdlib>
#include <string>
#include <map>
#include <utility>

#include "Netlist.hh"
#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"
#include "NetlistBlock.hh"
#include "SignalTypes.hh"

#include "MapTools.hh"
#include "Application.hh"
#include "Conversion.hh"
#include "AssocTools.hh"

namespace ProGe {

/**
 * The constructor.
 */
Netlist::Netlist() : coreEntityName_("") {
}


/**
 * The destructor.
 */
Netlist::~Netlist() {
}

/**
 * Partially connects two netlist ports.
 *
 * Connects two given subsets of bits of two netlist ports. The bit subsets
 * have equal cardinality and represent an ordered range of adjacent bits.
 * The bits are connected in the same order (the first bit of the subset of
 * the first port is connected to the first bit of the subset of the second
 * port).
 *
 * @param port1 The first port to connect.
 * @param port2 The second port to connect.
 * @param port1FirstBit The first bit of the first port to connect.
 * @param port2FirstBit The first bit of the second port to connect.
 * @param width The width of the connection.
 * @param Return true, if connection was successful.
 */
bool
Netlist::connect(
    const NetlistPort& port1, const NetlistPort& port2, int port1FirstBit,
    int port2FirstBit, int width) {
    // PortConnectionProperty regards width 0 as fully connected
    if (width == 0 && (port1.dataType() != port2.dataType())) {
        // BIT to/from BIT_VECTOR connection needs partial connectivity
        width = 1;
    }

    size_t port1Descriptor = descriptor(port1);
    size_t port2Descriptor = descriptor(port2);

    // create first edge
    PortConnectionProperty property1(port1FirstBit, port2FirstBit, width);
    boost::add_edge(port1Descriptor, port2Descriptor, property1, *this);

    // create the opposite edge
    PortConnectionProperty property2(port2FirstBit, port1FirstBit, width);
    boost::add_edge(port2Descriptor, port1Descriptor, property2, *this);
    return true;
}

/**
 * Connects two netlist ports completely.
 *
 * Each bit of the first port is connected to a bit of the second port. The
 * bits are connected in the same order (the first bit of the first port is
 * connected to the first bit of the second port).
 *
 * @param port1 The first port to connect.
 * @param port2 The second port to connect.
 * @param Return true, if connection was successful.
 */
bool
Netlist::connect(const NetlistPort& port1, const NetlistPort& port2) {
    size_t port1Descriptor = descriptor(port1);
    size_t port2Descriptor = descriptor(port2);
    bool needsInversion = port1.assignedSignal().activeState() !=
                          port2.assignedSignal().activeState();
    PortConnectionProperty property(needsInversion);

    // create first edge
    boost::add_edge(port1Descriptor, port2Descriptor, property, *this);
    // create the opposite edge
    boost::add_edge(port2Descriptor, port1Descriptor, property, *this);
    return true;
}

/**
 * Removes connection between two ports.
 */
void
Netlist::disconnectPorts(const NetlistPort& port1, const NetlistPort& port2) {
    size_t port1Descriptor = descriptor(port1);
    size_t port2Descriptor = descriptor(port2);

    boost::remove_edge(port1Descriptor, port2Descriptor, *this);
    boost::remove_edge(port2Descriptor, port1Descriptor, *this);
}

/**
 * Trivially connect ports of the groups together by their signal types.
 *
 * @param Return true, if connection was successful.
 */
bool
Netlist::connect(
    const NetlistPortGroup& group1, const NetlistPortGroup& group2) {
    if (group1.portCount() != group2.portCount()) {
        return false;
    }

    std::map<SignalType, const NetlistPort*> portsOfGroup2;
    for (const NetlistPort* port : group2) {
        assert(
            !AssocTools::containsKey(
                portsOfGroup2, port->assignedSignal().type()) &&
            "Netlist::Connect(): Currently cannot handle groups with "
            "multiply of same signal type");
        portsOfGroup2.insert(
            std::make_pair(port->assignedSignal().type(), port));
    }

    for (const NetlistPort* from : group1) {
        SignalType matchingType = from->assignedSignal().type();
        assert(AssocTools::containsKey(portsOfGroup2, matchingType) && "");
        const NetlistPort* to = portsOfGroup2.at(matchingType);
        connect(*from, *to);
    }

    return true;
}

/**
 * Make single port connection by given signal type.
 *
 * Signal type is assumed to be unique within the both port groups.
 */
bool
Netlist::connectBy(
    SignalType byType, const NetlistPortGroup& group1,
    const NetlistPortGroup& group2) {
    assert(group1.hasPortBySignal(byType));
    assert(group2.hasPortBySignal(byType));

    const NetlistPort& port1 = group1.portBySignal(byType);
    const NetlistPort& port2 = group2.portBySignal(byType);

    connect(port1, port2);

    return true;
}

/**
 * Makes connection between two different netlist port group by connection map
 * using their SignalTypes.
 *
 * The connections are made from the first group to the second group by using
 * the connection map. todo...
 *
 * @param group1 The first port group.
 * @param group2 The second port group.
 * @param connectionMap The connection rules.
 *
 */
bool
Netlist::connect(
    const NetlistPortGroup& group1, const NetlistPortGroup& group2,
    std::map<SignalType, SignalType> connectionMap) {
    for (const NetlistPort* port1 : group1) {
        SignalType type1 = port1->assignedSignal().type();
        SignalType type2 = SignalType::UNDEFINED;
        if (connectionMap.count(type1)) {
            type2 = connectionMap.at(type1);
        } else {
            continue;
        }
        if (type2 == SignalType::OPEN) {
            continue;
        }
        const NetlistPort& port2 = group2.portBySignal(type2);

        connect(*port1, port2);
    }

    return true;
}

/**
 * Trivially connect ports of the groups together by their port names.
 * Useful for exporting e.g. bus connections to higher-level netlist block
 *
 * @param Return true, if connection was successful.
 */
bool
Netlist::connectGroupByName(
    const NetlistPortGroup& group1, const NetlistPortGroup& group2) {
    if (group1.portCount() != group2.portCount()) {
        return false;
    }

    std::map<std::string, const NetlistPort*> portsOfGroup2;
    for (const NetlistPort* port : group2) {
        assert(
            !AssocTools::containsKey(portsOfGroup2, port->name()) &&
            "Netlist::Connect(): group2 has multiple ports "
            "with the same name");
        portsOfGroup2.insert(std::make_pair(port->name(), port));
    }

    for (const NetlistPort* from : group1) {
        std::string matchingName = from->name();
        assert(
            AssocTools::containsKey(portsOfGroup2, matchingName) &&
            "Netlist::connectByName: The two port groups' names "
            "do not match");
        const NetlistPort* to = portsOfGroup2.at(matchingName);
        connect(*from, *to);
    }

    return true;
}

/**
 * Tells whether the netlist port is connected in this netlist instance
 *
 * @param port The netlist port
 * @return True if port is connected
 */
bool
Netlist::isPortConnected(const NetlistPort& port) const {
    
    size_t vertDesc = descriptor(port);
    boost::graph_traits<Netlist>::degree_size_type edges = 
        boost::out_degree(vertDesc, *this);
    return edges != 0;
}


/**
 * Tells whether the netlist is empty.
 *
 * @return True if the netlist is empty, otherwise false.
 */
bool
Netlist::isEmpty() const {
    return boost::num_vertices(*this) == 0;
}

/**
 * Returns true if there are some connections between ports.
 */
bool
Netlist::hasConnections() const {
    return boost::num_edges(*this) != 0;
}

/**
 * Registers given port to the netlist to make connection possible.
 *
 * This method does not need to be called by clients.
 *
 * @param port The port.
 * @return Descriptor to the port.
 */
size_t
Netlist::registerPort(NetlistPort& port) {
    assert(
        !MapTools::containsKey(vertexDescriptorMap_, &port) &&
        "The port is already registered");
    size_t descriptor = boost::add_vertex(&port, *this);
    vertexDescriptorMap_.insert(
        std::pair<const NetlistPort*, size_t>(&port, descriptor));
    return descriptor;
}

/**
 * Returns descriptor for the given port.
 *
 * @return Descriptor for the given port.
 */
size_t
Netlist::descriptor(const NetlistPort& port) const {
    assert(MapTools::containsKey(vertexDescriptorMap_, &port));
    return MapTools::valueForKey<size_t>(vertexDescriptorMap_, &port);
}

/**
 * Returns true if the netlist has the given port.
 */
bool
Netlist::isRegistered(const NetlistPort& port) const {
    return MapTools::containsKey(vertexDescriptorMap_, &port);
}

/**
 * Removes port from the netlist and all connections related to it.
 */
void
Netlist::unregisterPort(NetlistPort& port) {
    if (MapTools::containsKey(vertexDescriptorMap_, &port)) {
        boost::clear_vertex(this->descriptor(port), *this);
        boost::remove_vertex(this->descriptor(port), *this);
        vertexDescriptorMap_.erase(&port);
    }
}

/**
 * [DEPRECATED]
 * Adds a parameter for the netlist.
 *
 * If the netlist already has a parameter with the given name, it is replaced
 * by the new parameter.
 *
 * @param name Name of the parameter.
 * @param type Type of the parameter.
 * @param value Value of the parameter.
 */
void
Netlist::setParameter(
    const std::string& name,
    const std::string& type,
    const std::string& value) {

    if (hasParameter(name)) {
        removeParameter(name);
    }

    Parameter param = {name, type, value};
    parameters_.push_back(param);
}

void
Netlist::setParameter(const Parameter& param) {
    setParameter(param.name(), param.type(), param.value());
}

/**
 * Removes the given parameter from the netlist.
 *
 * @param name Name of the parameter.
 */
void
Netlist::removeParameter(const std::string& name) {
    for (ParameterTable::iterator iter = parameters_.begin();
         iter != parameters_.end(); iter++) {
        if (iter->name() == name) {
            parameters_.erase(iter);
            break;
        }
    }
}


/**
 * Tells whether the netlist has the given parameter.
 *
 * @param name Name of the parameter.
 */
bool
Netlist::hasParameter(const std::string& name) const {

    for (ParameterTable::const_iterator iter = parameters_.begin(); 
         iter != parameters_.end(); iter++) {
        if (iter->name() == name) {
            return true;
        }
    }

    return false;
}


/**
 * Returns the number of parameters in the netlist.
 *
 * @return The number of parameters.
 */
size_t
Netlist::parameterCount() const {
    return parameters_.size();
}

/**
 * Returns the parameter at the given position.
 *
 * @param index The position.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of parameters.
 */
Parameter
Netlist::parameter(size_t index) const {
    if (index >= parameterCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return parameters_[index];
}

Netlist::iterator
Netlist::begin() {
    return boost::edges(*this).first;
}

Netlist::iterator
Netlist::end() {
    return boost::edges(*this).second;
}

Netlist::const_iterator
Netlist::begin() const {
    return boost::edges(*this).first;
}

Netlist::const_iterator
Netlist::end() const {
    return boost::edges(*this).second;
}

Netlist::descriptor_iterator
Netlist::descriptorBegin() {
    return vertexDescriptorMap_.begin();
}

Netlist::descriptor_iterator
Netlist::descriptorEnd() {
    return vertexDescriptorMap_.end();
}

Netlist::const_descriptor_iterator
Netlist::descriptorBegin() const {
    return vertexDescriptorMap_.begin();
}

Netlist::const_descriptor_iterator
Netlist::descriptorEnd() const {
    return vertexDescriptorMap_.end();
}

void
Netlist::connectClocks(NetlistBlock& block) {
    const NetlistPort* topClock = nullptr;
    {
        std::vector<const NetlistPort*> clockOfBlock;
        clockOfBlock = block.portsBy(SignalType::CLOCK);
        assert(clockOfBlock.size() < 2);
        if (clockOfBlock.empty()) {
            return;
        } else {
            topClock = clockOfBlock.at(0);
        }
    }

    for (size_t i = 0; i < block.subBlockCount(); i++) {
        std::vector<const NetlistPort*> clockOfSubBlock;
        const BaseNetlistBlock& cblock = block;
        clockOfSubBlock = cblock.subBlock(i).portsBy(SignalType::CLOCK);
        assert(clockOfSubBlock.size() < 2);
        if (clockOfSubBlock.empty() ||
            block.netlist().isPortConnected(*clockOfSubBlock.at(0))) {
            continue;
        } else {
            block.netlist().connect(*topClock, *clockOfSubBlock.at(0));
        }
    }
}

void
Netlist::connectResets(NetlistBlock& block) {
    const NetlistPort* topClock = nullptr;
    {
        std::vector<const NetlistPort*> clockOfBlock;
        clockOfBlock = block.portsBy(SignalType::RESET);
        assert(clockOfBlock.size() < 2);
        if (clockOfBlock.empty()) {
            return;
        } else {
            topClock = clockOfBlock.at(0);
        }
    }

    for (size_t i = 0; i < block.subBlockCount(); i++) {
        std::vector<const NetlistPort*> clockOfSubBlock;
        const BaseNetlistBlock& cblock = block;
        clockOfSubBlock = cblock.subBlock(i).portsBy(SignalType::RESET);
        assert(clockOfSubBlock.size() < 2);
        if (clockOfSubBlock.empty() ||
            block.netlist().isPortConnected(*clockOfSubBlock.at(0))) {
            continue;
        } else {
            block.netlist().connect(*topClock, *clockOfSubBlock.at(0));
        }
    }
}

} /* namespace ProGe  */
