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
#include "HWOperation.hh"
#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"
#include "SignalGroup.hh"
#include "NetlistBlock.hh"
#include "MachineImplementation.hh"
#include "XilinxBlockRamGenerator.hh"
#include "DummyMemGenerator.hh"
#include "Conversion.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "MathTools.hh"
#include "HDLTemplateInstantiator.hh"

using ProGe::NetlistBlock;
using ProGe::NetlistPort;
using ProGe::NetlistPortGroup;

const TCEString AlmaIFIntegrator::DMEM_NAME = "data";
const TCEString AlmaIFIntegrator::PMEM_NAME = "param";
const TCEString AlmaIFIntegrator::AXI_AS_NAME = "axi_as";
const TCEString AlmaIFIntegrator::ALMAIF_MODULE = "tta_accel";
const TCEString AlmaIFIntegrator::DEFAULT_DEVICE = "xc7z020clg400-1";

const int AlmaIFIntegrator::DEFAULT_RESERVED_PRIVATE_MEM_SIZE = 2048;
const int AlmaIFIntegrator::DEFAULT_LOCAL_MEMORY_WIDTH = 15;

AlmaIFIntegrator::AlmaIFIntegrator() : PlatformIntegrator() {
}

AlmaIFIntegrator::AlmaIFIntegrator(
    const TTAMachine::Machine* machine, const IDF::MachineImplementation* idf,
    ProGe::HDL hdl, TCEString progeOutputDir, TCEString coreEntityName,
    TCEString outputDir, TCEString programName, int targetClockFreq,
    std::ostream& warningStream, std::ostream& errorStream,
    const MemInfo& imem, MemType dmemType, bool syncReset,
    bool generateIntegratedTestbench)
    : PlatformIntegrator(
          machine, idf, hdl, progeOutputDir, coreEntityName, outputDir,
          programName, targetClockFreq, warningStream, errorStream, imem,
          dmemType),
      imemGen_(NULL),
      dmemGen_(),
      almaifBlock_(NULL),
      deviceFamily_(""),
      fileGen_(new DefaultProjectFileGenerator(coreEntityName, this)),
      secondDmem_(false),
      secondPmem_(false),
      dmemHandled_(false),
      pmemHandled_(false),
      syncReset_(syncReset),
      broadcast_pmem_(false),
      generateIntegratedTestbench_(generateIntegratedTestbench) {
    if (idf->icDecoderParameterValue("debugger") == "external") {
        hasMinimalDebugger_ = false;
    } else if (idf->icDecoderParameterValue("debugger") == "minimal") {
        hasMinimalDebugger_ = true;
    } else {
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

void
AlmaIFIntegrator::findMemories() {
    dmemInfo_ = pmemInfo_ = {UNKNOWN, 0, 0, 0, 0, false, "", ""};

    bool foundDmem = false;
    bool foundPmem = false;

    // This is more or less a duplicate of parseDataMemories in the superclass
    // to get around it not parsing memories if dmem type is 'none' and it not
    // handling multiple lsus well

    MemInfo secondDmemInfo;
    MemInfo secondPmemInfo;
    TTAMachine::Machine::FunctionUnitNavigator fuNav =
        this->machine()->functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit* fu = fuNav.item(i);
        if (fu->hasAddressSpace()) {
            TTAMachine::AddressSpace* as = fu->addressSpace();
            bool isLSU = false;
            for (int i = 0; i < fu->operationCount(); ++i) {
                std::string operation = fu->operation(i)->name();
                std::string prefix = operation.substr(0, 2);
                if (prefix == "ld" || prefix == "st") {
                    isLSU = true;
                    break;
                }
            }

            if (!isLSU) {
                continue;
            }

            if (as->name() == DMEM_NAME) {
                if (foundDmem) {
                    assert(!secondDmem_ && "Found third DMEM LSU");
                    secondDmem_ = true;
                    secondDmemInfo = readLsuParameters(*fu);
                    if (secondDmemInfo.portAddrw < dmemInfo_.portAddrw) {
                        // Second DMEM should have lesser or equal data
                        // width => greater or equal address width
                        // swap if this is not the case
                        secondDmemInfo = dmemInfo_;
                        dmemInfo_ = readLsuParameters(*fu);
                    }
                    secondDmemName_ = secondDmemInfo.lsuName;
                } else {
                    foundDmem = true;
                    dmemInfo_ = readLsuParameters(*fu);
                    if (dmem_dram_) {
                        dmemInfo_.type = DRAM;
                    }
                }
            } else if (as->name() == PMEM_NAME) {
                if (foundPmem) {
                    assert(!secondPmem_ && "Found third PMEM LSU");
                    secondPmem_ = true;
                    secondPmemInfo = readLsuParameters(*fu);
                } else {
                    foundPmem = true;
                    pmemInfo_ = readLsuParameters(*fu);

                    // Needed for the case when the local memory doesn't exist
                    // and needs to reserved from the buffer memory
                    if (as->hasNumericalId(0)) {
                        hasSeparateLocalMemory_ = false;
                    }
                }
            }
        }
    }

    if (secondDmem_) {
        secondDmemName_ = secondDmemInfo.lsuName;
        secondDmemDataw_ =
            secondDmemInfo.mauWidth * secondDmemInfo.widthInMaus;
        secondDmemAddrw_ = secondDmemInfo.portAddrw;
    } else if (foundDmem) {
        secondDmemDataw_ = 32;
        secondDmemAddrw_ = dmemInfo_.asAddrw - 2;
    }

    if (secondPmem_) {
        secondPmemName_ = secondPmemInfo.lsuName;
        secondPmemDataw_ =
            secondPmemInfo.mauWidth * secondDmemInfo.widthInMaus;
        secondPmemAddrw_ = secondPmemInfo.portAddrw;
    } else if (foundPmem) {
        secondPmemDataw_ = 32;
        secondPmemAddrw_ = pmemInfo_.asAddrw - 2;
    }
}

void
AlmaIFIntegrator::integrateProcessor(
    const ProGe::NetlistBlock* progeBlockInOldNetlist) {

    initPlatformNetlist(progeBlockInOldNetlist);
    findMemories();
    initAlmaifBlock();
    const NetlistBlock& cores = progeBlock();
    int coreCount = 1;
    for (int i = 0; i < coreCount; i++) {
        int coreId = coreCount == 1 ? -1 : i;
        if (!integrateCore(cores, coreId)) {
            return;
        }
    }

    writeNewToplevel();

    addAlmaifFiles();
    addProGeFiles();

    if (generateIntegratedTestbench_) {
        generateIntegratedTestbench();
    }

    projectFileGenerator()->writeProjectFiles();
}

/**
 * Returns the AXI bus facing address width: 2 bits for address space select
 * + maximum of the four address space widths (byte-addressed):
 *
 * - CTRL block (fixed 1024 bytes per core)
 * - Instruction memory
 * - Data and parameter memories
 */
TCEString
AlmaIFIntegrator::axiAddressWidth() const {
    // IMEM address width
    int imemAddressWidth = 0;
    if (imemInfo().type != VHDL_ARRAY) {
        imemAddressWidth =
            MathTools::requiredBits0Bit0(
                (imemInfo().widthInMaus * imemInfo().mauWidth + 7) / 8 - 1) +
            imemInfo().portAddrw;
    }
    // Debugger address space:
    int debugAddressWidth = 8;
    int dmemAddressWidth =
        dmemInfo_.portAddrw +
        MathTools::requiredBits0Bit0(
            std::max(0, dmemInfo_.mauWidth * dmemInfo_.widthInMaus / 8 - 1));

    // Skip param if its width is set by a generic: if there is a larger
    // memory (imem/dmem), the value will still be correct
    int pmemAddressWidth = 0;
    if (pmemInfo_.asAddrw != 32) {
        pmemAddressWidth =
            pmemInfo_.portAddrw +
            MathTools::requiredBits0Bit0(std::max(
                0, pmemInfo_.mauWidth * pmemInfo_.widthInMaus / 8 - 1));
    } else {
        pmemAddressWidth = DEFAULT_LOCAL_MEMORY_WIDTH;
    }
    int axiAddressWidth = std::max(
        std::max(imemAddressWidth, debugAddressWidth),
        std::max(dmemAddressWidth, pmemAddressWidth));

    return Conversion::toString(axiAddressWidth + 2);
}

void
AlmaIFIntegrator::initAlmaifBlock() {
    ProGe::Netlist& netlist = integratorBlock()->netlist();
    TCEString core_count = "1";

    TCEString platformPath = Environment::dataDirPath("ProGe");
    platformPath << FileSystem::DIRECTORY_SEPARATOR << "platform"
                 << FileSystem::DIRECTORY_SEPARATOR;

    int imemdataw = imemInfo().mauWidth * imemInfo().widthInMaus;
    int imemaddrw = imemInfo().portAddrw;
    int imemAxiAddrw =
        MathTools::requiredBits0Bit0((imemdataw + 31) / 32 - 1) + imemaddrw;
    int bustrace_width = 0;

    if (imem_dp_) {
        imemdataw = MathTools::roundUpToPowerTwo(imemdataw);
    }

    for (int i = 0; i < machine()->busNavigator().count(); ++i) {
        int bus_width = machine()->busNavigator().item(i)->width();
        // Busess are padded to a multiple of 32 bits
        bus_width = (bus_width + 31) / 32 * 32;
        bustrace_width += bus_width;
    }

    almaifBlock_ = new NetlistBlock(
        ALMAIF_MODULE, ALMAIF_MODULE + "_0", integratorBlock());

    netlist.setParameter("AXI_ADDR_WIDTH", "integer", axiAddressWidth());
    integratorBlock()->setParameter(
        "axi_addr_width_g", "integer", "AXI_ADDR_WIDTH");
    netlist.setParameter("AXI_ID_WIDTH", "integer", "12");
    integratorBlock()->setParameter(
        "axi_id_width_g", "integer", "AXI_ID_WIDTH");
    almaifBlock_->setParameter("core_count_g", "integer", core_count);
    almaifBlock_->setParameter(
        "axi_addr_width_g", "integer", "axi_addr_width_g");
    almaifBlock_->setParameter("axi_id_width_g", "integer", "axi_id_width_g");

    almaifBlock_->setParameter(
        "imem_addr_width_g", "integer", Conversion::toString(imemaddrw));

    almaifBlock_->setParameter(
        "imem_axi_addr_width_g", "integer",
        Conversion::toString(imemAxiAddrw));
    if (imemInfo().type != VHDL_ARRAY) {
        almaifBlock_->setParameter(
            "imem_data_width_g", "integer", Conversion::toString(imemdataw));
    } else {
        almaifBlock_->setParameter("imem_data_width_g", "integer", "0");
    }

    almaifBlock_->setParameter(
        "bus_count_g", "integer", Conversion::toString(bustrace_width / 32));
    almaifBlock_->setParameter(
        "local_mem_addrw_g", "integer", "local_mem_addrw_g");

    // Add and connect clock and reset ports
    NetlistPort* clk =
        new NetlistPort("clk", "1", 1, ProGe::BIT, ProGe::IN, *almaifBlock_);
    NetlistPort* rstx =
        new NetlistPort("rstx", "1", 1, ProGe::BIT, ProGe::IN, *almaifBlock_);
    netlist.connect(*clockPort(), *clk);
    netlist.connect(*resetPort(), *rstx);

    almaifBlock_->setParameter(
        "second_dmem_data_width_g", "integer",
        Conversion::toString(secondDmemDataw_));
    almaifBlock_->setParameter(
        "second_dmem_addr_width_g", "integer",
        Conversion::toString(secondDmemAddrw_));

    if (secondDmem_) {
        almaifBlock_->setParameter("enable_second_dmem_g", "integer", "1");
        accelInstantiator_.replacePlaceholderFromFile(
            "second-dmem-port-declarations",
            Path(platformPath + "second_dmem_port_declaration.snippet"));
    } else {
        almaifBlock_->setParameter("enable_second_dmem_g", "integer", "0");
        accelInstantiator_.replacePlaceholderFromFile(
            "second-dmem-signal-declarations",
            Path(platformPath + "second_dmem_signal_declaration.snippet"));
    }

    if (secondPmem_) {
        almaifBlock_->setParameter("enable_second_pmem_g", "integer", "1");
        accelInstantiator_.replacePlaceholderFromFile(
            "second-pmem-port-declarations",
            Path(platformPath + "second_pmem_port_declaration.snippet"));
    } else {
        almaifBlock_->setParameter("enable_second_pmem_g", "integer", "0");
        accelInstantiator_.replacePlaceholderFromFile(
            "second-pmem-signal-declarations",
            Path(platformPath + "second_pmem_signal_declaration.snippet"));
    }

    almaifBlock_->setParameter(
        "second_pmem_data_width_g", "integer",
        Conversion::toString(secondPmemDataw_));
    if (pmemInfo_.asAddrw == 32) {
        almaifBlock_->setParameter(
            "second_pmem_addr_width_g", "integer", "local_mem_addrw_g");
    } else {
        almaifBlock_->setParameter(
            "second_pmem_addr_width_g", "integer",
            Conversion::toString(secondPmemAddrw_));
    }

    if (syncReset_) {
        almaifBlock_->setParameter("sync_reset_g", "integer", "1");
    } else {
        almaifBlock_->setParameter("sync_reset_g", "integer", "0");
    }

    if (broadcast_pmem_) {
        almaifBlock_->setParameter("broadcast_pmem_g", "integer", "1");

        accelInstantiator_.replacePlaceholderFromFile(
            "pmem-bcast", Path(platformPath + "pmem_broadcast.snippet"));
    } else {
        almaifBlock_->setParameter("broadcast_pmem_g", "integer", "0");
    }

    // Add AXI4 slave interface to AlmaIF block and toplevel, and connect them
    NetlistPortGroup* axislave_almaif = axiSlavePortGroup();
    NetlistPortGroup* axislave_ext = axislave_almaif->clone();
    almaifBlock_->addPortGroup(axislave_almaif);
    integratorBlock()->addPortGroup(axislave_ext);
    netlist.connectGroupByName(*axislave_almaif, *axislave_ext);

    if (pmemInfo_.asAddrw == 32) {
        assert(
            broadcast_pmem_ == false &&
            "Broadcasting pmem not supported with m_axi");
        integratorBlock()->setParameter(
            "local_mem_addrw_g", "integer",
            Conversion::toString(DEFAULT_LOCAL_MEMORY_WIDTH - 2));

        integratorBlock()->setParameter(
            "axi_offset_low_g", "integer", "1073741824");
        integratorBlock()->setParameter("axi_offset_high_g", "integer", "0");
        almaifBlock_->setParameter(
            "axi_offset_low_g", "integer", "axi_offset_low_g");
        almaifBlock_->setParameter(
            "axi_offset_high_g", "integer", "axi_offset_high_g");

        NetlistPortGroup* aximaster_almaif = axiMasterPortGroup();
        NetlistPortGroup* aximaster_ext = aximaster_almaif->clone();
        almaifBlock_->addPortGroup(aximaster_almaif);
        integratorBlock()->addPortGroup(aximaster_ext);
        netlist.connectGroupByName(*aximaster_almaif, *aximaster_ext);
    } else {
        integratorBlock()->setParameter(
            "local_mem_addrw_g", "integer",
            Conversion::toString(pmemInfo_.portAddrw));
        almaifBlock_->setParameter("axi_offset_low_g", "integer", "0");
        almaifBlock_->setParameter(
            "axi_offset_high_g", "integer", "0");
    }

    int coreCount = 1;
    // Debug signals
    if (!hasMinimalDebugger_) {
        almaifBlock_->setParameter("full_debugger_g", "integer", "1");
        accelInstantiator_.replacePlaceholderFromFile(
            "full-debugger-port-declarations",
            Path(platformPath + "full_debugger_port_declaration.snippet"));
        accelInstantiator_.replacePlaceholderFromFile(
            "debugger", Path(platformPath + "full_debugger.snippet"));

        addPortToAlmaIFBlock(
            "core_db_pc_start", coreCount * imemaddrw, ProGe::OUT,
            "db_pc_start");
        addPortToAlmaIFBlock(
            "core_db_pc_next", coreCount * imemaddrw, ProGe::IN,
            "db_pc_next");
        addPortToAlmaIFBlock(
            "core_db_bustraces", coreCount * bustrace_width, ProGe::IN,
            "db_bustraces");
    } else {
        almaifBlock_->setParameter("full_debugger_g", "integer", "0");
        accelInstantiator_.replacePlaceholderFromFile(
            "mini-debugger-signal-declarations",
            Path(platformPath + "mini_debugger_signal_declaration.snippet"));
        accelInstantiator_.replacePlaceholderFromFile(
            "debugger", Path(platformPath + "mini_debugger.snippet"));
    }

    // Set the default value in case when there is no separate scratchpad memory.
    // AlmaIF interface must reserve some part of the global buffer memory
    // (pmem) and communicate that in the interface
    if (!hasSeparateLocalMemory_) {
        almaifBlock_->setParameter(
        "reserved_sp_bytes_g", "integer",
        Conversion::toString(DEFAULT_RESERVED_PRIVATE_MEM_SIZE));
    }

    addPortToAlmaIFBlock(
        "core_db_pc", coreCount * imemaddrw, ProGe::IN, "db_pc");
    addPortToAlmaIFBlock(
        "core_db_lockcnt", coreCount * 64, ProGe::IN, "db_lockcnt");
    addPortToAlmaIFBlock(
        "core_db_cyclecnt", coreCount * 64, ProGe::IN, "db_cyclecnt");
    addPortToAlmaIFBlock(
        "core_db_tta_nreset", coreCount, ProGe::OUT, "db_tta_nreset");
    addPortToAlmaIFBlock(
        "core_db_lockrq", coreCount, ProGe::OUT, "db_lockrq");
    if (dmem_dram_) {
        addPortToAlmaIFBlock(
            "core_db_dram_offset", 32, ProGe::OUT, "db_dram_offset");
        accelInstantiator_.replacePlaceholder(
            "dram-offset-port-declaration",
            "core_db_dram_offset : out std_logic_vector(32-1 downto 0);");
        // We want to disable dmem from tta-accel, because dmem exists in dram
        accelInstantiator_.replacePlaceholder(
            "enable-dmem", "constant enable_dmem : boolean := false;");
    } else {
        // Dummy signal to connect to not leave minidebuggers portmap
        // undefined
        accelInstantiator_.replacePlaceholder(
            "dram-offset-dummy-declaration",
            "signal core_db_dram_offset : std_logic_vector(32-1 downto 0);");
        accelInstantiator_.replacePlaceholder(
            "enable-dmem",
            "constant enable_dmem : boolean := dmem_data_width_g > 0;");
    }

    if (imemInfo().type != VHDL_ARRAY) {
        // Ifetch signals
        addPortToAlmaIFBlock("core_busy_out", coreCount, ProGe::OUT, "busy");
        addPortToAlmaIFBlock(
            "core_imem_data_out", coreCount * imemdataw, ProGe::OUT,
            "imem_data");
        addPortToAlmaIFBlock(
            "core_imem_en_x_in", coreCount, ProGe::IN, "imem_en_x");
        addPortToAlmaIFBlock(
            "core_imem_addr_in", coreCount * imemaddrw, ProGe::IN,
            "imem_addr");

        if (imem_dp_) {
            const int strb_width = imemdataw / 8;
            const int half_cores_ceil = (coreCount + 1) / 2;
            // Dualport memories have identical a-b ports, except with odd
            // corecounts the b is full and the a's msb-side is unconnected
            addMemoryPorts(
                "INSTR_a", half_cores_ceil, imemdataw * half_cores_ceil,
                imemaddrw * half_cores_ceil, strb_width * half_cores_ceil,
                false);
            addMemoryPorts(
                "INSTR_b", half_cores_ceil, imemdataw * half_cores_ceil,
                imemaddrw * half_cores_ceil, strb_width * half_cores_ceil,
                false);
        } else {
            addMemoryPorts("INSTR_a", imemdataw, imemaddrw, false, false);
            addMemoryPorts("INSTR_b", 32, imemAxiAddrw, false, false);
        }
        TCEString platformPath = Environment::dataDirPath("ProGe");
        platformPath << FileSystem::DIRECTORY_SEPARATOR << "platform"
                     << FileSystem::DIRECTORY_SEPARATOR;

        accelInstantiator_.replacePlaceholderFromFile(
            "imem-statements",
            Path(platformPath + "imem_statements.snippet"));
        if (imem_dp_) {
            accelInstantiator_.replacePlaceholderFromFile(
                "imem-bcast",
                Path(platformPath + "imem_broadcast_dualport.snippet"));
            accelInstantiator_.replacePlaceholderFromFile(
                "imem-port-declarations",
                Path(
                    platformPath + "imem_port_declaration_dualport.snippet"));
        } else {
            accelInstantiator_.replacePlaceholderFromFile(
                "imem-bcast", Path(platformPath + "imem_broadcast.snippet"));
            accelInstantiator_.replacePlaceholderFromFile(
                "imem-port-declarations",
                Path(platformPath + "imem_port_declaration.snippet"));
        }
    }

    connectCoreMemories(dmemInfo_, "dmem", "data", secondDmem_);
    connectCoreMemories(pmemInfo_, "pmem", "param", secondPmem_);
}

void
AlmaIFIntegrator::connectCoreMemories(
    MemInfo mem, TCEString mem_name, TCEString mem_block_name, bool seconds) {
    almaifBlock_->setParameter(
        mem_name + "_data_width_g", "integer",
        Conversion::toString(mem.mauWidth * mem.widthInMaus));
    almaifBlock_->setParameter(
        mem_name + "_addr_width_g", "integer",
        Conversion::toString(mem.portAddrw));

    TCEString platformPath = Environment::dataDirPath("ProGe");
    platformPath << FileSystem::DIRECTORY_SEPARATOR << "platform"
                 << FileSystem::DIRECTORY_SEPARATOR;

    // If given memory wasn't found by findMemories(), we need dummy signals
    // as a stand-in for the missing ports
    if (mem.portAddrw == 0 || mem.type == DRAM) {
        Path snippet(platformPath + mem_name + "_signal_declaration.snippet");
        accelInstantiator_.replacePlaceholderFromFile(
            mem_name + "-signal-declarations", snippet);
        return;
    }

    // Otherwise, instantiate the ports
    if (mem_name == "pmem" && broadcast_pmem_) {
        Path snippet(platformPath + "pmem_port_declaration_wide.snippet");
        accelInstantiator_.replacePlaceholderFromFile(
            mem_name + "-port-declarations", snippet);
    } else {
        Path snippet(platformPath + mem_name + "_port_declaration.snippet");
        accelInstantiator_.replacePlaceholderFromFile(
            mem_name + "-port-declarations", snippet);
    }

    TCEString lsu_prefix = "fu_" + mem.lsuName;

    int coreCount = 1;
    addPortToAlmaIFBlock(
        "core_" + mem_name + "_avalid_in", coreCount, ProGe::IN,
        lsu_prefix + "_avalid_out");
    addPortToAlmaIFBlock(
        "core_" + mem_name + "_aready_out", coreCount, ProGe::OUT,
        lsu_prefix + "_aready_in");
    addPortToAlmaIFBlock(
        "core_" + mem_name + "_aaddr_in", coreCount * mem.portAddrw,
        ProGe::IN, lsu_prefix + "_aaddr_out");
    addPortToAlmaIFBlock(
        "core_" + mem_name + "_awren_in", coreCount, ProGe::IN,
        lsu_prefix + "_awren_out");
    addPortToAlmaIFBlock(
        "core_" + mem_name + "_astrb_in",
        (mem.mauWidth * mem.widthInMaus + 7) / 8 * coreCount, ProGe::IN,
        lsu_prefix + "_astrb_out");

    addPortToAlmaIFBlock(
        "core_" + mem_name + "_adata_in",
        coreCount * mem.mauWidth * mem.widthInMaus, ProGe::IN,
        lsu_prefix + "_adata_out");
    addPortToAlmaIFBlock(
        "core_" + mem_name + "_rvalid_out", coreCount, ProGe::OUT,
        lsu_prefix + "_rvalid_in");
    addPortToAlmaIFBlock(
        "core_" + mem_name + "_rready_in", coreCount, ProGe::IN,
        lsu_prefix + "_rready_out");
    addPortToAlmaIFBlock(
        "core_" + mem_name + "_rdata_out",
        coreCount * mem.mauWidth * mem.widthInMaus, ProGe::OUT,
        lsu_prefix + "_rdata_in");

    if (seconds) {
        int addrWidth, dataWidth;
        if (mem.asName == DMEM_NAME) {
            addrWidth = secondDmemAddrw_;
            dataWidth = secondDmemDataw_;
            lsu_prefix = "fu_" + secondDmemName_;
        } else {
            addrWidth = secondPmemAddrw_;
            dataWidth = secondPmemDataw_;
            lsu_prefix = "fu_" + secondPmemName_;
        }
        addPortToAlmaIFBlock(
            "core_" + mem_name + "_2nd_avalid_in", coreCount, ProGe::IN,
            lsu_prefix + "_avalid_out");
        addPortToAlmaIFBlock(
            "core_" + mem_name + "_2nd_aready_out", coreCount, ProGe::OUT,
            lsu_prefix + "_aready_in");
        addPortToAlmaIFBlock(
            "core_" + mem_name + "_2nd_aaddr_in", coreCount * addrWidth,
            ProGe::IN, lsu_prefix + "_aaddr_out");
        addPortToAlmaIFBlock(
            "core_" + mem_name + "_2nd_awren_in", coreCount, ProGe::IN,
            lsu_prefix + "_awren_out");
        addPortToAlmaIFBlock(
            "core_" + mem_name + "_2nd_astrb_in", coreCount * dataWidth / 8,
            ProGe::IN, lsu_prefix + "_astrb_out");

        addPortToAlmaIFBlock(
            "core_" + mem_name + "_2nd_adata_in", coreCount * dataWidth,
            ProGe::IN, lsu_prefix + "_adata_out");
        addPortToAlmaIFBlock(
            "core_" + mem_name + "_2nd_rvalid_out", coreCount, ProGe::OUT,
            lsu_prefix + "_rvalid_in");
        addPortToAlmaIFBlock(
            "core_" + mem_name + "_2nd_rready_in", coreCount, ProGe::IN,
            lsu_prefix + "_rready_out");
        addPortToAlmaIFBlock(
            "core_" + mem_name + "_2nd_rdata_out", coreCount * dataWidth,
            ProGe::OUT, lsu_prefix + "_rdata_in");
    }

    addMemoryPorts(
        mem_block_name + "_a", mem.mauWidth * mem.widthInMaus, mem.portAddrw,
        mem.isShared, (mem.asAddrw == 32 && (mem.asName == PMEM_NAME)));
    if (mem.asName == DMEM_NAME) {
        addMemoryPorts(
            mem_block_name + "_b", secondDmemDataw_, secondDmemAddrw_,
            mem.isShared, false);
    } else {
        bool make_local_sized =
            mem.asAddrw == 32 && (mem.asName == PMEM_NAME);
        addMemoryPorts(
            mem_block_name + "_b", secondPmemDataw_, secondPmemAddrw_,
            mem.isShared, make_local_sized);
    }
}

void
AlmaIFIntegrator::addPortToAlmaIFBlock(
    const TCEString name, const TCEString width, const ProGe::Direction dir,
    const TCEString core_name) {
    NetlistPort* port =
        new NetlistPort(name, width, ProGe::BIT_VECTOR, dir, *almaifBlock_);
    if (core_name != "") almaif_ttacore_ports[core_name] = port;
}

void
AlmaIFIntegrator::addPortToAlmaIFBlock(
    const TCEString name, const int width, const ProGe::Direction dir,
    const TCEString core_name) {
    NetlistPort* port = new NetlistPort(
        name, Conversion::toString(width), width, ProGe::BIT_VECTOR, dir,
        *almaifBlock_);
    if (core_name != "") almaif_ttacore_ports[core_name] = port;
}

void
AlmaIFIntegrator::addPortToGroup(
    NetlistPortGroup* port_group, const ProGe::Direction dir,
    const TCEString name, const TCEString width) {
    ProGe::DataType type = ProGe::BIT_VECTOR;
    if (width == "1") {
        type = ProGe::BIT;
    }
    NetlistPort* port = new NetlistPort(name, width, type, dir);
    port_group->addPort(*port);
}

/**
 * Builds a representation of the AXI4 bus slave interface as a port group.
 * Hardcoded signal widths for data (32b) and IDs (12b)
 */
NetlistPortGroup*
AlmaIFIntegrator::axiSlavePortGroup() {
    NetlistPortGroup* axiBus = new NetlistPortGroup(ProGe::SignalGroup());

    addPortToGroup(axiBus, ProGe::IN, "s_axi_awid", "axi_id_width_g");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_awaddr", "axi_addr_width_g");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_awlen", "8");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_awsize", "3");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_awburst", "2");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_awvalid", "1");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_awready", "1");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_wdata", "32");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_wstrb", "4");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_wvalid", "1");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_wready", "1");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_bid", "axi_id_width_g");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_bresp", "2");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_bvalid", "1");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_bready", "1");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_arid", "axi_id_width_g");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_araddr", "axi_addr_width_g");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_arlen", "8");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_arsize", "3");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_arburst", "2");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_arvalid", "1");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_arready", "1");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_rid", "axi_id_width_g");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_rdata", "32");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_rresp", "2");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_rlast", "1");
    addPortToGroup(axiBus, ProGe::OUT, "s_axi_rvalid", "1");
    addPortToGroup(axiBus, ProGe::IN, "s_axi_rready", "1");

    return axiBus;
}

