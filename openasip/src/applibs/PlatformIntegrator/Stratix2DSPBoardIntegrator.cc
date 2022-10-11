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
 * @file Stratix2DSPBoardIntegrator.cc
 *
 * Implementation of Stratix2DSPBoardIntegrator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <map>
#include <vector>
#include "Exception.hh"
#include "Stratix2DSPBoardIntegrator.hh"
#include "Stratix2SramGenerator.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "VirtualNetlistBlock.hh"
#include "NetlistPort.hh"
#include "FileSystem.hh"
#include "Machine.hh"
using std::vector;
using std::endl;
using ProGe::Netlist;
using ProGe::NetlistBlock;
using ProGe::VirtualNetlistBlock;
using ProGe::NetlistPort;
using PlatInt::SignalMapping;
using PlatInt::SignalMappingList;

const TCEString Stratix2DSPBoardIntegrator::DEVICE_FAMILY_ = "Stratix II";
const TCEString Stratix2DSPBoardIntegrator::DEVICE_NAME_ =
    "EP2S180F1020C3";
const TCEString Stratix2DSPBoardIntegrator::DEVICE_PACKAGE_ = "F1020";
const TCEString Stratix2DSPBoardIntegrator::DEVICE_SPEED_CLASS_ = "3";
const TCEString Stratix2DSPBoardIntegrator::PIN_TAG_ = "STRATIXII";
const int Stratix2DSPBoardIntegrator::DEFAULT_FREQ_ = 100;


Stratix2DSPBoardIntegrator::Stratix2DSPBoardIntegrator():
    AlteraIntegrator(), quartusGen_(NULL) {

}

