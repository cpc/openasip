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
 * @file Stratix2DSPBoardIntegrator.cc
 *
 * Implementation of Stratix2DSPBoardIntegrator class.
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
#include "Stratix2DSPBoardIntegrator.hh"
#include "MemoryGenerator.hh"
#include "AlteraOnchipRamGenerator.hh"
#include "AlteraOnchipRomGenerator.hh"
#include "VhdlRomGenerator.hh"
#include "StringTools.hh"
using std::string;
using std::vector;
using std::endl;


const std::string Stratix2DSPBoardIntegrator::DEVICE_FAMILY_ = "Stratix II";
const std::string Stratix2DSPBoardIntegrator::DEVICE_NAME_ =
    "EP2S180F1020C3";
const std::string Stratix2DSPBoardIntegrator::DEVICE_PACKAGE_ = "F1020";
const std::string Stratix2DSPBoardIntegrator::DEVICE_SPEED_CLASS_ = "3";
const std::string Stratix2DSPBoardIntegrator::PIN_TAG_ = "STRATIXII";
const int Stratix2DSPBoardIntegrator::DEFAULT_FREQ_ = 100;


Stratix2DSPBoardIntegrator::Stratix2DSPBoardIntegrator():
    PlatformIntegrator(), quartusGen_(NULL) {

}

Stratix2DSPBoardIntegrator::Stratix2DSPBoardIntegrator(
        std::string progeOutputDir,
        std::string entityName,
        std::string outputDir,
        std::string programName,
        int targetClockFreq,
        std::ostream& warningStream,
        std::ostream& errorStream):
    PlatformIntegrator(progeOutputDir, entityName, outputDir, programName,
                       targetClockFreq, warningStream, errorStream),
    quartusGen_(new QuartusProjectGenerator(entityName, this)) {

 }


Stratix2DSPBoardIntegrator::~Stratix2DSPBoardIntegrator() {

    for (PinMap::iterator iter = stratixPins_.begin();
         iter != stratixPins_.end(); iter++) {
        if (iter->second != NULL) {
            delete iter->second;
            iter->second = NULL;
        }
    }
    if (quartusGen_ != NULL) {
        delete quartusGen_;
    }
}

void
Stratix2DSPBoardIntegrator::integrateProcessor(MemInfo& imem, MemInfo& dmem) {

    createPinMap();

    readTTAToplevel();

    if(!createSignals(imem, dmem)) {
        return;
    }

    finishNewToplevel();

    writeProjectFiles();
}

bool
Stratix2DSPBoardIntegrator::createSignals(const MemInfo& imem,
                                          const MemInfo& dmem) {
    
    newEntityStream()
        << "entity " << entityName() << " is" << endl
        << StringTools::indent(1) << "port (" << endl;

    ttaToplevelInstStream()
        << StringTools::indent(1) << "core : toplevel" << endl
        << StringTools::indent(2) << "port map (" << endl;

    ttaToplevelInstStream()
        << StringTools::indent(3) << "clk => clk," << endl;
    addSignalMapping("clk");
    newEntityStream() 
        << StringTools::indent(2) << "clk : in std_logic;" << endl;
 
    ttaToplevelInstStream()
       << StringTools::indent(3) << "rstx => rstx," << endl;
    addSignalMapping("rstx");
    // No semicolon at the end!
    newEntityStream() << StringTools::indent(2) << "rstx : in std_logic";

    ttaToplevelInstStream()
        << StringTools::indent(3) << "busy => '0'," << endl;

    ttaToplevelInstStream()
        << StringTools::indent(3) << "pc_init => (others => '0')";

    for (unsigned int i = 0; i < ttaToplevel().size(); i++) {
        string line = ttaToplevel().at(i);
        // port has pin tag and it is not data memory signal
        if (line.find(PIN_TAG_) != string::npos
            && line.find("dmem") == string::npos) {

            string entitySignal = 
                stripSignalEnd(chopSignalToTag(line, PIN_TAG_));
            string toplevelSignal = chopToSignalName(line);
            string mappingSignal = chopSignalToTag(toplevelSignal, PIN_TAG_);

            newEntityStream() 
                << ";" << endl
                << StringTools::indent(2) << entitySignal << endl;

            ttaToplevelInstStream()
                << "," << endl
                << StringTools::indent(3) << toplevelSignal << " => "
                << mappingSignal;
            
            addSignalMapping(mappingSignal);
        }
    }

    newEntityStream()
        << ");" << endl << "end " << entityName() << ";" << endl;

    if (!createMemories(imem, dmem)) {
        return false;
    }
    
    // finishing brace and semicolon for toplevel instantiation
    ttaToplevelInstStream()
        << StringTools::indent(1) << ");" << endl;
    return true;
}


