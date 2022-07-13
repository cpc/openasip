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
 * @file PlatformIntegerator.cc
 *
 * Implementation of PlatformIntegrator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <vector>
#include <string>
#include "PlatformIntegrator.hh"
#include "MemoryGenerator.hh"
#include "FileSystem.hh"
#include "StringTools.hh"
#include "Exception.hh"
#include "Netlist.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "VHDLNetlistWriter.hh"
#include "ProjectFileGenerator.hh"
#include "Machine.hh"
#include "HWOperation.hh"
#include "MachineImplementation.hh"
#include "FUPort.hh"
#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "MathTools.hh"
#include "HDBManager.hh"
#include "HDBRegistry.hh"
#include "FUImplementation.hh"
#include "FUExternalPort.hh"
using std::vector;
using std::endl;
using std::multimap;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;
using ProGe::Netlist;
using TTAMachine::FunctionUnit;

const TCEString PlatformIntegrator::TTA_CORE_CLK = "clk";
const TCEString PlatformIntegrator::TTA_CORE_RSTX = "rstx";


PlatformIntegrator::PlatformIntegrator():
    machine_(NULL), idf_(NULL), integratorBlock_(NULL),  hdl_(ProGe::VHDL),
    progeOutputDir_(""), coreEntityName_(""), outputDir_(""), 
    programName_(""), targetFrequency_(0), warningStream_(std::cout),
    errorStream_(std::cerr), ttaCores_(NULL), imem_(), dmemType_(UNKNOWN),
    dmem_(), lsus_(), clkPort_(NULL), resetPort_(NULL),
    unconnectedPorts_(NULL) {
    
    imem_.type = UNKNOWN;
}


