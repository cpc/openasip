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
 * @file NetlistBlock.cc
 *
 * Implementation of NetlistBlock class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <iostream>

#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"
#include "Netlist.hh"
#include "Parameter.hh"
#include "NetlistWriter.hh"
#include "VHDLNetlistWriter.hh"
#include "VerilogNetlistWriter.hh"
#include "FileSystem.hh"

#include "SequenceTools.hh"
#include "ContainerTools.hh"
#include "MapTools.hh"
#include "Application.hh"
#include "Conversion.hh"

using std::string;

namespace ProGe {

/**
 * Constructor. Creates a netlist  with no ports.
 *
 * The created  is empty. Only its name, the parent netlist and the
 * name of the instance module are defined.
 *
 * @param moduleName Name of the module.
 * @param instanceName Name of the instance of the module.
 * @param netlist The netlist which the  belongs to.
 */
NetlistBlock::NetlistBlock(
    const std::string& moduleName,
    const std::string& instanceName,
    BaseNetlistBlock* parent)
    : BaseNetlistBlock(moduleName, instanceName, parent) {
}

/**
 * The destructor.
 *
 * Deletes all the ports.
 */
NetlistBlock::~NetlistBlock() {
}

/**
 * Sets the given parameter for the .
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

    setParameter(Parameter(name, type, value));
}

NetlistPort* 
NetlistBlock::port(
    const std::string& portName,
    bool partialMatch) {

    return BaseNetlistBlock::findPort(portName, false, partialMatch);
}

/**
 * Returns a sub  by the given index.
 *
 * @param index The index.
 * @return The sub .
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of sub blocks.
 */
NetlistBlock&
NetlistBlock::subBlock(size_t index) {
    if (index >= subBlockCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    //todo check for and throw wrong subblass
    return *dynamic_cast<NetlistBlock*>(&BaseNetlistBlock::subBlock(index));
}

/**
 * Returns the parent .
 *
 * @return The parent .
 * @exception InstanceNotFound If the  does not have a parent .
 */
const NetlistBlock&
NetlistBlock::parentBlock() const {
    if (!hasParentBlock()) {
        const string procName = "NetlistBlock::parentBlock";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    //todo: fix parentblock may not be NetlistBlock.
    return *dynamic_cast<const NetlistBlock*>(
        &BaseNetlistBlock::parentBlock());
}

/**
 * Returns the parent .
 *
 * @return The parent .
 * @exception InstanceNotFound If the  does not have a parent .
 */
NetlistBlock&
NetlistBlock::parentBlock() {
    if (!hasParentBlock()) {
        const string procName = "NetlistBlock::parentBlock";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    return *dynamic_cast<NetlistBlock*>(&BaseNetlistBlock::parentBlock());
}

/**
 * Copies the block without its children or internal connections - leaving
 * only the outer portion of the block.
 *
 * @param instanceName New instance name for the copy
 * @return Pointer to the netlist  copy
 */
NetlistBlock*
NetlistBlock::shallowCopy(
    const std::string& instanceName) const {

    NetlistBlock* block = new NetlistBlock(
        this->moduleName(), instanceName, NULL);

    for (size_t i = 0; i < this->parameterCount(); i++) {
        block->setParameter(this->parameter(i));
    }
    for (size_t i = 0; i < netlist().parameterCount(); i++) {
        block->netlist().setParameter(this->netlist().parameter(i));
    }
    // Copy ports while preserving their insertion order.
    std::map<std::string, NetlistPort*> copiedPorts;
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

    return block;
}

/**
 * Writes self if non-empty and calls write() function of each sub block.
 */
void
NetlistBlock::write(const Path& targetBaseDir, HDL targetLang) const {
    if (netlist().hasConnections()) {
        NetlistWriter* writer;
        std::string topLevelDir = "";
        if (targetLang == ProGe::VHDL) {
            writer = new VHDLNetlistWriter(*this);
            topLevelDir = targetBaseDir.string() +
                FileSystem::DIRECTORY_SEPARATOR + "vhdl";
        } else if (targetLang == ProGe::Verilog) {
            writer = new VerilogNetlistWriter(*this);
            topLevelDir = targetBaseDir.string() +
                FileSystem::DIRECTORY_SEPARATOR + "verilog";
        } else {
            assert(false && "Unsupported HDL.");
        }

        if (!FileSystem::fileExists(topLevelDir)) {
            bool directoryCreated = FileSystem::createDirectory(topLevelDir);
            if (!directoryCreated) {
                std::string errorMsg = "Unable to create directory " +
                    topLevelDir + ".";
                throw IOException(__FILE__, __LINE__, __func__, errorMsg);
            }
        }

        writer->write(topLevelDir);
        delete writer;
    }

    BaseNetlistBlock::write(targetBaseDir, targetLang);
}

} // namespace ProGe
