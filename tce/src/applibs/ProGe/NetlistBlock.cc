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
 * @file NetlistBlock.cc
 *
 * Implementation of NetlistBlock class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <iostream>

#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "Netlist.hh"

#include "SequenceTools.hh"
#include "ContainerTools.hh"
#include "MapTools.hh"
#include "Application.hh"
#include "Conversion.hh"

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


void
NetlistBlock::setParameter(const Netlist::Parameter& param) {

    setParameter(param.name, param.type, param.value);
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
NetlistBlock::parameter(const std::string& name) const {
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
NetlistBlock::parameter(int index) const {
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
NetlistBlock::port(int index) const {
    if (index < 0 || index >= portCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return *ports_[index];
}

/**
 * Returns a port that matches (partially) the given name.
 *
 * @param name Name to be searched
 * @return First port that matches the search
 */
NetlistPort* 
NetlistBlock::portByName(const std::string& name) const {

    NetlistPort* port = NULL;
    for (unsigned int i = 0; i < ports_.size(); i++) {
        if (ports_.at(i)->name().find(name) != string::npos) {
            port = ports_.at(i);
            break;
        }
    }
    return port;
}


/**
 * Adds the given block as sub block of this block.
 *
 * @param block The block.
 * @exception IllegalRegistration If the block is already a sub block or
 *                                if it belongs to another netlist.
 */
void
NetlistBlock::addSubBlock(NetlistBlock* block) {
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
NetlistBlock::subBlock(int index) const {
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
NetlistBlock::parentBlock() const {
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


/**
 * Copies the toplevel block and it's parameters to the given netlist.
 * Sub blocks are not copied.
 *
 * @param instanceName New instance name for the copy
 * @param destination Destination netlist
 * @return Pointer to the netlist block copy
 */
NetlistBlock*
NetlistBlock::copyToNewNetlist(
    const std::string& instanceName,
    Netlist& destination) const {

    NetlistBlock* core = new NetlistBlock(moduleName_, instanceName,
                                          destination);
    for (int i = 0; i < netlist_.parameterCount(); i++) {
        Netlist::Parameter param = netlist_.parameter(i);
        destination.setParameter(param.name, param.type, param.value);
    }
    
    for (int i = 0; i < portCount(); i++) {
        NetlistPort* srcPort = &port(i);
        if (srcPort->realWidthAvailable()) {
            new NetlistPort(srcPort->name(), srcPort->widthFormula(),
                            srcPort->realWidth(), srcPort->dataType(),
                            srcPort->direction(), *core);
        } else {
            int width = 0;
            if (resolveRealWidth(srcPort, width)) {
                new NetlistPort(
                    srcPort->name(), srcPort->widthFormula(),
                    width, srcPort->dataType(), srcPort->direction(), *core);
            } else {
                new NetlistPort(
                    srcPort->name(), srcPort->widthFormula(),
                    srcPort->dataType(), srcPort->direction(), *core);
            }
        }
    }
    return core;
}


bool
NetlistBlock::isVirtual() const {

    return false;
}

bool
NetlistBlock::resolveRealWidth(const NetlistPort* port, int& width) const {
    
    string formula = port->widthFormula();
    // check if it is a parameter
    for (int i = 0; i < netlist_.parameterCount(); i++) {
        Netlist::Parameter param = netlist_.parameter(i);
        if (param.name == formula) {
            width = Conversion::toInt(param.value);
            return true;
        }
    }
    
    // check if formula is a plain number
    bool success = false;
    try {
        width = Conversion::toInt(formula);
        success = true;
    } catch (Exception& e) {
        success = false;
    }
    return success;
}

void
NetlistBlock::addPackage(
    const std::string& packageName) {

    if (!ContainerTools::containsValue(packages_, packageName)) {
        packages_.push_back(packageName);
    }
}

size_t
NetlistBlock::packageCount() const {
    return packages_.size();
}

const std::string&
NetlistBlock::package(size_t idx) const {
    return packages_.at(idx);
}

} // namespace ProGe