PlatformIntegrator::PlatformIntegrator(
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
    machine_(machine), idf_(idf), integratorBlock_(NULL), hdl_(hdl),
    progeOutputDir_(progeOutputDir), sharedOutputDir_(""), 
    coreEntityName_(coreEntityName), outputDir_(outputDir),
    programName_(programName), targetFrequency_(targetClockFreq),
    warningStream_(warningStream), errorStream_(errorStream), ttaCores_(NULL),
    imem_(imem), dmemType_(dmemType), dmem_(), lsus_(), clkPort_(NULL),
    resetPort_(NULL), unconnectedPorts_(NULL) {

    integratorBlock_ =
        new NetlistBlock(platformEntityName(), platformEntityName() + "inst");

    createOutputDir();
}


PlatformIntegrator::~PlatformIntegrator() {
    delete integratorBlock_;
}


TCEString 
PlatformIntegrator::coreEntityName() const {
    
    return coreEntityName_;
}


TCEString
PlatformIntegrator::programName() const {

    return programName_;
}


TCEString 
PlatformIntegrator::progeFilePath(TCEString fileName, bool absolute) const {
 
    TCEString pathToFile =
        progeOutputDir_ + FileSystem::DIRECTORY_SEPARATOR + fileName;

    if (absolute) {
        pathToFile = FileSystem::absolutePathOf(pathToFile);
    }

    return pathToFile;
}


TCEString
PlatformIntegrator::outputFilePath(
    TCEString fileName, bool absolute) const {

    TCEString pathToFile =
        outputDir_ + FileSystem::DIRECTORY_SEPARATOR + fileName;
    
    if (absolute) {
        pathToFile = FileSystem::absolutePathOf(pathToFile);
    }
    
    return pathToFile;
}


void
PlatformIntegrator::setSharedOutputDir(const TCEString& sharedDir) {

    if (FileSystem::absolutePathOf(sharedDir) != 
        FileSystem::absolutePathOf(progeOutputDir_)) {
        sharedOutputDir_ = sharedDir;
    }
}


TCEString
PlatformIntegrator::outputPath() const {
    return outputDir_;
}


TCEString
PlatformIntegrator::chopSignalToTag(
    const TCEString& original,
    const TCEString& tag) const {

    TCEString signal = original;
    if (original.find(tag) != TCEString::npos) {
        signal = original.substr(original.find(tag));
    }
    return StringTools::trim(signal);
}


void 
PlatformIntegrator::progeOutputHdlFiles(
    std::vector<TCEString>& files) const {

    bool makeAbsolute = false;
    try {
        TCEString gcuPath = 
            progeOutputDir_ + FileSystem::DIRECTORY_SEPARATOR + "gcu_ic";
        std::vector<std::string> gcuFiles = 
            FileSystem::directoryContents(gcuPath, makeAbsolute);

        for (unsigned int i = 0; i < gcuFiles.size(); i++) {
            files.push_back(gcuFiles.at(i));
        }
    } catch (FileNotFound& e) {
        errorStream() << "Error: " << e.errorMessage() << std::endl;
        throw e;
    }
    
    try {
        bool foundImemMau = false;
        TCEString imemMau = coreEntityName() + "_imem_mau_pkg.vhdl";

        TCEString vhdlPath =
            progeOutputDir_ + FileSystem::DIRECTORY_SEPARATOR + "vhdl";
        std::vector<std::string> vhdlFiles =
            FileSystem::directoryContents(vhdlPath, makeAbsolute);
        for (unsigned int i = 0; i < vhdlFiles.size(); i++) {
            if (vhdlFiles.at(i).find(imemMau) != TCEString::npos) {
                foundImemMau = true;
            }
            files.push_back(vhdlFiles.at(i));
        }
        
        // imem_mau_pkg was not yet present, but add it to file list
        if (!foundImemMau) {
            TCEString path = 
                vhdlPath + FileSystem::DIRECTORY_SEPARATOR + imemMau;
            files.push_back(path);
        }
    } catch (FileNotFound& e) {
        errorStream() << "Error: " << e.errorMessage() << std::endl;
        throw e;
    }

    if (!sharedOutputDir_.empty()) {
        try {
            std::string sharedVhdl =
                sharedOutputDir_ + FileSystem::DIRECTORY_SEPARATOR + "vhdl";
            std::vector<std::string> sharedFiles = 
                FileSystem::directoryContents(sharedVhdl, makeAbsolute);
            for (unsigned int i = 0; i < sharedFiles.size(); i++) {
                files.push_back(sharedFiles.at(i));
            }
        } catch (FileNotFound& e) {
            errorStream() << "Error: " << e.errorMessage() << std::endl;
            throw e;
        }
    }
}


void
PlatformIntegrator::createOutputDir() {

    TCEString absolute = FileSystem::absolutePathOf(outputDir_);
    if (!FileSystem::createDirectory(absolute)) {
        throw IOException(__FILE__, __LINE__, "PlatformIntegrator",
                          "Couldn't create dir " + absolute);
    }
}


std::ostream&
PlatformIntegrator::warningStream() const {

    return warningStream_;
}


std::ostream&
PlatformIntegrator::errorStream() const {

    return errorStream_;
}


int
PlatformIntegrator::targetClockFrequency() const {

    return targetFrequency_;
}


ProGe::NetlistBlock*
PlatformIntegrator::integratorBlock() {
    assert(integratorBlock_ != NULL);
    return integratorBlock_;
}

const TTAMachine::Machine*
PlatformIntegrator::machine() const {

    return machine_;
}


const IDF::MachineImplementation*
PlatformIntegrator::idf() const {

    return idf_;
}


TCEString
PlatformIntegrator::platformEntityName() const {

    return coreEntityName() + "_toplevel";
}

void
PlatformIntegrator::initPlatformNetlist(
    const ProGe::NetlistBlock* progeBlock) {

    NetlistBlock* highestBlock = integratorBlock_;
    // Must add ports to highest block *before* copying tta toplevel
    clkPort_ = new NetlistPort(TTA_CORE_CLK, "0", 1, ProGe::BIT,
        ProGe::IN, *highestBlock);
    resetPort_ = new NetlistPort(TTA_CORE_RSTX, "0", 1, ProGe::BIT,
        ProGe::IN, *highestBlock);
    copyProgeBlockToNetlist(progeBlock);

    if (dmemType_ != NONE) {
        parseDataMemories();
    }
}


void
PlatformIntegrator::parseDataMemories() {

    TTAMachine::Machine::FunctionUnitNavigator fuNav =
        machine_->functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        if (fu->hasAddressSpace()) {
            TTAMachine::AddressSpace* as = fu->addressSpace();

            bool isLSU = false;
            for (int i = 0; i < fu->operationCount(); ++i) {
                std::string operation = fu->operation(i)->name();
                std::string prefix = operation.substr(0, 2);
                if (prefix == "ld" || prefix == "st") {
                    isLSU = true;
                    break;
                }
            }

            if (isLSU) {
                dmem_[as] = readLsuParameters(*fu);
                lsus_.push_back(fu);
            }
        }
    }
}

