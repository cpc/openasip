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
 * @file Stratix2SramGenerator.cc
 *
 * Implementation of Stratix2SramGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Exception.hh"
#include "StringTools.hh"
#include "MemoryGenerator.hh"
#include "Stratix2SramGenerator.hh"
using std::string;
using std::vector;
using std::endl;

Stratix2SramGenerator::Stratix2SramGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    std::string initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream) :
    MemoryGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                    integrator, warningStream, errorStream) {

    sramSignals_.push_back("STRATIXII_SRAM_DQ");
    sramSignals_.push_back("STRATIXII_SRAM_ADDR");
    sramSignals_.push_back("STRATIXII_SRAM_WE_N");
    sramSignals_.push_back("STRATIXII_SRAM_OE_N");
    sramSignals_.push_back("STRATIXII_SRAM_CS_N");
    sramSignals_.push_back("STRATIXII_SRAM_BE_N0");
    sramSignals_.push_back("STRATIXII_SRAM_BE_N1");
    sramSignals_.push_back("STRATIXII_SRAM_BE_N2");
    sramSignals_.push_back("STRATIXII_SRAM_BE_N3");
}


Stratix2SramGenerator::~Stratix2SramGenerator() {
}

bool
Stratix2SramGenerator::isCompatible(
    const std::vector<std::string>& ttaCore,
    std::vector<std::string>& reasons) {

    bool foundAll = true;
    for (unsigned int i = 0; i < sramSignals_.size(); i++) {
        string signalName = sramSignals_.at(i);
        if (findSignal(signalName, ttaCore) < 0) {
            string message = "Signal " + signalName + " not found from "
                + "TTA core";
            reasons.push_back(message);
            foundAll = false;
        }
    }
    return foundAll;
}

void
Stratix2SramGenerator::writeComponentDeclaration(std::ostream& stream) {

    stream << StringTools::indent(1) 
           << "-- Sram controller is integrated to LSU" << endl;
}
    
void
Stratix2SramGenerator::writeComponentInstantiation(
    const std::vector<std::string>&,
    std::ostream& signalStream,
    std::ostream& signalConnections,
    std::ostream& toplevelInstantiation,
    std::ostream& memInstantiation) {

    // sram pins are mapped by the integrator
    signalStream << "";
    signalConnections << "";
    toplevelInstantiation << "";
    memInstantiation 
        << StringTools::indent(1) 
        << "-- SRAM signals are directly connected to toplevel" << endl;
    

}

bool
Stratix2SramGenerator::generatesComponentHdlFile() const {

    // Sram controller is integrated into the lsu itself
    // No need to generate separate file
    return false;
}

std::vector<std::string>
Stratix2SramGenerator::generateComponentFile(std::string) {

    // TODO: create simulation model for sram?
    vector<string> noFileToGenerate;
    return noFileToGenerate;
}

