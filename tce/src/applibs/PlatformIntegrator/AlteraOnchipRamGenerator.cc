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
#include "Conversion.hh"
#include "MemoryGenerator.hh"
#include "AlteraOnchipRamGenerator.hh"
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "HDLPort.hh"
using std::string;
using std::endl;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;


AlteraOnchipRamGenerator::AlteraOnchipRamGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    std::string initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream): 
    AlteraMegawizMemGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                              integrator, warningStream, errorStream) {
    
    bool inverted = true;
    bool noInvert = false;
    addPort("dmem_data_in", 
            new HDLPort("q", Conversion::toString(memoryTotalWidth()),
                        ProGe::BIT_VECTOR, HDB::OUT, noInvert,
                        memoryTotalWidth()));
    addPort("dmem_data_out",
            new HDLPort("data", Conversion::toString(memoryTotalWidth()),
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryTotalWidth()));
    addPort("dmem_addr",
            new HDLPort("address", Conversion::toString(memoryAddrWidth()),
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryAddrWidth()));
    addPort("dmem_mem_en_x",
            new HDLPort("clken", "1", ProGe::BIT, HDB::IN, inverted, 1));
    addPort("dmem_wr_en_x",
            new HDLPort("wren", "1", ProGe::BIT, HDB::IN, inverted, 1));
    addPort("dmem_bytemask",
            new HDLPort("byteena", Conversion::toString(memoryWidthInMaus()),
                        ProGe::BIT_VECTOR, HDB::IN, noInvert,
                        memoryWidthInMaus()));
    addPort("clk",
            new HDLPort("clock", "1", ProGe::BIT, HDB::IN, noInvert, 1));
}

AlteraOnchipRamGenerator::~AlteraOnchipRamGenerator() {
}

bool
AlteraOnchipRamGenerator::generatesComponentHdlFile() const {
    return true;
}


std::vector<std::string>
AlteraOnchipRamGenerator::generateComponentFile(std::string outputPath) {

    // extension must be .vhd
    string outputFile = outputPath + FileSystem::DIRECTORY_SEPARATOR + 
        "altera_onchip_ram_comp.vhd";
    
    return runMegawizard(outputFile);
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
        << bytemaskWidth << endl << "INIT_FILE=" << initFile << endl;

    if (!deviceFamily.empty()) {
        parameters
            << "INTENDED_DEVICE_FAMILY=\"" << deviceFamily << "\"" << endl;
    }

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


std::string
AlteraOnchipRamGenerator::moduleName() const {

    return "altera_onchip_ram_comp";
}
    

std::string
AlteraOnchipRamGenerator::instanceName() const {
    
    // TODO: variable instance name? Needed if more than 1 data memory
    return "onchip_dmem";
}