Stratix2DSPBoardIntegrator::Stratix2DSPBoardIntegrator(
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
    quartusGen_(new QuartusProjectGenerator(coreEntityName, this)),
    dmemGen_(NULL) {
    setDeviceName(DEVICE_NAME_);
}


Stratix2DSPBoardIntegrator::~Stratix2DSPBoardIntegrator() {

    for (PlatInt::PinMap::iterator iter = stratix2Pins_.begin();
         iter != stratix2Pins_.end(); iter++) {
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

    if (dmemGen_ != NULL) {
        delete dmemGen_;
    }
}

void
Stratix2DSPBoardIntegrator::integrateProcessor(
    const ProGe::NetlistBlock* ttaCore) {

    generatePinMap();

    initPlatformNetlist(ttaCore);
    
    const NetlistBlock& core = progeBlock();
    int coreId = -1;
    if (!integrateCore(core, coreId)) {
        return;
    }

    mapToplevelPorts();

    writeNewToplevel();
    
    addProGeFiles();

    projectFileGenerator()->writeProjectFiles();
}

MemoryGenerator&
Stratix2DSPBoardIntegrator::dmemInstance(
    MemInfo dmem, TTAMachine::FunctionUnit& lsuArch,
    std::vector<std::string> lsuPorts) {
    if (dmem.type == ONCHIP) {
        return AlteraIntegrator::dmemInstance(dmem, lsuArch, lsuPorts);
    } else if (dmem.type == SRAM) {
        if (dmemGen_ == NULL) {
            TCEString initFile = programName() + "_" + dmem.asName + ".img";
            // SRAM component has fixed size, thus use the addr width from hdb
            int addrw = dmem.portAddrw;
            dmemGen_ =
                new Stratix2SramGenerator(
                    dmem.mauWidth, dmem.widthInMaus, addrw, initFile,
                    this, warningStream(), errorStream());
            warningStream() << "Warning: Data memory is not initialized "
                            << "during FPGA programming." << endl;
            dmemGen_->addLsu(lsuArch, lsuPorts);
        }
    } else {
        TCEString msg = "Unsupported data memory type";
        throw InvalidData(__FILE__, __LINE__, "Stratix2DSPBoardIntegrator",
                          msg);
    }
    return *dmemGen_;
}

void
Stratix2DSPBoardIntegrator::mapToplevelPorts() {
    NetlistBlock& tl = *integratorBlock();
    for (size_t i = 0; i < tl.portCount(); i++) {
        addSignalMapping(tl.port(i).name());
    }
}

void
Stratix2DSPBoardIntegrator::addSignalMapping(const TCEString& signal) {
    
    if (stratix2Pins_.find(signal) == stratix2Pins_.end()) {
        warningStream() << "Warning: didn't find mapping for signal name "
                        << signal << endl;
        return;
    }

    SignalMappingList* mappings = stratix2Pins_.find(signal)->second;
    for (unsigned int i = 0; i < mappings->size(); i++) {
        quartusGen_->addSignalMapping(*mappings->at(i));
    }
}


TCEString
Stratix2DSPBoardIntegrator::pinTag() const {

    return PIN_TAG_;
}


bool
Stratix2DSPBoardIntegrator::chopTaggedSignals() const {

    return true;
}


ProjectFileGenerator*
Stratix2DSPBoardIntegrator::projectFileGenerator() const {

    return quartusGen_;
}



TCEString
Stratix2DSPBoardIntegrator::deviceFamily() const {
    
    return DEVICE_FAMILY_;
}


void
Stratix2DSPBoardIntegrator::setDeviceFamily(TCEString devFamily) {
    
    if (devFamily != DEVICE_FAMILY_) {
        warningStream() 
            << "Warning: Refusing to change device family!" << endl
            << "- Original device family: " << DEVICE_FAMILY_ << endl
            << "- New device family: " << devFamily << endl;
    }
}

TCEString
Stratix2DSPBoardIntegrator::devicePackage() const {

    return DEVICE_PACKAGE_;
}


TCEString
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
        << "Default clock frequency is 100 MHz." << std::endl 
        << "Active low reset is connected to CPU RESET button." << std::endl
        << std::endl;
}


// keep this in sync with stratixII.hdb
void
Stratix2DSPBoardIntegrator::generatePinMap() {

    // clk
    SignalMappingList* clk = new SignalMappingList;
    clk->push_back(new SignalMapping("PIN_AM17","clk"));
    stratix2Pins_["clk"] = clk;

    // reset to push button CPU_RESET
    SignalMappingList* rstx = new SignalMappingList;
    rstx->push_back(new SignalMapping("PIN_AG19","rstx"));
    stratix2Pins_["rstx"] = rstx;
    
    // leds
    SignalMappingList* ledMapping = new SignalMappingList;
    ledMapping->push_back(new SignalMapping("PIN_B4","STRATIXII_LED[0]"));
    ledMapping->push_back(new SignalMapping("PIN_D5","STRATIXII_LED[1]"));
    ledMapping->push_back(new SignalMapping("PIN_E5","STRATIXII_LED[2]"));
    ledMapping->push_back(new SignalMapping("PIN_A4","STRATIXII_LED[3]"));
    ledMapping->push_back(new SignalMapping("PIN_A5","STRATIXII_LED[4]"));
    ledMapping->push_back(new SignalMapping("PIN_D6","STRATIXII_LED[5]"));
    ledMapping->push_back(new SignalMapping("PIN_C6","STRATIXII_LED[6]"));
    ledMapping->push_back(new SignalMapping("PIN_A6","STRATIXII_LED[7]"));
    stratix2Pins_["STRATIXII_LED"] = ledMapping;

    // sram data signals
    SignalMappingList* sramData = new SignalMappingList;
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
    stratix2Pins_["STRATIXII_SRAM_DQ"] = sramData;

     // sram address signals
    SignalMappingList* sramAddr = new SignalMappingList;
    sramAddr->push_back(
        new SignalMapping("PIN_AM28","STRATIXII_SRAM_ADDR[0]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AJ27","STRATIXII_SRAM_ADDR[1]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AK27","STRATIXII_SRAM_ADDR[2]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL29","STRATIXII_SRAM_ADDR[3]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AM29","STRATIXII_SRAM_ADDR[4]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AJ28","STRATIXII_SRAM_ADDR[5]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AH28","STRATIXII_SRAM_ADDR[6]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AK20","STRATIXII_SRAM_ADDR[7]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AJ20","STRATIXII_SRAM_ADDR[8]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL21","STRATIXII_SRAM_ADDR[9]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL22","STRATIXII_SRAM_ADDR[10]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AJ22","STRATIXII_SRAM_ADDR[11]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AH22","STRATIXII_SRAM_ADDR[12]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL23","STRATIXII_SRAM_ADDR[13]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL24","STRATIXII_SRAM_ADDR[14]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AJ25","STRATIXII_SRAM_ADDR[15]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AH25","STRATIXII_SRAM_ADDR[16]"));
    sramAddr->push_back(
        new SignalMapping("PIN_AL25","STRATIXII_SRAM_ADDR[17]"));
    stratix2Pins_["STRATIXII_SRAM_ADDR"] = sramAddr;

    // sram control signals
    SignalMappingList* sramWe = new SignalMappingList;
    sramWe->push_back(new SignalMapping("PIN_AH14","STRATIXII_SRAM_WE_N"));
    stratix2Pins_["STRATIXII_SRAM_WE_N"] = sramWe;

    SignalMappingList* sramOe = new SignalMappingList;
    sramOe->push_back(new SignalMapping("PIN_AG14","STRATIXII_SRAM_OE_N"));
    stratix2Pins_["STRATIXII_SRAM_OE_N"] = sramOe;

    SignalMappingList* sramCs = new SignalMappingList;
    sramCs->push_back(new SignalMapping("PIN_AL12","STRATIXII_SRAM_CS_N"));
    stratix2Pins_["STRATIXII_SRAM_CS_N"] = sramCs;

    SignalMappingList* sramB0 = new SignalMappingList;
    SignalMappingList* sramB1 = new SignalMappingList;
    SignalMappingList* sramB2 = new SignalMappingList;
    SignalMappingList* sramB3 = new SignalMappingList;
    sramB0->push_back(new SignalMapping("PIN_AG11","STRATIXII_SRAM_BE_N0"));
    sramB1->push_back(new SignalMapping("PIN_AK10","STRATIXII_SRAM_BE_N1"));
    sramB2->push_back(new SignalMapping("PIN_AK11","STRATIXII_SRAM_BE_N2"));
    sramB3->push_back(new SignalMapping("PIN_AL11","STRATIXII_SRAM_BE_N3"));
    stratix2Pins_["STRATIXII_SRAM_BE_N0"] = sramB0;
    stratix2Pins_["STRATIXII_SRAM_BE_N1"] = sramB1;
    stratix2Pins_["STRATIXII_SRAM_BE_N2"] = sramB2;
    stratix2Pins_["STRATIXII_SRAM_BE_N3"] = sramB3;
}
