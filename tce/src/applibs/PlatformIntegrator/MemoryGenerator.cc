/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file MemoryGenerator.cc
 *
 * Implementation of MemoryGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <map>
#include "MemoryGenerator.hh"
#include "PlatformIntegrator.hh"
#include "NetlistBlock.hh"
#include "VirtualNetlistBlock.hh"
#include "NetlistPort.hh"
#include "Exception.hh"
#include "HDLPort.hh"
#include "FileSystem.hh"
#include "Environment.hh"
#include "HDLTemplateInstantiator.hh"
#include "FUArchitecture.hh"
#include "FUImplementation.hh"
#include "FUExternalPort.hh"
#include "FunctionUnit.hh"
#include "InverterBlock.hh"
using ProGe::NetlistBlock;
using ProGe::VirtualNetlistBlock;
using ProGe::NetlistPort;
using HDB::FUArchitecture;
using HDB::FUExternalPort;

const TCEString MemoryGenerator::CLOCK_PORT = "clk";
const TCEString MemoryGenerator::RESET_PORT = "rstx";

MemoryGenerator::MemoryGenerator(
    int mauWidth,
    int widthInMaus,
    int addrWidth,
    TCEString initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream):
    mauWidth_(mauWidth), widthInMaus_(widthInMaus), addrWidth_(addrWidth),
    initFile_(initFile), integrator_(integrator),
    warningStream_(warningStream), errorStream_(errorStream),
    lsuArch_(NULL), lsuPorts_() {

}

MemoryGenerator::~MemoryGenerator() {

    for (PortMap::iterator i = memPorts_.begin(); i != memPorts_.end(); i++) {
        if (i->second != NULL)
            delete i->second;
    }
}

bool
MemoryGenerator::isCompatible(
    const ProGe::NetlistBlock& ttaCore,
    int coreId,
    std::vector<TCEString>& reasons) const {

    for (std::string port : lsuPorts_) {
        if (!checkFuPort(port, reasons)) {
            return false;
        }
    }
    bool foundAll = true;
    PortMap::const_iterator iter = memPorts_.begin();
    while (iter != memPorts_.end()) {
        TCEString corePort = corePortName(iter->first, coreId);
        if (ttaCore.port(corePort) == NULL) {
            TCEString message = "Couldn't find port " + corePort +
                " from toplevel";
            reasons.push_back(message);
            foundAll = false;
        }
        iter++;
    }
    return foundAll;
}

bool
MemoryGenerator::checkFuPort(
    const std::string fuPort,
    std::vector<TCEString>& reasons) const {

    PortMap::const_iterator iter = memPorts_.find(fuPort);
    if (iter == memPorts_.end()) {
        TCEString msg;
        msg << "MemoryGenerator does not have port " << fuPort;
        reasons.push_back(msg);
        return false;
    }
    return true;
}


void
MemoryGenerator::addMemory(
    const ProGe::NetlistBlock& ttaCore,
    ProGe::NetlistBlock& integratorBlock,
    int memIndex,
    int coreId) {

    BlockPair blocks =
        createMemoryNetlistBlock(integratorBlock, memIndex, coreId);
    NetlistBlock* mem = blocks.first;
    VirtualNetlistBlock* virt = blocks.second;
    assert(mem != NULL);
    assert(virt != NULL);
    if (virt->portCount() > 0) {
        integratorBlock.addSubBlock(virt);
    }

    for (int i = 0; i < portCount(); i++) {
        const HDLPort* hdlPort = port(i);
        NetlistPort* memPort = mem->port(hdlPort->name());
        if (memPort == NULL) {
            memPort = virt->port(hdlPort->name());
            if (memPort == NULL) {
                TCEString msg = "Port ";
                msg << hdlPort->name() << " not found from netlist block";
                throw InvalidData(__FILE__, __LINE__, "MemoryGenerator", msg);
            }
        }

        TCEString portName = corePortName(portKeyName(hdlPort), coreId);
        const NetlistPort* corePort = NULL;
        // clock and reset must be connected to new toplevel ports
        if (portName == platformIntegrator()->clockPort()->name()) {
            corePort = platformIntegrator()->clockPort();
        } else if (portName == platformIntegrator()->resetPort()->name()) {
            corePort = platformIntegrator()->resetPort();
        } else {
            corePort = ttaCore.port(portName);
        }
        assert(corePort != NULL);

        connectPorts(
            integratorBlock, *memPort, *corePort,
            hdlPort->needsInversion(), coreId);
    }
}


MemoryGenerator::BlockPair
MemoryGenerator::createMemoryNetlistBlock(
    ProGe::NetlistBlock& integratorBlock,
    int memIndex,
    int coreId) {

    VirtualNetlistBlock* staticConnectionsBlock = new VirtualNetlistBlock(
        moduleName() + "_virt", instanceName(coreId, memIndex) + "_virt");


    NetlistBlock* mem =
        new NetlistBlock(moduleName(), instanceName(coreId, memIndex));
    integratorBlock.addSubBlock(mem);

    for (int i = 0; i < parameterCount(); i++) {
        mem->setParameter(parameter(i));
    }
    for (int i = 0; i < portCount(); i++) {
        const HDLPort* hdlPort = port(i);
        NetlistPort* memPort = NULL;
        if (hdlPort->hasStaticValue()) {
            memPort = hdlPort->convertToNetlistPort(*staticConnectionsBlock);
        } else {
            memPort = hdlPort->convertToNetlistPort(*mem);
        }
        assert(memPort != NULL);
    }

    BlockPair blocks;
    blocks.first = mem;
    blocks.second = staticConnectionsBlock;
    return blocks;
}


