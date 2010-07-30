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
 * @file KoskiIntegrator.cc
 *
 * Implementation of KoskiIntegrator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include "Exception.hh"
#include "PlatformIntegrator.hh"
#include "KoskiIntegrator.hh"
#include "MemoryGenerator.hh"
#include "AlteraOnchipRomGenerator.hh"
#include "AlteraHibiDpRamGenerator.hh"
#include "VhdlRomGenerator.hh"
#include "StringTools.hh"
#include "NetlistBlock.hh"
#include "IPXactFileGenerator.hh"
#include "FileSystem.hh"
using std::string;
using std::vector;
using std::endl;

const std::string KoskiIntegrator::PIN_TAG_ = "hibi";

KoskiIntegrator::KoskiIntegrator(): PlatformIntegrator(), ipXactGen_(NULL) {
}


KoskiIntegrator::KoskiIntegrator(
    ProGe::HDL hdl,
    std::string progeOutputDir,
    std::string entityName,
    std::string outputDir,
    std::string programName,
    int targetClockFreq,
    std::ostream& warningStream,
    std::ostream& errorStream):
    PlatformIntegrator(hdl, progeOutputDir, entityName, outputDir,
                       programName, targetClockFreq, warningStream,
                       errorStream),
    ipXactGen_(new IPXactFileGenerator(entityName, this)) {
}


KoskiIntegrator::~KoskiIntegrator() {

    if (ipXactGen_ != NULL) {
        delete ipXactGen_;
    }
}


void
KoskiIntegrator::integrateProcessor(
    const ProGe::NetlistBlock* ttaCore, 
    MemInfo& imem,
    MemInfo& dmem) {

    if(!createPorts(ttaCore)) {
        return;
    }

    if (!createMemories(imem, dmem)) {
        return;
    }

    writeNewToplevel();
    
    addProGeFiles();
    ipXactGen_->writeProjectFiles();
}


std::string
KoskiIntegrator::pinTag() const {

    return PIN_TAG_;
}

   
bool
KoskiIntegrator::isDataMemorySignal(const std::string& signalName) const {

    return signalName.find("dmem") != string::npos;
}


ProjectFileGenerator*
KoskiIntegrator::projectFileGenerator() const {

    return ipXactGen_;
}



MemoryGenerator*
KoskiIntegrator::imemInstance(const MemInfo& imem) {

    MemoryGenerator* imemGen = NULL;
    if (imem.type == ONCHIP) {
        string initFile = programName() + ".mif";
        imemGen = 
            new AlteraOnchipRomGenerator(
                imem.mauWidth, imem.widthInMaus, imem.portAddrw, initFile, this,
                warningStream(), errorStream());
        ipXactGen_->addMemInitFile(initFile);
    } else if (imem.type == VHDL_ARRAY) {
        string initFile = programName() + "_imem_pkg.vhdl";
        imemGen = new VhdlRomGenerator(
            imem.mauWidth, imem.widthInMaus, imem.portAddrw, initFile, this,
            warningStream(), errorStream());
    } else {
        string msg = "Unsupported instruction memory type";
        InvalidData exc(__FILE__, __LINE__, "KoskiIntegrator",
                        msg);
        throw exc;
    }
    return imemGen;
}


MemoryGenerator*
KoskiIntegrator::dmemInstance(const MemInfo& dmem) {

    MemoryGenerator* dmemGen = NULL;
    if (dmem.type == ONCHIP) {
        string initFile = programName() + "_" + dmem.asName + ".mif";
        // onchip mem size is scalable, use value from adf's Address Space
        int addrw = dmem.asAddrw;
        dmemGen =
            new AlteraHibiDpRamGenerator(
                dmem.mauWidth, dmem.widthInMaus, addrw, initFile,
                this, warningStream(), errorStream());
        ipXactGen_->addMemInitFile(initFile);
    } else {
        string msg = "Unsupported data memory type";
        InvalidData exc(__FILE__, __LINE__, "KoskiIntegrator",
                        msg);
        throw exc;
    }
    return dmemGen;
}


void
KoskiIntegrator::printInfo(std::ostream& stream) const {

    stream
        << "Integrator name: KoskiIntegrator" << endl
        << "---------------------------------" << endl
        << "Integrates TTA core to Koski flow compatible IP-block. "
        << "Processor must have a HiBi-LSU. " << endl
        << "Creates an IP-XACT description of the created IP." << endl
        << "Supported instruction memory types are 'onchip' and 'vhdl_array."
        << endl << "Supported data memory type is 'onchip'." << endl << endl;
}

// these are not relevant here
std::string
KoskiIntegrator::deviceFamily() const {
    return "";
}

std::string
KoskiIntegrator::deviceName() const {
    return "";
}
    
std::string
KoskiIntegrator::devicePackage() const {
    return "";
}

std::string
KoskiIntegrator::deviceSpeedClass() const {
    return "";
}

int
KoskiIntegrator::targetClockFrequency() const {
    return 1;
}
