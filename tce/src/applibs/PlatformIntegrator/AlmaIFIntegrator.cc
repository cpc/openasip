/*
    Copyright (c) 2002-2016 Tampere University.

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
 * @file AlmaIFIntegrator.cc
 *
 * Implementation of AlmaIFIntegrator class.
 */

#include "AlmaIFIntegrator.hh"
#include "VhdlRomGenerator.hh"
#include "DefaultProjectFileGenerator.hh"
#include "Machine.hh"
#include "NetlistPort.hh"
#include "NetlistBlock.hh"
#include "MachineImplementation.hh"
#include "XilinxBlockRamGenerator.hh"
#include "Conversion.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "MathTools.hh"
#include "HDLTemplateInstantiator.hh"


using ProGe::NetlistBlock;
using ProGe::NetlistPort;

const TCEString AlmaIFIntegrator::DMEM_NAME = "data";
const TCEString AlmaIFIntegrator::PMEM_NAME = "param";
const TCEString AlmaIFIntegrator::ALMAIF_MODULE = "tta_accel";


AlmaIFIntegrator::AlmaIFIntegrator() : PlatformIntegrator() {
}


AlmaIFIntegrator::AlmaIFIntegrator(
    const TTAMachine::Machine* machine,
    const IDF::MachineImplementation* idf,
    ProGe::HDL hdl,
    TCEString progeOutputDir,
    TCEString coreEntityName,
    TCEString outputDir,
    TCEString programName,
    int targetClockFreq,
    std::ostream& warningStream,
    std::ostream& errorStream,
    const MemInfo& imem,
    MemType dmemType):
    PlatformIntegrator(machine, idf, hdl, progeOutputDir, coreEntityName,
                       outputDir, programName, targetClockFreq, warningStream,
                       errorStream, imem, dmemType),
    imemGen_(NULL), dmemGen_(), almaifBlock_(NULL), deviceFamily_(""),
    fileGen_(new DefaultProjectFileGenerator(coreEntityName, this)) {
    if (idf->icDecoderParameterValue("debugger") != "external") {
        // TODO: Support for no debugger (e.g. instantiate a minimal one)
        TCEString msg =
            "AlmaIF interface requires connections to an external debugger.";
        throw InvalidData(__FILE__, __LINE__, "AlmaIFIntegrator", msg);
    }
}


AlmaIFIntegrator::~AlmaIFIntegrator() {

    if (imemGen_ != NULL) {
        delete imemGen_;
    }
    if (!dmemGen_.empty()) {
        std::map<TCEString, MemoryGenerator*>::iterator iter =
            dmemGen_.begin();
        while (iter != dmemGen_.end()) {
            delete iter->second;
            iter++;
        }
    }
}

bool
AlmaIFIntegrator::verifyMemories() const {
    bool found_dmem(false);
    bool found_pmem(false);
    for (int i = 0; i < dmemCount(); ++i) {
        MemInfo dmem = dmemInfo(i);
        if (dmem.widthInMaus * dmem.mauWidth == 32) {
            if (dmem.asName == DMEM_NAME) {
                found_dmem = true;
            } else if (dmem.asName == PMEM_NAME) {
                found_pmem = true;
            }
        }
    }
    return found_dmem && found_pmem;
}


void
AlmaIFIntegrator::integrateProcessor(
    const ProGe::NetlistBlock* progeBlockInOldNetlist) {

    initPlatformNetlist(progeBlockInOldNetlist);

    if (!verifyMemories()) {
        // TODO: Support empty DMEM/PMEM
        TCEString msg =  "AlmaIF interface requires 32-bit wide '";
        msg << DMEM_NAME << "' and '" << PMEM_NAME << "'' memory spaces.";
        throw InvalidData(__FILE__, __LINE__, "AlmaIFIntegrator", msg);
    }

    TCEString axi_addrw = Conversion::toString(axiAddressWidth());
    netlist()->setParameter("axi_addrw_g", "integer", axi_addrw);

    addAlmaifBlock();

    const NetlistBlock& core = progeBlock();
    if (!integrateCore(core)) {
        return;
    }

    writeNewToplevel();

    addAlmaifFiles();
    addProGeFiles();

    projectFileGenerator()->writeProjectFiles();
}