void
PlatformIntegrator::clearDataMemories() {
    dmem_.clear();
    lsus_.clear();
}


std::vector<std::string>
PlatformIntegrator::loadFUExternalPorts(TTAMachine::FunctionUnit& fu) const {

    if (idf()->hasFUImplementation(fu.name())) {
        IDF::FUImplementationLocation location =
            idf()->fuImplementation(fu.name());
        TCEString hdb = location.hdbFile();
        int id = location.id();
        HDB::HDBManager& manager = HDB::HDBRegistry::instance().hdb(hdb);
        HDB::FUEntry* entry = manager.fuByEntryID(id);

        if (!entry->hasImplementation()) {
            TCEString msg = "HDB entry for " + fu.name() + " does not contain "
                + "implementation!";
            throw InvalidData(__FILE__, __LINE__, "PlatformIntegrator", msg);
        }

        auto implementation = entry->implementation();

        std::vector<std::string> ports;
        for (int i = 0; i > implementation.externalPortCount(); ++i) {
            ports.push_back(implementation.externalPort(i).name());
        }
        return ports;
    } else if (idf()->hasFUGeneration(fu.name())) {
        // quick fix: assume AlmaIF ports
        std::vector<std::string> ports = {
            "avalid_out", "aready_in", "aaddr_out", "awren_out",
            "astrb_out", "rvalid_in", "rready_out", "rdata_in", "adata_out"};
        return ports;
    } else {
        TCEString msg = "Function Unit " + fu.name() + " does not have an "
            + "implementation!";
        throw InvalidData(__FILE__, __LINE__, "PlatformIntegrator", msg);
    }


}


MemInfo
PlatformIntegrator::readLsuParameters(const TTAMachine::FunctionUnit& lsu) {
    
    MemInfo dmem;
    dmem.type = dmemType_;
    dmem.mauWidth = lsu.addressSpace()->width();
    unsigned int internalAddrw =
        MathTools::requiredBits(lsu.addressSpace()->end());
    unsigned int dataWidth = 0;
    for (int i = 0; i < lsu.operationPortCount(); i++) {
        TTAMachine::FUPort* port = lsu.operationPort(i);
        if (port->isInput() && !port->isTriggering()) {
            dataWidth = std::max((unsigned)port->width(), dataWidth);
        }
    }
    dmem.widthInMaus = static_cast<int>(
        ceil(static_cast<double>(dataWidth)/dmem.mauWidth));
     int bytemaskWidth = 0;
    if (dmem.widthInMaus > 1) {
        unsigned int maus = static_cast<unsigned int>(dmem.widthInMaus) - 1;
        bytemaskWidth = MathTools::requiredBits(maus);
    }
    dmem.portAddrw = internalAddrw - bytemaskWidth;
    unsigned long int lastAddr = lsu.addressSpace()->end();
    dmem.asAddrw = MathTools::requiredBits(lastAddr);
    dmem.asName = lsu.addressSpace()->name();
    dmem.lsuName = lsu.name();
    return dmem;
}


bool
PlatformIntegrator::integrateCore(
    const ProGe::NetlistBlock& cores,
    int coreId) {
    
    TCEString clkPortName = PlatformIntegrator::TTA_CORE_CLK;
    TCEString resetPortName = PlatformIntegrator::TTA_CORE_RSTX;
    const NetlistPort* coreClk = cores.port(clkPortName);
    const NetlistPort* coreRstx = cores.port(resetPortName);
    integratorBlock()->netlist().connect(*clockPort(), *coreClk);
    integratorBlock()->netlist().connect(*resetPort(), *coreRstx);
    
    if(!createMemories(coreId)) {
        return false;
    }

    exportUnconnectedPorts(coreId);

    return true;
}

