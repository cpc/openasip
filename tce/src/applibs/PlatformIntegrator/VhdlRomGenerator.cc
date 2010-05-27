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
using std::string;
using std::endl;

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

}

VhdlRomGenerator::~VhdlRomGenerator() {
}


bool
VhdlRomGenerator::isCompatible(
    const std::vector<std::string>& ttaCore,
    std::vector<std::string>& reasons) {

    bool foundAll = true;
    if (findSignal("imem_addr", ttaCore) < 0) {
        reasons.push_back("Compatible address signal not found");
        foundAll = false;
    }
    if (findSignal("imem_en_x", ttaCore) < 0) {
        reasons.push_back("Compatible memory enable signal not found");
        foundAll = false;
    }
    if (findSignal("imem_data", ttaCore) < 0) {
        reasons.push_back("Compatible data signal not found");
        foundAll = false;
    }
    return foundAll;
}


void 
VhdlRomGenerator::writeComponentDeclaration(std::ostream& stream) {

    stream 
        << StringTools::indent(1) << "component rom_array_comp" 
        << endl << StringTools::indent(2) << "generic (" << endl
        << StringTools::indent(2)
        << "addrw  : integer := 10;" << endl
        << StringTools::indent(2)
        << "instrw : integer := 100);" << endl
        << endl << StringTools::indent(2) << "port (" << endl
        << StringTools::indent(3) 
        << "clock   : in  std_logic;" << endl
        << StringTools::indent(3) 
        << "addr    : in  std_logic_vector(addrw-1 downto 0);" << endl
        << StringTools::indent(3) 
        << "dataout : out std_logic_vector(instrw-1 downto 0));" << endl
        << StringTools::indent(1)
        << "end component;" << endl << endl;
}

void 
VhdlRomGenerator::writeComponentInstantiation(
    const std::vector<std::string>& toplevelSignals,
    std::ostream& signalStream,
    std::ostream& signalConnections,
    std::ostream& toplevelInstantiation,
    std::ostream& memInstantiation) {

    // write signals for connections
    signalStream 
        << StringTools::indent(1)
        << "signal imem_en_x_w : std_logic;" << endl
        << StringTools::indent(1)
        << "signal imem_addr_w : std_logic_vector(IMEMADDRWIDTH-1 downto 0);"
        << endl << StringTools::indent(1)
        << "signal imem_data_w : std_logic_vector(IMEMMAUWIDTH-1 downto 0);"
        << endl;

    // make signal connections
    signalConnections << endl;

    // connect toplevel and dmem
    memInstantiation 
        << StringTools::indent(1)
        << "imem_array : rom_array_comp" << endl
        << StringTools::indent(2)
        << "generic map (" << endl
        << StringTools::indent(3)
        << "addrw => IMEMADDRWIDTH," << endl
        << StringTools::indent(3)
        << "instrw => IMEMMAUWIDTH" << endl
        << StringTools::indent(2)
        << ")" << endl;
    memInstantiation 
        << StringTools::indent(2)
        << "port map (" << endl
        << StringTools::indent(3)
        << "clock => clk";
        
    for (unsigned int i = 0; i < toplevelSignals.size(); i++) {
        string line = toplevelSignals.at(i);
        if (line.find("imem") != string::npos) {
            connectSignals(line, toplevelInstantiation, memInstantiation);
        }
    }
    memInstantiation 
        << ");" << endl;

}


bool
VhdlRomGenerator::generatesComponentHdlFile() const {
    return true;
}


std::vector<std::string>
VhdlRomGenerator::generateComponentFile(std::string outputPath) {

    string outputFile = outputPath + FileSystem::DIRECTORY_SEPARATOR + 
        "rom_array_comp.vhd";

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
        << "use work.imem_image.all;" << endl << endl
        << "entity rom_array_comp is" << endl << endl
        << StringTools::indent(1) << "generic (" << endl
        << StringTools::indent(2) << "addrw  : integer := 10;" << endl
        << StringTools::indent(2) << "instrw : integer := 100);" << endl
        << StringTools::indent(1) << "port (" << endl
        << StringTools::indent(2) << "clock   : in  std_logic;" << endl
        << StringTools::indent(2) 
        << "addr    : in  std_logic_vector(addrw-1 downto 0);" << endl
        << StringTools::indent(2)
        << "dataout : out std_logic_vector(instrw-1 downto 0));" << endl
        << "end rom_array_comp;" << endl << endl;

    stream
        << "architecture rtl of rom_array_comp is" << endl << endl
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


void
VhdlRomGenerator::connectSignals(
    std::string line, 
    std::ostream& toplevelInstantiation,
    std::ostream& memInstantiation) {

    if (line.find("imem_en_x") != string::npos) {
        toplevelInstantiation
            << "," << endl
            << StringTools::indent(3) << line << " => " << line + "_w";

    } else if (line.find("imem_addr") != string::npos) {
        toplevelInstantiation
            << "," << endl
            << StringTools::indent(3) << line << " => " << line + "_w";
        memInstantiation
            << "," << endl 
            << StringTools::indent(3) << "addr => " << line + "_w";

    } else if (line.find("imem_data") != string::npos) {
        toplevelInstantiation
            << "," << endl
            << StringTools::indent(3) << line << " => " << line + "_w";
        memInstantiation
            << "," << endl 
            << StringTools::indent(3) << "dataout => " << line + "_w";

    } else {
        std::cerr << "Unknown signal " << line << std::endl;
    }
}