int
MemoryGenerator::memoryTotalWidth() const {

    return mauWidth_*widthInMaus_;
}
    
int 
MemoryGenerator::memoryMauSize() const {

    return mauWidth_;
}

int 
MemoryGenerator::memoryWidthInMaus() const {

    return widthInMaus_;
}


int
MemoryGenerator::memoryAddrWidth() const {

    return addrWidth_;
}


TCEString 
MemoryGenerator::initializationFile() const {
    
    return initFile_;
}

const PlatformIntegrator*
MemoryGenerator::platformIntegrator() const {

    return integrator_;
}

std::ostream&
MemoryGenerator::warningStream() {

    return warningStream_;
}

std::ostream&
MemoryGenerator::errorStream() {

    return errorStream_;
}

int
MemoryGenerator::portCount() const {

    return memPorts_.size();
}
    
const HDLPort*
MemoryGenerator::port(int index) const {
    
    if (index > static_cast<int>(memPorts_.size())) {
        TCEString message = "Index out of range";
        throw OutOfRange(__FILE__, __LINE__, "MemoryGenerator", message);
    }
    PortMap::const_iterator iter = memPorts_.begin();
    for (int i = 0; i < index; i++) {
        iter++;
    }
    return iter->second;
}


const HDLPort*
MemoryGenerator::portByKeyName(TCEString name) const {

    if (memPorts_.find(name) == memPorts_.end()) {
        TCEString message = "Port " + name + " not found";
        throw KeyNotFound(__FILE__, __LINE__, "MemoryGenerator", message);
    }
    return memPorts_.find(name)->second;
}


TCEString
MemoryGenerator::portKeyName(const HDLPort* port) const {
    
    TCEString name = "";
    PortMap::const_iterator iter = memPorts_.begin();
    while (iter != memPorts_.end()) {
        if (iter->second == port) {
            name = iter->first;
            break;
        }
        iter++;
    }
    if (name.empty()) {
        TCEString message = "Key for port " + port->name() + " not found";
        throw KeyNotFound(__FILE__, __LINE__, "MemoryGenerator", message);
    }
    return name;
}

void
MemoryGenerator::addPort(const TCEString& name, HDLPort* port) {

    assert(port != NULL);
    memPorts_.insert(std::pair<TCEString, HDLPort*>(name, port));
}


int
MemoryGenerator::parameterCount() const {

    return params_.size();
}
    
const ProGe::Parameter&
MemoryGenerator::parameter(int index) const {
    
    return params_.at(index);
}


void
MemoryGenerator::addParameter(const ProGe::Parameter& add) {

    ProGe::Parameter toAdd(add.name(), add.type(), add.value());
    params_.push_back(toAdd);
}

TCEString
MemoryGenerator::ttaCoreName() const {

    return platformIntegrator()->coreEntityName();
}

TCEString
MemoryGenerator::templatePath() const {
    
    TCEString path = Environment::dataDirPath("ProGe");
    path << FileSystem::DIRECTORY_SEPARATOR << "platform";
    return path;
}

void
MemoryGenerator::instantiateTemplate(
    const TCEString& inFile,
    const TCEString& outFile,
    const TCEString& entity) const {

    HDLTemplateInstantiator inst;
    inst.setEntityString(entity);
    inst.instantiateTemplateFile(inFile, outFile);
}

bool
MemoryGenerator::hasLSUArchitecture() const {
    return lsuArch_ != NULL;
}

const TTAMachine::FunctionUnit&
MemoryGenerator::lsuArchitecture() const {

    assert(lsuArch_ != NULL);
    return *lsuArch_;
}

TCEString
MemoryGenerator::corePortName(
    const TCEString& portBaseName,
    int coreId) const {

    // clock and reset port names are global
    if (portBaseName == integrator_->clockPort()->name() ||
        portBaseName == integrator_->resetPort()->name()) {
        return portBaseName;
    }

    TCEString portName;
    if (coreId >= 0) {
            portName << "core" << coreId << "_";
    }
    if (lsuArch_ != NULL) {
        portName << "fu_" << lsuArchitecture().name() << "_";
    }
    portName << portBaseName;
    return portName;
}


void
MemoryGenerator::addLsu(
    TTAMachine::FunctionUnit& lsuArch,
    std::vector<std::string> lsuPorts) {

    lsuArch_ = &lsuArch;
    lsuPorts_ = lsuPorts;
}


TCEString
MemoryGenerator::memoryIndexString(int coreId, int memIndex) const {

    TCEString index;
    if (coreId >= 0) {
        index << coreId << "_";
    }
    return index << memIndex;
}

void
MemoryGenerator::connectPorts(
    ProGe::NetlistBlock& netlistBlock,
    const ProGe::NetlistPort& memPort,
    const ProGe::NetlistPort& corePort,
    bool inverted,
    int /*coreId*/) {

    if (inverted) {
        ProGe::InverterBlock* InvertedBlock =
            new ProGe::InverterBlock(corePort, memPort);
        netlistBlock.addSubBlock(InvertedBlock);

        netlistBlock.netlist().connect(
            corePort, InvertedBlock->inputPort(), 0, 0, 1);
        netlistBlock.netlist().connect(
            InvertedBlock->outputPort(), memPort, 0, 0, 1);

    } else {
        if (memPort.dataType() == corePort.dataType()) {
            netlistBlock.netlist().connect(memPort, corePort);
        } else {
            // bit to bit vector connection, connect lowest bits
            netlistBlock.netlist().connect(memPort, corePort, 0, 0, 1);
        }
    }
}
