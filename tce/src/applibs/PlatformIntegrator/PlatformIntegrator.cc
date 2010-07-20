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
using std::string;
using std::vector;
using std::endl;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;

const std::string PlatformIntegrator::TTA_CORE_NAME = "toplevel";

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


std::string
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

    string fileName = entityName() + ".vhdl";
    string fullPath = outputFilePath(fileName);
    if (!FileSystem::fileExists(fullPath)) {
        string msg = "NetlistWriter failed to create file " + fullPath;
        FileNotFound exc(__FILE__, __LINE__, "platformIntegrator", msg);
        throw exc;
    }
    return fullPath;
}
