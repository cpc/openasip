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
#include "MachineImplementation.hh"
using std::vector;
using std::endl;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;
using TTAMachine::FunctionUnit;

const TCEString PlatformIntegrator::TTA_CORE_CLK = "clk";
const TCEString PlatformIntegrator::TTA_CORE_RSTX = "rstx";


PlatformIntegrator::PlatformIntegrator():
    machine_(NULL), idf_(NULL), netlist_(NULL),  hdl_(ProGe::VHDL),
    progeOutputDir_(""), coreEntityName_(""), outputDir_(""), 
    programName_(""), targetFrequency_(0), warningStream_(std::cout),
    errorStream_(std::cerr), ttaCore_(NULL), imem_(), dmem_() {
    
    imem_.type = UNKNOWN;
    dmem_.type = UNKNOWN;

    imemSignals_.push_back("imem_");
    imemSignals_.push_back("pc_init");
    imemSignals_.push_back("busy");
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
    const MemInfo& dmem): 
    machine_(machine), idf_(idf), netlist_(new ProGe::Netlist()), hdl_(hdl),
    progeOutputDir_(progeOutputDir), coreEntityName_(coreEntityName),
    outputDir_(outputDir), programName_(programName),
    targetFrequency_(targetClockFreq), warningStream_(warningStream),
    errorStream_(errorStream), ttaCore_(NULL), imem_(imem),
    dmem_(dmem) {

    imemSignals_.push_back("imem_");
    imemSignals_.push_back("pc_init");
    imemSignals_.push_back("busy"); 

    netlist_->setCoreEntityName(coreEntityName_);
   
    createOutputDir();
}


PlatformIntegrator::~PlatformIntegrator() {
    
    delete netlist_;
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
        std::cerr << "Error: " << e.errorMessage() << std::endl;
    }
    
    try {
        bool foundImemMau = false;
        TCEString imemMau = "imem_mau_pkg.vhdl";

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
        std::cerr << "Error: " << e.errorMessage() << std::endl;
        throw e;
    }
}


void
PlatformIntegrator::createOutputDir() {

    TCEString absolute = FileSystem::absolutePathOf(outputDir_);
    if (!FileSystem::createDirectory(absolute)) {
        IOException exc(__FILE__, __LINE__, "PlatformIntegrator",
                        "Couldn't create dir " + absolute);
        throw exc;
    }
}


std::ostream&
PlatformIntegrator::warningStream() {

    return warningStream_;
}


std::ostream&
PlatformIntegrator::errorStream() {

    return errorStream_;
}


int
PlatformIntegrator::targetClockFrequency() const {

    return targetFrequency_;
}


