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
 * @file BaseNetlistBlock.cc
 *
 * Implementation of BaseNetlistBlock class.
 *
 * Created on: 20.4.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "BaseNetlistBlock.hh"

#include <algorithm>
#include <locale>

#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"
#include "Netlist.hh"
#include "NetlistTools.hh"
#include "Parameter.hh"
#include "VHDLNetlistWriter.hh"
#include "VerilogNetlistWriter.hh"

#include "Application.hh"
#include "ContainerTools.hh"
#include "FileSystem.hh"

namespace ProGe {

BaseNetlistBlock::BaseNetlistBlock()
    : parent_(nullptr),
      subBlocks_(),
      parameters_(),
      ports_(),
      portGroups_(),
      netlist_(new Netlist()),
      instanceName_("defaultInstanceName_inst0"),
      moduleName_("defaultModuleName"),
      packages_() {}

BaseNetlistBlock::BaseNetlistBlock(BaseNetlistBlock* parent)
    : parent_(parent),
      subBlocks_(),
      parameters_(),
      ports_(),
      portGroups_(),
      netlist_(new Netlist()),
      instanceName_("defaultInstanceName_inst0"),
      moduleName_("defaultModuleName"),
      packages_() {
    if (parent_ != nullptr) {
        parent_->addSubBlock(this);
    }
}

BaseNetlistBlock::BaseNetlistBlock(
    const std::string& moduleName, const std::string& instanceName,
    BaseNetlistBlock* parent)
    : parent_(parent),
      subBlocks_(),
      parameters_(),
      ports_(),
      portGroups_(),
      netlist_(new Netlist()),
      instanceName_(instanceName),
      moduleName_(moduleName),
      packages_() {
    if (instanceName.empty()) {
        setInstanceName(moduleName + "_inst0");
    }

    if (parent_ != nullptr) {
        parent_->addSubBlock(this);
    }
}

BaseNetlistBlock::~BaseNetlistBlock() {
    if (parent_ != nullptr) {
        parent_->removeSubBlock(this);
        parent_ = nullptr;
    }

    for (PortGroupContainerType::reverse_iterator it = portGroups_.rbegin();
         it != portGroups_.rend();) {
        NetlistPortGroup* toDeleted = *it;
        it++;
        portGroups_.pop_back();
        delete toDeleted;
    }

    for (PortContainerType::reverse_iterator it = ports_.rbegin();
         it != ports_.rend();) {
        NetlistPort* toDeleted = *it;
        it++;
        ports_.pop_back();
        delete toDeleted;
    }

    for (BlockContainerType::reverse_iterator it = subBlocks_.rbegin();
         it != subBlocks_.rend();) {
        BaseNetlistBlock* toDeleted = *it;
        it++;
        subBlocks_.pop_back();
        delete toDeleted;
    }

    delete netlist_;
    netlist_ = nullptr;
}

const std::string&
BaseNetlistBlock::instanceName() const {
    return instanceName_;
}

const std::string&
BaseNetlistBlock::moduleName() const {
    return moduleName_;
}

const std::string
BaseNetlistBlock::name() const {
    return instanceName_ + " : " + moduleName();
}

size_t
BaseNetlistBlock::subBlockCount() const {
    return subBlocks_.size();
}

const BaseNetlistBlock&
BaseNetlistBlock::subBlock(size_t index) const {
    return *subBlocks_.at(index);
}

bool
BaseNetlistBlock::hasSubBlock(const std::string& instanceName) const {
    for (size_t i = 0; i < subBlockCount(); i++) {
        if (subBlock(i).instanceName() == instanceName) {
            return true;
        }
    }
    return false;
}

/**
 * Returns true if the given netlist block is sub block of this block.
 */
bool
BaseNetlistBlock::isSubBlock(const BaseNetlistBlock& block) const {
    return ContainerTools::containsValue(subBlocks_, &block);
}

BaseNetlistBlock&
BaseNetlistBlock::subBlock(size_t index) {
    return *subBlocks_.at(index);
}

bool
BaseNetlistBlock::hasParameter(const std::string& name) const {
    for (size_t i = 0; i < parameterCount(); i++) {
        if (parameter(i).name() == name) {
            return true;
        }
    }
    return false;
}

/**
 * Returns a Parameter object by name.
 *
 * @exception NotAvailable Thrown if the block does not have such Parameter.
 */
const Parameter&
BaseNetlistBlock::parameter(const std::string& name) const {
    for (size_t i = 0; i < parameterCount(); i++) {
        if (parameter(i).name() == name) {
            return parameter(i);
        }
    }

    throw NotAvailable(
        __FILE__, __LINE__, __func__,
        std::string("The block (") + instanceName() + ":" + moduleName() +
            ") does not have parameter \"" + name + "\"");
}

/**
 * Returns a Parameter object by name.
 *
 * @exception NotAvailable Thrown if the block does not have such Parameter.
 */
Parameter&
BaseNetlistBlock::parameter(const std::string& name) {
    for (size_t i = 0; i < parameterCount(); i++) {
        if (parameters_.at(i).name() == name) {
            return parameters_.at(i);
        }
    }

    throw NotAvailable(
        __FILE__, __LINE__, __func__,
        std::string("The block (") + instanceName() + ":" + moduleName() +
            ") does not have parameter \"" + name + "\"");
}

size_t
BaseNetlistBlock::parameterCount() const {
    return parameters_.size();
}

const Parameter&
BaseNetlistBlock::parameter(size_t index) const {
    if (index >= parameterCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return parameters_.at(index);
}

/**
 * Return port count in this block including ports from netlist port groups.
 */
size_t
BaseNetlistBlock::portCount() const {
    return ports_.size();
}

const NetlistPort&
BaseNetlistBlock::port(size_t index) const {
    return *ports_.at(index);
}

/**
 * Returns list of ports by Signal Type.
 *
 * @param The list of ports by matching signal type in insertion order.
 *
 */
std::vector<const NetlistPort*>
BaseNetlistBlock::portsBy(SignalType type) const {
    std::vector<const NetlistPort*> result;
    for (size_t i = 0; i < portCount(); i++) {
        const NetlistPort& port = this->port(i);
        if (port.assignedSignal().type() == type) {
            result.push_back(&port);
        }
    }
    return result;
}

/**
 * Returns a port by Signal Type at given index.
 *
 *
 * @param The list of ports by matching signal type at the index.
 * @exception OutOfRange If the index is the number of found matches or
 * larger.
 */
const NetlistPort&
BaseNetlistBlock::portBy(SignalType type, size_t index) const {
    std::vector<const NetlistPort*> matches = portsBy(type);
    if (index < matches.size()) {
        return *matches.at(index);
    } else {
        THROW_EXCEPTION(
            OutOfRange,
            "No port by signal at index " + Conversion::toString(index));
    }
}

/**
 * Return true if the netlist block has at least one port by given Signal
 * Type.
 */
bool
BaseNetlistBlock::hasPortsBy(SignalType type) const {
    std::vector<const NetlistPort*> result;
    for (size_t i = 0; i < portCount(); i++) {
        const NetlistPort& port = this->port(i);
        if (port.assignedSignal().type() == type) {
            return true;
        }
    }
    return false;
}

/**
 * Returns a port that matches (partially) the given name.
 *
 * @return The matching port. If not found, returns nullptr.
 */
const NetlistPort*
BaseNetlistBlock::port(const std::string& portName, bool partialMatch) const {
    return findPort(portName, false, partialMatch);
}

NetlistPort&
BaseNetlistBlock::port(size_t index) {
    return *ports_.at(index);
}

size_t
BaseNetlistBlock::portGroupCount() const {
    return portGroups_.size();
}

const NetlistPortGroup&
BaseNetlistBlock::portGroup(size_t index) const {
    return *portGroups_.at(index);
}

std::vector<const NetlistPortGroup*>
BaseNetlistBlock::portGroupsBy(SignalGroupType type) const {
    std::vector<const NetlistPortGroup*> found;
    for (size_t i = 0; i < portGroupCount(); i++) {
        if (portGroup(i).assignedSignalGroup() == type) {
            found.push_back(&portGroup(i));
        }
    }
    return found;
}

const Netlist&
BaseNetlistBlock::netlist() const {
    return *netlist_;
}

bool
BaseNetlistBlock::hasParentBlock() const {
    return parent_ != nullptr;
}

const BaseNetlistBlock&
BaseNetlistBlock::parentBlock() const {
    assert(hasParentBlock());
    return *parent_;
}

BaseNetlistBlock&
BaseNetlistBlock::parentBlock() {
    assert(hasParentBlock());
    return *parent_;
}

/**
 * Replaces the instance name of the block.
 *
 * The new name must be unique within sub blocks of parent block it is
 * attached to.
 *
 * @exception ObjectAlreadyExists Thrown if the name is not unique.
 *
 */
void
BaseNetlistBlock::setInstanceName(const std::string& name) {
    if (hasParentBlock() && parentBlock().hasSubBlock(name)) {
        THROW_EXCEPTION(
            ObjectAlreadyExists, "New instance name of " + name +
                                     " is not unique within parent "
                                     "block of " +
                                     parentBlock().instanceName() + " : " +
                                     parentBlock().moduleName());
    }
    instanceName_ = name;
}

void
BaseNetlistBlock::setModuleName(const std::string& name) {
    moduleName_ = name;
}

/**
 * Adds sub block to this block and acquires ownership of it.
 *
 * @param subBlock The block to be added.
 * @param instanceName The instance base name to be given for sub block. If
 * the given or sub block's instance name is not unique within the block The
 * name will be prefixed with a running number.
 */
void
BaseNetlistBlock::addSubBlock(
    BaseNetlistBlock* subBlock, const std::string& instanceName) {
    if (!instanceName.empty()) {
        subBlock->instanceName_ =
            NetlistTools::getUniqueInstanceName(*this, instanceName);
    } else {
        if (subBlock->instanceName().empty()) {
            subBlock->setInstanceName(NetlistTools::getUniqueInstanceName(
                *this, subBlock->moduleName() + "_inst0"));
        } else {
            subBlock->setInstanceName(NetlistTools::getUniqueInstanceName(
                *this, subBlock->instanceName()));
        }
    }

    assert(!subBlock->hasParentBlock() || &subBlock->parentBlock() == this);
    subBlock->setParent(this);
    subBlocks_.push_back(subBlock);

    // Register ports of the subblock so connections can be made.
    for (size_t i = 0; i < subBlock->portCount(); i++) {
        netlist_->registerPort(subBlock->port(i));
    }
}

/**
 * Deletes given sub block and its all sub blocks.
 */
void
BaseNetlistBlock::deleteSubBlock(BaseNetlistBlock* subBlock) {
    // todo remove port descriptions of deleted block in the netlist.
    for (size_t i = 0; i < subBlocks_.size(); i++) {
        if (subBlocks_.at(i) == subBlock) {
            BaseNetlistBlock* toDeleted = subBlocks_.at(i);
            ContainerTools::swapRemoveValue(subBlocks_, i);
            delete toDeleted;
        }
    }
}

/**
 * Removes sub block from this block and does not delete it.
 */
void
BaseNetlistBlock::removeSubBlock(BaseNetlistBlock* subBlock) {
    // todo remove port descriptions of removed block in the netlist.
    for (size_t i = 0; i < subBlocks_.size(); i++) {
        if (subBlocks_.at(i) == subBlock) {
            ContainerTools::swapRemoveValue(subBlocks_, i);
        }
    }
}

/**
 * Adds port to this block and acquires ownership of it.
 *
 * Added port will point to this block after the call.
 *
 * @exception ObjectAlreadyExists Thrown if the netlist block already has a
 *                                port by same name.
 */
NetlistPort*
BaseNetlistBlock::addPort(NetlistPort* port) {
    assert(port != nullptr && "Attempted to add null port.");
    if (findPort(port->name(), false, false)) {
        THROW_EXCEPTION(
            ObjectAlreadyExists,
            "Given port by name (" + port->name() +
                ") already exists in the netlist block (" +
                this->instanceName() + " : " + this->moduleName() + ").");
    }

    if (!port->hasParentBlock() || &port->parentBlock() != this) {
        port->setParent(this);
    }
    ports_.push_back(port);
    netlist_->registerPort(*port);
    if (parent_ != nullptr) {
        parent_->netlist_->registerPort(*port);
    }
    return port;
}

/**
 * Removes the given port from the block.
 *
 * After call the port is parent block reference is set to nullptr.
 */
void
BaseNetlistBlock::removePort(NetlistPort* port) {
    assert(
        &port->parentBlock() == this &&
        "Attempted to remove a port the block does not have.");

    netlist().unregisterPort(*port);
    if (this->hasParentBlock()) {
        parentBlock().netlist().unregisterPort(*port);
    }
    ContainerTools::removeValueIfExists(ports_, port);
    port->setParent(nullptr);
}

void
BaseNetlistBlock::addPortGroup(NetlistPortGroup* portGroup) {
    portGroups_.push_back(portGroup);
    portGroup->setParent(this);
    for (size_t i = 0; i < portGroup->portCount(); i++) {
        if (!netlist().isRegistered(portGroup->portAt(i))) {
            addPort(&portGroup->portAt(i));
        }
    }
}

void
BaseNetlistBlock::removePortGroup(NetlistPortGroup* port) {
    for (size_t i = 0; i < portGroups_.size(); i++) {
        if (portGroups_.at(i) == port) {
            ContainerTools::swapRemoveValue(portGroups_, i);
            break;
        }
    }
}

/**
 * Adds parameter to the block overwriting the existing one by name.
 */
void
BaseNetlistBlock::setParameter(const Parameter& param) {
    for (Parameter& p : parameters_) {
        if (p.name() == param.name()) {
            p = param;
            return;
        }
    }
    parameters_.push_back(param);
}

/**
 * Adds new parameter to the block. Throws exception if the block has the
 * parameter by the name already.
 */
void
BaseNetlistBlock::addParameter(const Parameter& param) {
    // Check for uniqueness //
    for (Parameter& p : parameters_) {
        if (p.name() == param.name()) {
            throw ObjectAlreadyExists(
                __FILE__, __LINE__,
                "The block \"" + name() + "\" already has parameter\"" +
                    param.name() + "\"");
        }
    }

    // Check if parameter's value refers to a constant //
    if (param.valueIsConstant()) {
        std::string packageName = param.packageNameOfConstant();
        assert(!packageName.empty());
        addPackage(packageName);
    }

    parameters_.push_back(param);
}

/**
 * Returns a port that matches (partially) the given name.
 *
 * If not found, returns nullptr
 */
NetlistPort*
BaseNetlistBlock::findPort(
    const std::string& portName, bool recursiveSearch,
    bool partialMatch) const {
    if (recursiveSearch) {
        assert(
            false &&
            "BaseNetlistBlock::findPort(): "
            "recursive search not implemented.");
    }

    NetlistPort* portPtr = nullptr;
    for (size_t i = 0; i < portCount(); i++) {
        if (ports_.at(i)->name() == portName) {
            return ports_.at(i);
        }
    }
    if (partialMatch) {
        for (size_t i = 0; i < portCount(); i++) {
            if (ports_.at(i)->name().find(portName) != std::string::npos) {
                return ports_.at(i);
            }
        }
    }

    return portPtr;
}

void
BaseNetlistBlock::build() {}

void
BaseNetlistBlock::connect() {}

void
BaseNetlistBlock::finalize() {}

/**
 * Does nothing on self but calls write function on each sub block.
 */
void
BaseNetlistBlock::write(const Path& targetBaseDir, HDL targetLang) const {
    // Call write of each sub block //
    for (BaseNetlistBlock* subblock : subBlocks_) {
        subblock->write(targetBaseDir, targetLang);
    }
}

/**
 * Writes HDL source of itself only using default netlist writer.
 */
void
BaseNetlistBlock::writeSelf(const Path& targetBaseDir, HDL targetLang) const {
    if (targetLang == HDL::VHDL) {
        VHDLNetlistWriter(*this).write(targetBaseDir.string());
    } else if (targetLang == HDL::Verilog) {
        VerilogNetlistWriter(*this).write(targetBaseDir.string());
    } else {
        THROW_EXCEPTION(NotAvailable, "Given HDL language is not supported.");
    }
}

/**
 * Creates new netlist block that only includes without of its subblocks or
 * interconnections.
 */
BaseNetlistBlock*
BaseNetlistBlock::shallowCopy(const std::string& instanceName) const {
    BaseNetlistBlock* block = new BaseNetlistBlock(
        this->moduleName(),
        (!instanceName.empty()) ? instanceName : this->instanceName(),
        nullptr);

    for (size_t i = 0; i < this->parameterCount(); i++) {
        block->setParameter(this->parameter(i));
    }
    for (size_t i = 0; i < netlist().parameterCount(); i++) {
        block->netlist().setParameter(this->netlist().parameter(i));
    }
    std::map<std::string, NetlistPort*> copiedPorts;
    // Copy ports while preserving their insertion order.
    for (size_t i = 0; i < portCount(); i++) {
        NetlistPort* copiedPort = nullptr;
        copiedPort = BaseNetlistBlock::port(i).copyTo(*block);
        copiedPorts.insert({{copiedPort->name(), copiedPort}});
    }
    for (size_t i = 0; i < portGroupCount(); i++) {
        const NetlistPortGroup* portGrp = &portGroup(i);
        // Cloning to copy original class type, but clear ports as they are
        // already created and added to the new block.
        NetlistPortGroup* newGroup = portGrp->clone();
        newGroup->clear();
        for (auto port : *portGrp) {
            newGroup->addPort(*copiedPorts.at(port->name()));
        }
        block->addPortGroup(newGroup);
    }
    for (size_t i = 0; i < packageCount(); i++) {
        block->addPackage(this->package(i));
    }

    assert(
        block->portCount() == this->portCount() &&
        "Port count mismatch in shallow copy.");
    return block;
}

Netlist&
BaseNetlistBlock::netlist() {
    assert(netlist_ != nullptr);
    return *netlist_;
}

void
BaseNetlistBlock::addPackage(const std::string& packageName) {
    if (!ContainerTools::containsValue(packages_, packageName)) {
        packages_.push_back(packageName);
    }
}

size_t
BaseNetlistBlock::packageCount() const {
    return packages_.size();
}

const std::string&
BaseNetlistBlock::package(size_t idx) const {
    return packages_.at(idx);
}

/**
 * Changes parent block to given one.
 */
void
BaseNetlistBlock::setParent(BaseNetlistBlock* newparent) {
    if (parent_ != nullptr && parent_ != newparent) {
        BaseNetlistBlock* oldParent = parent_;
        parent_ = nullptr;  // Prevent parent block causing recursive call of
        // setParent() when calling detachSubBlock().
        oldParent->removeSubBlock(this);
    }
    parent_ = newparent;
}

/**
 * Connects all unconnected clock ports of sub block to this block's clock
 * port if there is one.
 */
void
BaseNetlistBlock::connectClocks() {
    auto clkPorts = this->portsBy(SignalType::CLOCK);
    assert(clkPorts.size() == 1);
    const NetlistPort& thisClk = *clkPorts.at(0);

    for (size_t i = 0; i < subBlockCount(); i++) {
        const BaseNetlistBlock& block = subBlock(i);
        for (auto port : block.portsBy(SignalType::CLOCK)) {
            if (!netlist().isPortConnected(*port) &&
                thisClk.direction() == port->direction()) {
                netlist().connect(thisClk, *port);
            }
        }
    }
}

/**
 * Connects all unconnected reset ports of sub block to this block's reset
 * port if there is one.
 */
void
BaseNetlistBlock::connectResets() {
    auto resetPorts = this->portsBy(SignalType::RESET);
    assert(resetPorts.size() == 1);
    const NetlistPort& thisReset = *resetPorts.at(0);

    for (size_t i = 0; i < subBlockCount(); i++) {
        const BaseNetlistBlock& block = subBlock(i);
        for (auto port : block.portsBy(SignalType::RESET)) {
            if (!netlist().isPortConnected(*port) &&
                thisReset.direction() == port->direction()) {
                netlist().connect(thisReset, *port);
            }
        }
    }
}

} /* namespace ProGe */