/**
 * Builds a representation of the AXI4-Lite master interface as a port group.
 */
NetlistPortGroup*
AlmaIFIntegrator::axiMasterPortGroup() {
    NetlistPortGroup* axiBus = new NetlistPortGroup(ProGe::SignalGroup());

    addPortToGroup(axiBus, ProGe::OUT, "m_axi_awaddr", "32");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_awvalid", "1");
    addPortToGroup(axiBus, ProGe::IN, "m_axi_awready", "1");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_awprot", "3");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_wvalid", "1");
    addPortToGroup(axiBus, ProGe::IN, "m_axi_wready", "1");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_wdata", "32");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_wstrb", "4");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_wlast", "1");
    addPortToGroup(axiBus, ProGe::IN, "m_axi_bvalid", "1");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_bready", "1");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_arvalid", "1");
    addPortToGroup(axiBus, ProGe::IN, "m_axi_arready", "1");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_araddr", "32");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_arprot", "3");
    addPortToGroup(axiBus, ProGe::IN, "m_axi_rdata", "32");
    addPortToGroup(axiBus, ProGe::IN, "m_axi_rvalid", "1");
    addPortToGroup(axiBus, ProGe::OUT, "m_axi_rready", "1");

    return axiBus;
}

void
AlmaIFIntegrator::addMemoryPorts(
    const TCEString prefix, int data_width, int addr_width,
    const bool /* isShared */, const bool overrideAsWidth) {
    int mem_count = 1;

    data_width = data_width * mem_count;
    addr_width = addr_width * mem_count;
    // In case the data_width has been already multiplied by the mem_count,
    // we have to undo the multiplication, so the rounding is done properly
    // for the single memory version and only then multiplied by the
    // mem_count.
    int strb_width = (data_width / mem_count + 7) / 8 * mem_count;

    addMemoryPorts(
        prefix, mem_count, data_width, addr_width, strb_width,
        overrideAsWidth);
}

