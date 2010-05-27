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
#include "Stratix2SramGenerator.hh"
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
                << StringTools::indent(2) << entitySignal;

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
            new AlteraOnchipRomGenerator(
                imem.mauWidth, imem.widthInMaus, imem.addrw, initFile, this,
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

    if (!generateMemory(*imemGen, strippedToplevel, imemInstStream())) {
        delete imemGen;
        return false;
    }
    delete imemGen;

    MemoryGenerator* dmemGen = NULL;
    if (dmem.type == ONCHIP) {
        string initFile = programName() + "_" + dmem.asName + ".mif";
        dmemGen =
            new AlteraOnchipRamGenerator(
                dmem.mauWidth, dmem.widthInMaus, dmem.addrw, initFile, this,
                warningStream(), errorStream());
    } else if (dmem.type == SRAM) {
        string initFile = programName() + "_" + dmem.asName + ".img";
        dmemGen =
            new Stratix2SramGenerator(
                dmem.mauWidth, dmem.widthInMaus, dmem.addrw, initFile, this,
                warningStream(), errorStream());
        warningStream() << "Warning: Data memory is not initialized during "
                        << "FPGA programming." << endl;
    } else {
        string msg = "Unsupported data memory type";
        InvalidData exc(__FILE__, __LINE__, "Stratix2DSPBoardIntegrator",
                        msg);
        throw exc;
    }
    
    bool success =
        generateMemory(*dmemGen, strippedToplevel, dmemInstStream());
    delete dmemGen;
    return success;
}

