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
 * @file AlteraIntegrator.cc
 *
 * Implementation of AlteraIntegrator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include "AlteraIntegrator.hh"
#include "AlteraOnchipRamGenerator.hh"
#include "AlteraOnchipRomGenerator.hh"
#include "VhdlRomGenerator.hh"
#include "ProjectFileGenerator.hh"
#include "Machine.hh"
#include "NetlistPort.hh"
#include "NetlistBlock.hh"
using ProGe::NetlistBlock;
using ProGe::NetlistPort;

AlteraIntegrator::AlteraIntegrator(): PlatformIntegrator() {
}


AlteraIntegrator::AlteraIntegrator(
    const TTAMachine::Machine* machine,
    const IDF::MachineImplementation* idf,
    ProGe::HDL hdl,
    TCEString progeOutputDir,
    TCEString coreEntityName,
    TCEString outputDir,
    TCEString programName,
    int targetClockFreq,
    std::ostream& warningStream,
    std::ostream& errorStream,
    const MemInfo& imem,
    MemType dmemType):
    PlatformIntegrator(machine, idf, hdl, progeOutputDir, coreEntityName,
                       outputDir, programName, targetClockFreq, warningStream,
                       errorStream, imem, dmemType),
    imemGen_(NULL), dmemGen_() {
}


AlteraIntegrator::~AlteraIntegrator() {

    if (imemGen_ != NULL) {
        delete imemGen_;
    }
    if (!dmemGen_.empty()) {
        std::map<TCEString, MemoryGenerator*>::iterator iter =
            dmemGen_.begin();
        while (iter != dmemGen_.end()) {
            delete iter->second;
            iter++;
        }
    }
}


void
AlteraIntegrator::integrateProcessor(
    const ProGe::NetlistBlock* progeBlockInOldNetlist) {

    initPlatformNetlist(progeBlockInOldNetlist);

    const NetlistBlock& core = progeBlock();
    if (!integrateCore(core)) {
        return;
    }

    writeNewToplevel();
    
    addProGeFiles();

    projectFileGenerator()->writeProjectFiles();
}


MemoryGenerator&
AlteraIntegrator::imemInstance(MemInfo imem) {

    assert(imem.type != UNKNOWN && "Imem type not set!");
    
    if (imemGen_ == NULL) {
        if (imem.type == ONCHIP) {
            TCEString initFile = programName() + ".mif";
            imemGen_ = 
                new AlteraOnchipRomGenerator(
                    imem.mauWidth, imem.widthInMaus, imem.portAddrw, initFile,
                    this, warningStream(), errorStream());
            projectFileGenerator()->addMemInitFile(initFile);
        } else if (imem.type == VHDL_ARRAY) {
            TCEString initFile = programName() + "_imem_pkg.vhdl";
            imemGen_ = new VhdlRomGenerator(
                imem.mauWidth, imem.widthInMaus, imem.portAddrw, initFile,
                this, warningStream(), errorStream());
        } else {
            TCEString msg = "Unsupported instruction memory type";
            throw InvalidData(__FILE__, __LINE__, "AlteraIntegrator", msg);
        }
    }
    return *imemGen_;
}


MemoryGenerator&
AlteraIntegrator::dmemInstance(
    MemInfo dmem,
    TTAMachine::FunctionUnit& lsuArch,
    HDB::FUImplementation& lsuImplementation) {

    MemoryGenerator* memGen = NULL;
    if (dmemGen_.find(dmem.asName) != dmemGen_.end()) {
        memGen = dmemGen_.find(dmem.asName)->second;
    } else {
        if (dmem.type == ONCHIP) {
            TCEString initFile = programName() + "_" + dmem.asName + ".mif";
            // onchip mem size is scalable, use value from adf's Address Space
            int addrw = dmem.asAddrw;
            memGen =
                new AlteraOnchipRamGenerator(
                    dmem.mauWidth, dmem.widthInMaus, addrw, initFile,
                    this, warningStream(), errorStream());
            projectFileGenerator()->addMemInitFile(initFile); 
        } else {
            TCEString msg = "Unsupported data memory type";
            throw InvalidData(__FILE__, __LINE__, "AlteraIntegrator", msg);
        }
        memGen->addLsu(lsuArch, lsuImplementation);
        dmemGen_[dmem.asName] = memGen;
    }
    return *memGen;
}
