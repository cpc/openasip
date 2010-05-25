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
 * @file AlteraOnchipRamGenerator.cc
 *
 * Implementation of AlteraOnchipRamGenerator class.
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
#include "Application.hh"
#include "MemoryGenerator.hh"
#include "AlteraOnchipRamGenerator.hh"
using std::string;
using std::endl;

AlteraOnchipRamGenerator::AlteraOnchipRamGenerator(
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

AlteraOnchipRamGenerator::~AlteraOnchipRamGenerator() {
}

void 
AlteraOnchipRamGenerator::writeComponentDeclaration(std::ostream& stream) {

    stream 
        << StringTools::indent(1) << "component altera_onchip_ram_comp" 
        << endl << StringTools::indent(2) << "port (" << endl
        << StringTools::indent(3) 
        << "address : in  std_logic_vector(" << memoryAddrWidth() 
        << "-1 downto 0);" << endl
        << StringTools::indent(3)
        << "byteena : in  std_logic_vector(" << memoryWidthInMaus()
        << "-1 downto 0);" << endl
        << StringTools::indent(3)
        << "clken   : in  std_logic;" << endl
        << StringTools::indent(3)
        << "clock   : in  std_logic;" << endl
        << StringTools::indent(3)
        << "data    : in  std_logic_vector(" << memoryTotalWidth()
        << "-1 downto 0);" << endl
        << StringTools::indent(3)
        << "wren    : in  std_logic;" << endl
        << StringTools::indent(3)
        << "q       : out std_logic_vector(" << memoryTotalWidth()
        << "-1 downto 0));" << endl
        << StringTools::indent(1)
        << "end component;" << endl;
}

bool
AlteraOnchipRamGenerator::isCompatible(
    const std::vector<std::string>& ttaCore,
    std::vector<std::string>& reasons) {

    bool foundAll = true;
    if (findSignal("dmem_data_in", ttaCore) < 0) {
        reasons.push_back("Compatible data input signal not found");
        foundAll = false;
    }
    if (findSignal("dmem_data_out", ttaCore) < 0) {
        reasons.push_back("Compatible data output signal not found");
        foundAll = false;
    }  
    if (findSignal("dmem_addr", ttaCore) < 0) {
        reasons.push_back("Compatible memory address signal not found");
        foundAll = false;
    }
    if (findSignal("dmem_mem_en_x", ttaCore) < 0) {
        reasons.push_back("Compatible memory enable signal not found");
        foundAll = false;
    }
    if (findSignal("dmem_wr_en_x", ttaCore) < 0) {
        reasons.push_back("Compatible write enable signal not found");
        foundAll = false;
    }
    if (findSignal("dmem_bytemask", ttaCore) < 0) {
        reasons.push_back("Compatible bytemask signal not found");
        foundAll = false;
    }
    return foundAll;
}


void 
AlteraOnchipRamGenerator::writeComponentInstantiation(
    const std::vector<std::string>& toplevelSignals,
    std::ostream& signalStream,
    std::ostream& signalConnections,
    std::ostream& toplevelInstantiation,
    std::ostream& memInstantiation) {

    // write signals for connections
    signalStream 
        << StringTools::indent(1)
        << "signal dmem_data_in_w : std_logic_vector(" << memoryTotalWidth()
        << "-1 downto 0);" << endl
        << StringTools::indent(1)
        << "signal dmem_data_out_w : std_logic_vector(" << memoryTotalWidth()
        << "-1 downto 0);" << endl
        << StringTools::indent(1)
        << "signal dmem_addr_w : std_logic_vector(" << memoryAddrWidth()
        << "-1 downto 0);" << endl
        << StringTools::indent(1)
        << "signal dmem_mem_en_x_w : std_logic;" << endl
        << StringTools::indent(1)
        << "signal dmem_mem_en_w : std_logic;" << endl
        << StringTools::indent(1)
        << "signal dmem_wr_en_x_w : std_logic;" << endl
        << StringTools::indent(1)
        << "signal dmem_wr_en_w : std_logic;" << endl
        << StringTools::indent(1)
        << "signal dmem_bytemask_w : std_logic_vector("
        << memoryWidthInMaus() << "-1 downto 0);" << endl;

    // make signal connections
    signalConnections
        << StringTools::indent(1)
        << "dmem_mem_en_w <= not dmem_mem_en_x_w;" << endl
        << StringTools::indent(1)
        << "dmem_wr_en_w <= not dmem_wr_en_x_w;" << endl;

    // connect toplevel and dmem
    memInstantiation 
        << StringTools::indent(1)
        << "onchip_dmem : altera_onchip_ram_comp" << endl
        << StringTools::indent(2)
        << "port map (" << endl
        << StringTools::indent(3)
        << "clock => clk";
        
    for (unsigned int i = 0; i < toplevelSignals.size(); i++) {
        string line = toplevelSignals.at(i);
        if (line.find("dmem") != string::npos) {
            connectSignals(line, toplevelInstantiation, memInstantiation);
        }
    }
    memInstantiation 
        << ");" << endl;

}

std::vector<std::string>
AlteraOnchipRamGenerator::generateComponentFile(std::string outputPath) {
    
    std::vector<string> componentFiles;
    string tempDir = FileSystem::createTempDirectory();
    if (tempDir.empty()) {
        string msg = "Couldn't create temp directory";
        IOException exc(__FILE__, __LINE__, "AlteraOnchipRamGenerator",
                        msg);
        throw exc;
    }
    string parameterFile = tempDir + FileSystem::DIRECTORY_SEPARATOR + 
        "dmem.parameters";

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
        "altera_onchip_ram_comp.vhd";
    
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
AlteraOnchipRamGenerator::connectSignals(
    std::string line, 
    std::ostream& toplevelInstantiation,
    std::ostream& memInstantiation) const {

    if (line.find("dmem_data_in") != string::npos) {
        toplevelInstantiation
            << "," << endl
            << StringTools::indent(3) << line << " => " << "dmem_data_in_w";
        memInstantiation
            << "," << endl 
            << StringTools::indent(3) << "q => dmem_data_in_w";

    } else if (line.find("dmem_data_out") != string::npos) {
        toplevelInstantiation
            << "," << endl
            << StringTools::indent(3) << line << " => dmem_data_out_w";
        memInstantiation
            << "," << endl
            << StringTools::indent(3)  << "data => dmem_data_out_w";

    } else if (line.find("dmem_addr") != string::npos) {
        toplevelInstantiation
            << "," << endl
            << StringTools::indent(3) << line << " => dmem_addr_w";
        memInstantiation
            << "," << endl
            << StringTools::indent(3) << "address => dmem_addr_w";

    } else if (line.find("dmem_mem_en_x") != string::npos) {
        toplevelInstantiation
            << "," << endl
            << StringTools::indent(3) << line + "(0)" 
            << " => dmem_mem_en_x_w";
        memInstantiation
            << "," << endl
            << StringTools::indent(3) << "clken => dmem_mem_en_w";

    } else if (line.find("dmem_wr_en_x") != string::npos) {
        toplevelInstantiation
            << "," << endl
            << StringTools::indent(3) << line + "(0)"
            << " => dmem_wr_en_x_w";
        memInstantiation
            << "," << endl 
            << StringTools::indent(3) << "wren => dmem_wr_en_w";

    } else if (line.find("dmem_bytemask") != string::npos) {
        toplevelInstantiation
            << "," << endl
            << StringTools::indent(3) << line << " => " << "dmem_bytemask_w";
        memInstantiation
            << "," << endl
            << StringTools::indent(3) << "byteena => dmem_bytemask_w";

    } else {
        std::cerr << "Unknown signal " << line << std::endl;
    }
}


std::string 
AlteraOnchipRamGenerator::createMemParameters() const {

    int addrWidth = memoryAddrWidth();
    int dataWidth = memoryTotalWidth();
    int bytemaskWidth = memoryWidthInMaus();
    // 2^addrWidth
    int sizeInWords = 1 << addrWidth;
    string initFile = initializationFile();
    string deviceFamily = platformIntegrator()->deviceFamily();

    std::ostringstream parameters;
    
    parameters 
        << "WIDTH_A=" << dataWidth << endl << "WIDTHAD_A=" << addrWidth 
        << endl << "NUMWORDS_A=" << sizeInWords << endl << "WIDTH_BYTEENA_A="
        << bytemaskWidth << endl << "INTENDED_DEVICE_FAMILY=\"" 
        << deviceFamily << "\"" << endl << "INIT_FILE=" << initFile << endl;

    parameters
        << "INIT_FILE_LAYOUT=PORT_A ADDRESS_ACLR_A=UNUSED " 
        << "BYTEENA_ACLR_A=UNUSED INDATA_ACLR_A=UNUSED OUTDATA_ACLR_A=NONE "
        << "BYTE_ENABLE=1 BYTE_SIZE=8 CLOCK_ENABLE_INPUT_A=NORMAL "
        << "CLOCK_ENABLE_OUTPUT_A=BYPASS CLOCK_ENABLE_CORE_A=USE_INPUT_CLKEN "
        << "IMPLEMENT_IN_LES=OFF INIT_FILE_LAYOUT=PORT_A "
        << "ENABLE_RUNTIME_MOD=NO MAXIMUM_DEPTH=0 RAM_BLOCK_TYPE=AUTO "
        << "OUTDATA_REG_A=UNREGISTERED WRCONTROL_ACLR_A=UNUSED "
        << "RDEN_POWER_OPTIMIZATION=OFF LPM_TYPE=altsyncram "
        << "OPERATION_MODE=SINGLE_PORT POWER_UP_UNINITIALIZED=FALSE "
        << "OPTIONAL_FILES=NONE" << endl;

    parameters
        << "byteena_a=used clocken1=unused byteena_b=unused clocken2=unused "
        << "q_a=used rden_a=unused aclr0=unused address_a=used "
        << "addressstall_a=unused clocken3=unused data_a=used q_b=unused "
        << "rden_b=unused aclr1=unused address_b=unused "
        << "addressstall_b=unused clocken0=used data_b=unused "
        << "eccstatus=unused clock0=used wren_a=used clock1=unused "
        << "wren_b=unused" << endl;
    
    return parameters.str();
}