bool
Stratix2DSPBoardIntegrator::generateMemory(
    MemoryGenerator& memGen, 
    std::vector<std::string>& toplevelSignals,
    std::ostream& memInstStream) {
    
    vector<string> reasons;
    if (!memGen.isCompatible(toplevelSignals, reasons)) {
        errorStream() << "TTA core doesn't have compatible memory "
                      <<"interface:" << std::endl;
        for (unsigned int i = 0; i < reasons.size(); i++) {
            errorStream() << reasons.at(i) << std::endl;
        }
        return false;
    }

    memGen.writeComponentDeclaration(componentStream());
    memGen.writeComponentInstantiation(
        toplevelSignals, signalStream(), signalConnectionStream(),
        ttaToplevelInstStream(), memInstStream);

    if (memGen.generatesComponentHdlFile()) {
        vector<string> memFiles =
            memGen.generateComponentFile(outputPath());
        if (memFiles.size() == 0) {
            errorStream() << "Failed to create mem component" << endl;
            return false;
        }
        for (unsigned int i = 0; i < memFiles.size(); i++) {
            quartusGen_->addHdlFile(memFiles.at(i));
        }
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

    // sram data signals
    vector<string>* sramData = new vector<string>;
    sramData->push_back(
        "set_location_assignment PIN_AD18 -to STRATIXII_SRAM_DQ[0]");
    sramData->push_back(
        "set_location_assignment PIN_AB18 -to STRATIXII_SRAM_DQ[1]");
    sramData->push_back(
        "set_location_assignment PIN_AB19 -to STRATIXII_SRAM_DQ[2]");
    sramData->push_back(
        "set_location_assignment PIN_AC20 -to STRATIXII_SRAM_DQ[3]");
    sramData->push_back(
        "set_location_assignment PIN_AD20 -to STRATIXII_SRAM_DQ[4]");
    sramData->push_back(
        "set_location_assignment PIN_AE20 -to STRATIXII_SRAM_DQ[5]");
    sramData->push_back(
        "set_location_assignment PIN_AB20 -to STRATIXII_SRAM_DQ[6]");
    sramData->push_back(
        "set_location_assignment PIN_AF20 -to STRATIXII_SRAM_DQ[7]");
    sramData->push_back(
        "set_location_assignment PIN_AC21 -to STRATIXII_SRAM_DQ[8]");
    sramData->push_back(
        "set_location_assignment PIN_AD21 -to STRATIXII_SRAM_DQ[9]");
    sramData->push_back(
        "set_location_assignment PIN_AB21 -to STRATIXII_SRAM_DQ[10]");
    sramData->push_back(
        "set_location_assignment PIN_AE21 -to STRATIXII_SRAM_DQ[11]");
    sramData->push_back(
        "set_location_assignment PIN_AG20 -to STRATIXII_SRAM_DQ[12]");
    sramData->push_back(
        "set_location_assignment PIN_AF21 -to STRATIXII_SRAM_DQ[13]");
    sramData->push_back(
        "set_location_assignment PIN_AD22 -to STRATIXII_SRAM_DQ[14]");
    sramData->push_back(
        "set_location_assignment PIN_AF22 -to STRATIXII_SRAM_DQ[15]");
    sramData->push_back(
        "set_location_assignment PIN_AE22 -to STRATIXII_SRAM_DQ[16]");    
    sramData->push_back(
        "set_location_assignment PIN_AC17 -to STRATIXII_SRAM_DQ[17]");
    sramData->push_back(
        "set_location_assignment PIN_AE19 -to STRATIXII_SRAM_DQ[18]");
    sramData->push_back(
        "set_location_assignment PIN_AD19 -to STRATIXII_SRAM_DQ[19]");
    sramData->push_back(
        "set_location_assignment PIN_AC18 -to STRATIXII_SRAM_DQ[20]");
    sramData->push_back(
        "set_location_assignment PIN_AB17 -to STRATIXII_SRAM_DQ[21]");
    sramData->push_back(
        "set_location_assignment PIN_AC19 -to STRATIXII_SRAM_DQ[22]");
    sramData->push_back(
        "set_location_assignment PIN_AL26 -to STRATIXII_SRAM_DQ[23]");
    sramData->push_back(
        "set_location_assignment PIN_AL27 -to STRATIXII_SRAM_DQ[24]");
    sramData->push_back(
        "set_location_assignment PIN_AL28 -to STRATIXII_SRAM_DQ[25]");
    sramData->push_back(
        "set_location_assignment PIN_AK28 -to STRATIXII_SRAM_DQ[26]");
    sramData->push_back(
        "set_location_assignment PIN_AK29 -to STRATIXII_SRAM_DQ[27]");
    sramData->push_back(
        "set_location_assignment PIN_AC13 -to STRATIXII_SRAM_DQ[28]");
    sramData->push_back(
        "set_location_assignment PIN_AD10 -to STRATIXII_SRAM_DQ[29]");
    sramData->push_back(
        "set_location_assignment PIN_AC11 -to STRATIXII_SRAM_DQ[30]");
    sramData->push_back(
        "set_location_assignment PIN_AE11 -to STRATIXII_SRAM_DQ[31]");
    stratixPins_["STRATIXII_SRAM_DQ"] = sramData;

     // sram address signals
    vector<string>* sramAddr = new vector<string>;
    sramAddr->push_back(
        "set_location_assignment PIN_AD8 -to STRATIXII_SRAM_ADDR[0]");
    sramAddr->push_back(
        "set_location_assignment PIN_AM27 -to STRATIXII_SRAM_ADDR[1]");
    sramAddr->push_back(
        "set_location_assignment PIN_AM28 -to STRATIXII_SRAM_ADDR[2]");
    sramAddr->push_back(
        "set_location_assignment PIN_AJ27 -to STRATIXII_SRAM_ADDR[3]");
    sramAddr->push_back(
        "set_location_assignment PIN_AK27 -to STRATIXII_SRAM_ADDR[4]");
    sramAddr->push_back(
        "set_location_assignment PIN_AL29 -to STRATIXII_SRAM_ADDR[5]");
    sramAddr->push_back(
        "set_location_assignment PIN_AM29 -to STRATIXII_SRAM_ADDR[6]");
    sramAddr->push_back(
        "set_location_assignment PIN_AJ28 -to STRATIXII_SRAM_ADDR[7]");
    sramAddr->push_back(
        "set_location_assignment PIN_AH28 -to STRATIXII_SRAM_ADDR[8]");
    sramAddr->push_back(
        "set_location_assignment PIN_AK20 -to STRATIXII_SRAM_ADDR[9]");
    sramAddr->push_back(
        "set_location_assignment PIN_AJ20 -to STRATIXII_SRAM_ADDR[10]");
    sramAddr->push_back(
        "set_location_assignment PIN_AL21 -to STRATIXII_SRAM_ADDR[11]");
    sramAddr->push_back(
        "set_location_assignment PIN_AL22 -to STRATIXII_SRAM_ADDR[12]");
    sramAddr->push_back(
        "set_location_assignment PIN_AJ22 -to STRATIXII_SRAM_ADDR[13]");
    sramAddr->push_back(
        "set_location_assignment PIN_AH22 -to STRATIXII_SRAM_ADDR[14]");
    sramAddr->push_back(
        "set_location_assignment PIN_AL23 -to STRATIXII_SRAM_ADDR[15]");
    sramAddr->push_back(
        "set_location_assignment PIN_AL24 -to STRATIXII_SRAM_ADDR[16]");
    sramAddr->push_back(
        "set_location_assignment PIN_AJ25 -to STRATIXII_SRAM_ADDR[17]");
    sramAddr->push_back(
        "set_location_assignment PIN_AH25 -to STRATIXII_SRAM_ADDR[18]");
    sramAddr->push_back(
        "set_location_assignment PIN_AL25 -to STRATIXII_SRAM_ADDR[19]");
    stratixPins_["STRATIXII_SRAM_ADDR"] = sramAddr;

    // sram control signals
    vector<string>* sramWe = new vector<string>;
    sramWe->push_back(
        "set_location_assignment PIN_AH14 -to STRATIXII_SRAM_WE_N");
    stratixPins_["STRATIXII_SRAM_WE_N"] = sramWe;

    vector<string>* sramOe = new vector<string>;
    sramOe->push_back(
        "set_location_assignment PIN_AG14 -to STRATIXII_SRAM_OE_N");
    stratixPins_["STRATIXII_SRAM_OE_N"] = sramOe;

    vector<string>* sramCs = new vector<string>;
    sramCs->push_back(
        "set_location_assignment PIN_AL12 -to STRATIXII_SRAM_CS_N");
    stratixPins_["STRATIXII_SRAM_CS_N"] = sramCs;

    vector<string>* sramB0 = new vector<string>;
    vector<string>* sramB1 = new vector<string>;
    vector<string>* sramB2 = new vector<string>;
    vector<string>* sramB3 = new vector<string>;
    sramB0->push_back(
        "set_location_assignment PIN_AG11 -to STRATIXII_SRAM_BE_N0");
    sramB1->push_back(
        "set_location_assignment PIN_AK10 -to STRATIXII_SRAM_BE_N1");
    sramB2->push_back(
        "set_location_assignment PIN_AK11 -to STRATIXII_SRAM_BE_N2");
    sramB3->push_back(
        "set_location_assignment PIN_AL11 -to STRATIXII_SRAM_BE_N3");
    stratixPins_["STRATIXII_SRAM_BE_N0"] = sramB0;
    stratixPins_["STRATIXII_SRAM_BE_N1"] = sramB1;
    stratixPins_["STRATIXII_SRAM_BE_N2"] = sramB2;
    stratixPins_["STRATIXII_SRAM_BE_N3"] = sramB3;
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


