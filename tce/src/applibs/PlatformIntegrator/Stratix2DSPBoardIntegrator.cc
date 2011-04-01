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
#include "Stratix2DSPBoardIntegrator.hh"
#include "Stratix2SramGenerator.hh"
#include "StringTools.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "VirtualNetlistBlock.hh"
#include "NetlistPort.hh"
#include "FileSystem.hh"
using std::string;
using std::vector;
using std::endl;
using ProGe::Netlist;
using ProGe::NetlistBlock;
using ProGe::VirtualNetlistBlock;
using ProGe::NetlistPort;

const std::string Stratix2DSPBoardIntegrator::DEVICE_FAMILY_ = "Stratix II";
const std::string Stratix2DSPBoardIntegrator::DEVICE_NAME_ =
    "EP2S180F1020C3";
const std::string Stratix2DSPBoardIntegrator::DEVICE_PACKAGE_ = "F1020";
const std::string Stratix2DSPBoardIntegrator::DEVICE_SPEED_CLASS_ = "3";
const std::string Stratix2DSPBoardIntegrator::PIN_TAG_ = "STRATIXII";
const int Stratix2DSPBoardIntegrator::DEFAULT_FREQ_ = 100;


Stratix2DSPBoardIntegrator::Stratix2DSPBoardIntegrator():
    AlteraIntegrator(), quartusGen_(NULL) {

}

Stratix2DSPBoardIntegrator::Stratix2DSPBoardIntegrator(
    ProGe::HDL hdl,
    std::string progeOutputDir,
    std::string coreEntityName,
    std::string outputDir,
    std::string programName,
    int targetClockFreq,
    std::ostream& warningStream,
    std::ostream& errorStream,
    const MemInfo& imem,
    const MemInfo& dmem):
    AlteraIntegrator(hdl, progeOutputDir, coreEntityName, outputDir,
                     programName, targetClockFreq, warningStream,
                     errorStream, imem, dmem),
    quartusGen_(new QuartusProjectGenerator(coreEntityName, this)) {
}


Stratix2DSPBoardIntegrator::~Stratix2DSPBoardIntegrator() {

    for (PinMap::iterator iter = stratixPins_.begin();
         iter != stratixPins_.end(); iter++) {
        if (iter->second != NULL) {
            for (unsigned int i = 0; i < iter->second->size(); i++) {
                delete iter->second->at(i);
            }
            delete iter->second;
        }
    }
    if (quartusGen_ != NULL) {
        delete quartusGen_;
    }
}

void
Stratix2DSPBoardIntegrator::integrateProcessor(
    const ProGe::NetlistBlock* ttaCore) {

    generatePinMap();

    if(!createPorts(ttaCore)) {
        return;
    }
 
    if (!createMemories()) {
        return;
    }

    mapToplevelPorts();

    writeNewToplevel();

    addProGeFiles();

    quartusGen_->writeProjectFiles();
}   

MemoryGenerator*
Stratix2DSPBoardIntegrator::dmemInstance() {

    const MemInfo& dmem = dmemInfo();
    MemoryGenerator* dmemGen = NULL;
    if (dmem.type == ONCHIP) {
        dmemGen = AlteraIntegrator::dmemInstance();
    } else if (dmemInfo().type == SRAM) {
        string initFile = programName() + "_" + dmem.asName + ".img";
        // SRAM component has a fixed size, thus use the addr width from hdb
        int addrw = dmem.portAddrw;
        dmemGen =
            new Stratix2SramGenerator(
                dmem.mauWidth, dmem.widthInMaus, addrw, initFile,
                this, warningStream(), errorStream());
        warningStream() << "Warning: Data memory is not initialized during "
                        << "FPGA programming." << endl;
    } else {
        string msg = "Unsupported data memory type";
        InvalidData exc(__FILE__, __LINE__, "Stratix2DSPBoardIntegrator",
                        msg);
        throw exc;
    }
    return dmemGen;
}

void
Stratix2DSPBoardIntegrator::mapToplevelPorts() {

    NetlistBlock& tl = netlist()->topLevelBlock();
    for (int i = 0; i < tl.portCount(); i++) {
        addSignalMapping(tl.port(i).name());
    }
}

void
Stratix2DSPBoardIntegrator::addSignalMapping(const std::string& signal) {
    
    if (stratixPins_.find(signal) == stratixPins_.end()) {
        warningStream() << "Warning: didn't find mapping for signal name "
                        << signal << endl;
        return;
    }

    MappingList* mappings = stratixPins_.find(signal)->second;
    for (unsigned int i = 0; i < mappings->size(); i++) {
        quartusGen_->addSignalMapping(*mappings->at(i));
    }
}


