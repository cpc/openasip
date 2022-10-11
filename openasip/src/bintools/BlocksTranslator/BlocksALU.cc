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
 * @file BlocksALU.cc
 *
 * This class resembles a single output in the Blocks CGRA architecture.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#include "BlocksALU.hh"

#include "ExecutionPipeline.hh"

using namespace TTAMachine;
/**
 * Create BlocksALU with default instructions
 *
 * @param mach The TTA machine where the ALU needs to be added.
 * @param name The name of the ALU.
 * @param sources A list of sources that are attached to this unit's input.
 * @param in1sock (shared) pointer to the TTA socket for in1 input port.
 * @param in2sock (shared) pointer to the TTA socket for in2 input port.
 */
BlocksALU::BlocksALU(
    Machine& mach, const std::string& name, std::list<std::string> sources,
    std::shared_ptr<TTAMachine::Socket> in1sock,
    std::shared_ptr<TTAMachine::Socket> in2sock)
    : BlocksFU::BlocksFU(mach, name, sources, in1sock, in2sock) {
    // Function unit and ports
    alu = this->fu;

    // HW ops
    ops.push_back(CreateHWOp("add", 3));
    ops.push_back(CreateHWOp("and", 3));
    ops.push_back(CreateHWOp("eq", 3));
    ops.push_back(CreateHWOp("ge", 3));
    ops.push_back(CreateHWOp("geu", 3));
    ops.push_back(CreateHWOp("gt", 3));
    ops.push_back(CreateHWOp("gtu", 3));
    ops.push_back(CreateHWOp("ior", 3));
    ops.push_back(CreateHWOp("lt", 3));
    ops.push_back(CreateHWOp("ltu", 3));
    ops.push_back(CreateHWOp("ne", 3));
    ops.push_back(CreateHWOp("not", 2));
    ops.push_back(CreateHWOp("shl1_32", 2));
    ops.push_back(CreateHWOp("shl4_32", 2));
    ops.push_back(CreateHWOp("shr1_32", 2));
    ops.push_back(CreateHWOp("shr4_32", 2));
    ops.push_back(CreateHWOp("shru1_32", 2));
    ops.push_back(CreateHWOp("shru4_32", 2));
    ops.push_back(CreateHWOp("sub", 3));
    ops.push_back(CreateHWOp("xor", 3));
    ops.push_back(CreateHWOp("copy", 2));
}

/**
 * Emulates second output port with copy of FU with input port sharing
 *
 * @param mach The TTA machine where the ALU pair needs to be added.
 * @param name The name of the ALU pair.
 * @param sources A list of sources that are attached to this unit's input.
 * @param usesOut0 A boolean that indicates whether output port 0 of the FU is
 * used in the CGRA.
 * @param usesOut1 A boolean that indicates whether output port 1 of the FU is
 * used in the CGRA.
 */
BlocksALUPair::BlocksALUPair(
    TTAMachine::Machine& mach, const std::string& name,
    std::list<std::string> sources, bool usesOut0, bool usesOut1)
    : sources(sources) {
    in1sock = std::make_shared<TTAMachine::Socket>(name + "_in1t");
    in2sock = std::make_shared<TTAMachine::Socket>(name + "_in2");
    mach.addSocket(*(in1sock.get()));
    mach.addSocket(*(in2sock.get()));
    if (usesOut0)
        alu0.reset(
            new BlocksALU(mach, name + "_out0", sources, in1sock, in2sock));
    if (usesOut1)
        alu1.reset(
            new BlocksALU(mach, name + "_out1", sources, in1sock, in2sock));
}

/**
 * Creates a hardware operation.
 *
 * @param name The name of the hardware operation. This needs to exactly match
 * the name in OSEd.
 * @param numOfOperands The number of operands that this hardware operation
 * requires.
 */
HWOperation*
BlocksALU::CreateHWOp(const std::string& name, int numOfOperands) {
    HWOperation* op = new HWOperation(name, *alu);
    BindPorts(op, numOfOperands);
    ConfigurePipeline(op->pipeline(), numOfOperands);
    return op;
}

/**
 * Bind the operation's operands to FU ports.
 *
 * @param hwOp A pointer to the hardware operation of which the operands need
 * to be binded to FU ports.
 * @param numOfOperands The number of operands of the given hwOp.
 */
void
BlocksALU::BindPorts(HWOperation* hwOp, int numOfOperands) {
    if (numOfOperands == 2) {
        hwOp->bindPort(1, *in1);
        hwOp->bindPort(2, *out);
    } else if (numOfOperands == 3) {
        hwOp->bindPort(1, *in1);
        hwOp->bindPort(2, *in2);
        hwOp->bindPort(3, *out);
    } else {
        // Currently no operation known that uses a different numbers of
        // operands.
        assert(
            false &&
            "Trying to add an ALU operation with a different number of "
            "operands than 2 or 3.");
    }
}

/**
 * Configure the operation pipeline.
 *
 * @param pipeline A pointer to the execution pipeline of the operation of
 * which the pipeline needs to be configured.
 * @param numOfOperands The number of operands of the given hwOp.
 */
void
BlocksALU::ConfigurePipeline(ExecutionPipeline* pipeline, int numOfOperands) {
    if (numOfOperands < 3) {
        pipeline->addPortRead(1, 0, 1);
        pipeline->addPortWrite(2, 0, 1);
    } else {
        pipeline->addPortRead(1, 0, 1);
        pipeline->addPortRead(2, 0, 1);
        pipeline->addPortWrite(3, 0, 1);
    }
}
