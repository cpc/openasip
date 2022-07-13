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
#include <vector>
#include "Exception.hh"
#include "StringTools.hh"
#include "FileSystem.hh"
#include "MemoryGenerator.hh"
#include "VhdlRomGenerator.hh"
#include "PlatformIntegrator.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "HDLPort.hh"
using std::endl;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;

VhdlRomGenerator::VhdlRomGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    TCEString initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream): 
        MemoryGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
            integrator, warningStream, errorStream) {

    addPort("clk", new HDLPort("clock", "1", ProGe::BIT, ProGe::IN, false, 1));
    addPort("imem_addr", new HDLPort("addr", "addrw", ProGe::BIT_VECTOR,
        ProGe::IN, false, memoryAddrWidth()));

    addPort("imem_en_x", new HDLPort("en_x", "1", ProGe::BIT,
        ProGe::IN, false, 1));

    addPort("imem_data", new HDLPort("dataout", "instrw", ProGe::BIT_VECTOR,
        ProGe::OUT, false, memoryTotalWidth()));

    HDLPort* busyToGnd = new HDLPort("wait", "1", ProGe::BIT, ProGe::OUT,
        false, 1);
    busyToGnd->setToStatic(ProGe::StaticSignal::GND);
    addPort("busy", busyToGnd);

    ProGe::Parameter addr("addrw", "integer", "IMEMADDRWIDTH");
    ProGe::Parameter data("instrw", "integer", "IMEMMAUWIDTH*IMEMWIDTHINMAUS");
    
    addParameter(addr);
    addParameter(data);
}

VhdlRomGenerator::~VhdlRomGenerator() {
}


bool
VhdlRomGenerator::generatesComponentHdlFile() const {
    return true;
}


std::vector<TCEString>
VhdlRomGenerator::generateComponentFile(TCEString outputPath) {

    TCEString outputFile =
        outputPath << FileSystem::DIRECTORY_SEPARATOR << moduleName() 
                   << ".vhd";

    std::ofstream file;
    file.open(outputFile.c_str());
    if (!file) {
        TCEString msg = "Couldn't open file " + outputFile + " for writing";
        IOException exc(__FILE__, __LINE__, "VhdlRomGenerator", msg);
        throw exc;
    }
    
    TCEString indentL1 = StringTools::indent(1);
    TCEString indentL2 = StringTools::indent(2);
    std::ostringstream stream;
    stream
        << "library ieee;" << endl
        << "use ieee.std_logic_1164.all;" << endl
        << "use ieee.std_logic_arith.all;" << endl
        << "use work." << imagePackageName() << ".all;" << endl << endl
        << "entity " << moduleName() << " is" << endl << endl
        << indentL1 << "generic (" << endl
        << indentL2 << "addrw  : integer := 10;" << endl
        << indentL2 << "instrw : integer := 100);" << endl
        << indentL1 << "port (" << endl
        << indentL2 << "clock   : in  std_logic;" << endl
        << indentL2 << "en_x    : in std_logic; -- not used" << endl
        << indentL2 << "addr    : in  std_logic_vector(addrw-1 downto 0);"
        << endl << indentL2
        << "dataout : out std_logic_vector(instrw-1 downto 0));" << endl
        << "end " << moduleName() << ";" << endl << endl;

    stream
        << "architecture rtl of " << moduleName() << " is" << endl << endl
        << indentL1
        << "subtype imem_index is integer range 0 to imem_array'length-1;"
        << endl 
        << indentL1 << "constant imem : std_logic_imem_matrix"
        << "(0 to imem_array'length-1) := imem_array;" << endl
        << indentL1 << "signal en_x_dummy : std_logic;" << endl << endl
        << "begin --rtl" << endl << endl
        << indentL1 << "process" << endl
        << indentL2 << "variable imem_line : imem_index;" << endl
        << indentL1 << "begin -- process" << endl
        << indentL2 << "wait until clock'event and clock='1';" << endl
        << indentL2 << "imem_line := conv_integer(unsigned(addr));" << endl
        << indentL2 << "dataout <= imem(imem_line);" << endl
        << indentL1 << "end process;" << endl << endl
        << indentL1 << "en_x_dummy <= en_x; -- dummy connection" << endl
        << endl << "end rtl;" << endl;

    file << stream.str();
    file.close();

    std::vector<TCEString> componentFiles;
    componentFiles.push_back(outputFile);
    componentFiles.push_back(initializationFile());
    return componentFiles;
} 


TCEString
VhdlRomGenerator::moduleName() const {

    return ttaCoreName() + "_rom_array_comp";
}

    
TCEString
VhdlRomGenerator::instanceName(int coreId, int memIndex) const {

    TCEString iname("imem_array_instance_");
    return iname << memoryIndexString(coreId, memIndex);
}

TCEString
VhdlRomGenerator::imagePackageName() const {

    return ttaCoreName() + "_imem_image";
}
