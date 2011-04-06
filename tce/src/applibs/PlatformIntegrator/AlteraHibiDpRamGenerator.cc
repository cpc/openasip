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
 * @file AlteraHibiDpRamGenerator.cc
 *
 * Implementation of AlteraHibiDpRamGenerator class.
 *
 * Hibi lsu needs a special dual port ram generator because both of the
 * memory ports are in the same FU.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/format.hpp>
#include "Exception.hh"
#include "StringTools.hh"
#include "FileSystem.hh"
#include "MemoryGenerator.hh"
#include "AlteraHibiDpRamGenerator.hh"
#include "Conversion.hh"
#include "HDLPort.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
using std::string;
using std::endl;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;

AlteraHibiDpRamGenerator::AlteraHibiDpRamGenerator(
        int memMauWidth,
        int widthInMaus,
        int addrWidth,
        std::string initFile,
        const PlatformIntegrator* integrator,
        std::ostream& warningStream,
        std::ostream& errorStream):
    AlteraMegawizMemGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                              integrator, warningStream, errorStream) {

    bool noInvert = false;
    // memory port 1
    addPort("dmem1_data_in",
            new HDLPort("q_a", Conversion::toString(memoryTotalWidth()),
                        ProGe::BIT_VECTOR, HDB::OUT, noInvert,
                        memoryTotalWidth()));
    addPort("dmem1_data_out",
            new HDLPort("data_a", Conversion::toString(memoryTotalWidth()),
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryTotalWidth()));
    addPort("dmem1_addr",
            new HDLPort("address_a", Conversion::toString(memoryAddrWidth()),
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryAddrWidth()));
    addPort("dmem1_mem_en",
            new HDLPort("enable_a", "1", ProGe::BIT, HDB::IN, noInvert, 1));
    addPort("dmem1_wr_en",
            new HDLPort("wren_a", "1", ProGe::BIT, HDB::IN, noInvert, 1));
    addPort("dmem1_wr_mask",
            new HDLPort("byteena_a",
                        Conversion::toString(memoryWidthInMaus()),
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryWidthInMaus()));
    addPort("clk",
            new HDLPort("clock_a", "1", ProGe::BIT, HDB::IN, noInvert, 1));

    // memory port 2
    addPort("dmem2_data_in",
            new HDLPort("q_b", Conversion::toString(memoryTotalWidth()),
                        ProGe::BIT_VECTOR, HDB::OUT, noInvert,
                        memoryTotalWidth()));
    addPort("dmem2_data_out",
            new HDLPort("data_b", Conversion::toString(memoryTotalWidth()),
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryTotalWidth()));
    addPort("dmem2_addr",
            new HDLPort("address_b", Conversion::toString(memoryAddrWidth()),
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryAddrWidth()));
    addPort("dmem2_mem_en",
            new HDLPort("enable_b", "1", ProGe::BIT, HDB::IN, noInvert, 1));
    addPort("dmem2_wr_en",
            new HDLPort("wren_b", "1", ProGe::BIT, HDB::IN, noInvert, 1));
    addPort("dmem2_wr_mask",
            new HDLPort("byteena_b",
                        Conversion::toString(memoryWidthInMaus()),
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryWidthInMaus()));
    addPort("clk",
            new HDLPort("clock_b", "1", ProGe::BIT, HDB::IN, noInvert, 1));
}


AlteraHibiDpRamGenerator::~AlteraHibiDpRamGenerator() {
}


void
AlteraHibiDpRamGenerator::addMemory(ProGe::Netlist& netlist) {
    
    const NetlistBlock& core = platformIntegrator()->ttaCoreBlock();
    NetlistBlock* mem =
        new NetlistBlock(moduleName(), instanceName(), netlist);
    netlist.topLevelBlock().addSubBlock(mem);

    for (int i = 0; i < portCount(); i++) {
        const HDLPort* hdlPort = port(i);
        NetlistPort* memPort = hdlPort->convertToNetlistPort(*mem);
        
        string corePortName = portKeyName(hdlPort);
        NetlistPort* corePort = NULL;
        // clock and reset must be connected to new toplevel ports
        if (corePortName == "clk") {
            corePort = netlist.topLevelBlock().portByName("clk");
        } else if (corePortName == "rstx") {
            corePort = netlist.topLevelBlock().portByName("rstx");
        } else {
            corePort = core.portByName(corePortName);
        }

        // address ports need special connection
        if (memPort->name().find("address_") != string::npos) {
            netlist.connectPorts(*memPort, *corePort, 0, 0,
                                 memoryAddrWidth());
        } else {
            if (memPort->dataType() == corePort->dataType()) {
                netlist.connectPorts(*memPort, *corePort);
            } else {
                // bit to bit vector connection, connect lowest bits
                netlist.connectPorts(*memPort, *corePort, 0, 0, 1);
            }
        }
    }
}

