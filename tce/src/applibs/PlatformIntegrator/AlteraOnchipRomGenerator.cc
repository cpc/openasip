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
 * @file AlteraOnchipRomGenerator.cc
 *
 * Implementation of AlteraOnchipRomGenerator class.
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
#include "StringTools.hh"
#include "FileSystem.hh"
#include "Application.hh"
#include "MemoryGenerator.hh"
#include "AlteraOnchipRomGenerator.hh"
#include "PlatformIntegrator.hh"
using std::string;
using std::endl;

AlteraOnchipRomGenerator::AlteraOnchipRomGenerator(
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

AlteraOnchipRomGenerator::~AlteraOnchipRomGenerator() {
}


bool
AlteraOnchipRomGenerator::isCompatible(
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
AlteraOnchipRomGenerator::writeComponentDeclaration(std::ostream& stream) {

    stream 
        << StringTools::indent(1) << "component altera_onchip_rom_comp" 
        << endl << StringTools::indent(2) << "port (" << endl
        << StringTools::indent(3) 
        << "address : in  std_logic_vector(IMEMADDRWIDTH-1 downto 0);" << endl
        << StringTools::indent(3)
        << "clock   : in  std_logic;" << endl
        << StringTools::indent(3)
        << "q       : out std_logic_vector(" 
        << "IMEMWIDTHINMAUS*IMEMMAUWIDTH-1 downto 0));" << endl
        << StringTools::indent(1)
        << "end component;" << endl << endl;
}

void 
AlteraOnchipRomGenerator::writeComponentInstantiation(
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
        << "onchip_imem : altera_onchip_rom_comp" << endl
        << StringTools::indent(2)
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
AlteraOnchipRomGenerator::generatesComponentHdlFile() const {
    return true;
}


std::vector<std::string>
AlteraOnchipRomGenerator::generateComponentFile(std::string outputPath) {

    std::vector<string> componentFiles;
    string tempDir = FileSystem::createTempDirectory();
    if (tempDir.empty()) {
        string msg = "Couldn't create temp directory";
        IOException exc(__FILE__, __LINE__, "AlteraOnchipRomGenerator",
                        msg);
        throw exc;
    }
    string parameterFile = tempDir + FileSystem::DIRECTORY_SEPARATOR + 
        "imem.parameters";
    
    std::ofstream file;
    file.open(parameterFile.c_str());
    if (!file) {
        string msg = "Couldn't open file " + parameterFile + " for writing";
        IOException exc(__FILE__, __LINE__, "AlteraOnchipRamGenerator",
                        msg);
        throw exc;
    }
    file << createMemParameters();
    file.close();

    // extension must be .vhd
    string outputFile = outputPath + FileSystem::DIRECTORY_SEPARATOR + 
        "altera_onchip_rom_comp.vhd";
    
    // execute "Altera MegaWizard Plug-In Manager(c)"
    string command = "qmegawiz -silent module=altsyncram -f:" +
        parameterFile + " " + outputFile + " 2>&1";
    std::vector<string> output;
    int rv = Application::runShellCommandAndGetOutput(command, output);
    
    if (rv != 0 || output.size() != 0) {
        errorStream() 
            << "Failed to create memory component. Make sure 'qmegawiz' "
            << "is in PATH" << endl;
        for (unsigned int i = 0; i < output.size(); i++) {
            errorStream() << output.at(i) << endl;
        }
    } else {
        componentFiles.push_back(outputFile);
    }

    // clean up
    FileSystem::removeFileOrDirectory(tempDir);
    return componentFiles;
} 


void
AlteraOnchipRomGenerator::connectSignals(
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
            << StringTools::indent(3) << "address => " << line + "_w";
    } else if (line.find("imem_data") != string::npos) {
        toplevelInstantiation
            << "," << endl
            << StringTools::indent(3) << line << " => " << line + "_w";
        memInstantiation
            << "," << endl 
            << StringTools::indent(3) << "q => " << line + "_w";
    } else {
        std::cerr << "Unknown signal " << line << std::endl;
    }
}


std::string
AlteraOnchipRomGenerator::createMemParameters() const {

    int addrWidth = memoryAddrWidth();
    int dataWidth = memoryTotalWidth();
    // 2^addrWidth
    int sizeInWords = 1 << addrWidth;
    string initFile = initializationFile();
    string deviceFamily = platformIntegrator()->deviceFamily();
    
    std::ostringstream parameters;
    
    parameters
        << "WIDTH_A=" << dataWidth << endl << "WIDTHAD_A=" << addrWidth
        << endl << "NUMWORDS_A=" << sizeInWords << endl 
        << "INTENDED_DEVICE_FAMILY=\"" << deviceFamily << "\"" << endl
        << "INIT_FILE=" << initFile << endl;

    parameters
        << "INIT_FILE_LAYOUT=PORT_A " << "ADDRESS_ACLR_A=UNUSED "
        << "BYTEENA_ACLR_A=UNUSED " << "INDATA_ACLR_A=UNUSED "
        << "OUTDATA_ACLR_A=NONE " << "BYTE_ENABLE=0 " << "BYTE_SIZE=8 "
        << "CLOCK_ENABLE_INPUT_A=BYPASS " << "CLOCK_ENABLE_OUTPUT_A=BYPASS "
        << "CLOCK_ENABLE_CORE_A=BYPASS " << "IMPLEMENT_IN_LES=OFF "
        << "ENABLE_RUNTIME_MOD=NO " << "MAXIMUM_DEPTH=0 " 
        << "RAM_BLOCK_TYPE=AUTO " << "OUTDATA_REG_A=UNREGISTERED "
        << "WRCONTROL_ACLR_A=UNUSED " << "RDEN_POWER_OPTIMIZATION=OFF "
        << "LPM_TYPE=altsyncram " << "OPERATION_MODE=ROM "
        << "POWER_UP_UNINITIALIZED=FALSE " << "OPTIONAL_FILES=NONE " << endl;
    
    parameters
        << "q_a=used " << "address_a=used " << "clock0=used " 
        << "byteena_a=unused " << "clocken1=unused " << "byteena_b=unused "
        << "clocken2=unused " << "rden_a=unused " << "aclr0=unused "
        << "addressstall_a=unused " << "clocken3=unused " << "data_a=unused "
        << "q_b=unused " << "rden_b=unused " << "aclr1=unused "
        << "address_b=unused " << "addressstall_b=unused " 
        << "clocken0=unused " << "data_b=unused " << "eccstatus=unused "
        << "wren_a=unused " << "clock1=unused " << "wren_b=unused " << endl;

    return parameters.str();
}
