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
 * @file VhdlRomGenerator.hh
 *
 * Implementation of VhdlRomGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/format.hpp>
#include "Exception.hh"
#include "StringTools.hh"
#include "FileSystem.hh"
#include "MemoryGenerator.hh"
#include "VhdlRomGenerator.hh"
#include "PlatformIntegrator.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "HDLPort.hh"
using std::string;
using std::endl;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;

VhdlRomGenerator::VhdlRomGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    std::string initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream): 
    MemoryGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                    integrator, warningStream, errorStream) {

    addPort("clk", new HDLPort("clock", "1", ProGe::BIT, HDB::IN, false, 1));
    addPort("imem_addr", new HDLPort("addr", "addrw", ProGe::BIT_VECTOR,
                                     HDB::IN, false, memoryAddrWidth()));

    // imem_en_x signal is left unconnected on purpose

    addPort("imem_data", new HDLPort("dataout", "instrw", ProGe::BIT_VECTOR,
                                     HDB::OUT, false, memoryTotalWidth()));

    HDLPort* busyToGnd = new HDLPort("wait", "1", ProGe::BIT, HDB::OUT, false,
                                     1);
    busyToGnd->setToStatic(ProGe::GND);
    addPort("busy", busyToGnd);

    HDLPort* initToZero = new HDLPort("startAddr", "IMEMADDRWIDTH",
                                      ProGe::BIT_VECTOR, HDB::OUT, false);
    initToZero->setToStatic(ProGe::GND);
    addPort("pc_init", initToZero);

    ProGe::Netlist::Parameter addr = {"addrw", "integer", "IMEMADDRWIDTH"};
    ProGe::Netlist::Parameter data = {"instrw", "integer",
                                      "IMEMMAUWIDTH*IMEMWIDTHINMAUS"};
    addParameter(addr);
    addParameter(data);
}

VhdlRomGenerator::~VhdlRomGenerator() {
}


bool
VhdlRomGenerator::generatesComponentHdlFile() const {
    return true;
}


std::vector<std::string>
VhdlRomGenerator::generateComponentFile(std::string outputPath) {

    string outputFile = outputPath + FileSystem::DIRECTORY_SEPARATOR + 
        moduleName() + ".vhd";

    std::ofstream file;
    file.open(outputFile.c_str());
    if (!file) {
        string msg = "Couldn't open file " + outputFile + " for writing";
        IOException exc(__FILE__, __LINE__, "VhdlRomGenerator", msg);
        throw exc;
    }
    
    std::ostringstream stream;
    stream
        << "library ieee;" << endl
        << "use ieee.std_logic_1164.all;" << endl
        << "use ieee.std_logic_arith.all;" << endl
        << "use work." << imagePackageName() << ".all;" << endl << endl
        << "entity " << moduleName() << " is" << endl << endl
        << StringTools::indent(1) << "generic (" << endl
        << StringTools::indent(2) << "addrw  : integer := 10;" << endl
        << StringTools::indent(2) << "instrw : integer := 100);" << endl
        << StringTools::indent(1) << "port (" << endl
        << StringTools::indent(2) << "clock   : in  std_logic;" << endl
        << StringTools::indent(2) 
        << "addr    : in  std_logic_vector(addrw-1 downto 0);" << endl
        << StringTools::indent(2)
        << "dataout : out std_logic_vector(instrw-1 downto 0));" << endl
        << "end " << moduleName() << ";" << endl << endl;

    stream
        << "architecture rtl of " << moduleName() << " is" << endl << endl
        << StringTools::indent(1)
        << "subtype imem_index is integer range 0 to imem_array'length-1;"
        << endl 
        << StringTools::indent(1) << "constant imem : std_logic_imem_matrix"
        << "(0 to imem_array'length-1) := imem_array;" << endl << endl
        << "begin --rtl" << endl << endl
        << StringTools::indent(1) << "process" << endl
        << StringTools::indent(2)
        << "variable imem_line : imem_index;" << endl
        << StringTools::indent(1) << "begin -- process" << endl
        << StringTools::indent(2) << "wait until clock'event and clock='1';"
        << endl << StringTools::indent(2) 
        << "imem_line := conv_integer(unsigned(addr));" << endl
        << StringTools::indent(2) << "dataout <= imem(imem_line);" << endl
        << StringTools::indent(1) << "end process;" << endl
        << "end rtl;" << endl;

    file << stream.str();
    file.close();

    std::vector<string> componentFiles;
    componentFiles.push_back(outputFile);
    componentFiles.push_back(initializationFile());
    return componentFiles;
} 


std::string
VhdlRomGenerator::moduleName() const {

    return ttaCoreName() + "_rom_array_comp";
}

    
std::string
VhdlRomGenerator::instanceName() const {

    return "imem_array_instance";
}

std::string
VhdlRomGenerator::imagePackageName() const {

    return ttaCoreName() + "_imem_image";
}