void
AlmaIFIntegrator::addMemoryPorts(
    const TCEString prefix, int mem_count, int data_width, int addr_width,
    int strb_width, const bool overrideAsWidth) {
    addPortToAlmaIFBlock(prefix + "_avalid_out", mem_count, ProGe::OUT);
    addPortToAlmaIFBlock(prefix + "_aready_in", mem_count, ProGe::IN);
    if (overrideAsWidth) {
        addPortToAlmaIFBlock(
            prefix + "_aaddr_out", "local_mem_addrw_g", ProGe::OUT);
    } else {
        addPortToAlmaIFBlock(prefix + "_aaddr_out", addr_width, ProGe::OUT);
    }
    addPortToAlmaIFBlock(prefix + "_awren_out", mem_count, ProGe::OUT);
    addPortToAlmaIFBlock(prefix + "_astrb_out", strb_width, ProGe::OUT);
    addPortToAlmaIFBlock(prefix + "_adata_out", data_width, ProGe::OUT);
    addPortToAlmaIFBlock(prefix + "_rvalid_in", mem_count, ProGe::IN);
    addPortToAlmaIFBlock(prefix + "_rready_out", mem_count, ProGe::OUT);
    addPortToAlmaIFBlock(prefix + "_rdata_in", data_width, ProGe::IN);
}