int
AlmaIFIntegrator::axiAddressWidth() const {
    int addressWidth = MathTools::requiredBits(
        (imemInfo().widthInMaus*imemInfo().mauWidth+31)/32-1)
        +imemInfo().portAddrw;
    for (int i = 0; i < dmemCount(); ++i) {
        MemInfo dmem = dmemInfo(i);
        if (dmem.asName == DMEM_NAME || dmem.asName == PMEM_NAME) {
            addressWidth = std::max(addressWidth, dmem.portAddrw);
        }
    }

    // Debugger address space:
    addressWidth = std::max(addressWidth, 8);

    // Pad by four; 2 on MSB end for memory select bits, 2 on LSB end
    // (byte-addressed memory)
    return addressWidth + 4;
}

void
AlmaIFIntegrator::addAlmaifBlock() {
    NetlistBlock& toplevel = netlist()->topLevelBlock();
    almaifBlock_ = new NetlistBlock(ALMAIF_MODULE, ALMAIF_MODULE+ "_0",
                                  *netlist());

    NetlistPort* clk = new NetlistPort(
            "clk", "1", 1, ProGe::BIT, HDB::IN, *almaifBlock_);
    NetlistPort* nreset = new NetlistPort(
            "nreset", "1", 1, ProGe::BIT, HDB::IN, *almaifBlock_);
    netlist()->connectPorts(*clockPort(), *clk);
    netlist()->connectPorts(*resetPort(), *nreset);

    // AXI4 bus for toplevel and AlmaIF block
    NetlistPort* s_axi_awaddr        = new NetlistPort("s_axi_awaddr",
            "axi_addrw_g", ProGe::BIT_VECTOR, HDB::IN, toplevel);
    NetlistPort* s_axi_awaddr_almaif = new NetlistPort("s_axi_awaddr",
            "axi_addrw_g", ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    netlist()->connectPorts(*s_axi_awaddr, *s_axi_awaddr_almaif);

    NetlistPort* s_axi_awvalid        = new NetlistPort("s_axi_awvalid",
            "1", ProGe::BIT, HDB::IN, toplevel);
    NetlistPort* s_axi_awvalid_almaif = new NetlistPort("s_axi_awvalid",
            "1", ProGe::BIT, HDB::IN, *almaifBlock_);
    netlist()->connectPorts(*s_axi_awvalid, *s_axi_awvalid_almaif);

    NetlistPort* s_axi_awready        = new NetlistPort("s_axi_awready",
            "1", ProGe::BIT, HDB::OUT, toplevel);
    NetlistPort* s_axi_awready_almaif = new NetlistPort("s_axi_awready",
            "1", ProGe::BIT, HDB::OUT, *almaifBlock_);
    netlist()->connectPorts(*s_axi_awready, *s_axi_awready_almaif);

    NetlistPort* s_axi_wdata        = new NetlistPort("s_axi_wdata",
            "32", 32, ProGe::BIT_VECTOR, HDB::IN, toplevel);
    NetlistPort* s_axi_wdata_almaif = new NetlistPort("s_axi_wdata",
            "32", 32, ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    netlist()->connectPorts(*s_axi_wdata, *s_axi_wdata_almaif);

    NetlistPort* s_axi_wstrb        = new NetlistPort("s_axi_wstrb",
            "4", 4, ProGe::BIT_VECTOR, HDB::IN, toplevel);
    NetlistPort* s_axi_wstrb_almaif = new NetlistPort("s_axi_wstrb",
            "4", 4, ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    netlist()->connectPorts(*s_axi_wstrb, *s_axi_wstrb_almaif);

    NetlistPort* s_axi_wvalid        = new NetlistPort("s_axi_wvalid",
            "1", ProGe::BIT, HDB::IN, toplevel);
    NetlistPort* s_axi_wvalid_almaif = new NetlistPort("s_axi_wvalid",
            "1", ProGe::BIT, HDB::IN, *almaifBlock_);
    netlist()->connectPorts(*s_axi_wvalid, *s_axi_wvalid_almaif);

    NetlistPort* s_axi_wready        = new NetlistPort("s_axi_wready",
            "1", ProGe::BIT, HDB::OUT, toplevel);
    NetlistPort* s_axi_wready_almaif = new NetlistPort("s_axi_wready",
            "1", ProGe::BIT, HDB::OUT, *almaifBlock_);
    netlist()->connectPorts(*s_axi_wready, *s_axi_wready_almaif);

    NetlistPort* s_axi_bresp        = new NetlistPort("s_axi_bresp",
            "2", 2, ProGe::BIT_VECTOR, HDB::OUT, toplevel);
    NetlistPort* s_axi_bresp_almaif = new NetlistPort("s_axi_bresp",
            "2", 2, ProGe::BIT_VECTOR, HDB::OUT, *almaifBlock_);
    netlist()->connectPorts(*s_axi_bresp, *s_axi_bresp_almaif);

    NetlistPort* s_axi_bvalid        = new NetlistPort("s_axi_bvalid",
            "1", ProGe::BIT, HDB::OUT, toplevel);
    NetlistPort* s_axi_bvalid_almaif = new NetlistPort("s_axi_bvalid",
            "1", ProGe::BIT, HDB::OUT, *almaifBlock_);
    netlist()->connectPorts(*s_axi_bvalid, *s_axi_bvalid_almaif);

    NetlistPort* s_axi_bready        = new NetlistPort("s_axi_bready",
            "1", ProGe::BIT, HDB::IN, toplevel);
    NetlistPort* s_axi_bready_almaif = new NetlistPort("s_axi_bready",
            "1", ProGe::BIT, HDB::IN, *almaifBlock_);
    netlist()->connectPorts(*s_axi_bready, *s_axi_bready_almaif);

    NetlistPort* s_axi_araddr        = new NetlistPort("s_axi_araddr",
            "axi_addrw_g", ProGe::BIT_VECTOR, HDB::IN, toplevel);
    NetlistPort* s_axi_araddr_almaif = new NetlistPort("s_axi_araddr",
            "axi_addrw_g", ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    netlist()->connectPorts(*s_axi_araddr, *s_axi_araddr_almaif);

    NetlistPort* s_axi_arvalid        = new NetlistPort("s_axi_arvalid",
            "1", ProGe::BIT, HDB::IN, toplevel);
    NetlistPort* s_axi_arvalid_almaif = new NetlistPort("s_axi_arvalid",
            "1", ProGe::BIT, HDB::IN, *almaifBlock_);
    netlist()->connectPorts(*s_axi_arvalid, *s_axi_arvalid_almaif);

    NetlistPort* s_axi_arready        = new NetlistPort("s_axi_arready",
            "1", ProGe::BIT, HDB::OUT, toplevel);
    NetlistPort* s_axi_arready_almaif = new NetlistPort("s_axi_arready",
            "1", ProGe::BIT, HDB::OUT, *almaifBlock_);
    netlist()->connectPorts(*s_axi_arready, *s_axi_arready_almaif);

    NetlistPort* s_axi_rdata        = new NetlistPort("s_axi_rdata",
            "32", 32, ProGe::BIT_VECTOR, HDB::OUT, toplevel);
    NetlistPort* s_axi_rdata_almaif = new NetlistPort("s_axi_rdata",
            "32", 32, ProGe::BIT_VECTOR, HDB::OUT, *almaifBlock_);
    netlist()->connectPorts(*s_axi_rdata, *s_axi_rdata_almaif);

    NetlistPort* s_axi_rresp        = new NetlistPort("s_axi_rresp",
            "2", 2, ProGe::BIT_VECTOR, HDB::OUT, toplevel);
    NetlistPort* s_axi_rresp_almaif = new NetlistPort("s_axi_rresp",
            "2", 2, ProGe::BIT_VECTOR, HDB::OUT, *almaifBlock_);
    netlist()->connectPorts(*s_axi_rresp, *s_axi_rresp_almaif);

    NetlistPort* s_axi_rvalid        = new NetlistPort("s_axi_rvalid",
            "1", ProGe::BIT, HDB::OUT, toplevel);
    NetlistPort* s_axi_rvalid_almaif = new NetlistPort("s_axi_rvalid",
            "1", ProGe::BIT, HDB::OUT, *almaifBlock_);
    netlist()->connectPorts(*s_axi_rvalid, *s_axi_rvalid_almaif);

    NetlistPort* s_axi_rready        = new NetlistPort("s_axi_rready",
            "1", ProGe::BIT, HDB::IN, toplevel);
    NetlistPort* s_axi_rready_almaif = new NetlistPort("s_axi_rready",
            "1", ProGe::BIT, HDB::IN, *almaifBlock_);
    netlist()->connectPorts(*s_axi_rready, *s_axi_rready_almaif);

    // Signals between AlmaIF block and TTA core
    almaif_ttacore_ports["busy"] = new NetlistPort("core_busy",
            "1", ProGe::BIT, HDB::OUT, *almaifBlock_);
    almaif_ttacore_ports["imem_data"] = new NetlistPort("core_imem_data",
            "IMEMDATAWIDTH",
            ProGe::BIT_VECTOR, HDB::OUT, *almaifBlock_);
    almaif_ttacore_ports["imem_en_x"] = new NetlistPort("core_imem_en_x",
            "1", ProGe::BIT, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["imem_addr"] = new NetlistPort("core_imem_addr",
            "IMEMADDRWIDTH",
            ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_DATA_addr"] = new NetlistPort(
            "core_fu_LSU_DATA_addr", "fu_LSU_DATA_addrw-2",
            ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_DATA_mem_en"] = new NetlistPort(
            "core_fu_LSU_DATA_mem_en",
            "1", 1, ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_DATA_wr_en"] = new NetlistPort(
            "core_fu_LSU_DATA_wr_en",
            "1", 1, ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_DATA_wr_mask"] = new NetlistPort(
            "core_fu_LSU_DATA_wr_mask", "fu_LSU_DATA_dataw/8",
            ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_DATA_data_in"] = new NetlistPort(
            "core_fu_LSU_DATA_data_in", "fu_LSU_DATA_dataw",
            ProGe::BIT_VECTOR, HDB::OUT, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_DATA_data_out"] = new NetlistPort(
            "core_fu_LSU_DATA_data_out", "fu_LSU_DATA_dataw",
            ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_PARAM_addr"] = new NetlistPort(
            "core_fu_LSU_PARAM_addr", "fu_LSU_PARAM_addrw-2",
            ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_PARAM_mem_en"] =
        new NetlistPort("core_fu_LSU_PARAM_mem_en",
            "1", 1, ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_PARAM_wr_en"] =
        new NetlistPort("core_fu_LSU_PARAM_wr_en",
            "1", 1, ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_PARAM_wr_mask"] = new NetlistPort(
            "core_fu_LSU_PARAM_wr_mask", "fu_LSU_PARAM_dataw/8",
            ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_PARAM_data_in"] = new NetlistPort(
            "core_fu_LSU_PARAM_data_in", "fu_LSU_PARAM_dataw",
            ProGe::BIT_VECTOR, HDB::OUT, *almaifBlock_);
    almaif_ttacore_ports["fu_LSU_PARAM_data_out"] = new NetlistPort(
            "core_fu_LSU_PARAM_data_out", "fu_LSU_PARAM_dataw",
            ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["db_pc_start"] = new NetlistPort(
            "core_db_pc_start", "IMEMADDRWIDTH", ProGe::BIT_VECTOR,
            HDB::OUT, *almaifBlock_);
    almaif_ttacore_ports["db_instr"] = new NetlistPort(
            "core_db_instr", "IMEMDATAWIDTH", ProGe::BIT_VECTOR,
            HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["db_pc"] = new NetlistPort(
            "core_db_pc", "IMEMADDRWIDTH", ProGe::BIT_VECTOR,
            HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["db_pc_next"] = new NetlistPort(
            "core_db_pc_next", "IMEMADDRWIDTH", ProGe::BIT_VECTOR,
            HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["db_bustraces"] = new NetlistPort(
            "core_db_bustraces", "32*BUSCOUNT",
            ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["db_lockcnt"] = new NetlistPort(
            "core_db_lockcnt", "32", 32, ProGe::BIT_VECTOR,
            HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["db_cyclecnt"] = new NetlistPort(
            "core_db_cyclecnt", "32", 32, ProGe::BIT_VECTOR,
            HDB::IN, *almaifBlock_);
    almaif_ttacore_ports["db_tta_nreset"] = new NetlistPort(
            "core_db_tta_nreset", "1", ProGe::BIT,
            HDB::OUT, *almaifBlock_);
    almaif_ttacore_ports["db_lockrq"] = new NetlistPort(
            "core_db_lockrq", "1", ProGe::BIT, HDB::OUT, *almaifBlock_);

    // Signals to memory
    addMemoryPorts(DMEM_NAME, "fu_LSU_DATA_dataw", "fu_LSU_DATA_addrw-2");
    addMemoryPorts(PMEM_NAME, "fu_LSU_PARAM_dataw", "fu_LSU_PARAM_addrw-2");
    addMemoryPorts("INSTR", "((IMEMDATAWIDTH+7)/8)*8", "IMEMADDRWIDTH");

    toplevel.addSubBlock(almaifBlock_);
}

void
AlmaIFIntegrator::addMemoryPorts(const TCEString as_name,
    const TCEString data_width, const TCEString addr_width) {
    new NetlistPort(as_name + "_wr_en", "1", ProGe::BIT, HDB::OUT,
        *almaifBlock_);
    new NetlistPort(as_name + "_mem_en", "1", ProGe::BIT, HDB::OUT,
        *almaifBlock_);
    new NetlistPort(as_name + "_addr", addr_width, ProGe::BIT_VECTOR,
        HDB::OUT, *almaifBlock_);
    new NetlistPort(as_name + "_data_in", data_width,
         ProGe::BIT_VECTOR, HDB::OUT, *almaifBlock_);
    new NetlistPort(as_name + "_data_out", data_width,
         ProGe::BIT_VECTOR, HDB::IN, *almaifBlock_);
    new NetlistPort(as_name + "_wr_mask", data_width + "/8",
        ProGe::BIT_VECTOR, HDB::OUT, *almaifBlock_);
}

void
AlmaIFIntegrator::addAlmaifFiles() {
    std::vector<TCEString> almaifFiles;

    TCEString basePath = Environment::dataDirPath("ProGe");
    TCEString platformPath = basePath;
    platformPath << FileSystem::DIRECTORY_SEPARATOR << "platform" <<
        FileSystem::DIRECTORY_SEPARATOR;
    TCEString dbPath = basePath;
    dbPath << FileSystem::DIRECTORY_SEPARATOR << "debugger" <<
        FileSystem::DIRECTORY_SEPARATOR;

    instantiatePlatformFile(platformPath + "tta-accel-entity.vhdl.tmpl",
        almaifFiles);
    instantiatePlatformFile(platformPath + "tta-accel-rtl.vhdl.tmpl",
        almaifFiles);
    instantiatePlatformFile(platformPath + "tta-axislave-entity.vhdl.tmpl",
        almaifFiles);
    instantiatePlatformFile(platformPath + "tta-axislave-rtl.vhdl.tmpl",
        almaifFiles);


    instantiatePlatformFile(dbPath + "debugger_if-pkg.vhdl.tmpl",
        almaifFiles);
    copyPlatformFile(dbPath + "cdc-entity.vhdl",
        almaifFiles);
    copyPlatformFile(dbPath + "cdc-rtl.vhdl",
        almaifFiles);
    copyPlatformFile(dbPath + "dbregbank-entity.vhdl",
        almaifFiles);
    copyPlatformFile(dbPath + "dbregbank-rtl.vhdl",
        almaifFiles);
    copyPlatformFile(dbPath + "dbsm-entity.vhdl",
        almaifFiles);
    copyPlatformFile(dbPath + "dbsm-rtl.vhdl",
        almaifFiles);
    copyPlatformFile(dbPath + "debugger-entity.vhdl",
        almaifFiles);
    copyPlatformFile(dbPath + "debugger-struct.vhdl",
        almaifFiles);
    copyPlatformFile(dbPath + "dbregbank-rtl.vhdl",
        almaifFiles);
    copyPlatformFile(dbPath + "registers-pkg.vhdl",
        almaifFiles);


    for (unsigned int i = 0; i < almaifFiles.size(); i++) {
        projectFileGenerator()->addHdlFile(almaifFiles.at(i));
    }
}

void
AlmaIFIntegrator::copyPlatformFile(const TCEString inputPath,
    std::vector<TCEString>& fileList) const {

    TCEString outputPath = outputFilePath(FileSystem::fileOfPath(inputPath),
        true);

    FileSystem::copy(inputPath, outputPath);
    fileList.push_back(outputPath);
}

void
AlmaIFIntegrator::instantiatePlatformFile(const TCEString inputPath,
    std::vector<TCEString>& fileList) const {

    TCEString filename = FileSystem::fileOfPath(inputPath);
    filename.replaceString(".tmpl", "");
    TCEString outputPath = outputFilePath(filename, true);

    HDLTemplateInstantiator inst;
    inst.setEntityString(coreEntityName());
    inst.instantiateTemplateFile(inputPath, outputPath);

    fileList.push_back(outputPath);
}

bool
AlmaIFIntegrator::integrateCore(const ProGe::NetlistBlock& core) {
    TCEString clkPortName = PlatformIntegrator::TTA_CORE_CLK;
    TCEString resetPortName = PlatformIntegrator::TTA_CORE_RSTX;
    for (int i = 0; i < core.portCount(); ++i) {
        NetlistPort& corePort = core.port(i);
        TCEString portName = corePort.name();

        // Connect global clock and reset ports
        if (portName == PlatformIntegrator::TTA_CORE_CLK) {
            netlist()->connectPorts(*clockPort(), corePort);
        } else if (portName == PlatformIntegrator::TTA_CORE_RSTX) {
            netlist()->connectPorts(*resetPort(), corePort);
        } else if (almaif_ttacore_ports.find(portName)
                   != almaif_ttacore_ports.end()) {
            // Connect AlmaIF block to TTA
            netlist()->connectPorts(corePort, *almaif_ttacore_ports[portName]);
        }
    }

    if(!createMemories()) {
        return false;
    }

    exportUnconnectedPorts();

    return true;
}

MemoryGenerator&
AlmaIFIntegrator::imemInstance(MemInfo imem) {

    assert(imem.type != UNKNOWN && "Imem type not set!");

    if (imemGen_ == NULL) {
        if (imem.type == ONCHIP) {
            imemGen_ =
                new XilinxBlockRamGenerator(
                    imem.mauWidth, imem.widthInMaus, imem.portAddrw, "",
                    this, warningStream(), errorStream(), true, almaifBlock_,
                    "INSTR");
        } else {
            TCEString msg = "Unsupported instruction memory type";
            throw InvalidData(__FILE__, __LINE__, "AlmaIFIntegrator", msg);
        }
    }
    return *imemGen_;
}


MemoryGenerator&
AlmaIFIntegrator::dmemInstance(
    MemInfo dmem,
    TTAMachine::FunctionUnit& lsuArch,
    HDB::FUImplementation& lsuImplementation) {

    bool connectToArbiter = false;
    if (dmem.asName == DMEM_NAME || dmem.asName == PMEM_NAME) {
        connectToArbiter = true;
        if (dmem.mauWidth*dmem.widthInMaus != 32){
            TCEString msg = "Data memory must be 32 bits wide";
            throw InvalidData(__FILE__, __LINE__, "AlmaIFIntegrator", msg);
        }
    }

    MemoryGenerator* memGen = NULL;
    if (dmemGen_.find(dmem.asName) != dmemGen_.end()) {
        memGen = dmemGen_.find(dmem.asName)->second;
    } else {
        if (dmem.type == ONCHIP) {
            // onchip mem size is scalable, use value from adf's Address Space
            int addrw = dmem.asAddrw;
            memGen =
                new XilinxBlockRamGenerator(
                    dmem.mauWidth, dmem.widthInMaus, addrw, "",
                    this, warningStream(), errorStream(), connectToArbiter,
                    almaifBlock_, dmem.asName);
        } else {
            TCEString msg = "Unsupported data memory type";
            throw InvalidData(__FILE__, __LINE__, "AlmaIFIntegrator", msg);
        }
        memGen->addLsu(lsuArch, lsuImplementation);
        dmemGen_[dmem.asName] = memGen;
    }
    return *memGen;
}

void
AlmaIFIntegrator::printInfo(std::ostream& stream) const {
    stream
        << "Integrator name: AlmaIFIntegrator" << std::endl
        << "-----------------------------" << std::endl
        << "Integrates the processor core to an AXI4 bus interface according "
        << "to ALMARVI HW Integration Interface specification." << std::endl
        << "Supported instruction memory type is 'onchip'." << std::endl
        << "Supported data memory type is 'onchip'." << std::endl
        << "Data and Parameter memory spaces must be named '" << DMEM_NAME
        << "' and '" << PMEM_NAME << "' respectively." << std::endl
        << std::endl;
}

bool
AlmaIFIntegrator::chopTaggedSignals() const {
    return true;
}

TCEString
AlmaIFIntegrator::deviceFamily() const {

    return deviceFamily_;
}

void
AlmaIFIntegrator::setDeviceFamily(TCEString devFamily) {

    deviceFamily_ = devFamily;
}

// these are not relevant here
TCEString
AlmaIFIntegrator::deviceName() const {
    return "";
}

TCEString
AlmaIFIntegrator::devicePackage() const {
    return "";
}

TCEString
AlmaIFIntegrator::deviceSpeedClass() const {
    return "";
}

int
AlmaIFIntegrator::targetClockFrequency() const {
    return 1;
}

TCEString
AlmaIFIntegrator::pinTag() const {
    return "";
}

ProjectFileGenerator*
AlmaIFIntegrator::projectFileGenerator() const {
    return fileGen_;
}