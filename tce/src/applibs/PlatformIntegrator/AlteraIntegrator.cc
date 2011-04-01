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
using std::string;

AlteraIntegrator::AlteraIntegrator(): PlatformIntegrator() {
}


AlteraIntegrator::AlteraIntegrator(
    ProGe::HDL hdl,
    std::string progeOutputDir,
    std::string entityName,
    std::string outputDir,
    std::string programName,
    int targetClockFreq,
    std::ostream& warningStream,
    std::ostream& errorStream,
    const MemInfo& imem,
    const MemInfo& dmem):
    PlatformIntegrator(hdl, progeOutputDir, entityName, outputDir,
                       programName, targetClockFreq, warningStream,
                       errorStream, imem, dmem) {
}


AlteraIntegrator::~AlteraIntegrator() {
}


void
AlteraIntegrator::integrateProcessor(const ProGe::NetlistBlock* ttaCore) {

    if (!createPorts(ttaCore)) {
        return;
    }

    if (!createMemories()) {
        return;
    }

    writeNewToplevel();
    
    addProGeFiles();

    projectFileGenerator()->writeProjectFiles();
}


MemoryGenerator*
AlteraIntegrator::imemInstance() {

    assert(imemInfo().type != UNKNOWN && "Imem type not set!");
    
    const MemInfo& imem = imemInfo();
    MemoryGenerator* imemGen = NULL;
    if (imem.type == ONCHIP) {
        string initFile = programName() + ".mif";
        imemGen = 
            new AlteraOnchipRomGenerator(
                imem.mauWidth, imem.widthInMaus, imem.portAddrw, initFile, this,
                warningStream(), errorStream());
        projectFileGenerator()->addMemInitFile(initFile);
    } else if (imem.type == VHDL_ARRAY) {
        string initFile = programName() + "_imem_pkg.vhdl";
        imemGen = new VhdlRomGenerator(
            imem.mauWidth, imem.widthInMaus, imem.portAddrw, initFile, this,
            warningStream(), errorStream());
    } else {
        string msg = "Unsupported instruction memory type";
        InvalidData exc(__FILE__, __LINE__, "AlteraIntegrator",
                        msg);
        throw exc;
    }
    return imemGen;
}


MemoryGenerator*
AlteraIntegrator::dmemInstance() {

    const MemInfo& dmem = dmemInfo();
    MemoryGenerator* dmemGen = NULL;
    if (dmem.type == ONCHIP) {
        string initFile = programName() + "_" + dmem.asName + ".mif";
        // onchip mem size is scalable, use value from adf's Address Space
        int addrw = dmem.asAddrw;
        dmemGen =
            new AlteraOnchipRamGenerator(
                dmem.mauWidth, dmem.widthInMaus, addrw, initFile,
                this, warningStream(), errorStream());
        projectFileGenerator()->addMemInitFile(initFile); 
    } else {
        string msg = "Unsupported data memory type";
        InvalidData exc(__FILE__, __LINE__, "AlteraIntegrator",
                        msg);
        throw exc;
    }
    return dmemGen;
}

bool
AlteraIntegrator::isDataMemorySignal(const std::string& signalName) const {

    bool isDmemSignal = false;
    if (dmemInfo().type == NONE) {
        isDmemSignal = false;
    } else if (dmemInfo().type == ONCHIP) {
        isDmemSignal = signalName.find("dmem") != string::npos;
    } else {
        isDmemSignal = false;
    }
    return isDmemSignal;
}
