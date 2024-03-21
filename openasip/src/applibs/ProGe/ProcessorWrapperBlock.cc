/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file ProcessorWrapperBlock.cc
 *
 * Implementation/Declaration of ProcessorWrapperBlock class.
 *
 * Created on: 7.9.2015
 * @author Henry Linjam�ki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "ProcessorWrapperBlock.hh"

#include "Netlist.hh"
#include "NetlistTools.hh"
#include "NetlistFactories.hh"
#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"
#include "SinglePortSSRAMBlock.hh"
#include "SinglePortByteMaskSSRAMBlock.hh"
#include "GlobalPackage.hh"
#include "MemoryBusInterface.hh"

#include "FileSystem.hh"
#include "MathTools.hh"
#include "ControlUnit.hh"
#include "InverterBlock.hh"

namespace ProGe {

ProcessorWrapperBlock::ProcessorWrapperBlock(
    const ProGeContext& context, const BaseNetlistBlock& processorBlock)
    : BaseNetlistBlock("proc", ""),
      context_(context),
      coreBlock_(processorBlock.shallowCopy()) {
    assert(processorBlock.portCount() > 0);
    assert(coreBlock_->portCount() > 0);

    // Wrapper interface //
    addPort(PortFactory::clockPort());
    addPort(PortFactory::resetPort());
    NetlistPort* coreLocked = new OutPort("locked", "1");
    addPort(coreLocked);

    // Instantiate core //
    addSubBlock(coreBlock_);
    // Memory instantiations and connections //
    for (size_t i = 0; i < coreBlock_->portGroupCount(); i++) {
        const NetlistPortGroup& portGrp = coreBlock_->portGroup(i);
        SignalGroupType type = portGrp.assignedSignalGroup().type();
        if (type == SignalGroupType::INSTRUCTION_LINE) {
            addInstructionMemory(portGrp);
        } else if (type == SignalGroupType::BITMASKED_SRAM_PORT) {
            auto dmemIf = dynamic_cast<const MemoryBusInterface*>(&portGrp);
            assert(dmemIf != nullptr);
            addDataMemory(*dmemIf);
        } else if (type == SignalGroupType::BYTEMASKED_SRAM_PORT) {
            auto dmemIf = dynamic_cast<const MemoryBusInterface*>(&portGrp);
            assert(dmemIf != nullptr);
            addDataMemory2(*dmemIf);
        }
    }

    connectClocks();
    connectResets();
    connectLockStatus(*coreLocked);

    // Other packages possibly holding constants used in core ports.
    for (size_t i = 0; i < processorBlock.packageCount(); i++) {
        addPackage(processorBlock.package(i));
    }

    // Package holding instruction bus constants
    addPackage(context.globalPackage().name());

    // TODO: Check if there are params for this block and
    //       Add package if needed.
    //       
    //addPackage(context.coreEntityName() + "_params");

    // Handle unknown ports
    handleUnconnectedPorts();
}

ProcessorWrapperBlock::~ProcessorWrapperBlock() {}

void
ProcessorWrapperBlock::write(
    const Path& targetBaseDir, HDL targetLang) const {
    BaseNetlistBlock::writeSelf(targetBaseDir / "tb", targetLang);
    BaseNetlistBlock::write(targetBaseDir, targetLang);
}

void
ProcessorWrapperBlock::addInstructionMemory(
    const NetlistPortGroup& coreImemPort) {
    using SigT = SignalType;
    bool isRISCV = context_.adf().isRISCVMachine();

    const int imemWidthInMaus = (context_.adf().isRISCVMachine()) ? 4 : 1;
    const int unusedBits = std::ceil(std::log2(imemWidthInMaus));

    std::string addrWidth = context_.globalPackage().fetchBlockAddressWidth();
    if (isRISCV) {
        const int unusedBits = std::ceil(std::log2(imemWidthInMaus));
        addrWidth = context_.globalPackage().fetchBlockAddressWidth() + "-" +
                    std::to_string(unusedBits);
    }

    SinglePortSSRAMBlock* imemBlock = new SinglePortSSRAMBlock(
        addrWidth, context_.globalPackage().fetchBlockDataWidth(),
        "tb/imem_init.img", true);
    addSubBlock(imemBlock, "imem0");
    // todo: use core id instead of counter value.
    std::string accessTrace = std::string("core") +
                              Conversion::toString(imemCount_++) +
                              "_imem_access_trace.dump";
    imemBlock->setAccessTraceFile(accessTrace);

    if (isRISCV) {
        // With RISC-V the two lower bits are unused
        const int realAddrWidth = MathTools::requiredBits(
            context_.adf().controlUnit()->addressSpace()->end());
        netlist().connect(
            imemBlock->memoryPort(), coreImemPort,
            {{SigT::READ_DATA, SigT::FETCHBLOCK},
             {SigT::READ_WRITE_REQUEST, SigT::READ_REQUEST}});
        netlist().connect(
            imemBlock->memoryPort().portBySignal(SigT::ADDRESS),
            coreImemPort.portBySignal(SigT::ADDRESS), 0, unusedBits,
            realAddrWidth - unusedBits);
    } else {
        netlist().connect(
            imemBlock->memoryPort(), coreImemPort,
            {{SigT::ADDRESS, SigT::ADDRESS},
             {SigT::READ_DATA, SigT::FETCHBLOCK},
             {SigT::READ_WRITE_REQUEST, SigT::READ_REQUEST}});
    }

    imemBlock->memoryPort()
        .portBySignal(SigT::WRITE_BITMASK)
        .setToStatic(StaticSignal::VCC);
    imemBlock->memoryPort()
        .portBySignal(SigT::WRITEMODE)
        .setToStatic(StaticSignal::VCC);
    imemBlock->memoryPort()
        .portBySignal(SigT::WRITE_DATA)
        .setToStatic(StaticSignal::GND);
    coreImemPort.portBySignal(SigT::READ_REQUEST_READY)
        .setToStatic(StaticSignal::GND);  // Active low
}

void
ProcessorWrapperBlock::addDataMemory(const MemoryBusInterface& coreDmemPort) {
    using SigT = SignalType;
    std::cout << "--9" << std::endl;

    const NetlistPort& addrPort = coreDmemPort.portBySignal(SigT::ADDRESS);
    const NetlistPort& dataPort = coreDmemPort.portBySignal(SigT::WRITE_DATA);
std::cout << "--10" << std::endl;
    SinglePortSSRAMBlock* dmemBlock = new SinglePortSSRAMBlock(
        addrPort.widthFormula(), dataPort.widthFormula(),
        TCEString("tb/dmem_") + coreDmemPort.addressSpace() + "_init.img",
        /* isForSmulation = */ true);
        std::cout << "dmem addr width formula: " << addrPort.widthFormula() << std::endl;
        std::cout << "dmem data width formula: " << dataPort.widthFormula() << std::endl;
    std::cout << "Adding bitmasked data memory" << std::endl;
    addSubBlock(dmemBlock, TCEString("dmem_") + coreDmemPort.addressSpace());

    if (!netlist().connect(dmemBlock->memoryPort(), coreDmemPort)) {
        THROW_EXCEPTION(
            IllegalConnectivity,
            "Could not connect two port groups together.");
    }
}

void
ProcessorWrapperBlock::addDataMemory2(
    const MemoryBusInterface& coreDmemPort) {
    using SigT = SignalType;

    const NetlistPort& addrPort = coreDmemPort.portBySignal(SigT::AADDR);
    const NetlistPort& dataPort = coreDmemPort.portBySignal(SigT::RDATA);

    SinglePortByteMaskSSRAMBlock* dmemBlock =
        new SinglePortByteMaskSSRAMBlock(
            addrPort.widthFormula(), dataPort.widthFormula(),
            TCEString("tb/dmem_") + coreDmemPort.addressSpace() + "_init.img",
            /* isForSmulation = */ true);
            std::cout << "dmem addr width formula: " << addrPort.widthFormula() << std::endl;
        std::cout << "dmem data width formula: " << dataPort.widthFormula() << std::endl;
    std::cout << "Adding bytemasked data memory" << std::endl;
    addSubBlock(dmemBlock, TCEString("dmem_") + coreDmemPort.addressSpace());

    if (!netlist().connect(dmemBlock->memoryPort(), coreDmemPort)) {
        THROW_EXCEPTION(
            IllegalConnectivity,
            "Could not connect two port groups together.");
    }
}

void
ProcessorWrapperBlock::connectLockStatus(
    const NetlistPort& topLockStatusPort) {
    assert(
        coreBlock_->port("locked") != nullptr &&
        "Could not found lock status port.");
    if (!netlist().connect(*coreBlock_->port("locked"), topLockStatusPort)) {
        THROW_EXCEPTION(
            IllegalConnectivity,
            "Could not connect \"locked\" signal to the toplevel");
    }
}

/**
 * Handles unconnected ports of the top-level TTA processor by connecting
 * them to the toplevel
 */
void
ProcessorWrapperBlock::handleUnconnectedPorts() {
    for (size_t i = 0; i < coreBlock_->portCount(); i++) {
        const NetlistPort& port =
            ((const BaseNetlistBlock*)coreBlock_)->port(i);
        if (!netlist().isPortConnected(port) && !port.hasStaticValue()) {
            NetlistPort* topPort = port.clone();
            addPort(topPort);
            netlist().connect(*topPort, port);
        }
    }
}

} /* namespace ProGe */
