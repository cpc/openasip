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
 * @file AlteraOnchipRomGenerator.cc
 *
 * Implementation of AlteraOnchipRomGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <vector>
#include "AlteraOnchipRomGenerator.hh"
#include "PlatformIntegrator.hh"
#include "HDLPort.hh"
using std::endl;
using ProGe::StaticSignal;

const TCEString AlteraOnchipRomGenerator::COMPONENT_FILE =
    "altera_onchip_rom_comp.vhd";

AlteraOnchipRomGenerator::AlteraOnchipRomGenerator(
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
    bool inverted = true;

    addPort("clk",
        new HDLPort("clock", "1", ProGe::BIT, ProGe::IN, noInvert, 1));
    addPort("imem_addr",
        new HDLPort("address", "IMEMADDRWIDTH", ProGe::BIT_VECTOR,
            ProGe::IN, noInvert));
    addPort("imem_en_x",
        new HDLPort("clken", "1", ProGe::BIT, ProGe::IN, inverted, 1));
    addPort("imem_data",
        new HDLPort("q", "IMEMWIDTHINMAUS*IMEMMAUWIDTH",
            ProGe::BIT_VECTOR, ProGe::OUT, noInvert));

    // these signals are not driven by the imem component, connect to zero
    HDLPort* busyToGnd =
        new HDLPort("wait", "1", ProGe::BIT, ProGe::OUT, noInvert, 1);
    busyToGnd->setToStatic(ProGe::StaticSignal::GND);
    addPort("busy", busyToGnd);
}


AlteraOnchipRomGenerator::~AlteraOnchipRomGenerator() {
}

bool
AlteraOnchipRomGenerator::generatesComponentHdlFile() const {
    return true;
}


std::vector<TCEString>
AlteraOnchipRomGenerator::generateComponentFile(TCEString outputPath) {

    return instantiateAlteraTemplate(COMPONENT_FILE, outputPath);
}

void
AlteraOnchipRomGenerator::addMemory(
    const ProGe::NetlistBlock& ttaCore,
    ProGe::NetlistBlock& integratorBlock,
    int memIndex,
    int coreId) {

    // Add generics as string constants!
    TCEString addrwGeneric = "IMEMADDRWIDTH";
    TCEString datawGeneric = "IMEMWIDTHINMAUS*IMEMMAUWIDTH";
    addGenerics(integratorBlock, addrwGeneric, datawGeneric, memIndex);

    MemoryGenerator::addMemory(ttaCore, integratorBlock, memIndex, coreId);
}

TCEString
AlteraOnchipRomGenerator::moduleName() const {

    return ttaCoreName() + "_altera_onchip_rom_comp";
}
    

TCEString
AlteraOnchipRomGenerator::instanceName(int coreId, int memIndex) const {

    TCEString iname("onchip_imem_");
    return iname << memoryIndexString(coreId, memIndex);
}
