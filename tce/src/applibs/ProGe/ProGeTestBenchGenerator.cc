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
 * @file ProGeTestBenchGenerator.cc
 *
 * Implementation of ProGeTestBenchGenerator class.
 *
 * @author Esa Määttä 2007 (esa.maatta-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <cmath> // for log

#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wkeyword-macro")
#include <boost/regex.hpp>
POP_CLANG_DIAGS

#include "ProGeTypes.hh"
#include "ProGeTestBenchGenerator.hh"
#include "HDLTemplateInstantiator.hh"
#include "FileSystem.hh"
#include "Conversion.hh"
#include "Machine.hh"

#include "HDBManager.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "HDBRegistry.hh"
#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "FUImplementation.hh"
#include "FUPortImplementation.hh"
#include "MachineImplementation.hh"
#include "FUImplementationLocation.hh"
#include "FUExternalPort.hh"
#include "Exception.hh"
#include "StringTools.hh"


using namespace IDF; 
using namespace HDB; 
using namespace TTAMachine;
using namespace ProGe;

using std::string;
using std::endl;

using namespace TTAMachine;

const string ADDR_WIDTH = "addrw";

/**
 * The constructor.
 *
 */
ProGeTestBenchGenerator::ProGeTestBenchGenerator() {
    // empty
}


/**
 * The destructor.
 */
ProGeTestBenchGenerator::~ProGeTestBenchGenerator() {
    // empty
}

/** 
 * Generates testbench to given destination directory.
 * 
 * At the moment multiple address spaces for FUs are not supported. Only one
 * data memory and instruction memory are usable. There is no code for
 * generating new memory components, this is a todo item for later.
 * There is also additional limit that at most two function units can use the
 * one address space. Most of the TODO comments mark the places where code
 * needs to be changed to accomodate the removal of these limitations.
 *
 * @param mach Machine for which test bench is generated
 * @param implementation Implementation of the machine 
 * @param dstDirectory Destination directory where test bench is generated
 * @param progeOutDir Processor generators output directory 
 *
 * @exception IOException Problems in handling files.
 * @exception OutOfRange Too many FUs using same address space or same FU uses
 * more than one address space.
 * @exception InvalidName 
 * @exception InvalidData FU was not found from HDB.
 */
void
ProGeTestBenchGenerator::generate(
    const ProGe::HDL language, const TTAMachine::Machine& mach,
    const IDF::MachineImplementation& implementation,
    const std::string& dstDirectory, const std::string& progeOutDir,
    const std::string& entityStr) {
    entityStr_ = entityStr;
    language_  = language;
    // map to store FUs that use address spaces
    std::map<string, std::vector<FunctionUnit*> > ASFUs;
    
    // for now limit the number of function units that use certain address
    // space
    const unsigned int FUSPERAS = 2;
    // get all address spaces and function units that use them
    // every function unit can use different address space
    Machine::FunctionUnitNavigator FUNav = mach.functionUnitNavigator();
    for (int i = 0; i < FUNav.count(); ++i) {
        FunctionUnit* FU = FUNav.item(i);

        if (FU->hasAddressSpace()) {
            AddressSpace* AS = FU->addressSpace();

            std::map<string, std::vector<FunctionUnit*> >::iterator it
                = ASFUs.find(AS->name());
            if (it != ASFUs.end()) {
                // the number of FUs that can use same address space is
                // limited for now
                if (it->second.size() < FUSPERAS) { 
                    it->second.push_back(FU); 
                } else {
                    OutOfRange error(
                        __FILE__, __LINE__, __func__,
                         "More than two FUs use same address space.\n");
                    throw error;
                }
            } else {
                std::vector<FunctionUnit*> FUList;
                FUList.push_back(FU);
                ASFUs.insert(make_pair(AS->name(), FUList));
            }
        } 
    }

    if (ASFUs.size() > 1) {
        // only one address space for now
        string eMsg = "More than one address space used by FUs.";
        OutOfRange error(__FILE__, __LINE__, __func__, eMsg);
        throw error;
    }
    
    // copy test bench files used as a template
    copyTestBenchFiles(dstDirectory);

    // for every address space a memory component should be created to
    // the test bench vhdl files
    // TODO: for now it's quite unclear how memory components should be 
    //       created in vhdl and is there some clean and dynamic way to do
    //       it. So for now there is no support for multiple address spaces.
    // get this name from address space name when creating dynamically memory
    // units
    const string MEMORYNAME = "dmem";
    std::map<string, std::vector<FunctionUnit*> >::iterator it 
        = ASFUs.begin();
    std::map<string, std::vector<FunctionUnit*> >::iterator it_secLast
        = (ASFUs.empty()) ? ASFUs.end() : --(ASFUs.end());
    string LSUMap = "";
    while (it != ASFUs.end()) {
        // go through all FUs that have been mapped to address spaces
        HDB::FUImplementation* fuImplementation = NULL;
        for (unsigned int i = 0; i < it->second.size(); ++i) {
            FunctionUnit* FU = it->second.at(i);

            // get FUs implementation from hdb
            IDF::FUImplementationLocation& impl = 
                implementation.fuImplementation(FU->name());

            FUEntry* entry = NULL;
            try {
                HDBManager& manager = HDBRegistry::instance().hdb(
                    impl.hdbFile());
                entry = manager.fuByEntryID(impl.id());
            } catch (const KeyNotFound& e) {
                throw InvalidData(__FILE__, __LINE__, __func__, 
                    e.errorMessage());
            }

            // external ports can be found from implementation (hdb)
            fuImplementation = &entry->implementation();
            for (int j = 0, count = fuImplementation->externalPortCount();
                j < count; j++) {

                FUExternalPort& ep = fuImplementation->externalPort(j);
                LSUMap.append(getSignalMapping(FU->name(), ep.name(), 
                    (ep.widthFormula() == "1"), MEMORYNAME,
                    ((i > 0) ? "b" : "a"))); // only two FUs for same AS
                
                if (it == it_secLast && (i+1) == it->second.size() && 
                    (j + 1) == count) {
                    // last mapping
                    LSUMap.append(");\n");
                } else {
                    LSUMap.append(",\n");
                }
            }

        }

        // get data width and address width of memory unit that fu is
        // connected to, only once for every address space
        string dataWidth;
        string addrWidth;
        string dmemImageFilename("dmem_");
        if (ASFUs.size() == 1) {
            dmemImageFilename +=
                ASFUs.begin()->second.at(0)->addressSpace()->name();
            dmemImageFilename += "_";
        }
        dmemImageFilename += "init.img";
        for (int p = 0, count = fuImplementation->parameterCount();
            p < count; ++p) {

            FUImplementation::Parameter param = fuImplementation->parameter(p);
            if (string::npos != param.name.find("dataw") 
                && param.type == "integer") {

                if (param.value.length() < 1) {
                    // if width isn't stored read it from port
                    FunctionUnit* FU = it->second.at(0);
                    TTAMachine::HWOperation* hwop = FU->operation(0);
                    dataWidth = Conversion::toString(hwop->port(2)->width());
                } else {
                    dataWidth = param.value;
                }
            } else if (string::npos != param.name.find(ADDR_WIDTH) 
                && param.type == "integer") {
                // calculate the internal address width from the address space
                FunctionUnit* FU = it->second.at(0);
                AddressSpace* AS = FU->addressSpace();
                string internalAddrWidth = Conversion::toString(
                    static_cast<int>(ceil(log(AS->end()) / log(2))));

                // locate the external address port and its width formula
                string widthFormula;
                for (int i = 0; i < fuImplementation->externalPortCount();
                     i++) {
                    FUExternalPort& ep = fuImplementation->externalPort(i);
                    if (ep.widthFormula().find(ADDR_WIDTH) != string::npos) {
                        widthFormula = ep.widthFormula();
                        break;
                    }
                }

                if (widthFormula.empty()) {
                    addrWidth = internalAddrWidth;
                } else {
                    addrWidth = StringTools::replaceAllOccurrences(
                        widthFormula, ADDR_WIDTH, internalAddrWidth);
                }
            }
        }
        // TODO: don't create whole file here just add memory widths and 
        // the init file entry
        createTBConstFile(
            dstDirectory, dmemImageFilename, dataWidth, addrWidth);
        ++it;
    }  

    if (ASFUs.empty()) {
        createTBConstFile(dstDirectory); 
    }

    // the beginning of the core FU (load store unit) mappings
    string LSUMapConst;
    if (language == VHDL) {
        LSUMapConst =
            "port map (\n"
            "clk  => clk,\n"
            "rstx => rst_x,\n"
            "busy => '0',\n"
            "imem_en_x => imem_en_x,\n"
            "imem_addr => imem_addr,\n"
            "imem_data => imem_data,\n"
            "locked => locked";
        // Add external debugger ports, if needed
        if (implementation.icDecoderParameterValue("debugger") == 
            "external") {
            LSUMapConst.append(",\n"
                "db_pc_start => (others => '0'),\n"
                "db_tta_nreset => '1',\n"
                "db_lockrq => '0'");
        }
    } else {
        LSUMapConst=
            ".clk                (clk),\n"
            ".rstx               (rst_x),\n"
            ".busy               (1'b0),\n"
            ".imem_en_x          (imem_en_x),\n"
            ".imem_addr          (imem_addr),\n"
            ".imem_data          (imem_data)";
    }
    
    if (LSUMap.length() < 1) {
        LSUMapConst.append(");\n");
    } else {
        LSUMapConst.append(",\n");
        // append generated mappings
        LSUMapConst.append(LSUMap);
    }
    
    // read toplevel.vhdl from proge output dir for proc_arch.vhdl
    string toplevel = progeOutDir + FileSystem::DIRECTORY_SEPARATOR +
                      ((language == VHDL) ? "vhdl" : "verilog") +
                      FileSystem::DIRECTORY_SEPARATOR + entityStr +
                      ((language == VHDL) ? ".vhdl" : ".v");

    createProcArchVhdl(dstDirectory, toplevel, LSUMapConst);
}

/** 
 * Creates a new proc_arch vhdl file from a template file.
 * 
 * Writes a new toplevel and core signal mapping.
 *
 * @param dstDirectory Directory where new proc_arch file is created.
 * @param topLevelVhdl Toplevel vhdl file where toplevel is read for writing
 * to the proc_arch file. 
 * @param signalMappings Core signal mappings as a string to be writen to
 * the proc_arch file.
 *
 * @exception IOException Problems in handling files.
 */
void
ProGeTestBenchGenerator::createProcArchVhdl(
    const std::string& dstDirectory, const std::string& topLevelVhdl,
    const std::string& signalMappings) {
    if (!FileSystem::fileIsReadable(topLevelVhdl)) {
        string eMsg = "File was not readable: " + topLevelVhdl;
        IOException error(__FILE__, __LINE__, __func__, eMsg);
        throw error;
    }

    string startRE,endRE;
    if(language_==VHDL){
        startRE = std::string(".*entity.") + entityStr_ + ".is.*";
        endRE = std::string(".*end.") + entityStr_ + ";.*";
    } else {
        startRE = std::string(".*module.") + entityStr_;
        endRE = std::string(".*endmodule.*");
    }
    
    string block = "";
    bool ok = 
        FileSystem::readBlockFromFile(
            topLevelVhdl, startRE, endRE, block, false);
    
    if (!ok || block == "") 
        throw IOException(__FILE__, __LINE__, __func__, 
                          TCEString(
                              "Could not read processor entity from ") +
                          topLevelVhdl);
               

    TCEString sourceFile = 
        Environment::dataDirPath("ProGe") + FileSystem::DIRECTORY_SEPARATOR +
        "tb" + FileSystem::DIRECTORY_SEPARATOR +
        ((language_==VHDL)?"proc_arch.vhdl.tmpl":"proc_arch.v.tmpl");

    // change proc_arch.vhdl
    string procArch = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
        ((language_==VHDL)?"proc_arch.vhdl":"proc_arch.v");

    HDLTemplateInstantiator inst;
    inst.setEntityString(entityStr_);
    inst.instantiateTemplateFile(sourceFile, procArch);

    // check if readable and writable
    if(!FileSystem::fileIsReadable(procArch) || 
        !FileSystem::fileIsWritable(procArch)) {

        string eMsg = "File was not readable: " + procArch;
        IOException error(__FILE__, __LINE__, __func__, eMsg);
        throw error;
    }

    if(language_==VHDL){
        startRE = std::string(".*component.") + entityStr_ + ".*";
        endRE = std::string(".*end.component;.*");
        if (!FileSystem::appendReplaceFile(procArch, startRE, block, endRE,
            false)) {

            string eMsg = "Could not write toplevel to: " + procArch;
            IOException error(__FILE__, __LINE__, __func__, eMsg);
            throw error;
        }
    }
    
    if(language_==VHDL){
        startRE = std::string(".*core.:.") + entityStr_ + ".*";
        endRE = ".*datamem.:.synch_dualport_sram.*";
    }else{
        startRE = entityStr_ + std::string(".*core.*");
        endRE = ".*synch_dualport_sram.*";
    }
    if (!FileSystem::appendReplaceFile(procArch, startRE, signalMappings, 
        endRE, false)) {

        string eMsg = "Could not write core to: " + procArch;
        IOException error(__FILE__, __LINE__, __func__, eMsg);
        throw error;
    }
}

/** 
 * Creates core signal mapping for FUs external port -> data memory.
 * 
 * Maps signals according to some defined naming "standard".
 *
 * @param fuName Function unit name.
 * @param epName External port name. 
 * @param widthIsOne True if port width is one, false otherwise.
 * @param memoryName Memory name. 
 * @param memoryLine Memory port that is used (for dual port memory a or b).
 * 
 * @return Signal mapping as a string. 
 * @exception InvalidName External port name was invalid (not matching).
 */
std::string
ProGeTestBenchGenerator::getSignalMapping(
    const std::string& fuName, const std::string& epName, bool widthIsOne,
    const std::string& memoryName, const std::string& memoryLine) {
    const string sep = "_"; // separator between signal name elements
    
    // create fu signal name 
    string fuSignalName("fu" + sep);
    if(language_==VHDL)
        fuSignalName.append(fuName + sep + epName + ((widthIsOne) ? "(0)" : ""));
    else
        fuSignalName.append(fuName + sep + epName);

    // create memory signal name
    string memSignalName(memoryName + sep);

    if (epName == "data_in") {
        memSignalName.append("q" + sep + memoryLine);
    } else
    if (epName == "data_out") {
        memSignalName.append("d" + sep + memoryLine);
    } else
    if (epName == "addr") {
        memSignalName.append(epName + sep + memoryLine);
    } else
    if (epName == "mem_en_x") {
        memSignalName.append("en" + sep + memoryLine + sep + "x");
    } else
    if (epName == "wr_en_x") {
        memSignalName.append("wr" + sep + memoryLine + sep + "x");
    } else
    if (epName == "wr_mask_x") {
        memSignalName.append("bit" + sep + "wr" + sep + memoryLine + sep 
                             + "x");
    } else {
        string eMsg = "External port name didn't match any: " + epName;
        InvalidName error(__FILE__, __LINE__, __func__, eMsg);
        throw error;
    }

    if(language_==VHDL)
        return string(fuSignalName + " => " + memSignalName);

    return string("."+fuSignalName + "(" + memSignalName + ")");
}

/** 
 * Creates test bench constants package vhdl file.
 * 
 * @param dstDirectory Directory where the file is created.
 * @param dataWidth Memory data width. 
 * @param addrWidth Memory address width. 
 */
void
ProGeTestBenchGenerator::createTBConstFile(
    std::string dstDirectory, const std::string& dmemImage,
    const string& dataWidth, const string& addrWidth) {
    string dstFile = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
                     ((language_ == VHDL) ? "testbench_constants_pkg.vhdl"
                                          : "testbench_constants_pkg.vh");

    createFile(dstFile);

    std::ofstream stream(dstFile.c_str(), std::ofstream::out);
    if(language_==VHDL){
        stream << "package testbench_constants is" << endl
               << "-- width of the data memory" << endl
               << "constant DMEMDATAWIDTH : positive := "
               << ((dataWidth.empty()) ? "1" : dataWidth) << ";" << endl

               << "-- address width of the data memory" << endl
               << "constant DMEMADDRWIDTH : positive := "
               << ((addrWidth.empty()) ? "1" : addrWidth) << ";" << endl

               << "-- simulation run time" << endl
               << "constant RUNTIME : time := 5234 * 10 ns;" << endl

               << "-- memory init files" << endl
               << "constant DMEM_INIT_FILE : string := "
               << ((dataWidth.empty())
                       ? "\"\";"
                       : "\"tb" + FileSystem::DIRECTORY_SEPARATOR +
                             dmemImage + "\";")
               << endl

               << "constant IMEM_INIT_FILE : string := "
               << "\"tb" + FileSystem::DIRECTORY_SEPARATOR +
                      "imem_init.img\";"
               << endl
               << "end testbench_constants;" << endl;
    } else {
        stream << "// width of the data memory" << endl
               << "parameter DMEMDATAWIDTH = "
               << ((dataWidth.empty()) ? "1" : dataWidth) << "," << endl

               << "// address width of the data memory" << endl
               << "parameter DMEMADDRWIDTH = "
               << ((addrWidth.empty()) ? "1" : addrWidth) << "," << endl

               << "// simulation run time" << endl
               << "parameter RUNTIME = `SIMTIME,// ns" << endl

               << "// memory init files" << endl
               << "parameter DMEM_INIT_FILE = "
               << ((dataWidth.empty())
                       ? "\"\","
                       : "\"tb" + FileSystem::DIRECTORY_SEPARATOR +
                             dmemImage + "\",")
               << endl

               << "parameter IMEM_INIT_FILE = "
               << ((addrWidth.empty())
                       ? "\"\""
                       : "\"tb" + FileSystem::DIRECTORY_SEPARATOR +
                             "imem_init.img\"")
               << endl;
    }
    stream.close();
}

/**
 * Copies general testbench files to given destination directory.
 *
 * @param dstDirectory Destination directory for test bench files.
 */
void
ProGeTestBenchGenerator::copyTestBenchFiles(const std::string& dstDirectory) {
    // create destination directory for the testbench
    if(!FileSystem::createDirectory(dstDirectory) && 
       !FileSystem::fileIsDirectory(dstDirectory)) {
        return; 
    }

    const std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    // copy testbench base files to dstDirectory
    string sourceDir = Environment::dataDirPath("ProGe");
    sourceDir = sourceDir + DS + "tb";
    std::list<string> foundSourceFiles;

    string vhdlRegex = ((language_==VHDL)?".*\\.vhdl$":".*\\.v*$");
    FileSystem::findFromDirectory(vhdlRegex, sourceDir, foundSourceFiles);
    std::list<string>::iterator it = foundSourceFiles.begin();
    while (it != foundSourceFiles.end()) {
        FileSystem::copy(*it, dstDirectory);
        it++;
    }

    HDLTemplateInstantiator inst;
    inst.setEntityString(entityStr_);
    
    inst.instantiateTemplateFile(
        sourceDir + DS + 
        ((language_==VHDL)?"testbench.vhdl.tmpl":"testbench.v.tmpl"), 
        dstDirectory + DS +
        ((language_==VHDL)?"testbench.vhdl":"testbench.v"));
        
    if(language_==VHDL)
        inst.instantiateTemplateFile(
            sourceDir + DS + "proc_ent.vhdl.tmpl", 
            dstDirectory + DS + "proc_ent.vhdl");

}

/**
 * Creates a file.
 *
 * @param Name of the script file to be created.
 * @exception IOException Couldn't create the file.
 */
void
ProGeTestBenchGenerator::createFile(const std::string& fileName) {
    FileSystem::removeFileOrDirectory(fileName);
    bool isCreated = FileSystem::createFile(fileName);
    if (!isCreated) {
        string errorMsg = "Unable to create file " + fileName;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }
}