void
AlmaIFIntegrator::addAlmaifFiles() {
    const std::string DS = FileSystem::DIRECTORY_SEPARATOR;
    std::vector<TCEString> almaifFiles;

    TCEString basePath = Environment::dataDirPath("ProGe");
    TCEString platformPath = basePath + DS + "platform" + DS;
    TCEString dbPath = basePath + DS + "debugger" + DS;

    TCEString outputPath = outputFilePath("tta-accel.vhdl", true);
    if (pmemInfo_.asAddrw == 32) {
        Path snippet(platformPath + "axi_master_port_declaration.snippet");
        accelInstantiator_.replacePlaceholderFromFile(
            "m-axi-port-declarations", snippet);
    } else {
        Path snippet(platformPath + "axi_master_signal_declaration.snippet");
        accelInstantiator_.replacePlaceholderFromFile(
            "m-axi-signal-declarations", snippet);
    }

    accelInstantiator_.instantiateTemplateFile(
        platformPath + "tta-accel.vhdl.tmpl", outputPath);
    almaifFiles.push_back(outputPath);

    copyPlatformFile(platformPath + "tta-axislave.vhdl", almaifFiles);
    copyPlatformFile(platformPath + "almaif_decoder.vhdl", almaifFiles);
    copyPlatformFile(platformPath + "almaif_axi_arbiter.vhdl", almaifFiles);
    copyPlatformFile(platformPath + "almaif_mc_arbiter.vhdl", almaifFiles);
    copyPlatformFile(platformPath + "almaif_axi_expander.vhdl", almaifFiles);
    copyPlatformFile(platformPath + "membus_splitter.vhdl", almaifFiles);
    copyPlatformFile(platformPath + "almaif_membus_delay.vhdl", almaifFiles);
    copyPlatformFile(dbPath + "registers-pkg.vhdl", almaifFiles);

    if (hasMinimalDebugger_) {
        copyPlatformFile(dbPath + "minidebugger.vhdl", almaifFiles);
    } else {
        copyPlatformFile(dbPath + "dbregbank.vhdl", almaifFiles);
        copyPlatformFile(dbPath + "dbsm-entity.vhdl", almaifFiles);
        copyPlatformFile(dbPath + "dbsm-rtl.vhdl", almaifFiles);
        copyPlatformFile(dbPath + "debugger.vhdl", almaifFiles);
    }

    // Copy synthesis scripts
    TCEString scriptPath = basePath + DS + "synthesis" + DS;
    accelInstantiator_.replacePlaceholder("part_name", deviceName());
    accelInstantiator_.replacePlaceholder(
        "toplevel_entity", coreEntityName() + "_toplevel");
    copyPlatformFile(scriptPath + "find_fmax.py", almaifFiles, true);
    outputPath = progeFilePath("timing.tcl", true);
    accelInstantiator_.instantiateTemplateFile(
        scriptPath + "timing.tcl.tmpl", outputPath);
    outputPath = progeFilePath("utilization.tcl", true);
    accelInstantiator_.instantiateTemplateFile(
        scriptPath + "utilization.tcl.tmpl", outputPath);

    for (unsigned int i = 0; i < almaifFiles.size(); i++) {
        projectFileGenerator()->addHdlFile(almaifFiles.at(i));
    }
}

