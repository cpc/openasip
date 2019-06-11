/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file AlteraMemGenerator.cc
 *
 * Implementation of AlteraMemGenerator class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>
#include "FileSystem.hh"
#include "AlteraMemGenerator.hh"
#include "NetlistBlock.hh"
#include "PlatformIntegrator.hh"
#include "Conversion.hh"

const TCEString AlteraMemGenerator::INIT_FILE_G = "init_file_g";
const TCEString AlteraMemGenerator::DEV_FAMILY_G = "dev_family_g";
const TCEString AlteraMemGenerator::ADDRW_G = "addrw_g";
const TCEString AlteraMemGenerator::DATAW_G = "dataw_g";

AlteraMemGenerator::AlteraMemGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    TCEString initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream): 
    MemoryGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                    integrator, warningStream, errorStream) {
}

AlteraMemGenerator::~AlteraMemGenerator() {
}


std::vector<TCEString>
AlteraMemGenerator::instantiateAlteraTemplate(
        const TCEString& templateFile,
        const TCEString& outputPath) const {

    TCEString inputFile = 
        templatePath() << FileSystem::DIRECTORY_SEPARATOR
                       << templateFile;
    TCEString outputFile;
    outputFile << outputPath << FileSystem::DIRECTORY_SEPARATOR  
               << moduleName() << ".vhd";
    
    instantiateTemplate(inputFile, outputFile, ttaCoreName());
    std::vector<TCEString> files;
    files.push_back(outputFile);
    return files;
}


void
AlteraMemGenerator::addMemory(
    const ProGe::NetlistBlock& ttaCore,
    ProGe::Netlist& netlist,
    int memIndex) {

    ProGe::NetlistBlock& topBlock = netlist.topLevelBlock();
    TCEString addrwGeneric = Conversion::toString(memoryAddrWidth());
    TCEString datawGeneric = Conversion::toString(memoryTotalWidth());
    addGenerics(topBlock, addrwGeneric, datawGeneric, memIndex);

    MemoryGenerator::addMemory(ttaCore, netlist, memIndex);
}


void
AlteraMemGenerator::addGenerics(
    ProGe::NetlistBlock& topBlock, 
    const TCEString& addrWidth,
    const TCEString& dataWidth,
    int memIndex) {
    
    if (!topBlock.hasParameter(DEV_FAMILY_G)) {
        ProGe::Netlist::Parameter devFamilyTop = 
            {DEV_FAMILY_G, "string", platformIntegrator()->deviceFamily()};
        topBlock.setParameter(devFamilyTop);
    }
    ProGe::Netlist::Parameter devFamilyComp = 
        {DEV_FAMILY_G, "string", DEV_FAMILY_G};
    addParameter(devFamilyComp);

    TCEString initFileGenericTop;
    initFileGenericTop << instanceName(memIndex) << "_" << INIT_FILE_G;
    ProGe::Netlist::Parameter initFileParamTop = 
        {initFileGenericTop, "string", initializationFile()};
    topBlock.setParameter(initFileParamTop);
    ProGe::Netlist::Parameter initFileParamComp =
        {INIT_FILE_G, "string", initFileParamTop.name};
    addParameter(initFileParamComp);

    ProGe::Netlist::Parameter addrw = {ADDRW_G,"integer", addrWidth};
    ProGe::Netlist::Parameter memw =  {DATAW_G, "integer", dataWidth};
    addParameter(addrw);
    addParameter(memw);
}
