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
 * @file SinglePortByteMaskSSRAMBlock.cc
 *
 * Implementation of SinglePortByteMaskSSRAMBlock class.
 *
 * Created on: 8.9.2015
 * @author Henry Linjam�ki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "SinglePortByteMaskSSRAMBlock.hh"

#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"
#include "Parameter.hh"
#include "NetlistFactories.hh"
#include "HDLTemplateInstantiator.hh"

#include "FileSystem.hh"

namespace ProGe {

/**
 * Constructs Single port synchronous SRAM block.
 *
 * @param addressWidth The width of the address port.
 * @param dataWidth The width of the data port.
 * @param memInitFile Name of the memory initialization file loaded during
 *                    RTL-simulation.
 * @param isForSimulation Tells if the block is used in RTL simulation.
 * Affects placement of the HDL source.
 */
SinglePortByteMaskSSRAMBlock::SinglePortByteMaskSSRAMBlock(
    const std::string& addressWidth, const std::string& dataWidth,
    const std::string& memInitFile, bool isForSimulation)
    : BaseNetlistBlock("synch_byte_mask_sram", ""),
      isForSimulation_(isForSimulation) {
    addParameter(Parameter("DATAW", "integer", dataWidth));
    addParameter(Parameter("ADDRW", "integer", addressWidth));
    addParameter(Parameter("INITFILENAME", "string", memInitFile));
    addParameter(Parameter("access_trace", "boolean", "false"));
    addParameter(
        Parameter("ACCESSTRACEFILENAME", "string", "\"access_trace\""));

    addPort(PortFactory::clockPort());

    // todo add memory ports via PortFactory
    addPortGroup(
        memoryPortGroup_ = new NetlistPortGroup(
            SignalGroupType::BYTEMASKED_SRAM_PORT,
            new InBitPort(
                "avalid", Signal(SignalType::AVALID, ActiveState::HIGH)),
            new OutBitPort(
                "aready", Signal(SignalType::AREADY, ActiveState::HIGH)),
            new InPort("aaddr", "ADDRW", BIT_VECTOR, SignalType::AADDR),
            new InBitPort(
                "awren", Signal(SignalType::AWREN, ActiveState::HIGH)),
            new InPort(
                "astrb", dataWidth + "/8", BIT_VECTOR, SignalType::ASTRB),
            new InBitPort(
                "rready", Signal(SignalType::RREADY, ActiveState::HIGH)),
            new OutPort("rdata", "DATAW", BIT_VECTOR, SignalType::RDATA),
            new InPort("adata", "DATAW", BIT_VECTOR, SignalType::ADATA),
            new OutBitPort(
                "rvalid", Signal(SignalType::RVALID, ActiveState::HIGH))));
}

SinglePortByteMaskSSRAMBlock::~SinglePortByteMaskSSRAMBlock() {}

/**
 * Sets a file name where memory access trace is dumped for this memory.
 */
void
SinglePortByteMaskSSRAMBlock::setAccessTraceFile(const std::string filename) {
    this->setParameter(Parameter("access_trace", "boolean", "true"));
    this->setParameter(Parameter("ACCESSTRACEFILENAME", "string", filename));
}

const NetlistPortGroup&
SinglePortByteMaskSSRAMBlock::memoryPort() const {
    assert(memoryPortGroup_ != nullptr);
    return *memoryPortGroup_;
}

void
SinglePortByteMaskSSRAMBlock::write(
    const Path& targetBaseDir, HDL targetLang) const {
    Path progeDataDir(Environment::dataDirPath("ProGe"));

    assert(targetLang == VHDL || targetLang == Verilog);

    std::string tempFile = (targetLang == VHDL)
                               ? std::string("synch_byte_mask_sram.vhdl")
                               : std::string("synch_byte_mask_sram.v");
    std::string targetDir =
        (isForSimulation_) ? std::string("tb")
                           : ((targetLang == VHDL) ? std::string("vhdl")
                                                   : std::string("verilog"));

    HDLTemplateInstantiator().instantiateTemplateFile(
        (progeDataDir / "tb" / tempFile).string(),
        (targetBaseDir / targetDir / tempFile).string());
}

} /* namespace ProGe */
