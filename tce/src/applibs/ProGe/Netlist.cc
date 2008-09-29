/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file Netlist.cc
 *
 * Implementation of the Netlist class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <cstdlib>

#include "Netlist.hh"
#include "NetlistPort.hh"
#include "NetlistBlock.hh"

#include "MapTools.hh"
#include "Application.hh"
#include "Conversion.hh"

namespace ProGe {

/**
 * The constructor.
 */
Netlist::Netlist() {
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
 */
void
Netlist::connectPorts(
    NetlistPort& port1,
    NetlistPort& port2,
    int port1FirstBit,
    int port2FirstBit,
    int width) {

    size_t port1Descriptor = descriptor(port1);
    size_t port2Descriptor = descriptor(port2);

    // create first edge
    PortConnectionProperty property1(
        port1FirstBit, port2FirstBit, width);
    boost::add_edge(port1Descriptor, port2Descriptor, property1, *this);

    // create the opposite edge
    PortConnectionProperty property2(
        port2FirstBit, port1FirstBit, width);
    boost::add_edge(port2Descriptor, port1Descriptor, property2, *this);
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
 */
void
Netlist::connectPorts(
    NetlistPort& port1,
    NetlistPort& port2) {

    size_t port1Descriptor = descriptor(port1);
    size_t port2Descriptor = descriptor(port2);
    PortConnectionProperty property;

    // create first edge
    boost::add_edge(port1Descriptor, port2Descriptor, property, *this);
    // create the opposite edge
    boost::add_edge(port2Descriptor, port1Descriptor, property, *this);
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
 * Returns the top-level block in the netlist.
 *
 * @return The top-level block.
 * @exception InstanceNotFound If the netlist is empty.
 */
NetlistBlock&
Netlist::topLevelBlock() const
    throw (InstanceNotFound) {

    if (isEmpty()) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }

    boost::graph_traits<Netlist>::vertex_iterator iter =
	boost::vertices(*this).first;
    size_t vertexDescriptor = *iter;
    NetlistPort* port = (*this)[vertexDescriptor];
    NetlistBlock* block = port->parentBlock();
    while (block->hasParentBlock()) {
        block = &block->parentBlock();
    }

    return *block;
}


/**
 * Maps the given descriptor for the given port.
 *
 * This method does not need to be called by clients.
 *
 * @param port The port.
 * @param descriptor The descriptor.
 */
void
Netlist::mapDescriptor(const NetlistPort& port, size_t descriptor) {
    assert(!MapTools::containsKey(vertexDescriptorMap_, &port));
    vertexDescriptorMap_.insert(
        std::pair<const NetlistPort*, size_t>(&port, descriptor));
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


/**
 * Removes the given parameter from the netlist.
 *
 * @param name Name of the parameter.
 */
void
Netlist::removeParameter(const std::string& name) {
    for (ParameterTable::iterator iter = parameters_.begin();
         iter != parameters_.end(); iter++) {
        if (iter->name == name) {
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
        if (iter->name == name) {
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
int
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
Netlist::Parameter
Netlist::parameter(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= parameterCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return parameters_[index];
}
}