bool
Stratix2DSPBoardIntegrator::createMemories(const MemInfo& imem,
                                           const MemInfo& dmem) {
    
    // strip the toplevel signal names
    vector<string> strippedToplevel;
    for (unsigned int i = 0; i < ttaToplevel().size(); i++) {
        string strippedSignal = chopToSignalName(ttaToplevel().at(i));
        strippedToplevel.push_back(strippedSignal);
    }

    
    MemoryGenerator* imemGen = NULL;
    if (imem.type == ONCHIP) {
        string initFile = programName() + ".mif";
        imemGen = 
            new AlteraOnchipRomGenerator(42, 1, 12, "imem_init.mif", this,
                                         warningStream(), errorStream());
    } else if (imem.type == VHDL_ARRAY) {
        string initFile = programName() + "_imem_pkg.vhdl";
        imemGen = new VhdlRomGenerator(
            imem.mauWidth, imem.widthInMaus, imem.addrw, initFile, this,
            warningStream(), errorStream());
    } else {
        string msg = "Unsupported instruction memory type";
        InvalidData exc(__FILE__, __LINE__, "Stratix2DSPBoardIntegrator",
                        msg);
        throw exc;
    }

    vector<string> reasonsImem;
    if (!imemGen->isCompatible(strippedToplevel, reasonsImem)) {
        errorStream() << "TTA core doesn't have compatible instruction "
                      << "memory interface:" << std::endl;
        for (unsigned int i = 0; i < reasonsImem.size(); i++) {
            errorStream() << reasonsImem.at(i) << std::endl;
        }
        delete imemGen;
        return false;
    }
    imemGen->writeComponentDeclaration(componentStream());
    imemGen->writeComponentInstantiation(
        strippedToplevel, signalStream(), signalConnectionStream(),
        ttaToplevelInstStream(), imemInstStream());

    vector<string> imemFiles = imemGen->generateComponentFile(outputPath());
    delete imemGen;
    if (imemFiles.size() == 0) {
        errorStream() << "Failed to create imem component" << endl;
        return false;
    }
    for (unsigned int i = 0; i < imemFiles.size(); i++) {
        quartusGen_->addHdlFile(imemFiles.at(i));
    }

    
    MemoryGenerator* dmemGen = NULL;
    if (dmem.type == ONCHIP) {
        string initFile = programName() + "_" + dmem.asName + ".mif";
        dmemGen =
            new AlteraOnchipRamGenerator(
                dmem.mauWidth, dmem.widthInMaus, dmem.addrw, initFile, this,
                warningStream(), errorStream());
    } else {
        string msg = "Unsupported data memory type";
        InvalidData exc(__FILE__, __LINE__, "Stratix2DSPBoardIntegrator",
                        msg);
        throw exc;
    }
    
    vector<string> reasonsDmem;
    if (!dmemGen->isCompatible(strippedToplevel, reasonsDmem)) {
        errorStream() << "TTA core doesn't have compatible data memory "
                      <<"interface:" << std::endl;
        for (unsigned int i = 0; i < reasonsDmem.size(); i++) {
            errorStream() << reasonsDmem.at(i) << std::endl;
        }
        delete dmemGen;
        return false;
    }

    dmemGen->writeComponentDeclaration(componentStream());
    dmemGen->writeComponentInstantiation(
        strippedToplevel, signalStream(), signalConnectionStream(),
        ttaToplevelInstStream(), dmemInstStream());

    vector<string> dmemFiles = dmemGen->generateComponentFile(outputPath());
    delete dmemGen;
    if (dmemFiles.size() == 0) {
        errorStream() << "Failed to create dmem component" << endl;
        return false;
    }
    for (unsigned int i = 0; i < dmemFiles.size(); i++) {
        quartusGen_->addHdlFile(dmemFiles.at(i));
    }
    return true;
}


