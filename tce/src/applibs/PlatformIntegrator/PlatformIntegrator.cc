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
 * @file PlatformIntegerator.hh
 *
 * Implementation of PlatformIntegrator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/format.hpp>
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
using std::string;
using std::vector;
using std::endl;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;

const std::string PlatformIntegrator::TTA_CORE_NAME = "toplevel";
const std::string PlatformIntegrator::TTA_CORE_CLK = "clk";
const std::string PlatformIntegrator::TTA_CORE_RSTX = "rstx";


PlatformIntegrator::PlatformIntegrator():
    netlist_(NULL),  hdl_(ProGe::VHDL), progeOutputDir_(""), entityName_(""),
    outputDir_(""), outputDirCreated_(false), programName_(""),
    targetFrequency_(0), warningStream_(std::cout), errorStream_(std::cerr),
    ttaCore_(NULL) {
    
}


PlatformIntegrator::PlatformIntegrator(
    ProGe::HDL hdl,
    std::string progeOutputDir,
    std::string entityName,
    std::string outputDir,
    std::string programName,
    int targetClockFreq,
    std::ostream& warningStream,
    std::ostream& errorStream): 
    netlist_(new ProGe::Netlist()), hdl_(hdl),
    progeOutputDir_(progeOutputDir), entityName_(entityName),
    outputDir_(outputDir), outputDirCreated_(false),
    programName_(programName), targetFrequency_(targetClockFreq),
    warningStream_(warningStream), errorStream_(errorStream), ttaCore_(NULL) {

}


PlatformIntegrator::~PlatformIntegrator() {
    
    delete netlist_;
}


std::string 
PlatformIntegrator::entityName() const {
    
    return entityName_;
}


std::string
PlatformIntegrator::programName() const {

    return programName_;
}


std::string 
PlatformIntegrator::progeFilePath(std::string fileName, bool absolute) const {
 
    std::string pathToFile =
        progeOutputDir_ + FileSystem::DIRECTORY_SEPARATOR + fileName;

    if (absolute) {
        pathToFile = FileSystem::absolutePathOf(pathToFile);
    }

    return pathToFile;
}


std::string
PlatformIntegrator::outputFilePath(std::string fileName, bool absolute) {

    if (!outputDirCreated_) {
        createOutputDir();
    }

    std::string pathToFile =
        outputDir_ + FileSystem::DIRECTORY_SEPARATOR + fileName;
    
    if (absolute) {
        pathToFile = FileSystem::absolutePathOf(pathToFile);
    }
    
    return pathToFile;
}


std::string
PlatformIntegrator::outputPath() {

    if (!outputDirCreated_) {
        createOutputDir();
    }

    return outputDir_;
}


std::string
PlatformIntegrator::chopSignalToTag(
    const std::string& original,
    const std::string& tag) const {

    string signal = original;
    if (original.find(tag) != string::npos) {
        signal = original.substr(original.find(tag));
    }
    return StringTools::trim(signal);
}


