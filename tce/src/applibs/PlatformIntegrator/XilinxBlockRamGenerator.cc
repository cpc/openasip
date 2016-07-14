/*
    Copyright (c) 2016 Tampere University of Technology.

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
 * @file XilinxBlockRamGenerator.cc
 *
 * Implementation of XilinxBlockRamGenerator class.
 */

#include <iostream>
#include <vector>
#include "XilinxBlockRamGenerator.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "VirtualNetlistBlock.hh"
#include "HDLPort.hh"
#include "PlatformIntegrator.hh"
#include "FileSystem.hh"
#include "FunctionUnit.hh"
#include "Conversion.hh"
using std::endl;

const TCEString XilinxBlockRamGenerator::COMPONENT_FILE = 
    "xilinx_blockram.vhdl.tmpl";

XilinxBlockRamGenerator::XilinxBlockRamGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    TCEString initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream,
    bool connectToArbiter,
    ProGe::NetlistBlock* almarviIF,
    TCEString signalPrefix): 
    MemoryGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                       integrator, warningStream, errorStream),
    connectToArbiter_(connectToArbiter), almarviIF_(almarviIF), 
    signalPrefix_(signalPrefix) {
    assert (!connectToArbiter || almarviIF != nullptr);
    
    ProGe::Netlist::Parameter dataw = {"dataw", "integer", 
        Conversion::toString(memoryTotalWidth())};
    ProGe::Netlist::Parameter addrw = {"addrw", "integer", 
        Conversion::toString(memoryAddrWidth())};

    addParameter(dataw);
    addParameter(addrw);

    bool noInvert = false;
    addPort("data_in", 
            new HDLPort("data", "dataw",
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryTotalWidth()));
    addPort("data_out",
            new HDLPort("q", "dataw",
                        ProGe::BIT_VECTOR, HDB::OUT, noInvert,
                        memoryTotalWidth()));
    addPort("addr",
            new HDLPort("address", "addrw",
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryAddrWidth()));
    addPort("mem_en",
            new HDLPort("clken", "1", ProGe::BIT, HDB::IN, noInvert, 1));
    addPort("wr_en",
            new HDLPort("wren", "1", ProGe::BIT, HDB::IN, noInvert, 1));
    addPort("wr_mask",
            new HDLPort("byteena", "dataw/8", ProGe::BIT_VECTOR,
                        HDB::IN, noInvert, memoryTotalWidth()/8));
    addPort("clk",
            new HDLPort("clk", "1", ProGe::BIT, HDB::IN, noInvert, 1));
}

XilinxBlockRamGenerator::~XilinxBlockRamGenerator() {
}

bool
XilinxBlockRamGenerator::generatesComponentHdlFile() const {
    return true;
}

void
XilinxBlockRamGenerator::addMemory(
    const ProGe::NetlistBlock& ttaCore,
    ProGe::Netlist& netlist,
    int memIndex) {

    BlockPair blocks =
        createMemoryNetlistBlock(netlist, memIndex);
    ProGe::NetlistBlock* mem = blocks.first;
    ProGe::VirtualNetlistBlock* virt = blocks.second;
    assert(mem != NULL);
    assert(virt != NULL);

    for (int i = 0; i < portCount(); i++) {
        const HDLPort* hdlPort = port(i);
        ProGe::NetlistPort* memPort = mem->portByName(hdlPort->name());
        if (memPort == NULL) {
            memPort = virt->portByName(hdlPort->name());
            if (memPort == NULL) {
                TCEString msg = "Port ";
                msg << hdlPort->name() << " not found from netlist block";
                throw InvalidData(__FILE__, __LINE__, "MemoryGenerator", msg);
            }
        } 

        
        TCEString portName = corePortName(portKeyName(hdlPort));
        ProGe::NetlistPort* corePort = NULL;
        // clock and reset must be connected to new toplevel ports
        if (portName == platformIntegrator()->clockPort()->name()) {
            corePort = platformIntegrator()->clockPort();
        } else if (portName == platformIntegrator()->resetPort()->name()) {
            corePort = platformIntegrator()->resetPort();
        } else {
            if (connectToArbiter_) {
                portName = almaifPortName(portKeyName(hdlPort));
                corePort = almarviIF_->portByName(portName);
            } else {
                corePort = ttaCore.portByName(portName);
            }
        }
        if (corePort == NULL) {
            TCEString msg = "Port ";
            msg << portName << " not found from";
            if (connectToArbiter_) {
                msg << " Almarvi IF ";
            } else {
                msg << " TTA core ";
            }
            msg << "netlist block";
            throw InvalidData(__FILE__, __LINE__, "MemoryGenerator", msg);
        }

        connectPorts(
            netlist, *corePort, *memPort, hdlPort->needsInversion());
    }

    if (virt->portCount() > 0) {
        netlist.topLevelBlock().addSubBlock(virt);
    } else {
        delete virt;
    }
}

std::vector<TCEString>
XilinxBlockRamGenerator::generateComponentFile(TCEString outputPath) {
    TCEString inputFile = 
        templatePath() << FileSystem::DIRECTORY_SEPARATOR
                       << COMPONENT_FILE;
    TCEString outputFile;
    outputFile << outputPath << FileSystem::DIRECTORY_SEPARATOR  
               << moduleName() << ".vhdl";
    
    instantiateTemplate(inputFile, outputFile, ttaCoreName());
    std::vector<TCEString> files;
    files.push_back(outputFile);
    return files;
}


TCEString
XilinxBlockRamGenerator::moduleName() const {
    return ttaCoreName() + "_xilinx_blockram";
}
    

TCEString
XilinxBlockRamGenerator::instanceName(int memIndex) const {
    TCEString iname("onchip_mem_");
    return iname << signalPrefix_;
}

TCEString
XilinxBlockRamGenerator::almaifPortName(const TCEString& portBaseName) {
    // clock and reset port names are global
    if (portBaseName == platformIntegrator()->clockPort()->name() ||
        portBaseName == platformIntegrator()->resetPort()->name()) {
        return portBaseName;
    }

    TCEString portName = signalPrefix_;
    portName << "_" << portBaseName;
    return portName;

}

bool
XilinxBlockRamGenerator::isCompatible(const ProGe::NetlistBlock& ttaCore,
        std::vector<TCEString>& reasons) const {
    if (connectToArbiter_) {
        // TODO: Actually check almarviIF_ ports? 
        return true;
    } else {
        return MemoryGenerator::isCompatible(ttaCore, reasons);
    }
}