bool
AlteraHibiDpRamGenerator::generatesComponentHdlFile() const {
    return true;
}


std::vector<std::string>
AlteraHibiDpRamGenerator::generateComponentFile(std::string outputPath) {

    string outputFile = outputPath + FileSystem::DIRECTORY_SEPARATOR + 
        "altera_onchip_dp_ram_comp.vhd";

    return runMegawizard(outputFile);
}

std::string
AlteraHibiDpRamGenerator::createMemParameters() const {

    int addrWidth = memoryAddrWidth();
    int dataWidth = memoryTotalWidth();
    int bytemaskWidth = memoryWidthInMaus();
    // 2^addrWidth
    unsigned int sizeInWords = 1 << addrWidth;
    string initFile = initializationFile();
    string deviceFamily = platformIntegrator()->deviceFamily();

    std::ostringstream parameters;

    // variable parameters
    parameters
        << "WIDTH_A=" << dataWidth << " WIDTH_B=" << dataWidth
        << " WIDTHAD_A=" << addrWidth << " WIDTHAD_B=" << addrWidth
        << " NUMWORDS_A=" << sizeInWords << " NUMWORDS_B=" << sizeInWords
        << " WIDTH_BYTEENA_A=" << bytemaskWidth << " WIDTH_BYTEENA_B="
        << bytemaskWidth << " INIT_FILE=" << initFile << endl;
    
    if (!deviceFamily.empty()) {
        parameters
            << "INTENDED_DEVICE_FAMILY=\"" << deviceFamily << "\"" << endl;
    } else {
        parameters << "INTENDED_DEVICE_FAMILY=\"" << defaultDeviceFamily()
                   << "\"" << endl;
    }

    // fixed parameters
    parameters
        << "ADDRESS_ACLR_A=UNUSED ADDRESS_ACLR_B=UNUSED ADDRESS_REG_B=CLOCK1 "
        << "BYTEENA_ACLR_A=NONE BYTEENA_ACLR_B=NONE BYTEENA_REG_B=CLOCK1 "
        << "BYTE_SIZE=8 CLOCK_ENABLE_CORE_A=USE_INPUT_CLKEN "
        << "CLOCK_ENABLE_CORE_B=USE_INPUT_CLKEN CLOCK_ENABLE_INPUT_A=NORMAL "
        << "CLOCK_ENABLE_INPUT_B=NORMAL CLOCK_ENABLE_OUTPUT_A=UNUSED "
        << "CLOCK_ENABLE_OUTPUT_B=NORMAL ENABLE_RUNTIME_MOD=NO "
        << "IMPLEMENT_IN_LES=OFF INDATA_ACLR_A=NONE INDATA_ACLR_B=NONE "
        << "INDATA_REG_B=CLOCK1 INIT_FILE_LAYOUT=PORT_A LPM_TYPE=altsyncram "
        << "MAXIMUM_DEPTH=0 OPERATION_MODE=BIDIR_DUAL_PORT "
        << "OUTDATA_ACLR_A=NONE OUTDATA_ACLR_B=NONE "
        << "OUTDATA_REG_A=UNREGISTERED OUTDATA_REG_B=UNREGISTERED "
        << "POWER_UP_UNINITIALIZED=FALSE RAM_BLOCK_TYPE=AUTO "
        << "RDCONTROL_ACLR_B=NONE RDCONTROL_REG_B=UNUSED "
        << "RDEN_POWER_OPTIMIZATION=OFF "
        << "READ_DURING_WRITE_MODE_MIXED_PORTS=DONT_CARE "
        << "WRCONTROL_ACLR_A=UNUSED WRCONTROL_ACLR_B=NONE "
        << "WRCONTROL_WRADDRESS_REG_B=CLOCK1 " << "LOW_POWER_MODE=NONE"
        << endl;

    // port parameters
    parameters
        << "read_during_write_mode_port_a=unused "
        << "read_during_write_mode_port_b=unused aclr0=unused aclr1=unused "
        << "address_a=used address_b=used addressstall_a=unused "
        << "addressstall_b=unused byteena_a=used byteena_b=used clock0=used "
        << "clock1=used clocken0=used clocken1=used clocken2=unused "
        << "clocken3=unused data_a=used data_b=used eccstatus=unused "
        << "q_a=used q_b=used rden_a=unused rden_b=unused wren_a=used "
        << "wren_b=used" << endl;

    return parameters.str();
}


std::string
AlteraHibiDpRamGenerator::moduleName() const {

    return "altera_onchip_dp_ram_comp";
}
    
std::string
AlteraHibiDpRamGenerator::instanceName() const {

    return "onchip_dp_dmem";
}