void
AlmaIFIntegrator::copyPlatformFile(
    const TCEString inputPath, std::vector<TCEString>& fileList,
    bool isScript) const {
    TCEString outputPath;
    if (isScript) {
        outputPath = progeFilePath(FileSystem::fileOfPath(inputPath), true);
    } else {
        outputPath = outputFilePath(FileSystem::fileOfPath(inputPath), true);
    }

    FileSystem::copy(inputPath, outputPath);
    fileList.push_back(outputPath);
}

bool
AlmaIFIntegrator::integrateCore(const ProGe::NetlistBlock& core, int coreId) {
    ProGe::Netlist& netlist = integratorBlock()->netlist();

    TCEString clkPortName = PlatformIntegrator::TTA_CORE_CLK;
    TCEString resetPortName = PlatformIntegrator::TTA_CORE_RSTX;

    if (!createMemories(coreId)) {
        return false;
    }

    // Connect cycle count, stall count ports if needed by an FU
    auto ttaCCPort = core.port("debug_cycle_count_in");
    if (ttaCCPort) {
        netlist.connect(*core.port("db_cyclecnt"), *ttaCCPort);
    }
    auto ttaLCPort = core.port("debug_lock_count_in");
    if (ttaLCPort) {
        netlist.connect(*core.port("db_lockcnt"), *ttaLCPort);
    }
    for (size_t i = 0; i < core.portCount(); ++i) {
        TCEString portName = core.port(i).name();

        // Connect global clock and reset ports
        if (portName == PlatformIntegrator::TTA_CORE_CLK) {
            netlist.connect(*clockPort(), core.port(i));
        } else if (portName == PlatformIntegrator::TTA_CORE_RSTX) {
            netlist.connect(*resetPort(), core.port(i));
        } else {
            // Strip coreN_ -prefix for multicore TTAs
            if (portName.substr(0, 4) == "core") {
                TCEString coreKey;
                coreKey << "core" << coreId;
                size_t cutoff = portName.find_first_of('_');
                if (portName.substr(0, cutoff) != coreKey) {
                    // Port doesn't belong to core, skip
                    continue;
                }
                portName = portName.substr(cutoff + 1);
            }
            if (almaif_ttacore_ports.find(portName) !=
                almaif_ttacore_ports.end()) {
                int portWidth = almaif_ttacore_ports[portName]->realWidth();

                // Connect AlmaIF block to TTA

                if (coreId != -1 && imem_dp_ && portName == "imem_data") {
                    // imem_dp_ works with padded imem data signals. So here
                    // we connect only the real bits, but with padded
                    // differences.
                    netlist.connect(
                        core.port(i), *almaif_ttacore_ports[portName], 0,
                        portWidth * coreId, imemInfo().mauWidth);

                } else if (coreId != -1) {
                    netlist.connect(
                        core.port(i), *almaif_ttacore_ports[portName], 0,
                        portWidth * coreId, portWidth);
                } else {
                    netlist.connect(
                        core.port(i), *almaif_ttacore_ports[portName]);
                }
            } else if (
                dmem_dram_ &&
                portName ==
                    (boost::format("fu_%s_dram_offset") % dmemInfo_.lsuName)
                        .str()) {
                netlist.connect(
                    core.port(i), *almaifBlock_->port("db_dram_offset"));
            }
        }
    }

    exportUnconnectedPorts(coreId);
    return true;
}

