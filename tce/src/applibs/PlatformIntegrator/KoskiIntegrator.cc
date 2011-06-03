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

#include "Exception.hh"
#include "KoskiIntegrator.hh"
#include "MemoryGenerator.hh"
#include "AlteraHibiDpRamGenerator.hh"
#include "StringTools.hh"
#include "NetlistBlock.hh"
#include "IPXactFileGenerator.hh"
#include "FileSystem.hh"
using std::vector;
using std::endl;

const TCEString KoskiIntegrator::PIN_TAG_ = "hibi";
const TCEString KoskiIntegrator::DEFAULT_DEVICE_FAMILY_ = "Stratix II";

KoskiIntegrator::KoskiIntegrator():
    AlteraIntegrator(), ipXactGen_(NULL),
    deviceFamily_(DEFAULT_DEVICE_FAMILY_) {
}


KoskiIntegrator::KoskiIntegrator(
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
    const MemInfo& dmem):
    AlteraIntegrator(machine, idf, hdl, progeOutputDir, coreEntityName,
                     outputDir, programName, targetClockFreq, warningStream,
                     errorStream, imem, dmem),
    ipXactGen_(new IPXactFileGenerator(coreEntityName, this)),
    deviceFamily_(DEFAULT_DEVICE_FAMILY_) {
}


KoskiIntegrator::~KoskiIntegrator() {

    if (ipXactGen_ != NULL) {
        delete ipXactGen_;
    }
}

TCEString
KoskiIntegrator::pinTag() const {

    return PIN_TAG_;
}


bool
KoskiIntegrator::chopTaggedSignals() const {

    return false;
}


ProjectFileGenerator*
KoskiIntegrator::projectFileGenerator() const {

    return ipXactGen_;
}


MemoryGenerator*
KoskiIntegrator::dmemInstance() {

    const MemInfo& dmem = dmemInfo();
    MemoryGenerator* dmemGen = NULL;
    if (dmem.type == ONCHIP) {
        TCEString initFile = programName() + "_" + dmem.asName + ".mif";
        // onchip mem size is scalable, use value from adf's Address Space
        int addrw = dmem.asAddrw;
        dmemGen =
            new AlteraHibiDpRamGenerator(
                dmem.mauWidth, dmem.widthInMaus, addrw, initFile,
                this, warningStream(), errorStream());
        ipXactGen_->addMemInitFile(initFile);
    } else {
        TCEString msg = "Unsupported data memory type";
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
        << "Processor must have a HIBIs-LSU. " << endl
        << "Creates an IP-XACT description of the created IP." << endl
        << "This integrator works with Altera tools and FPGAs." << endl
        << "Requires Altera's 'qmegawiz' program to be found from PATH." 
        << endl
        << "Supported instruction memory types are 'onchip' and 'vhdl_array."
        << endl << "Supported data memory type is 'onchip'." << endl
        << "FPGA device family can be changed. Default device family is "
        << DEFAULT_DEVICE_FAMILY_ << endl << endl;
}

TCEString
KoskiIntegrator::deviceFamily() const {

    return deviceFamily_;
}

void
KoskiIntegrator::setDeviceFamily(TCEString devFamily) {

    deviceFamily_ = devFamily;
}

// these are not relevant here
TCEString
KoskiIntegrator::deviceName() const {
    return "";
}
    
TCEString
KoskiIntegrator::devicePackage() const {
    return "";
}

TCEString
KoskiIntegrator::deviceSpeedClass() const {
    return "";
}

int
KoskiIntegrator::targetClockFrequency() const {
    return 1;
}
