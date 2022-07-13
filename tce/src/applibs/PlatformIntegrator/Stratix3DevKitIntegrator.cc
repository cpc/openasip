/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file Stratix3DevKitIntegrator.hh
 *
 * Implementation of Stratix3DevKitIntegrator class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include "Exception.hh"
#include "Stratix3DevKitIntegrator.hh"
#include "Netlist.hh"
#include "NetlistPort.hh"
#include "NetlistBlock.hh"
using ProGe::Netlist;
using ProGe::NetlistPort;
using ProGe::NetlistBlock;
using PlatInt::SignalMapping;
using PlatInt::SignalMappingList;
using std::vector;
using std::endl;

const TCEString Stratix3DevKitIntegrator::DEVICE_FAMILY_ = "Stratix III";

const TCEString Stratix3DevKitIntegrator::DEVICE_NAME_ = "EP3SL150F1152C2";

const TCEString Stratix3DevKitIntegrator::DEVICE_PACKAGE_ = "F1152";

const TCEString Stratix3DevKitIntegrator::DEVICE_SPEED_CLASS_ = "2";

const TCEString Stratix3DevKitIntegrator::PIN_TAG_ = "STRATIXIII";

const int Stratix3DevKitIntegrator::DEFAULT_FREQ_ = 125;


Stratix3DevKitIntegrator::Stratix3DevKitIntegrator():
    AlteraIntegrator(), quartusGen_(NULL) {
}


Stratix3DevKitIntegrator::Stratix3DevKitIntegrator(
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
    quartusGen_(new QuartusProjectGenerator(coreEntityName, this)) {
    setDeviceName(DEVICE_NAME_);
}


Stratix3DevKitIntegrator::~Stratix3DevKitIntegrator() {

    for (PlatInt::PinMap::iterator iter = stratix3Pins_.begin();
         iter != stratix3Pins_.end(); iter++) {
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
Stratix3DevKitIntegrator::integrateProcessor(
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

    
TCEString
Stratix3DevKitIntegrator::deviceFamily() const {
    
    return DEVICE_FAMILY_;
}


void
Stratix3DevKitIntegrator::setDeviceFamily(TCEString devFamily) {
    
    if (devFamily != DEVICE_FAMILY_) {
        warningStream() 
            << "Warning: Refusing to change device family!" << endl
            << "- Original device family: " << DEVICE_FAMILY_ << endl
            << "- New device family: " << devFamily << endl;
    }
}


TCEString
Stratix3DevKitIntegrator::devicePackage() const {

    return DEVICE_PACKAGE_;
}


TCEString
Stratix3DevKitIntegrator::deviceSpeedClass() const {

    return DEVICE_SPEED_CLASS_;
}


int
Stratix3DevKitIntegrator::targetClockFrequency() const {

    int freq = DEFAULT_FREQ_;

    if (PlatformIntegrator::targetClockFrequency() > 0) {
        freq = PlatformIntegrator::targetClockFrequency();
    }
    return freq;
}


void
Stratix3DevKitIntegrator::printInfo(std::ostream& stream) const {

    stream
         << "Integrator name: Stratix3DevKit" << std::endl
         << "---------------------------------" << std::endl
         << "Integrates TTA core to Altera Stratix III Development Kit"
         << std::endl << "with" << DEVICE_NAME_ << " device." << std::endl
         << "Creates project files for QuartusII v8.0 program." << std::endl
         << "Supported instruction memory types are 'onchip' and 'vhdl_array."
         << std::endl
         << "Supported data memory type is 'onchip'."
         << std::endl
         << "Default clock frequency is 125 MHz." << std::endl
         << "Active low reset is connected to CPU RESET button."
         << std::endl << std::endl;
}


TCEString
Stratix3DevKitIntegrator::pinTag() const {

    return PIN_TAG_;
}


bool
Stratix3DevKitIntegrator::chopTaggedSignals() const {

    return true;
}


ProjectFileGenerator*
Stratix3DevKitIntegrator::projectFileGenerator() const {

    return quartusGen_;
}


void
Stratix3DevKitIntegrator::mapToplevelPorts() {
    NetlistBlock& tl = *integratorBlock();
    for (size_t i = 0; i < tl.portCount(); i++) {
        addSignalMapping(tl.port(i).name());
    }
}


void
Stratix3DevKitIntegrator::addSignalMapping(const TCEString& signal) {
    
    if (stratix3Pins_.find(signal) == stratix3Pins_.end()) {
        warningStream() << "Warning: didn't find mapping for signal name "
                        << signal << endl;
        return;
    }

    SignalMappingList* mappings = stratix3Pins_.find(signal)->second;
    for (unsigned int i = 0; i < mappings->size(); i++) {
        quartusGen_->addSignalMapping(*mappings->at(i));
    }
}


void
Stratix3DevKitIntegrator::generatePinMap() {

    // clk
    SignalMappingList* clk = new SignalMappingList;
    clk->push_back(new SignalMapping("PIN_B16","clk"));
    stratix3Pins_["clk"] = clk;

    // reset to push button CPU_RESET
    SignalMappingList* rstx = new SignalMappingList;
    rstx->push_back(new SignalMapping("PIN_AP5","rstx"));
    stratix3Pins_["rstx"] = rstx;

    // leds
    const TCEString led = PIN_TAG_+"_LED";
    SignalMappingList* ledMapping = new SignalMappingList;
    ledMapping->push_back(new SignalMapping("PIN_F21", led+"[0]"));
    ledMapping->push_back(new SignalMapping("PIN_C23", led+"[1]"));
    ledMapping->push_back(new SignalMapping("PIN_B23", led+"[2]"));
    ledMapping->push_back(new SignalMapping("PIN_A23", led+"[3]"));
    ledMapping->push_back(new SignalMapping("PIN_D19", led+"[4]"));
    ledMapping->push_back(new SignalMapping("PIN_C19", led+"[5]"));
    ledMapping->push_back(new SignalMapping("PIN_F19", led+"[6]"));
    ledMapping->push_back(new SignalMapping("PIN_E19", led+"[7]"));
    stratix3Pins_[led] = ledMapping;

    // dip switches
    const TCEString sw = PIN_TAG_+"_USER_DIPSW";
    SignalMappingList* switchMapping = new SignalMappingList;
    switchMapping->push_back(new SignalMapping("PIN_B19", sw+"[0]"));
    switchMapping->push_back(new SignalMapping("PIN_A19", sw+"[1]"));
    switchMapping->push_back(new SignalMapping("PIN_C18", sw+"[2]"));
    switchMapping->push_back(new SignalMapping("PIN_A20", sw+"[3]"));
    switchMapping->push_back(new SignalMapping("PIN_K19", sw+"[4]"));
    switchMapping->push_back(new SignalMapping("PIN_J19", sw+"[5]"));
    switchMapping->push_back(new SignalMapping("PIN_L19", sw+"[6]"));
    switchMapping->push_back(new SignalMapping("PIN_L20", sw+"[7]"));
    stratix3Pins_[sw] = switchMapping;

    // push buttons
    const TCEString pb = PIN_TAG_+"_USER_PB";
    SignalMappingList* pbMapping = new SignalMappingList;
    pbMapping->push_back(new SignalMapping("PIN_B17", pb+"[0]"));
    pbMapping->push_back(new SignalMapping("PIN_A17", pb+"[1]"));
    pbMapping->push_back(new SignalMapping("PIN_A16", pb+"[2]"));
    pbMapping->push_back(new SignalMapping("PIN_K17", pb+"[3]"));
    stratix3Pins_[pb] = pbMapping;
    
}
