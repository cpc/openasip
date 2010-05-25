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
using std::string;
using std::vector;
using std::endl;

PlatformIntegrator::PlatformIntegrator(): 
    progeOutputDir_(""), entityName_(""), outputDir_(""),
    outputDirCreated_(false), programName_(""), targetFrequency_(0),
    warningStream_(std::cout), errorStream_(std::cerr) {
    
}

PlatformIntegrator::PlatformIntegrator(
        std::string progeOutputDir,
        std::string entityName,
        std::string outputDir,
        std::string programName,
        int targetClockFreq,
        std::ostream& warningStream,
        std::ostream& errorStream): 
    progeOutputDir_(progeOutputDir), entityName_(entityName),
    outputDir_(outputDir), outputDirCreated_(false),
    programName_(programName), targetFrequency_(targetClockFreq), 
    warningStream_(warningStream), errorStream_(errorStream) {

}

PlatformIntegrator::~PlatformIntegrator() {
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
PlatformIntegrator::chopToSignalName(const std::string& original) const {

    vector<string> lines;
    StringTools::chopString(original, ":", lines);
    assert(lines.size() != 0);

    return StringTools::trim(lines.at(0));
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


std::string
PlatformIntegrator::stripSignalEnd(const std::string& original) const {

    string signal = original;
    if (original.empty()) {
        return signal;
    }
    
    // remove semicolon
    int lastIndex = original.size()-1;
    if (original.at(lastIndex) == ';') {
        signal = original.substr(0, lastIndex);
        signal = StringTools::trim(signal);
    }
    
    // remove duplicate braces
    if (signal.size() > 2) {
        int lastIndex = signal.size()-1;
        if (signal.at(lastIndex) == ')' && signal.at(lastIndex-1) == ')') {
            signal = signal.substr(0, lastIndex);
            signal = StringTools::trim(signal);
        }
    }
    return signal;
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

std::ostream&
PlatformIntegrator::newToplevelStream() {

    return newToplevelStream_;
}

std::ostream&
PlatformIntegrator::newEntityStream() {

    return newEntityStream_;
}

std::ostream&
PlatformIntegrator::componentStream() {

    return componentStream_;
}

std::ostream&
PlatformIntegrator::signalStream() {

    return signalStream_;
}

std::ostream&
PlatformIntegrator::ttaToplevelInstStream() {

    return toplevelInstantiationStream_;
}

std::ostream&
PlatformIntegrator::imemInstStream() {

    return imemInstantiationStream_;
}

std::ostream&
PlatformIntegrator::dmemInstStream() {

    return dmemInstantiationStream_;
}

std::ostream&
PlatformIntegrator::signalConnectionStream() {

    return signalConnectionStream_;
}

const std::vector<std::string>&
PlatformIntegrator::ttaToplevel() const {

    assert(ttaToplevel_.size() != 0);
    return ttaToplevel_;
}


void
PlatformIntegrator::readTTAToplevel() {
    
    string fileName = 
        "vhdl" + FileSystem::DIRECTORY_SEPARATOR + "toplevel.vhdl";
    string fullPath = progeFilePath(fileName);
    string startRE = ".*entity.toplevel.is.*";
    string endRE = ".*end.toplevel;.*";
    string block = "";
    FileSystem::readBlockFromFile(fullPath, startRE, endRE, block, false);

    StringTools::chopString(block, "\n", ttaToplevel_);

    componentStream() 
        << StringTools::indent(1) << "component toplevel is" << std::endl;
    for (unsigned int i = 0; i < ttaToplevel_.size(); i++) {
        componentStream()
            << StringTools::indent(2) << ttaToplevel_.at(i) << endl;
    }
    componentStream()
        << StringTools::indent(1) << "end component;" << endl << endl;
}


void
PlatformIntegrator::createNewToplevelTemplate() {
    
    newToplevelStream() 
       << "library ieee;" << endl
       << "use ieee.std_logic_1164.all;" << endl
       << "use ieee.std_logic_arith.all;" << endl
       << "use work.imem_mau.all;" << endl
       << "use work.toplevel_params.all;" << endl
       << "use work.globals.all;" << endl << endl;

    newToplevelStream()
        << "-- new toplevel entity" << endl
        << "%1%" << endl << endl
        << "architecture structural of " << entityName() << " is" << endl; 

    newToplevelStream()
        << "  -- component declarations" << endl << endl
        << "%2%" << endl << endl;

    newToplevelStream()
        << "-- signal declarations" << endl
        << "%3%" << endl << endl
        << "begin -- structural" << endl << endl;
    
    newToplevelStream()
        << "  -- tta toplevel instantion" << endl
        << "%4%" << endl << endl;
    
    newToplevelStream()
        << "  -- imem component instantion" << endl
        << "%5%" << endl << endl;

    newToplevelStream()
        << "  -- dmem component instantion" << endl
        << "%6%" << endl << endl;

    newToplevelStream()
        << "  -- other signal mappings" << endl
        << "%7%" << endl << endl
        << "end structural;" << endl;
}


std::string
PlatformIntegrator::writeNewToplevel() {
    
    createNewToplevelTemplate();

    string fileName = entityName() + ".vhdl";
    string fullPath = outputFilePath(fileName);
    
    std::ofstream file;
    file.open(fullPath.c_str());

    if (!file) {
        string msg = "Couldn't open file " + fullPath + " for writing";
        IOException exc(__FILE__, __LINE__, "PlatformIntegrator",
                        msg);
        throw exc;
    }

    string newToplevel = 
        (boost::format(newToplevelStream_.str())
         % newEntityStream_.str()
         % componentStream_.str()
         % signalStream_.str()
         % toplevelInstantiationStream_.str()
         % imemInstantiationStream_.str()
         % dmemInstantiationStream_.str()
         % signalConnectionStream_.str()).str();
        
    file << newToplevel;
    file.close();
    return fullPath;
}
