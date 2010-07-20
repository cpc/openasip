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
#include "NetlistBlock.hh"
#include "NetlistPort.hh"
#include "HDLPort.hh"
using std::string;
using std::endl;
using ProGe::NetlistBlock;
using ProGe::NetlistPort;
using ProGe::StaticSignal;


AlteraOnchipRomGenerator::AlteraOnchipRomGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    std::string initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream): 
    AlteraMegawizMemGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                              integrator, warningStream, errorStream) {
    
    bool noInvert = false;
    
    addPort("clk",
            new HDLPort("clock", "1", ProGe::BIT, HDB::IN, noInvert, 1));

    addPort("imem_addr",
            new HDLPort("address", "IMEMADDRWIDTH", ProGe::BIT_VECTOR,
                        HDB::IN, noInvert));

    // imem_en_x signal is left unconnected on purpose

    addPort("imem_data",
            new HDLPort("q", "IMEMWIDTHINMAUS*IMEMMAUWIDTH",
                        ProGe::BIT_VECTOR, HDB::OUT, noInvert));

    // these signals are not driven by the imem component, connect to zero
    HDLPort* busyToGnd =
        new HDLPort("wait", "1", ProGe::BIT, HDB::OUT, noInvert, 1);
    busyToGnd->setToStatic(ProGe::GND);
    addPort("busy", busyToGnd);

    
    HDLPort* initToZero = new HDLPort("startAddr", "IMEMADDRWIDTH",
                                      ProGe::BIT_VECTOR, HDB::OUT, noInvert);
    initToZero->setToStatic(ProGe::GND);
    addPort("pc_init", initToZero);
}


AlteraOnchipRomGenerator::~AlteraOnchipRomGenerator() {
}

bool
AlteraOnchipRomGenerator::generatesComponentHdlFile() const {
    return true;
}


std::vector<std::string>
AlteraOnchipRomGenerator::generateComponentFile(std::string outputPath) {

    // extension must be .vhd
    string outputFile = outputPath + FileSystem::DIRECTORY_SEPARATOR + 
        "altera_onchip_rom_comp.vhd";
    
    return runMegawizard(outputFile);
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


std::string
AlteraOnchipRomGenerator::moduleName() const {

    return "altera_onchip_rom_comp";
}
    

std::string
AlteraOnchipRomGenerator::instanceName() const {

    return "onchip_imem";
}
