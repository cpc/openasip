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
 * @file NetlistBlock.cc
 *
 * Implementation of NetlistBlock class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "Netlist.hh"

#include "SequenceTools.hh"
#include "ContainerTools.hh"
#include "MapTools.hh"
#include "Application.hh"

using std::string;

namespace ProGe {

/**
 * Constructor. Creates a netlist block with no ports.
 *
 * The created block is empty. Only its name, the parent netlist and the
 * name of the instance module are defined.
 *
 * @param moduleName Name of the module.
 * @param instanceName Name of the instance of the module.
 * @param netlist The netlist which the block belongs to.
 */
NetlistBlock::NetlistBlock(
    const std::string& moduleName,
    const std::string& instanceName,
    Netlist& netlist) :
    moduleName_(moduleName), instanceName_(instanceName),
    parentBlock_(NULL), netlist_(netlist) {
}


/**
 * The destructor.
 *
 * Deletes all the ports.
 */
NetlistBlock::~NetlistBlock() {
    SequenceTools::deleteAllItems(ports_);
}


/**
 * Returns the name of the block instance.
 *
 * @return The name of the block instance.
 */
std::string
NetlistBlock::instanceName() const {
    return instanceName_;
}


/**
 * Returns the name of the module.
 *
 * @return The name of the module.
 */
std::string
NetlistBlock::moduleName() const {
    return moduleName_;
}


/**
 * Sets the given parameter for the block.
 *
 * @param name Name of the parameter.
 * @param type Type of the parameter.
 * @param value Value of the parameter.
 */
void
NetlistBlock::setParameter(
    const std::string& name,
    const std::string& type,
    const std::string& value) {

    // remove the old parameter
    for (ParameterTable::iterator iter = parameters_.begin();
         iter != parameters_.end(); iter++) {
        if (iter->name == name) {
            parameters_.erase(iter);
            break;
        }
    }

    Netlist::Parameter toAdd = {name, type, value};
    parameters_.push_back(toAdd);
}


/**
 * Tells whether the given parameter is defined for the block.
 *
 * @param name Name of the parameter.
 * @return True if the parameter is defined, otherwise false.
 */
bool
NetlistBlock::hasParameter(const std::string& name) const {
    for (ParameterTable::const_iterator iter = parameters_.begin();
         iter != parameters_.end(); iter++) {
        if ((*iter).name == name) {
            return true;
        }
    }

    return false;
}


/**
 * Returns the parameter that has the given name.
 *
 * @param name Name of the parameter.
 * @return The parameter.
 * @exception NotAvailable If the block does not have the given parameter.
 */
Netlist::Parameter
NetlistBlock::parameter(const std::string& name) const
    throw (NotAvailable) {

    for (ParameterTable::const_iterator iter = parameters_.begin();
         iter != parameters_.end(); iter++) {
        if (iter->name == name) {
            return *iter;
        }
    }

    throw NotAvailable(__FILE__, __LINE__, __func__);
}


/**
 * Returns the number of parameters.
 *
 * @return The number of parameters.
 */
int
NetlistBlock::parameterCount() const {
    return parameters_.size();
}


/**
 * Returns a parameter by the given index.
 *
 * @param index The index.
 * @return The parameter at the given index.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of parameters.
 */
Netlist::Parameter
NetlistBlock::parameter(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= parameterCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return parameters_[index];
}


/**
 * Adds the given port to the block.
 *
 * @param port The port to add.
 */
void
NetlistBlock::addPort(NetlistPort* port) {
    assert(port != NULL);
    assert(port->parentBlock() == NULL);
    ports_.push_back(port);
    size_t descriptor = boost::add_vertex(port, netlist_);
    netlist_.mapDescriptor(*port, descriptor);
}


/**
 * Removes the given port from the block and from the netlist graph.
 *
 * This method is to be called from the destructor of NetlistPort only!
 *
 * @param port The port to remove.
 */
void
NetlistBlock::removePort(NetlistPort& port) {
    assert(port.parentBlock() == NULL);
    boost::clear_vertex(netlist_.descriptor(port), netlist_);
    boost::remove_vertex(netlist_.descriptor(port), netlist_);
    assert(ContainerTools::removeValueIfExists(ports_, &port));
}


/**
 * Returns the number of ports in the block.
 *
 * @return The number of ports.
 */
int
NetlistBlock::portCount() const {
    return ports_.size();
}


/**
 * Returns a port by the given index.
 *
 * @param index The index.
 * @exception OutOfRange If the index is negative or not smaller than the
 *                       number of ports.
 */
NetlistPort&
NetlistBlock::port(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= portCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return *ports_[index];
}


/**
 * Adds the given block as sub block of this block.
 *
 * @param block The block.
 * @exception IllegalRegistration If the block is already a sub block or
 *                                if it belongs to another netlist.
 */
void
NetlistBlock::addSubBlock(NetlistBlock* block)
    throw (IllegalRegistration) {

    if (block->hasParentBlock() || &block->netlist() != &netlist()) {
        const string procName = "NetlistBlock::addSubBlock";
        throw IllegalRegistration(__FILE__, __LINE__, procName);
    }

    childBlocks_.push_back(block);
}


/**
 * Returns the number of sub blocks.
 *
 * @return The number of sub blocks.
 */
int
NetlistBlock::subBlockCount() const {
    return childBlocks_.size();
}


/**
 * Returns a sub block by the given index.
 *
 * @param index The index.
 * @return The sub block.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of sub blocks.
 */
NetlistBlock&
NetlistBlock::subBlock(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= subBlockCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return *childBlocks_[index];
}


/**
 * Tells whether the given block is an immediate sub block of this block.
 *
 * @param block The block.
 * @return True if the given block is an immediate sub block, otherwise
 *         false.
 */
bool
NetlistBlock::isSubBlock(const NetlistBlock& block) const {
    return ContainerTools::containsValue(childBlocks_, &block);
}


/**
 * Tells whether the block has a parent block.
 *
 * @return True if the block has a parent block, otherwise false.
 */
bool
NetlistBlock::hasParentBlock() const {
    return parentBlock_ != NULL;
}


/**
 * Returns the parent block.
 *
 * @return The parent block.
 * @exception InstanceNotFound If the block does not have a parent block.
 */
NetlistBlock&
NetlistBlock::parentBlock() const
    throw (InstanceNotFound) {

    if (!hasParentBlock()) {
        const string procName = "NetlistBlock::parentBlock";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    return *parentBlock_;
}


/**
 * Returns the netlist which the block belongs to.
 *
 * @return The netlist.
 */
Netlist&
NetlistBlock::netlist() const {
    return netlist_;
}

} // namespace ProGe