std::string
Stratix2DSPBoardIntegrator::deviceFamily() const {
    
    return DEVICE_FAMILY_;
}


std::string
Stratix2DSPBoardIntegrator::deviceName() const {

    return DEVICE_NAME_;
}


std::string
Stratix2DSPBoardIntegrator::devicePackage() const {

    return DEVICE_PACKAGE_;
}


std::string
Stratix2DSPBoardIntegrator::deviceSpeedClass() const {

    return DEVICE_SPEED_CLASS_;
}

int
Stratix2DSPBoardIntegrator::targetClockFrequency() const {

    int freq = DEFAULT_FREQ_;

    if (PlatformIntegrator::targetClockFrequency() > 0) {
        freq = PlatformIntegrator::targetClockFrequency();
    }
    return freq;
}

void
Stratix2DSPBoardIntegrator::printInfo(std::ostream& stream) const {
    
    stream 
        << "Integrator name: Stratix2DSP" << std::endl
        << "-----------------------------" << std::endl
        << "Integrates the processor core to Altera Stratix II DSP board "
        << "with EP2S180F1020C3 device." << std::endl
        << "Creates project files for QuartusII v8.0 program." << std::endl
        << "Supported instruction memory types are 'onchip' and 'vhdl_array."
        << std::endl
        << "Supported data memory types are 'onchip' and 'sram'." << std::endl
        << "Default clock frequency is 100 MHz." << std::endl << std::endl;
}

void
Stratix2DSPBoardIntegrator::writeProjectFiles() {

    vector<string> progeOutFiles;
    progeOutputHdlFiles(progeOutFiles);
    for (unsigned int i = 0; i < progeOutFiles.size(); i++) {
        quartusGen_->addHdlFile(progeOutFiles.at(i));
    }
    quartusGen_->writeProjectFiles();
}

// keep this in sync with stratixII.hdb
void
Stratix2DSPBoardIntegrator::createPinMap() {

    // clk
    vector<string>* clk = new vector<string>;
    clk->push_back("set_location_assignment PIN_AM17 -to clk");
    stratixPins_["clk"] = clk;

    // reset to push button USER_PB3
    vector<string>* rstx = new vector<string>;
    rstx->push_back("set_location_assignment PIN_J13 -to rstx");
    stratixPins_["rstx"] = rstx;
    

    // leds
    vector<string>* ledMapping = new vector<string>;
    ledMapping->push_back(
        "set_location_assignment PIN_B4 -to STRATIXII_LED[0]");
    ledMapping->push_back(
        "set_location_assignment PIN_D5 -to STRATIXII_LED[1]");
    ledMapping->push_back(
        "set_location_assignment PIN_E5 -to STRATIXII_LED[2]");
    ledMapping->push_back(
        "set_location_assignment PIN_A4 -to STRATIXII_LED[3]");
    ledMapping->push_back(
        "set_location_assignment PIN_A5 -to STRATIXII_LED[4]");
    ledMapping->push_back(
        "set_location_assignment PIN_D6 -to STRATIXII_LED[5]");
    ledMapping->push_back(
        "set_location_assignment PIN_C6 -to STRATIXII_LED[6]");
    ledMapping->push_back(
        "set_location_assignment PIN_A6 -to STRATIXII_LED[7]");
    stratixPins_["STRATIXII_LED"] = ledMapping;
}


void
Stratix2DSPBoardIntegrator::finishNewToplevel() {
    
    string toplevelFile = writeNewToplevel();
    quartusGen_->addHdlFile(toplevelFile);
}

void
Stratix2DSPBoardIntegrator::addSignalMapping(const std::string& signal) {
    
    if (stratixPins_.find(signal) == stratixPins_.end()) {
        warningStream() << "Warning: didn't find mapping for signal name "
                        << signal << endl;
        return;
    }

    vector<string>* mappings = stratixPins_.find(signal)->second;
    for (unsigned int i = 0; i < mappings->size(); i++) {
        quartusGen_->addPinMapping(mappings->at(i));
    }
}


