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
 * @file TestbenchGenerator.cc
 *
 * Implementation of TestbenchGenerator class
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <stdint.h>
#include <boost/format.hpp>
#include "TestbenchGenerator.hh"
#include "StringTools.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "Conversion.hh"

using std::string;
using std::vector;
using std::ostringstream;
using std::ifstream;

#define INDENT "   "

const std::string TestbenchGenerator::TB_TEMPLATE_ = 
    "testbench.vhdl.template";

TestbenchGenerator::TestbenchGenerator():
    componentDeclaration_(), componentBinding_(), signalDeclaration_(),
    componentInstantiation_(), inputArrays_(), opcodeArrays_(),
    loadSignalArrays_(), outputArrays_(), testbenchCode_() {
}

TestbenchGenerator::~TestbenchGenerator() {
}


void
TestbenchGenerator::writeStimulusArray(
    std::ostringstream& stream, 
    std::vector<uint32_t>& dataArray, std::string portName, int portWidth) {
        
    stream
        << INDENT INDENT  << "type " << portName 
        << "_data_array is array (natural range <>) of" << std::endl
        << INDENT INDENT INDENT
        << "std_logic_vector(" << portWidth - 1 << " downto 0);"
        << std::endl << std::endl << INDENT INDENT
        << "constant " << portName << "_data : "
        << portName << "_data_array :=" << std::endl;
    
    for (std::size_t i = 0; i < dataArray.size(); ++i) {
        uint32_t input = dataArray.at(i);
        std::string inputAsBinaryLiteral = 
            Conversion::toBinary(input, portWidth);
        stream << INDENT INDENT;
        
        if (i == 0) {
            stream << "(";
        } else {
            stream << " ";
        }
        stream << "\"" << inputAsBinaryLiteral << "\"";
        if (i == dataArray.size() - 1) {
            stream << ");";
        } else {
            stream << ",";
        }
        stream << "\t -- @" << i << " = " << input << std::endl;
    }
    stream << std::endl;
}

void 
TestbenchGenerator::writeTbConstants(
    int totalCycles, int outputIgnoreCycles) {

    outputArrays_
        << INDENT INDENT << "constant IGNORE_OUTPUT_COUNT : integer := " 
        << outputIgnoreCycles << ";" << std::endl;
    outputArrays_
        << INDENT INDENT << "constant TOTAL_CYCLE_COUNT : integer := " 
        << totalCycles << ";" << std::endl;
}

void
TestbenchGenerator::writeTestbench(
    std::ofstream& file, HDB::HWBlockImplementation* impl) {

    string templateFile = findVhdlTemplate();
    string vhdlTemplate = "";
    loadVhdlTemplate(templateFile, vhdlTemplate);
    
    string testBench = 
        (boost::format(vhdlTemplate)         
         % componentDeclaration_.str()
         % componentBinding_.str()
         % signalDeclaration_.str()
         % componentInstantiation_.str()
         % inputArrays_.str() 
         % opcodeArrays_.str()
         % loadSignalArrays_.str()
         % outputArrays_.str()
         % impl->clkPort()
         % impl->rstPort()
         % impl->glockPort()
         % testbenchCode_.str()).str();

    file << testBench;
}

std::string
TestbenchGenerator::findVhdlTemplate() const {

    vector<string> paths = Environment::implementationTesterTemplatePaths();
    for (unsigned int i = 0; i < paths.size(); i++) {
        string file = paths.at(i) + FileSystem::DIRECTORY_SEPARATOR 
            + TB_TEMPLATE_;
        if (FileSystem::fileExists(file)) {
            return file;
        }
    }

    InvalidData exception(__FILE__, __LINE__, "", 
                          "The VHDL template file " + TB_TEMPLATE_ + 
                          "not found");
    throw exception;

    string notFound = "";
    return notFound;
}

void 
TestbenchGenerator::loadVhdlTemplate(
    const std::string& fileName, std::string& vhdlTemplate) const {
    
    ifstream input(fileName.c_str());
    if (!input.is_open()) {
        InvalidData exception(__FILE__, __LINE__, "", 
                              "The VHDL template file " + fileName + 
                              "unreadable.");
        throw exception;
    }

    string line = "";
    while (getline(input, line)) {
        vhdlTemplate += line;
        vhdlTemplate += "\n";
    }
    input.close();
}

std::ostringstream&
TestbenchGenerator::declarationStream() {
    return componentDeclaration_;
}

std::ostringstream& 
TestbenchGenerator::bindingStream() {
    return componentBinding_;
}

std::ostringstream& 
TestbenchGenerator::signalStream() {
    return signalDeclaration_;
}

std::ostringstream& 
TestbenchGenerator::instantiationStream() {
    return componentInstantiation_;
}

std::ostringstream& 
TestbenchGenerator::inputArrayStream() {
    return inputArrays_;
}

std::ostringstream& 
TestbenchGenerator::opcodeArrayStream() {
    return opcodeArrays_;
}

std::ostringstream& 
TestbenchGenerator::loadArrayStream() {
    return loadSignalArrays_;
}

std::ostringstream& 
TestbenchGenerator::outputArrayStream() {
    return outputArrays_;
}

std::ostringstream& 
TestbenchGenerator::tbCodeStream() {
    return testbenchCode_;
}
