/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
#include <string>
#include <map>
#include "MemoryGenerator.hh"
#include "PlatformIntegrator.hh"
#include "NetlistBlock.hh"
#include "VirtualNetlistBlock.hh"
#include "NetlistPort.hh"
#include "Exception.hh"
#include "HDLPort.hh"
using std::string;
using std::vector;
using ProGe::NetlistBlock;
using ProGe::VirtualNetlistBlock;
using ProGe::NetlistPort;

const std::string MemoryGenerator::CLOCK_PORT = "clk";
const std::string MemoryGenerator::RESET_PORT = "rstx";

MemoryGenerator::MemoryGenerator(
    int mauWidth,
    int widthInMaus,
    int addrWidth,
    std::string initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream):
    mauWidth_(mauWidth), widthInMaus_(widthInMaus), addrWidth_(addrWidth),
    initFile_(initFile), integrator_(integrator),
    warningStream_(warningStream), errorStream_(errorStream) {

}

MemoryGenerator::~MemoryGenerator() {

    for (PortMap::iterator i = memPorts_.begin(); i != memPorts_.end(); i++) {
        delete i->second;
    }
}

bool
MemoryGenerator::isCompatible(
    const ProGe::NetlistBlock& ttaCore,
    std::vector<std::string>& reasons) const {

    bool foundAll = true;
    PortMap::const_iterator iter = memPorts_.begin();
    while (iter != memPorts_.end()) {
        if (ttaCore.portByName(iter->first) == NULL) {
            string message = "Couldn't find port " + iter->first +
                " from toplevel";
            reasons.push_back(message);
            foundAll = false;
        }
        iter++;
    }
    return foundAll;
}


void
MemoryGenerator::addMemory(ProGe::Netlist& netlist) {

    const NetlistBlock& core = integrator_->ttaCoreBlock();

    VirtualNetlistBlock* virt =
        new VirtualNetlistBlock(
            moduleName() + "_virt",instanceName() + "_virt", netlist);

    NetlistBlock* mem =
        new NetlistBlock(moduleName(), instanceName(), netlist);
    netlist.topLevelBlock().addSubBlock(mem);
    
    for (int i = 0; i < parameterCount(); i++) {
        mem->setParameter(parameter(i));
    }

    for (int i = 0; i < portCount(); i++) {
        const HDLPort* hdlPort = port(i);
        NetlistPort* memPort = NULL;
        if (hdlPort->hasStaticValue()) {
             memPort = hdlPort->convertToNetlistPort(*virt);
        } else {
            memPort = hdlPort->convertToNetlistPort(*mem);
        }
        assert(memPort != NULL);

        string corePortName = portKeyName(hdlPort);
        NetlistPort* corePort = NULL;
        // clock and reset must be connected to new toplevel ports
        if (corePortName == CLOCK_PORT) {
            corePort = netlist.topLevelBlock().portByName(CLOCK_PORT);
        } else if (corePortName == RESET_PORT) {
            corePort = netlist.topLevelBlock().portByName(RESET_PORT);
        } else {
            corePort = core.portByName(corePortName);
        }
        assert(corePort != NULL);

        if (hdlPort->needsInversion()) {
            netlist.connectPortsInverted(*corePort, *memPort);
        } else {
            if (memPort->dataType() == corePort->dataType()) {
                netlist.connectPorts(*memPort, *corePort);
            } else {
                // bit to bit vector connection, connect lowest bits
                netlist.connectPorts(*memPort, *corePort, 0, 0, 1);
            }
        }
    }

    if (virt->portCount() > 0) {
        netlist.topLevelBlock().addSubBlock(virt);
    } else {
        delete virt;
    }
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


std::string 
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
        string message = "Index out of range";
        OutOfRange exc(__FILE__, __LINE__, "MemoryGenerator", message);
        throw exc;
    }
    PortMap::const_iterator iter = memPorts_.begin();
    for (int i = 0; i < index; i++) {
        iter++;
    }
    return iter->second;
}


const HDLPort*
MemoryGenerator::portByKeyName(std::string name) const {

    if (memPorts_.find(name) == memPorts_.end()) {
        string message = "Port " + name + " not found";
        KeyNotFound exc(__FILE__, __LINE__, "MemoryGenerator", message);
        throw exc;
    }
    return memPorts_.find(name)->second;
}


std::string
MemoryGenerator::portKeyName(const HDLPort* port) const {
    
    string name = "";
    PortMap::const_iterator iter = memPorts_.begin();
    while (iter != memPorts_.end()) {
        if (iter->second == port) {
            name = iter->first;
            break;
        }
        iter++;
    }
    if (name.empty()) {
        string message = "Key for port " + port->name() + " not found";
        KeyNotFound exc(__FILE__, __LINE__, "MemoryGenerator", message);
        throw exc;
    }
    return name;
}

void
MemoryGenerator::addPort(const std::string& name, HDLPort* port) {

    assert(port != NULL);
    memPorts_.insert(std::pair<string, HDLPort*>(name, port));
}


int
MemoryGenerator::parameterCount() const {

    return params_.size();
}
    
const ProGe::Netlist::Parameter&
MemoryGenerator::parameter(int index) const {
    
    return params_.at(index);
}


void
MemoryGenerator::addParameter(const ProGe::Netlist::Parameter& add) {

    ProGe::Netlist::Parameter toAdd = {add.name, add.type, add.value};
    params_.push_back(toAdd);
}
