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
using std::string;
using std::endl;

AvalonIntegrator::AvalonIntegrator():
    AlteraIntegrator(), sopcGenerator_(NULL) {
}

AvalonIntegrator::AvalonIntegrator(
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
    AlteraIntegrator(hdl, progeOutputDir, entityName, outputDir,
                     programName, targetClockFreq, warningStream,
                     errorStream, imem, dmem),
    sopcGenerator_(new SOPCBuilderFileGenerator(entityName, this))
{
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
        << "If 'normal' lsu is used, supported data memory type is 'onchip'."
        << "If avalon_lsu is used, data memory type must be 'none'."
        << endl;
}
    

MemoryGenerator*
AvalonIntegrator::dmemInstance() {

    assert(dmemInfo().type != NONE);

    MemoryGenerator* dmemGen = NULL;
    if (dmemInfo().type == ONCHIP) {
        dmemGen = AlteraIntegrator::dmemInstance();
    } else {
        string msg = "Unsupported data memory type";
        InvalidData exc(__FILE__, __LINE__, "AvalonIntegrator",
                        msg);
        throw exc;
    }
    return dmemGen;
}


std::string
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


// not relevant in this case
std::string
AvalonIntegrator::deviceFamily() const {
    return "";
}

std::string
AvalonIntegrator::deviceName() const {
    return "";
}

std::string
AvalonIntegrator::devicePackage() const {
    return "";
}

std::string
AvalonIntegrator::deviceSpeedClass() const {
    return "";
}

int
AvalonIntegrator::targetClockFrequency() const {
    return 1;
}
