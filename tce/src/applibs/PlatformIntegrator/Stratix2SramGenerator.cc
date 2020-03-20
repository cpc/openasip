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
 * @file Stratix2SramGenerator.cc
 *
 * Implementation of Stratix2SramGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <vector>
#include "StringTools.hh"
#include "PlatformIntegrator.hh"
#include "MemoryGenerator.hh"
#include "Stratix2SramGenerator.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "HDLPort.hh"
using std::vector;
using std::endl;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;

Stratix2SramGenerator::Stratix2SramGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    TCEString initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream) :
    MemoryGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                    integrator, warningStream, errorStream) {

    ProGe::Netlist::Parameter dataw = {"sram_dataw", "integer", "32"};
    ProGe::Netlist::Parameter addrw = {"sram_addrw", "integer", "18"};
    addParameter(dataw);
    addParameter(addrw);
    addPort("STRATIXII_SRAM_DQ",
            new HDLPort("STRATIXII_SRAM_DQ", "sram_dataw", ProGe::BIT_VECTOR,
                        HDB::BIDIR, false, 32));
    addPort("STRATIXII_SRAM_ADDR",
            new HDLPort("STRATIXII_SRAM_ADDR", "sram_addrw",
                        ProGe::BIT_VECTOR, HDB::OUT, false, 18));
    addPort("STRATIXII_SRAM_WE_N",
            new HDLPort("STRATIXII_SRAM_WE_N", "1", ProGe::BIT_VECTOR,
                        HDB::OUT, false, 1));
    addPort("STRATIXII_SRAM_OE_N",
            new HDLPort("STRATIXII_SRAM_OE_N", "1", ProGe::BIT_VECTOR,
                        HDB::OUT, false, 1));
    addPort("STRATIXII_SRAM_CS_N",
            new HDLPort("STRATIXII_SRAM_CS_N", "1", ProGe::BIT_VECTOR,
                        HDB::OUT, false, 1));
    addPort("STRATIXII_SRAM_BE_N0",
            new HDLPort("STRATIXII_SRAM_BE_N0", "1", ProGe::BIT_VECTOR,
                        HDB::OUT, false, 1));
    addPort("STRATIXII_SRAM_BE_N1",
            new HDLPort("STRATIXII_SRAM_BE_N1", "1", ProGe::BIT_VECTOR,
                        HDB::OUT, false, 1));
    addPort("STRATIXII_SRAM_BE_N2",
            new HDLPort("STRATIXII_SRAM_BE_N2", "1", ProGe::BIT_VECTOR,
                        HDB::OUT, false, 1));
    addPort("STRATIXII_SRAM_BE_N3",
            new HDLPort("STRATIXII_SRAM_BE_N3", "1", ProGe::BIT_VECTOR,
                        HDB::OUT, false, 1));
}


Stratix2SramGenerator::~Stratix2SramGenerator() {
}


void
Stratix2SramGenerator::addMemory(
    const ProGe::NetlistBlock& ttaCore,
    ProGe::Netlist& netlist,
    int /*memIndex*/) {
    
    NetlistBlock& toplevel = netlist.topLevelBlock();
    for (int i = 0; i < parameterCount(); i++) {
        toplevel.setParameter(parameter(i).name, parameter(i).type,
                              parameter(i).value);
    }

    for (int i = 0; i < portCount(); i++) {
        const HDLPort* hdlPort = port(i);
        NetlistPort* memPort = hdlPort->convertToNetlistPort(toplevel);

        TCEString corePortName = portKeyName(hdlPort);
        NetlistPort* corePort = ttaCore.portByName(corePortName);
        assert(corePort != NULL);

        MemoryGenerator::connectPorts(
            netlist, *memPort, *corePort, hdlPort->needsInversion());
    }

}

bool
Stratix2SramGenerator::generatesComponentHdlFile() const {

    // Sram controller is integrated into the lsu itself
    // No need to generate separate file
    return false;
}

std::vector<TCEString>
Stratix2SramGenerator::generateComponentFile(TCEString) {

    vector<TCEString> noFileToGenerate;
    return noFileToGenerate;
}


TCEString
Stratix2SramGenerator::moduleName() const {

    return "stratixII_sram_comp";
}
    

TCEString
Stratix2SramGenerator::instanceName(int memIndex) const {
    
    TCEString iname("stratixII_sram_");
    return iname << memoryIndexString(memIndex);
}