void
AlmaIFIntegrator::exportUnconnectedPorts(int coreId) {
    PlatformIntegrator::exportUnconnectedPorts(coreId);

    for (size_t i = 0; i < almaifBlock_->portCount(); i++) {
        const NetlistPort& port = almaifBlock_->port(i);
        if (!integratorBlock()->netlist().isPortConnected(port)) {
            connectToplevelPort(port);
        }
    }
}

MemoryGenerator&
AlmaIFIntegrator::imemInstance(MemInfo imem, int /* coreId */) {
    assert(imem.type != UNKNOWN && "Imem type not set!");
    int axiAddrWidth =
        MathTools::requiredBits0Bit0(
            (imemInfo().widthInMaus * imemInfo().mauWidth + 31) / 32 - 1) +
        imemInfo().portAddrw;
    if (imemGen_ == NULL) {
        if (imem.type == ONCHIP) {
            if (imem_dp_) {
                imemGen_ = new XilinxBlockRamGenerator(
                    MathTools::roundUpToPowerTwo(imem.mauWidth),
                    imem.widthInMaus, imem.portAddrw,
                    MathTools::roundUpToPowerTwo(imem.mauWidth),
                    imem.portAddrw, this, warningStream(), errorStream(),
                    true, almaifBlock_, "INSTR", false, false);
            } else {
                imemGen_ = new XilinxBlockRamGenerator(
                    imem.mauWidth, imem.widthInMaus, imem.portAddrw, 32,
                    axiAddrWidth, this, warningStream(), errorStream(), true,
                    almaifBlock_, "INSTR", false, false);
            }
        } else if (imem.type == VHDL_ARRAY) {
            imemGen_ = new VhdlRomGenerator(
                imem.mauWidth, imem.widthInMaus, imem.portAddrw,
                programName() + "_imem_pkg.vhdl", this, warningStream(),
                errorStream());
        } else if (imem.type != NONE) {
            TCEString msg = "Unsupported instruction memory type";
            throw InvalidData(__FILE__, __LINE__, "AlmaIFIntegrator", msg);
        }
    }
    return *imemGen_;
}

