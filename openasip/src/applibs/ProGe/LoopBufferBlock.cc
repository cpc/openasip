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
 * @file LoopBufferBlock.cc
 *
 * Implementation of LoopBufferBlock class.
 *
 * Created on: 3.12.2015
 * @author Henry Linjam�ki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "LoopBufferBlock.hh"

#include "NetlistFactories.hh"
#include "Netlist.hh"
#include "NetlistPort.hh"
#include "NetlistPortGroup.hh"

#include "MachineInfo.hh"

#include "HDLTemplateInstantiator.hh"
#include "FileSystem.hh"

namespace ProGe {

/**
 * Constructs the loop buffer block that automatically selects suitable
 * implementation.
 *
 * The constructor inspects operations in the CU of the machine and selects
 * an appropriate loop buffer implementation.
 *
 * @exception NotAvailable If the CU does not include an operation, which
 *                         utilizes the loop buffer.
 */
LoopBufferBlock::LoopBufferBlock(
    const ProGeContext& context, BaseNetlistBlock* parent)

    : BaseNetlistBlock("not_set_yet", "loopbuffer", parent) {
    MachineInfo::OperationSet cuOps;
    cuOps = MachineInfo::getOpset(*context.adf().controlUnit());

    if (!cuOps.count("hwloop") && !cuOps.count("lbufs") &&
        !cuOps.count("infloop")) {
        THROW_EXCEPTION(
            NotAvailable,
            "Could not recognize any loop buffer "
            "utilizing operations in CU");
    } else if (cuOps.count("hwloop") && cuOps.count("lbufs")) {
        THROW_EXCEPTION(
            NotAvailable,
            "Can not support hwloop and lbufs "
            "operation combination.");
    } else if (cuOps.count("lbufs") && cuOps.count("infloop")) {
        THROW_EXCEPTION(
            NotAvailable,
            "Can not support lbufs and infloop "
            "operation combination.");
    }

    entityStr_ = context.coreEntityName();

    // Common block construction //
    setParameter(Parameter("depth", "integer", ""));
    setParameter(Parameter("instw", "integer", ""));
    setParameter(Parameter("core_id", "integer", "0"));
    setParameter(Parameter("enable_usage_trace", "boolean", "false"));

    addPort(PortFactory::clockPort());
    addPort(PortFactory::resetPort())->rename("rstx");

    lockReqPortIn_ = addPort(new InBitPort("lock_req_in"));
    lockReqPortOut_ = addPort(new OutBitPort("lock_req_out"));
    lockPortIn_ = addPort(new InBitPort("glock_in"));
    lockPortOut_ = addPort(new OutBitPort("glock_out"));
    // note: port widths "IMEMWIDTHINMAUS*IMEMMAUWIDTH", "LBUFMAXDEPTH" and
    // "LBUFMAXITER" are temporary solutions for instancing blocks. The actual
    // port width formulas use generics.
    instructionPortIn_ =
        addPort(new InPort("fetchblock_in", "IMEMWIDTHINMAUS*IMEMMAUWIDTH"));
    instructionPortOut_ =
        addPort(new OutPort("dispatch_out", "IMEMWIDTHINMAUS*IMEMMAUWIDTH"));
    startPortIn_ = addPort(new InBitPort("loop_start_in"));

    if (cuOps.count("hwloop")) {
        loopBodySizePortIn_ =
            addPort(new InPort("loop_len_in", "IMEMADDRWIDTH"));
    } else {
        loopBodySizePortIn_ =
            addPort(new InPort("loop_len_in", "bit_width(LBUFMAXDEPTH+1)"));
    }

    // Ports used with HWLOOP
    if (cuOps.count("hwloop")) {
        lenCntrPortOut_ =
            addPort(new OutPort("len_cntr_out", "IMEMADDRWIDTH"));
        loopFromImemPortOut_ = addPort(new OutBitPort("loop_from_imem_out"));
    }

    // Variated block construction //
    // note: assuming here that only one operation in CU utilizes loop buffer.
    if (cuOps.count("hwloop")) {
        if (cuOps.count("lbufc")) {
            THROW_EXCEPTION(
                NotAvailable,
                "BREAK-operation is not currently "
                "available with HWLOOP operation.");
        }
        setModuleName(context.coreEntityName() + "_hwloop");
        addParameter(Parameter("iterw", "integer", ""));
        loopIterationPortIn_ =
            addPort(new InPort("loop_iter_in", "LBUFMAXITER"));
        implmenetationFile_ = "hwloop";
    } else if (cuOps.count("lbufs")) {
        if (cuOps.count("lbufc")) {
            THROW_EXCEPTION(
                NotAvailable,
                "BREAK-operation is not currently "
                "available with LBUFS operation.");
        }
        setModuleName(context.coreEntityName() + "_loopbuf");
        addParameter(Parameter("iterw", "integer", ""));
        loopIterationPortIn_ =
            addPort(new InPort("loop_iter_in", "LBUFMAXITER"));
        implmenetationFile_ = "loopbuffer";
    } else if (cuOps.count("infloop")) {
        if (cuOps.count("lbufc")) {
            stopPortIn_ = addPort(new InBitPort("loop_stop_in"));
        }
        setModuleName(context.coreEntityName() + "_inflooper");
        implmenetationFile_ = "inflooper";
    } else {
        assert(false);
    }
}

LoopBufferBlock::~LoopBufferBlock() {
    // Managed by BaseNetlistBlock
}

/**
 * Sets the width of the blocks (i.e. instructions).
 */
void
LoopBufferBlock::setBlockWidthParameter(const std::string value) {
    setParameter(Parameter("instw", "integer", value));
}

/**
 * Sets the depth of the loop buffer.
 *
 * The depth is the number of blocks that can be stored in the buffer at most.
 *
 */
void
LoopBufferBlock::setBufferSizeParameter(const std::string value) {
    setParameter(Parameter("depth", "integer", value));
}

/**
 * Sets the width of control port for iteration count if applicable.
 *
 * The maximum amount of iterations via the port is 2^n-1, where n is the
 * port width.
 *
 */
void
LoopBufferBlock::setIterationPortWidthParameter(const std::string value) {
    // Constructor creates the parameter when needed.
    if (hasParameter("iterw")) {
        setParameter(Parameter("iterw", "integer", value));
    }
}

/**
 * Sets core id for usage tracing.
 *
 * By default the core id is set to "0".
 */
void
LoopBufferBlock::setCoreIdParameter(const std::string value) {
    setParameter(Parameter("core_id", "integer", value));
}

/**
 * Sets usage tracing.
 *
 * By default the trace is disabled (set to false).
 *
 * When enabled the instance dumps the usage trace into a file
 * (core<core_id>_l0_access_trace.dump) in RTL-simulation.
 *
 */
void
LoopBufferBlock::setUsageTracingParameter(bool value) {
    setParameter(
        Parameter("enable_usage_trace", "boolean", value ? "true" : "false"));
}

/**
 * Returns (global) lock request input port.
 *
 */
const NetlistPort&
LoopBufferBlock::lockReqPortIn() const {
    assert(lockReqPortIn_ != nullptr && "Unset port.");
    return *lockReqPortIn_;
}

/**
 * Returns (global) lock request output port.
 */
const NetlistPort&
LoopBufferBlock::lockReqPortOut() const {
    assert(lockReqPortOut_ != nullptr && "Unset port.");
    return *lockReqPortOut_;
}

/**
 * Returns (global) lock input port.
 */
const NetlistPort&
LoopBufferBlock::lockPortIn() const {
    assert(lockPortIn_ != nullptr && "Unset port.");
    return *lockPortIn_;
}

/**
 * Returns (global) lock output port.
 */
const NetlistPort&
LoopBufferBlock::lockPortOut() const {
    assert(lockPortOut_ != nullptr && "Unset port.");
    return *lockPortOut_;
}

/**
 * Returns input port for instruction word.
 */
const NetlistPort&
LoopBufferBlock::instructionPortIn() const {
    assert(instructionPortIn_ != nullptr && "Unset port.");
    return *instructionPortIn_;
}

/**
 * Returns output port for instruction word.
 */
const NetlistPort&
LoopBufferBlock::instructionPortOut() const {
    assert(instructionPortOut_ != nullptr && "Unset port.");
    return *instructionPortOut_;
}

/**
 * Returns control port for starting loop buffer.
 */
const NetlistPort&
LoopBufferBlock::startPortIn() const {
    assert(startPortIn_ != nullptr && "Unset port.");
    return *startPortIn_;
}

/**
 * Returns port that is used to stop looping.
 *
  @return The stop looping control port, if applicable.
 *        Otherwise, returns nullptr denoting unavailability of the port.
 */
const NetlistPort*
LoopBufferBlock::stopPortIn() const {
    return stopPortIn_;
}

/**
 * Returns control port for setting loop body size.
 */
const NetlistPort&
LoopBufferBlock::loopBodySizePortIn() const {
    assert(loopBodySizePortIn_ != nullptr && "Unset port.");
    return *loopBodySizePortIn_;
}

/**
 * Returns control port for setting iteration count.
 *
 * @return The control port for setting iteration count, if applicable.
 *         Otherwise, returns nullptr denoting unavailability of the port.
 */
const NetlistPort*
LoopBufferBlock::loopIterationPortIn() const {
    return loopIterationPortIn_;
}

/**
 * Returns control port for outputting current loop instruction index.
 */
const NetlistPort&
LoopBufferBlock::lenCntrPortOut() const {
    assert(lenCntrPortOut_ != nullptr && "Unset port.");
    return *lenCntrPortOut_;
}

/**
 * Returns control port for indicating that loop is executed
 * from outside of loopbuffer module.
 */
const NetlistPort&
LoopBufferBlock::loopFromImemPortOut() const {
    assert(loopFromImemPortOut_ != nullptr && "Unset port.");
    return *loopFromImemPortOut_;
}

void
LoopBufferBlock::write(const Path& targetBaseDir, HDL targetLang) const {
    if (targetLang != HDL::VHDL) {
        THROW_EXCEPTION(NotAvailable, "Unsupported HDL language.");
    }
    assert(moduleName() != "not_set_yet");

    std::string target = targetBaseDir.string() +
                         FileSystem::DIRECTORY_SEPARATOR + "gcu_ic" +
                         FileSystem::DIRECTORY_SEPARATOR;

    bool usesLoopBreaking = stopPortIn() != nullptr;

    Path progeDataDir(Environment::dataDirPath("ProGe"));
    HDLTemplateInstantiator instantiator(entityStr_);

    if (usesLoopBreaking) {
        instantiator.replacePlaceholder(
            "port-declarations",
            "-- Stops looping\n"
            "loop_stop_in : in std_logic;");
        instantiator.replacePlaceholderFromFile(
            "fsm-logic",
            progeDataDir / "inflooper_fsm_with_stopping.snippet");
        instantiator.replacePlaceholderFromFile(
            "signal-declarations",
            progeDataDir / "inflooper_stop_signals.snippet");
        instantiator.replacePlaceholderFromFile(
            "stop-reg", progeDataDir / "inflooper_stop_register.snippet");
    } else {
        instantiator.replacePlaceholderFromFile(
            "fsm-logic", progeDataDir / "inflooper_fsm_default.snippet");
    }

    instantiator.instantiateTemplateFile(
        (progeDataDir / (implmenetationFile_ + ".vhdl.tmpl")).string(),
        target + implmenetationFile_ + ".vhdl");
}

} /* namespace ProGe */
