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
 * @file AvalonIntegrator.cc
 *
 * Implementation of AvalonIntegrator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include "AvalonIntegrator.hh"
#include "MemoryGenerator.hh"
#include "SOPCBuilderFileGenerator.hh"
using std::endl;

const TCEString AvalonIntegrator::DEFAULT_DEVICE_FAMILY_ = "Stratix II";

AvalonIntegrator::AvalonIntegrator():
    AlteraIntegrator(), sopcGenerator_(NULL) {
}

AvalonIntegrator::AvalonIntegrator(
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
    AlteraIntegrator(machine, idf, hdl, progeOutputDir, coreEntityName,
                     outputDir, programName, targetClockFreq, warningStream,
                     errorStream, imem, dmemType),
    sopcGenerator_(new SOPCBuilderFileGenerator(coreEntityName, this)),
    deviceFamily_(DEFAULT_DEVICE_FAMILY_) {
}


AvalonIntegrator::~AvalonIntegrator() {

    if (sopcGenerator_ != NULL) {
        delete sopcGenerator_;
    }
}

void
AvalonIntegrator::printInfo(std::ostream& stream) const {

    stream 
        << "Integrator name: AvalonIntegrator" << endl
        << "---------------------------------" << endl
        << "Creates a SOPC Builder component from TTA core." << endl
        << "Processor must have avalon_lsu or avalon_sfu (from avalon.hdb) "
        << "for correct behaviour." << endl
        << "Supported instruction memory types are 'onchip' and 'vhdl_array."
        << endl
        << "If 'normal' lsu is used, supported data memory type is 'onchip'."
        << endl
        << "If avalon_lsu is used, data memory type must be 'none'." << endl
        << "FPGA device family can be changed. Default device family is "
        << DEFAULT_DEVICE_FAMILY_ << endl << endl;
}


TCEString
AvalonIntegrator::pinTag() const {

    return "avalon_d";
}


bool
AvalonIntegrator::chopTaggedSignals() const {

    return false;
}


ProjectFileGenerator*
AvalonIntegrator::projectFileGenerator() const {

    return sopcGenerator_;
}


TCEString
AvalonIntegrator::deviceFamily() const {

    return deviceFamily_;
}


void
AvalonIntegrator::setDeviceFamily(TCEString devFamily) {

    deviceFamily_ = devFamily;
}

TCEString
AvalonIntegrator::deviceName() const {
    return "";
}

TCEString
AvalonIntegrator::devicePackage() const {
    return "";
}

TCEString
AvalonIntegrator::deviceSpeedClass() const {
    return "";
}

int
AvalonIntegrator::targetClockFrequency() const {
    return 1;
}