void
PlatformIntegrator::exportUnconnectedPorts(int coreId) {

    const NetlistBlock& core = progeBlock();
    for (size_t i = 0; i < core.portCount(); i++) {
        const NetlistPort& port = core.port(i);
        if (!integratorBlock()->netlist().isPortConnected(port)) {
            connectToplevelPort(port);
        }
    }
}

void
PlatformIntegrator::connectToplevelPort(const ProGe::NetlistPort& corePort,
                                        const TCEString signalPrefix) {

    TCEString toplevelName = corePort.name();
    if (chopTaggedSignals() && hasPinTag(toplevelName)) {
        toplevelName = signalPrefix + chopSignalToTag(toplevelName, pinTag());
    }
    NetlistPort* topPort = NULL;
    if (corePort.realWidthAvailable()) {
        int width = corePort.realWidth();
        if (width == 0 || width == 1) {
            topPort = new NetlistPort(
                toplevelName, corePort.widthFormula(), corePort.realWidth(),
                ProGe::BIT, corePort.direction(),
                *integratorBlock());
        } else {
            topPort = new NetlistPort(
                toplevelName, corePort.widthFormula(), corePort.realWidth(),
                ProGe::BIT_VECTOR, corePort.direction(),
                *integratorBlock());
        }
    } else {
        topPort = new NetlistPort(
            toplevelName, corePort.widthFormula(), corePort.dataType(),
            corePort.direction(), *integratorBlock());
    }
    if (topPort->dataType() == corePort.dataType()) {
        integratorBlock()->netlist().connect(*topPort, corePort);
    } else {
        integratorBlock()->netlist().connect(*topPort, corePort, 0, 0, 1);
    }
}


bool
PlatformIntegrator::hasPinTag(const TCEString& signal) const {
    
    return signal.find(pinTag()) != TCEString::npos;
}


void
PlatformIntegrator::copyProgeBlockToNetlist(
    const ProGe::NetlistBlock* progeBlock) {
    
    ttaCores_ = progeBlock->shallowCopy("core");
    NetlistBlock& top = *integratorBlock_;
    top.addSubBlock(ttaCores_);

    // copy parameters to the current toplevel
    for (size_t i = 0; i < ttaCores_->parameterCount(); i++) {

        // Filter out some parameters not usable in the integrator block.
        if (ttaCores_->parameter(i).name() == "core_id") {
            continue;
        }

        top.setParameter(ttaCores_->parameter(i));
    }

    // copy package references to the current toplevel
    for (size_t i = 0; i < ttaCores_->packageCount(); i++) {
        top.addPackage(ttaCores_->package(i));
    }

    for (size_t i = 0; i < ttaCores_->netlist().parameterCount(); i++) {
        top.netlist().setParameter(ttaCores_->netlist().parameter(i));
    }
}


const ProGe::NetlistBlock&
PlatformIntegrator::progeBlock() const {
    
    assert(ttaCores_ != NULL);
    return *ttaCores_;
}


const ProGe::NetlistBlock&
PlatformIntegrator::toplevelBlock() const {

    return *integratorBlock_;
}


bool
PlatformIntegrator::createMemories(int coreId) {

    assert(imem_.type != UNKNOWN);

    int imemIndex = 0;
    MemoryGenerator& imemGen = imemInstance(imem_, coreId);
    vector<TCEString> imemFiles;

    if (imem_.type != NONE) {
        if (!generateMemory(imemGen, imemFiles, imemIndex, coreId)) {
            return false;
        }
        if (imemFiles.size() != 0) {
            projectFileGenerator()->addHdlFiles(imemFiles);
        }
    }

    for (unsigned int i = 0; i < lsus_.size(); i++) {
        TTAMachine::FunctionUnit* lsuArch = lsus_.at(i);
        std::vector<std::string> ports = loadFUExternalPorts(*lsuArch);

        TTAMachine::AddressSpace* as = lsuArch->addressSpace();
        assert(dmem_.find(as) != dmem_.end() && "Address space not found!");
        
        MemoryGenerator& dmemGen =
            dmemInstance(dmem_.find(as)->second, *lsuArch, ports);
        vector<TCEString> dmemFiles;
        if (!generateMemory(dmemGen, dmemFiles, i, coreId)) {
            return false;
        }
        if (dmemFiles.size() != 0) {
            projectFileGenerator()->addHdlFiles(dmemFiles);
        }
    }
    return true;
}