std::string
Stratix2DSPBoardIntegrator::pinTag() const {

    return PIN_TAG_;
}


bool
Stratix2DSPBoardIntegrator::chopTaggedSignals() const {

    return true;
}


bool
Stratix2DSPBoardIntegrator::isDataMemorySignal(
    const std::string& signalName) const {

    bool isDmemSignal = false;
    if (dmemInfo().type == ONCHIP) {
        isDmemSignal = AlteraIntegrator::isDataMemorySignal(signalName);
    } else if (dmemInfo().type == SRAM) {
        isDmemSignal = signalName.find("SRAM") != string::npos;
    } else {
        isDmemSignal = false;
    }
    return isDmemSignal;
}


ProjectFileGenerator*
Stratix2DSPBoardIntegrator::projectFileGenerator() const {

    return quartusGen_;
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


// keep this in sync with stratixII.hdb
void
Stratix2DSPBoardIntegrator::generatePinMap() {

    // clk
    MappingList* clk = new MappingList;
    clk->push_back(new SignalMapping("PIN_AM17","clk"));
    stratixPins_["clk"] = clk;

    // reset to push button USER_PB3
    MappingList* rstx = new MappingList;
    rstx->push_back(new SignalMapping("PIN_J13","rstx"));
    stratixPins_["rstx"] = rstx;
    
    // leds
    MappingList* ledMapping = new MappingList;
    ledMapping->push_back(new SignalMapping("PIN_B4","STRATIXII_LED[0]"));
    ledMapping->push_back(new SignalMapping("PIN_D5","STRATIXII_LED[1]"));
    ledMapping->push_back(new SignalMapping("PIN_E5","STRATIXII_LED[2]"));
    ledMapping->push_back(new SignalMapping("PIN_A4","STRATIXII_LED[3]"));
    ledMapping->push_back(new SignalMapping("PIN_A5","STRATIXII_LED[4]"));
    ledMapping->push_back(new SignalMapping("PIN_D6","STRATIXII_LED[5]"));
    ledMapping->push_back(new SignalMapping("PIN_C6","STRATIXII_LED[6]"));
    ledMapping->push_back(new SignalMapping("PIN_A6","STRATIXII_LED[7]"));
    stratixPins_["STRATIXII_LED"] = ledMapping;

    // sram data signals
    MappingList* sramData = new MappingList;
    sramData->push_back(new SignalMapping("PIN_AD18","STRATIXII_SRAM_DQ[0]"));
    sramData->push_back(new SignalMapping("PIN_AB18","STRATIXII_SRAM_DQ[1]"));
    sramData->push_back(new SignalMapping("PIN_AB19","STRATIXII_SRAM_DQ[2]"));
    sramData->push_back(new SignalMapping("PIN_AC20","STRATIXII_SRAM_DQ[3]"));
    sramData->push_back(new SignalMapping("PIN_AD20","STRATIXII_SRAM_DQ[4]"));
    sramData->push_back(new SignalMapping("PIN_AE20","STRATIXII_SRAM_DQ[5]"));
    sramData->push_back(new SignalMapping("PIN_AB20","STRATIXII_SRAM_DQ[6]"));
    sramData->push_back(new SignalMapping("PIN_AF20","STRATIXII_SRAM_DQ[7]"));
    sramData->push_back(new SignalMapping("PIN_AC21","STRATIXII_SRAM_DQ[8]"));
    sramData->push_back(new SignalMapping("PIN_AD21","STRATIXII_SRAM_DQ[9]"));
    sramData->push_back(
        new SignalMapping("PIN_AB21","STRATIXII_SRAM_DQ[10]"));
    sramData->push_back(
        new SignalMapping("PIN_AE21","STRATIXII_SRAM_DQ[11]"));
    sramData->push_back(
        new SignalMapping("PIN_AG20","STRATIXII_SRAM_DQ[12]"));
    sramData->push_back(
        new SignalMapping("PIN_AF21","STRATIXII_SRAM_DQ[13]"));
    sramData->push_back(
        new SignalMapping("PIN_AD22","STRATIXII_SRAM_DQ[14]"));
    sramData->push_back(
        new SignalMapping("PIN_AF22","STRATIXII_SRAM_DQ[15]"));
    sramData->push_back(
        new SignalMapping("PIN_AE22","STRATIXII_SRAM_DQ[16]"));
    sramData->push_back(
        new SignalMapping("PIN_AC17","STRATIXII_SRAM_DQ[17]"));
    sramData->push_back(
        new SignalMapping("PIN_AE19","STRATIXII_SRAM_DQ[18]"));
    sramData->push_back(
        new SignalMapping("PIN_AD19","STRATIXII_SRAM_DQ[19]"));
    sramData->push_back(
        new SignalMapping("PIN_AC18","STRATIXII_SRAM_DQ[20]"));
    sramData->push_back(
        new SignalMapping("PIN_AB17","STRATIXII_SRAM_DQ[21]"));
    sramData->push_back(
        new SignalMapping("PIN_AC19","STRATIXII_SRAM_DQ[22]"));
    sramData->push_back(
        new SignalMapping("PIN_AL26","STRATIXII_SRAM_DQ[23]"));
    sramData->push_back(
        new SignalMapping("PIN_AL27","STRATIXII_SRAM_DQ[24]"));
    sramData->push_back(
        new SignalMapping("PIN_AL28","STRATIXII_SRAM_DQ[25]"));
    sramData->push_back(
        new SignalMapping("PIN_AK28","STRATIXII_SRAM_DQ[26]"));
    sramData->push_back(
        new SignalMapping("PIN_AK29","STRATIXII_SRAM_DQ[27]"));
    sramData->push_back(
        new SignalMapping("PIN_AC13","STRATIXII_SRAM_DQ[28]"));
    sramData->push_back(
        new SignalMapping("PIN_AD10","STRATIXII_SRAM_DQ[29]"));
    sramData->push_back(
        new SignalMapping("PIN_AC11","STRATIXII_SRAM_DQ[30]"));
    sramData->push_back(
        new SignalMapping("PIN_AE11","STRATIXII_SRAM_DQ[31]"));
    stratixPins_["STRATIXII_SRAM_DQ"] = sramData;

     // sram address signals
    MappingList* sramAddr = new MappingList;
    sramAddr->push_back(
        new SignalMapping("PIN_AD8","STRATIXII_SRAM_ADDR[0]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AM27","STRATIXII_SRAM_ADDR[1]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AM28","STRATIXII_SRAM_ADDR[2]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AJ27","STRATIXII_SRAM_ADDR[3]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AK27","STRATIXII_SRAM_ADDR[4]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL29","STRATIXII_SRAM_ADDR[5]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AM29","STRATIXII_SRAM_ADDR[6]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AJ28","STRATIXII_SRAM_ADDR[7]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AH28","STRATIXII_SRAM_ADDR[8]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AK20","STRATIXII_SRAM_ADDR[9]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AJ20","STRATIXII_SRAM_ADDR[10]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL21","STRATIXII_SRAM_ADDR[11]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL22","STRATIXII_SRAM_ADDR[12]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AJ22","STRATIXII_SRAM_ADDR[13]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AH22","STRATIXII_SRAM_ADDR[14]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL23","STRATIXII_SRAM_ADDR[15]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL24","STRATIXII_SRAM_ADDR[16]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AJ25","STRATIXII_SRAM_ADDR[17]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AH25","STRATIXII_SRAM_ADDR[18]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL25","STRATIXII_SRAM_ADDR[19]"));
    stratixPins_["STRATIXII_SRAM_ADDR"] = sramAddr;

    // sram control signals
    MappingList* sramWe = new MappingList;
    sramWe->push_back(new SignalMapping("PIN_AH14","STRATIXII_SRAM_WE_N"));
    stratixPins_["STRATIXII_SRAM_WE_N"] = sramWe;

    MappingList* sramOe = new MappingList;
    sramOe->push_back(new SignalMapping("PIN_AG14","STRATIXII_SRAM_OE_N"));
    stratixPins_["STRATIXII_SRAM_OE_N"] = sramOe;

    MappingList* sramCs = new MappingList;
    sramCs->push_back(new SignalMapping("PIN_AL12","STRATIXII_SRAM_CS_N"));
    stratixPins_["STRATIXII_SRAM_CS_N"] = sramCs;

    MappingList* sramB0 = new MappingList;
    MappingList* sramB1 = new MappingList;
    MappingList* sramB2 = new MappingList;
    MappingList* sramB3 = new MappingList;
    sramB0->push_back(new SignalMapping("PIN_AG11","STRATIXII_SRAM_BE_N0"));
    sramB1->push_back(new SignalMapping("PIN_AK10","STRATIXII_SRAM_BE_N1"));
    sramB2->push_back(new SignalMapping("PIN_AK11","STRATIXII_SRAM_BE_N2"));
    sramB3->push_back(new SignalMapping("PIN_AL11","STRATIXII_SRAM_BE_N3"));
    stratixPins_["STRATIXII_SRAM_BE_N0"] = sramB0;
    stratixPins_["STRATIXII_SRAM_BE_N1"] = sramB1;
    stratixPins_["STRATIXII_SRAM_BE_N2"] = sramB2;
    stratixPins_["STRATIXII_SRAM_BE_N3"] = sramB3;
}
