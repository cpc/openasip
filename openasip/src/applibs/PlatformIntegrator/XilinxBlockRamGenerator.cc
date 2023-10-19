/*
    Copyright (c) 2016 Tampere University.

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
 #include "Parameter.hh"
using std::endl;

const TCEString XilinxBlockRamGenerator::SP_FILE = "xilinx_blockram.vhdl";
const TCEString XilinxBlockRamGenerator::DP_FILE = "xilinx_dp_blockram.vhdl";
const TCEString XilinxBlockRamGenerator::INTEL_SP_DW_FILE =
    "intel_dw_blockram.vhdl";
const TCEString XilinxBlockRamGenerator::INTEL_DP_FILE =
    "intel_dp_blockram.vhdl";

XilinxBlockRamGenerator::XilinxBlockRamGenerator(
    int memMauWidth, int widthInMaus, int addrWidth, int portBDataWidth,
    int portBAddrWidth, const PlatformIntegrator* integrator,
    std::ostream& warningStream, std::ostream& errorStream,
    bool connectToArbiter, ProGe::NetlistBlock* almaifBlock,
    TCEString signalPrefix, bool overrideAddrWidth, bool singleMemoryBlock,
    bool intelCompatible, bool pseudoDualPort)
    : MemoryGenerator(
          memMauWidth, widthInMaus, addrWidth, "", integrator, warningStream,
          errorStream),
      connectToArbiter_(connectToArbiter),
      almaifBlock_(almaifBlock),
      signalPrefix_(signalPrefix),
      overrideAddrWidth_(overrideAddrWidth),
      singleMemoryBlock_(singleMemoryBlock),
      intelCompatible_(intelCompatible),
      pseudoDualPort_(pseudoDualPort) {
    assert (!connectToArbiter || almaifBlock != nullptr);

    ProGe::Parameter dataw = {"dataw_g", "integer",
        Conversion::toString(memoryTotalWidth())};
    ProGe::Parameter addrw = {"addrw_g", "integer",
        Conversion::toString(memoryAddrWidth())};


    ProGe::Parameter second_dataw = {"dataw_b_g", "integer",
        Conversion::toString(portBDataWidth)};
    ProGe::Parameter second_addrw = {"addrw_b_g", "integer",
        Conversion::toString(portBAddrWidth)};

    if (overrideAddrWidth) {
        addrw.setValue("local_mem_addrw_g");
        second_addrw.setValue("local_mem_addrw_g");
    }

    addParameter(dataw);
    addParameter(addrw);

    if (connectToArbiter_) {
        addParameter(second_dataw);
        addParameter(second_addrw);
    }

    addPort("clk", new HDLPort("clk", "1", ProGe::BIT, ProGe::IN, false, 1));
    addPort("rstx", new HDLPort("rstx", "1", ProGe::BIT, ProGe::IN, false, 1));
    if (connectToArbiter) {
        addPorts("a_", memoryAddrWidth(), memoryTotalWidth());
        addPorts("b_", portBAddrWidth, portBDataWidth);
    } else {
        addPorts("", memoryAddrWidth(), memoryTotalWidth());
    }
}

XilinxBlockRamGenerator::~XilinxBlockRamGenerator() {
}

bool
XilinxBlockRamGenerator::generatesComponentHdlFile() const {
    return true;
}

void
XilinxBlockRamGenerator::addPorts(std::string pfx, int addrWidth,
                                  int dataWidth) {
    std::string datawGeneric = "dataw_g";
    std::string addrwGeneric = "addrw_g";
    if (pfx == "b_") {
        datawGeneric = "dataw_b_g";
        addrwGeneric = "addrw_b_g";
    }
    const ProGe::DataType BIT = ProGe::BIT;
    const ProGe::DataType VEC = ProGe::BIT_VECTOR;
    const ProGe::Direction IN  = ProGe::IN;
    const ProGe::Direction OUT = ProGe::OUT;
    const bool noInvert = false;
    addPort(pfx + "avalid_out",
            new HDLPort(pfx + "avalid_in", "1", BIT, IN, noInvert, 1));
    addPort(pfx + "aready_in",
            new HDLPort(pfx + "aready_out", "1", BIT, OUT, noInvert, 1));
    if (!overrideAddrWidth_) {
        addPort(pfx + "aaddr_out",
                new HDLPort(pfx + "aaddr_in", addrwGeneric, VEC, IN, noInvert,
                            addrWidth));
    } else {
        addPort(pfx + "aaddr_out",
                new HDLPort(pfx + "aaddr_in", addrwGeneric, VEC, IN, noInvert));
    }
    addPort(pfx + "awren_out",
            new HDLPort(pfx + "awren_in", "1", BIT, IN, noInvert, 1));
    addPort(pfx + "astrb_out",
            new HDLPort(pfx + "astrb_in", "(" + datawGeneric + "+7)/8", VEC, IN,
                        noInvert, (dataWidth+7)/8));
    addPort(pfx + "adata_out",
            new HDLPort(pfx + "adata_in", datawGeneric, VEC, IN, noInvert,
                        dataWidth));
    addPort(pfx + "rvalid_in",
            new HDLPort(pfx + "rvalid_out", "1", BIT, ProGe::OUT, noInvert, 1));
    addPort(pfx + "rready_out",
            new HDLPort(pfx + "rready_in", "1", BIT, IN, noInvert, 1));
    addPort(pfx + "rdata_in",
            new HDLPort(pfx + "rdata_out", datawGeneric, VEC, OUT, noInvert,
                        dataWidth));
}

void
XilinxBlockRamGenerator::addMemory(
    const ProGe::NetlistBlock& ttaCore,
    ProGe::NetlistBlock& integratorBlock,
    int memIndex, int coreId) {

    // Do not instantiate multiple physical memories for shared AS when shared
    if (coreId > 0 && connectToArbiter_ && singleMemoryBlock_) {
        return;
    }
    BlockPair blocks =
        createMemoryNetlistBlock(integratorBlock, memIndex, coreId);
    ProGe::NetlistBlock* mem = blocks.first;
    ProGe::VirtualNetlistBlock* virt = blocks.second;
    assert(mem != NULL);
    assert(virt != NULL);

    if (virt->portCount() > 0) {
        integratorBlock.addSubBlock(virt);
    }

    for (int i = 0; i < portCount(); i++) {
        const HDLPort* hdlPort = port(i);
        ProGe::NetlistPort* memPort = mem->port(hdlPort->name());
        if (memPort == NULL) {
            memPort = virt->port(hdlPort->name());
            if (memPort == NULL) {
                TCEString msg = "Port ";
                msg << hdlPort->name() << " not found from netlist block";
                throw InvalidData(__FILE__, __LINE__, "MemoryGenerator", msg);
            }
        }


        TCEString portName = corePortName(portKeyName(hdlPort), coreId);
        const ProGe::NetlistPort* corePort = NULL;
        // clock and reset must be connected to new toplevel ports
        if (portName == platformIntegrator()->clockPort()->name()) {
            corePort = platformIntegrator()->clockPort();
        } else if (portName == platformIntegrator()->resetPort()->name()) {
            corePort = platformIntegrator()->resetPort();
        } else {
            if (connectToArbiter_) {
                portName = almaifPortName(portKeyName(hdlPort));
                corePort = almaifBlock_->port(portName);
            } else {
                corePort = ttaCore.port(portName);
            }
        }
        if (corePort == NULL) {
            TCEString msg = "Port ";
            msg << portName << " not found from";
            if (connectToArbiter_) {
                msg << " AlmaIF ";
            } else {
                msg << " TTA core ";
            }
            msg << "netlist block";
            throw InvalidData(__FILE__, __LINE__, "MemoryGenerator", msg);
        }

        if (connectToArbiter_ && !overrideAddrWidth_ &&
            corePort->realWidth() != memPort->realWidth()) {
            // Assume a private memory, split wide integrator signals accross
            // multiple cores
            int portWidth = memPort->realWidth();
            // This assumes no ports are inverted
            integratorBlock.netlist().connect(*memPort, *corePort,
                                              0, portWidth*coreId,
                                              portWidth);
        } else {
            connectPorts(
                integratorBlock, *corePort, *memPort,
                hdlPort->needsInversion(), coreId);
        }
    }
}

std::vector<TCEString>
XilinxBlockRamGenerator::generateComponentFile(TCEString outputPath) {
    TCEString componentFileName = "";
    if (connectToArbiter_) {
        if (intelCompatible_) {
            if (pseudoDualPort_) {
                componentFileName = INTEL_SP_DW_FILE;
            } else {
                componentFileName = INTEL_DP_FILE;
            }
        } else {
            componentFileName = DP_FILE;
        }
    } else {
        componentFileName = SP_FILE;
    }

    TCEString inputFile = templatePath() << FileSystem::DIRECTORY_SEPARATOR
                                         << componentFileName;
    TCEString outputFile;
    outputFile << outputPath << FileSystem::DIRECTORY_SEPARATOR
               << moduleName() << ".vhdl";

    FileSystem::copy(inputFile, outputPath);
    std::vector<TCEString> files;
    files.push_back(outputFile);
    return files;
}


TCEString
XilinxBlockRamGenerator::moduleName() const {
    TCEString name;
    if (connectToArbiter_) {
        if (intelCompatible_) {
            if (pseudoDualPort_) {
                name = "intel_dw_";
            } else {
                name = "intel_dp_";
            }
        } else {
            name = "xilinx_dp_";
        }
    } else {
        // single-port xilinx_blockram.vhdl is also intel compatible
        name = "xilinx_";
    }

    name += "blockram";
    return name;
}


TCEString
XilinxBlockRamGenerator::instanceName(int coreId, int) const {
    TCEString iname = "onchip_mem_";

    if (coreId != -1) {
        iname << "core" << coreId << "_";
    }
    return iname << signalPrefix_;
}

TCEString
XilinxBlockRamGenerator::almaifPortName(const TCEString& portBaseName) {
    // clock and reset port names are global
    if (portBaseName == platformIntegrator()->clockPort()->name() ||
        portBaseName == platformIntegrator()->resetPort()->name()) {
        return portBaseName;
    }

    return signalPrefix_ + "_" + portBaseName;
}

bool
XilinxBlockRamGenerator::isCompatible(const ProGe::NetlistBlock& ttaCore,
        int coreId, std::vector<TCEString>& reasons) const {
    if (connectToArbiter_) {
        // TODO: Actually check almaifBlock_ ports?
        return true;
    } else {
        return MemoryGenerator::isCompatible(ttaCore, coreId, reasons);
    }
}