ProGe::Netlist*
PlatformIntegrator::netlist() {
    
    assert(netlist_ != NULL);
    return netlist_;
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

bool
PlatformIntegrator::createPorts(const ProGe::NetlistBlock* ttaCore) {

    NetlistBlock* highestBlock =
        new NetlistBlock(
            platformEntityName(), platformEntityName(), *netlist());
    
    // Must add ports to highest block *before* copying tta toplevel
    NetlistPort* clk = new NetlistPort(TTA_CORE_CLK, "0", 1, ProGe::BIT,
                                       HDB::IN,*highestBlock);
    NetlistPort* rstx = new NetlistPort(TTA_CORE_RSTX, "0", 1, ProGe::BIT,
                                        HDB::IN, *highestBlock);
    copyTTACoreToNetlist(ttaCore);

    const NetlistBlock& core = ttaCoreBlock();
    NetlistPort* coreClk = core.portByName(TTA_CORE_CLK);
    NetlistPort* coreRstx = core.portByName(TTA_CORE_RSTX);
    netlist()->connectPorts(*clk, *coreClk);
    netlist()->connectPorts(*rstx, *coreRstx);
    
    for (int i = 0; i < core.portCount(); i++) {
        TCEString portName = core.port(i).name();
        if (portName == TTA_CORE_CLK || portName == TTA_CORE_RSTX) {
            continue;
        } else if (!isInstructionMemorySignal(portName) 
            && !isDataMemorySignal(portName)) {

            TCEString toplevelName = portName;
            if (chopTaggedSignals() && hasPinTag(portName)) {
                toplevelName = chopSignalToTag(portName, pinTag());
            }
            connectToplevelPort(toplevelName, core.port(i));
        }
    }
    return true;
}


void
PlatformIntegrator::connectToplevelPort(
    const TCEString& toplevelName, ProGe::NetlistPort& corePort) {

    NetlistPort* topPort = NULL;
    if (corePort.realWidthAvailable()) {
        int width = corePort.realWidth();
        if (width == 0 || width == 1) {
            topPort = new NetlistPort(
                toplevelName, corePort.widthFormula(), corePort.realWidth(),
                ProGe::BIT, corePort.direction(),
                netlist()->topLevelBlock());
        } else {
            topPort = new NetlistPort(
                toplevelName, corePort.widthFormula(), corePort.realWidth(),
                ProGe::BIT_VECTOR, corePort.direction(),
                netlist()->topLevelBlock());
        }
    } else {
        topPort = new NetlistPort(
            toplevelName, corePort.widthFormula(), corePort.dataType(),
            corePort.direction(), netlist()->topLevelBlock());
    }
    if (topPort->dataType() == corePort.dataType()) {
        netlist()->connectPorts(*topPort, corePort);
    } else {
        netlist()->connectPorts(*topPort, corePort, 0, 0, 1);
    }
}


bool
PlatformIntegrator::hasPinTag(const TCEString& signal) const {
    
    return signal.find(pinTag()) != TCEString::npos;
}


bool
PlatformIntegrator::isInstructionMemorySignal(
    const TCEString& signalName) const {

    bool isMatch = false;
    for (unsigned int i = 0; i < imemSignals_.size(); i++) {
        if (signalName.find(imemSignals_.at(i)) != TCEString::npos) {
            isMatch = true;
            break;
        }
    }
    return isMatch;
}


void
PlatformIntegrator::copyTTACoreToNetlist(
    const ProGe::NetlistBlock* original) {
    
    ttaCore_ = original->copyToNewNetlist("core", *netlist_);
    NetlistBlock& top = netlist_->topLevelBlock();
    top.addSubBlock(ttaCore_);

    // copy parameters to the current toplevel
    for (int i = 0; i < ttaCore_->parameterCount(); i++) {
        top.setParameter(ttaCore_->parameter(i));
    }
}


const ProGe::NetlistBlock&
PlatformIntegrator::ttaCoreBlock() const {
    
    assert(ttaCore_ != NULL);
    return *ttaCore_;
}


const ProGe::NetlistBlock&
PlatformIntegrator::toplevelBlock() const {

    return netlist_->topLevelBlock();
}


bool
PlatformIntegrator::createMemories() {

    assert(imem_.type != UNKNOWN);
    assert(dmem_.type != UNKNOWN);

    int imemIndex = 0;
    MemoryGenerator* imemGen = imemInstance();
    vector<TCEString> imemFiles;
    if (!generateMemory(*imemGen, imemFiles, imemIndex)) {
        delete imemGen;
        return false;
    }
    if (imemFiles.size() != 0) {
        projectFileGenerator()->addHdlFiles(imemFiles);
    }
    delete imemGen;

    int dmemIndex = 0;
    bool success = true;
    if (dmem_.type != NONE) {
        MemoryGenerator* dmemGen = dmemInstance();
        vector<TCEString> dmemFiles;
        success = generateMemory(*dmemGen, dmemFiles, dmemIndex);
        if (dmemFiles.size() != 0) {
            projectFileGenerator()->addHdlFiles(dmemFiles);
        }
        delete dmemGen;
    }
    return success;
}

bool
PlatformIntegrator::generateMemory(
    MemoryGenerator& memGen,
    std::vector<TCEString>& generatedFiles, 
    int index) {

    vector<TCEString> reasons;
    if (!memGen.isCompatible(ttaCoreBlock(), reasons)) {
        errorStream() << "TTA core doesn't have compatible memory "
                      <<"interface:" << std::endl;
        for (unsigned int i = 0; i < reasons.size(); i++) {
            errorStream() << reasons.at(i) << std::endl;
        }
        return false;
    }

    memGen.addMemory(*netlist(), index);

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
        writer = new ProGe::VHDLNetlistWriter(*netlist_);
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
        FileNotFound exc(__FILE__, __LINE__, "platformIntegrator", msg);
        throw exc;
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
PlatformIntegrator::dmemInfo() const {

    return dmem_;
}
