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
#include <vector>
#include "PlatformIntegrator.hh"
#include "AlteraHibiDpRamGenerator.hh"
#include "HDLPort.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "FUExternalPort.hh"
#include "Conversion.hh"
using std::endl;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;

const TCEString AlteraHibiDpRamGenerator::COMPONENT_FILE = 
    "altera_onchip_dp_ram_comp.vhd";

AlteraHibiDpRamGenerator::AlteraHibiDpRamGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    TCEString initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream):
    AlteraMemGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                       integrator, warningStream, errorStream) {

    bool noInvert = false;
    TCEString byteEnableWidth;
    byteEnableWidth << DATAW_G << "/8";
    // memory port 1
    addPort("dmem1_data_in",
            new HDLPort("q_a", DATAW_G, ProGe::BIT_VECTOR, HDB::OUT, noInvert,
                        memoryTotalWidth()));
    addPort("dmem1_data_out",
            new HDLPort("data_a", DATAW_G, ProGe::BIT_VECTOR, HDB::IN,
                        noInvert, memoryTotalWidth()));
    addPort("dmem1_addr",
            new HDLPort("address_a", ADDRW_G, ProGe::BIT_VECTOR, HDB::IN,
                        noInvert, memoryAddrWidth()));
    addPort("dmem1_mem_en",
            new HDLPort("enable_a", "1", ProGe::BIT, HDB::IN, noInvert, 1));
    addPort("dmem1_wr_en",
            new HDLPort("wren_a", "1", ProGe::BIT, HDB::IN, noInvert, 1));
    addPort("dmem1_wr_mask",
            new HDLPort("byteena_a", byteEnableWidth, ProGe::BIT_VECTOR,
                        HDB::IN, noInvert, memoryWidthInMaus()));
    addPort("clk",
            new HDLPort("clock_a", "1", ProGe::BIT, HDB::IN, noInvert, 1));

    // memory port 2
    addPort("dmem2_data_in",
            new HDLPort("q_b", DATAW_G, ProGe::BIT_VECTOR, HDB::OUT, noInvert,
                        memoryTotalWidth()));
    addPort("dmem2_data_out",
            new HDLPort("data_b",DATAW_G, ProGe::BIT_VECTOR, HDB::IN,
                        noInvert, memoryTotalWidth()));
    addPort("dmem2_addr",
            new HDLPort("address_b", ADDRW_G, ProGe::BIT_VECTOR, HDB::IN,
                        noInvert, memoryAddrWidth())); 
    addPort("dmem2_mem_en",
            new HDLPort("enable_b", "1", ProGe::BIT, HDB::IN, noInvert, 1));
    addPort("dmem2_wr_en",
            new HDLPort("wren_b", "1", ProGe::BIT, HDB::IN, noInvert, 1));
    addPort("dmem2_wr_mask",
            new HDLPort("byteena_b", byteEnableWidth, ProGe::BIT_VECTOR,
                        HDB::IN, noInvert, memoryWidthInMaus()));
    addPort("clk",
            new HDLPort("clock_b", "1", ProGe::BIT, HDB::IN, noInvert, 1));
}


AlteraHibiDpRamGenerator::~AlteraHibiDpRamGenerator() {
}


bool
AlteraHibiDpRamGenerator::checkFuPort(
    const HDB::FUExternalPort& fuPort,
    std::vector<TCEString>& reasons) const {
    
    // skip the hibi ports
    TCEString hibiSignal = "hibi_";
    TCEString portName = fuPort.name();
    if (portName.find(hibiSignal) != TCEString::npos) {
        return true;
    }
    return MemoryGenerator::checkFuPort(fuPort, reasons);
}


void
AlteraHibiDpRamGenerator::connectPorts(
    ProGe::Netlist& netlist,
    ProGe::NetlistPort& memPort,
    ProGe::NetlistPort& corePort,
    bool inverted) {
    
    // address ports need special connection
    if (memPort.name().find("address_") != TCEString::npos) {
        netlist.connectPorts(memPort, corePort, 0, 0, memoryAddrWidth());
    } else {
        MemoryGenerator::connectPorts(
            netlist, memPort, corePort, inverted);
    }
}

bool
AlteraHibiDpRamGenerator::generatesComponentHdlFile() const {
    return true;
}


std::vector<TCEString>
AlteraHibiDpRamGenerator::generateComponentFile(TCEString outputPath) {

    return instantiateAlteraTemplate(COMPONENT_FILE, outputPath);
}


TCEString
AlteraHibiDpRamGenerator::moduleName() const {

    return ttaCoreName() + "_altera_onchip_dp_ram_comp";
}

    
TCEString
AlteraHibiDpRamGenerator::instanceName(int memIndex) const {

    TCEString iname("onchip_dp_dmem_");
    return iname << memoryIndexString(memIndex);
}