MemoryGenerator&
AlmaIFIntegrator::dmemInstance(
    MemInfo dmem, TTAMachine::FunctionUnit& lsuArch,
    std::vector<std::string> lsuPorts) {
    if (dmem.asName ==
        AXI_AS_NAME) {  // AXI bus, export all ports to toplevel
        return *(new DummyMemGenerator(
            dmem.mauWidth, dmem.widthInMaus, dmem.portAddrw, this,
            warningStream(), errorStream()));
    }

    bool genSingleRam = false;
    bool genDualPortRam = false;
    bool overrideAsWidth = false;
    bool isDmem = dmem.asName == DMEM_NAME;
    bool isPmem = dmem.asName == PMEM_NAME;
    bool isSecondInstance = false;

    if (dmem.type == DRAM) {  // AXI bus, export all ports to toplevel
        return *(new DummyMemGenerator(
            dmem.mauWidth, dmem.widthInMaus, dmem.portAddrw, this,
            warningStream(), errorStream()));
    }

    int bDataWidth = 0;
    int bAddrWidth = 0;

    if (isDmem || isPmem) {
        genDualPortRam = true;
        genSingleRam = dmem.isShared;
        if (isDmem) {
            if (dmemHandled_) {
                isSecondInstance = true;
            }
            dmem = dmemInfo_;
            bDataWidth = secondDmemDataw_;
            bAddrWidth = secondDmemAddrw_;
            dmemHandled_ = true;
        }
        if (isPmem) {
            if (pmemHandled_) {
                isSecondInstance = true;
            }
            dmem = pmemInfo_;
            bDataWidth = secondPmemDataw_;
            bAddrWidth = secondPmemAddrw_;
            pmemHandled_ = true;
        }

        // TODO: Assumes 32b wide memory (32b address - 2 mask bits)
        //       (the AXI-lite IF also assumes this)
        if (dmem.asName == PMEM_NAME && dmem.asAddrw == 32) {
            overrideAsWidth = true;
        }
    }

    MemoryGenerator* memGen = NULL;

    if ((isSecondInstance && dmem.isShared) || dmem.type == DRAM) {
        memGen = new DummyMemGenerator(
            dmem.mauWidth, dmem.widthInMaus, dmem.portAddrw, this,
            warningStream(), errorStream());
    } else if (dmemGen_.find(dmem.asName) != dmemGen_.end()) {
        memGen = dmemGen_.find(dmem.asName)->second;
    } else {
        if (dmem.type == ONCHIP) {
            // onchip mem size is scalable, use value from adf's Address Space
            int addrw = dmem.portAddrw;
            memGen = new XilinxBlockRamGenerator(
                dmem.mauWidth, dmem.widthInMaus, addrw, bDataWidth,
                bAddrWidth, this, warningStream(), errorStream(),
                genDualPortRam, almaifBlock_, dmem.asName, overrideAsWidth,
                genSingleRam);
        } else {
            TCEString msg = "Unsupported data memory type";
            throw InvalidData(__FILE__, __LINE__, "AlmaIFIntegrator", msg);
        }
        memGen->addLsu(lsuArch, lsuPorts);
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
        << "Supported instruction memory types are 'onchip' and 'none'."
        << std::endl
        << "Supported data memory types are 'onchip' and 'none'." << std::endl
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

void
AlmaIFIntegrator::generateIntegratedTestbench() {
    const TCEString DS = FileSystem::DIRECTORY_SEPARATOR;

    FileSystem::createDirectory(progeFilePath("tb", true));

    TCEString basePath = Environment::dataDirPath("ProGe");
    TCEString tbPath = basePath + DS + "tb" + DS + "almaif" + DS;
    TCEString outputPath = tbFilePath("almaif-tb.vhdl", true);

    HDLTemplateInstantiator tbInstantiator(coreEntityName());
    if (pmemInfo_.asAddrw == 32) {
        tbInstantiator.replacePlaceholderFromFile(
            "m-axi-port-declarations",
            Path(tbPath + "almaif-tb-m-axi-port-declarations-snippet.vhdl"));
        tbInstantiator.replacePlaceholderFromFile(
            "m-axi-port-connections",
            Path(tbPath + "almaif-tb-m-axi-port-connections-snippet.vhdl"));
        tbInstantiator.replacePlaceholderFromFile(
            "m-axi-external-mem-instantiation",
            Path(
                tbPath +
                "almaif-tb-m-axi-external-mem-instantiation-snippet.vhdl"));
        FileSystem::copy(tbPath + "axi-mem.vhdl", tbFilePath("axi-mem.vhdl"));
    }
    tbInstantiator.instantiateTemplateFile(
        tbPath + "almaif-tb.vhdl.tmpl", outputPath);
}
