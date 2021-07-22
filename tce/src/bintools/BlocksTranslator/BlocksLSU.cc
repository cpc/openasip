/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file BlocksLSU.cc
 *
 * Implementation of BlocksLSU Class.
 *  This class resembles a single output in the Blocks architecture.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */
#include "BlocksLSU.hh"

using namespace TTAMachine;

/**
 * BlocksLSU with default instructions
 *
 * @param mach The TTA machine where the LSU needs to be added.
 * @param name The name of the LSU.
 * @param sources A list of sources that are attached to this unit's input.
 * @param asData The address space containing the program data.
 * @param in1sock (shared) pointer to the TTA socket for in1 input port.
 * @param in2sock (shared) pointer to the TTA socket for in2 input port.
 */
BlocksLSU::BlocksLSU(
    Machine& mach, const std::string& name, std::list<std::string> sources,
    AddressSpace& asData, std::shared_ptr<TTAMachine::Socket> in1sock,
    std::shared_ptr<TTAMachine::Socket> in2sock)
    : BlocksFU::BlocksFU(mach, name, sources, in1sock, in2sock) {
    // Function unit and ports
    lsu = this->fu;
    // Set adress space to data
    lsu->setAddressSpace(&asData);

    // HWOps
    ops.push_back(CreateHWLoadOp("ld32"));
    ops.push_back(CreateHWLoadOp("ldu16"));
    ops.push_back(CreateHWLoadOp("ldu8"));
    ops.push_back(CreateHWStoreOp("st16"));
    ops.push_back(CreateHWStoreOp("st8"));
    ops.push_back(CreateHWStoreOp("st32"));
    ops.push_back(CreateHWLoadOp("copy"));
}

/**
 * Constructor for BlocksLSUPair
 *
 * @param mach The TTA machine where the LSU pair needs to be added.
 * @param name The name of the LSU pair.
 * @param sources A list of sources that are attached to this unit's input.
 * @param asData The address space containing the program data.
 * @param usesOut0 A boolean that indicates whether output port 0 of the FU is
 * used in the Blocks.
 * @param usesOut1 A boolean that indicates whether output port 1 of the FU is
 * used in the Blocks.
 */
BlocksLSUPair::BlocksLSUPair(
    TTAMachine::Machine& mach, const std::string& name,
    std::list<std::string> sources, TTAMachine::AddressSpace& asData,
    bool usesOut0, bool usesOut1)
    : sources(sources) {
    in1sock = std::make_shared<TTAMachine::Socket>(name + "_in1t");
    in2sock = std::make_shared<TTAMachine::Socket>(name + "_in2");
    mach.addSocket(*(in1sock.get()));
    mach.addSocket(*(in2sock.get()));
    if (usesOut0)
        lsu0.reset(new BlocksLSU(
            mach, name + "_out0", sources, asData, in1sock, in2sock));
    if (usesOut1)
        lsu1.reset(new BlocksLSU(
            mach, name + "_out1", sources, asData, in1sock, in2sock));
}

/**
 * Creates a hardware load operation.
 *
 * @param name The name of the hardware operation. This needs to exactly match
 * the name in OSEd.
 */
HWOperation*
BlocksLSU::CreateHWLoadOp(const std::string& name) {
    HWOperation* op = new HWOperation(name, *lsu);
    BindPorts(op, true);
    ConfigurePipeline(op->pipeline(), true);
    return op;
}
/**
 * Creates a hardware store operation.
 *
 * @param name The name of the hardware operation. This needs to exactly match
 * the name in OSEd.
 */
HWOperation*
BlocksLSU::CreateHWStoreOp(const std::string& name) {
    HWOperation* op = new HWOperation(name, *lsu);
    BindPorts(op, false);
    ConfigurePipeline(op->pipeline(), false);
    return op;
}

/**
 * Bind the operation's operands to FU ports.
 *
 * @param hwOp A pointer to the hardware operation of which the operands need
 * to be binded to FU ports.
 * @param isLoadOp A boolean that indicates whether the operation given is a
 * load operation(true) or store operation(false).
 */
void
BlocksLSU::BindPorts(TTAMachine::HWOperation* hwOp, bool isLoadOp) {
    if (isLoadOp) {               // Load operation
        hwOp->bindPort(1, *in1);  // Memory address to read
        hwOp->bindPort(2, *out);  // Data
    } else {                      // Store operation
        hwOp->bindPort(1, *in1);  // Memory address to write
        hwOp->bindPort(2, *in2);  // Data to write
    }
}

/**
 * Configure the operation pipeline.
 *
 * @param pipeline A pointer to the execution pipeline of the operation of
 * which the pipeline needs to be configured.
 * @param isLoadOp A boolean that indicates whether the operation given is a
 * load operation(true) or store operation(false).
 */
void
BlocksLSU::ConfigurePipeline(
    TTAMachine::ExecutionPipeline* pipeline, bool isLoadOp) {
    if (isLoadOp) {
        pipeline->addPortRead(1, 0, 1);
        pipeline->addPortWrite(2, 0, 1);
    } else {
        pipeline->addPortRead(1, 0, 1);
        pipeline->addPortRead(2, 0, 1);
    }
}