bool
PlatformIntegrator::generateMemory(
    MemoryGenerator& memGen,
    std::vector<TCEString>& generatedFiles,
    int memIndex,
    int coreId) {

    const NetlistBlock& ttaCores = progeBlock();

    vector<TCEString> reasons;
    if (!memGen.isCompatible(ttaCores, coreId, reasons)) {
        errorStream() << "TTA core doesn't have compatible memory "
                      <<"interface:" << std::endl;
        for (unsigned int i = 0; i < reasons.size(); i++) {
            errorStream() << reasons.at(i) << std::endl;
        }
        return false;
    }

    memGen.addMemory(ttaCores, *integratorBlock(), memIndex, coreId);

    if (memGen.generatesComponentHdlFile()) {
        generatedFiles =
            memGen.generateComponentFile(outputPath());
        if (generatedFiles.size() == 0) {
            errorStream() << "Failed to create mem component" << endl;
            return false;
        }
    }

    return true;
}


void
PlatformIntegrator::writeNewToplevel() {
    
    ProGe::NetlistWriter* writer;
    if (hdl_ == ProGe::VHDL) {
        writer = new ProGe::VHDLNetlistWriter(*integratorBlock_);
    } else {
        assert(false);
    }

    TCEString platformDir = outputPath();
    writer->write(platformDir);
    delete writer;

    
    TCEString toplevelFile =
        outputFilePath(coreEntityName() + "_toplevel.vhdl");
    if (!FileSystem::fileExists(toplevelFile)) {
        TCEString msg = "NetlistWriter failed to create file " + toplevelFile;
        throw FileNotFound(__FILE__, __LINE__, "platformIntegrator", msg);
    }
    projectFileGenerator()->addHdlFile(toplevelFile);

    TCEString paramFile =
        outputFilePath(platformEntityName() + "_params_pkg.vhdl");
    if (FileSystem::fileExists(paramFile)) {
        projectFileGenerator()->addHdlFile(paramFile);
    }
}


void
PlatformIntegrator::addProGeFiles() const {

    vector<TCEString> progeOutFiles;
    progeOutputHdlFiles(progeOutFiles);
    for (unsigned int i = 0; i < progeOutFiles.size(); i++) {
        projectFileGenerator()->addHdlFile(progeOutFiles.at(i));
    }
}


const MemInfo&
PlatformIntegrator::imemInfo() const {

    return imem_;
}
 
const MemInfo&
PlatformIntegrator::dmemInfo(TTAMachine::AddressSpace* as) const {

    if (as == NULL || dmem_.find(as) == dmem_.end()) {
        TCEString msg = "Invalid address space";
        throw InvalidData(__FILE__, __LINE__, "PlatformIntegrator", msg);
    }
    return dmem_.find(as)->second;
}

const MemInfo&
PlatformIntegrator::dmemInfo(int index) const {

    if (index > static_cast<int>(dmem_.size())) {
        TCEString msg = "Data memory index out of range";
        throw OutOfRange(__FILE__, __LINE__, "PlatformIntegrator", msg);
    }
    std::map<TTAMachine::AddressSpace*, MemInfo>::const_iterator iter =
        dmem_.begin();
    int i = 0;
    while (i < index) {
        iter++;
        i++;
    }
    assert(iter != dmem_.end());
    return iter->second;
}

int
PlatformIntegrator::dmemCount() const {

    return dmem_.size();
}


ProGe::NetlistPort*
PlatformIntegrator::clockPort() const {

    if (clkPort_ == NULL) {
        TCEString msg;
        msg << "PlatformIntegrator was not initialized properly";
        throw ObjectNotInitialized(__FILE__, __LINE__, __func__, msg);
    }
    return clkPort_;
}


ProGe::NetlistPort*
PlatformIntegrator::resetPort() const {

    if (resetPort_ == NULL) {
        TCEString msg;
        msg << "PlatformIntegrator was not initialized properly";
        throw ObjectNotInitialized(__FILE__, __LINE__, __func__, msg);
    }
    return resetPort_;
}