void 
PlatformIntegrator::progeOutputHdlFiles(
    std::vector<std::string>& files) const {

    bool makeAbsolute = false;
    try {
        std::string gcuPath = 
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
        string imemMau = "imem_mau_pkg.vhdl";

        std::string vhdlPath =
            progeOutputDir_ + FileSystem::DIRECTORY_SEPARATOR + "vhdl";
        std::vector<std::string> vhdlFiles =
            FileSystem::directoryContents(vhdlPath, makeAbsolute);
        for (unsigned int i = 0; i < vhdlFiles.size(); i++) {
            if (vhdlFiles.at(i).find(imemMau) != string::npos) {
                foundImemMau = true;
            }
            files.push_back(vhdlFiles.at(i));
        }
        
        // imem_mau_pkg was not yet present, but add it to file list
        if (!foundImemMau) {
            string path = 
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

    std::string absolute = FileSystem::absolutePathOf(outputDir_);
    if (!FileSystem::createDirectory(absolute)) {
        IOException exc(__FILE__, __LINE__, "PlatformIntegrator",
                        "Couldn't create dir " + absolute);
        throw exc;
    }
    outputDirCreated_ = true;
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


bool
PlatformIntegrator::createPorts(const ProGe::NetlistBlock* ttaCore) {

    NetlistBlock* highestBlock =
        new NetlistBlock(entityName(), entityName(), *netlist());
    
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
        string portName = core.port(i).name();
        if (hasPinTag(portName) && !isDataMemorySignal(portName)) {
            connectToplevelPort(core.port(i));
        }
    }
    return true;
}

void
PlatformIntegrator::connectToplevelPort(ProGe::NetlistPort& corePort) {

    string topName = chopSignalToTag(corePort.name(), pinTag());
    NetlistPort* topPort = NULL;
    if (corePort.realWidthAvailable()) {
        int width = corePort.realWidth();
        if (width == 0 || width == 1) {
            topPort = new NetlistPort(
                topName, corePort.widthFormula(), corePort.realWidth(),
                ProGe::BIT, corePort.direction(),
                netlist()->topLevelBlock());
        } else {
            topPort = new NetlistPort(
                topName, corePort.widthFormula(), corePort.realWidth(),
                ProGe::BIT_VECTOR, corePort.direction(),
                netlist()->topLevelBlock());
        }
    } else {
        topPort = new NetlistPort(
            topName, corePort.widthFormula(), corePort.dataType(),
            corePort.direction(), netlist()->topLevelBlock());
    }
    if (topPort->dataType() == corePort.dataType()) {
        netlist()->connectPorts(*topPort, corePort);
    } else {
        netlist()->connectPorts(*topPort, corePort, 0, 0, 1);
    }
}


bool
PlatformIntegrator::hasPinTag(const std::string& signal) const {
    
    return signal.find(pinTag()) != string::npos;
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
PlatformIntegrator::createMemories(const MemInfo& imem,
                                   const MemInfo& dmem) {
    
    MemoryGenerator* imemGen = imemInstance(imem);
    vector<string> imemFiles;
    if (!generateMemory(*imemGen, imemFiles)) {
        delete imemGen;
        return false;
    }
    if (imemFiles.size() != 0) {
        projectFileGenerator()->addHdlFiles(imemFiles);
    }
    delete imemGen;

    MemoryGenerator* dmemGen = dmemInstance(dmem);
    vector<string> dmemFiles;
    bool success = generateMemory(*dmemGen, dmemFiles);
    if (dmemFiles.size() != 0) {
        projectFileGenerator()->addHdlFiles(dmemFiles);
    }
    delete dmemGen;
    return success;
}

bool
PlatformIntegrator::generateMemory(
    MemoryGenerator& memGen,
    std::vector<std::string>& generatedFiles) {

    vector<string> reasons;
    if (!memGen.isCompatible(ttaCoreBlock(), reasons)) {
        errorStream() << "TTA core doesn't have compatible memory "
                      <<"interface:" << std::endl;
        for (unsigned int i = 0; i < reasons.size(); i++) {
            errorStream() << reasons.at(i) << std::endl;
        }
        return false;
    }

    memGen.addMemory(*netlist());

    if (memGen.generatesComponentHdlFile()) {
        generatedFiles =
            memGen.generateComponentFile(outputPath());
        if (generatedFiles.size() == 0) {
            errorStream() << "Failed to create mem component" << endl;
            //return false;
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

    string platformDir = outputPath();
    writer->write(platformDir);
    delete writer;

    string toplevelFile = outputFilePath(entityName() + ".vhdl");
    if (!FileSystem::fileExists(toplevelFile)) {
        string msg = "NetlistWriter failed to create file " + toplevelFile;
        FileNotFound exc(__FILE__, __LINE__, "platformIntegrator", msg);
        throw exc;
    }
    projectFileGenerator()->addHdlFile(toplevelFile);

    string paramFile = outputFilePath(entityName() + "_params_pkg.vhdl");
    if (FileSystem::fileExists(paramFile)) {
        projectFileGenerator()->addHdlFile(paramFile);
    }
}


void
PlatformIntegrator::addProGeFiles() const {

    vector<string> progeOutFiles;
    progeOutputHdlFiles(progeOutFiles);
    for (unsigned int i = 0; i < progeOutFiles.size(); i++) {
        projectFileGenerator()->addHdlFile(progeOutFiles.at(i));
    }
}
