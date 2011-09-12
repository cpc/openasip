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
 * @file AlteraOnchipRamGenerator.cc
 *
 * Implementation of AlteraOnchipRamGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <vector>
#include "AlteraOnchipRamGenerator.hh"
#include "Netlist.hh"
#include "HDLPort.hh"
using std::endl;

const TCEString AlteraOnchipRamGenerator::COMPONENT_FILE = 
    "altera_onchip_ram_comp.vhd";

AlteraOnchipRamGenerator::AlteraOnchipRamGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    TCEString initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream): 
    AlteraMemGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                       integrator, warningStream, errorStream) {
    
    bool inverted = true;
    bool noInvert = false;
    TCEString byteEnableWidth;
    byteEnableWidth << DATAW_G << "/8";
    addPort("dmem_data_in", 
            new HDLPort("q", DATAW_G,
                        ProGe::BIT_VECTOR, HDB::OUT, noInvert,
                        memoryTotalWidth()));
    addPort("dmem_data_out",
            new HDLPort("data", DATAW_G,
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryTotalWidth()));
    addPort("dmem_addr",
            new HDLPort("address", ADDRW_G,
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryAddrWidth()));
    addPort("dmem_mem_en_x",
            new HDLPort("clken", "1", ProGe::BIT, HDB::IN, inverted, 1));
    addPort("dmem_wr_en_x",
            new HDLPort("wren", "1", ProGe::BIT, HDB::IN, inverted, 1));
    addPort("dmem_bytemask",
            new HDLPort("byteena", byteEnableWidth, ProGe::BIT_VECTOR,
                        HDB::IN, noInvert, memoryWidthInMaus()));
    addPort("clk",
            new HDLPort("clock", "1", ProGe::BIT, HDB::IN, noInvert, 1));
}

AlteraOnchipRamGenerator::~AlteraOnchipRamGenerator() {
}

bool
AlteraOnchipRamGenerator::generatesComponentHdlFile() const {
    return true;
}


std::vector<TCEString>
AlteraOnchipRamGenerator::generateComponentFile(TCEString outputPath) {

    return instantiateAlteraTemplate(COMPONENT_FILE, outputPath);
}


TCEString
AlteraOnchipRamGenerator::moduleName() const {

    return ttaCoreName() + "_altera_onchip_ram_comp";
}
    

TCEString
AlteraOnchipRamGenerator::instanceName(int memIndex) const {
    
    TCEString iname("onchip_dmem_");
    return iname << memoryIndexString(memIndex);
}

