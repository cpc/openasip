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
 * @file BlocksMUL.cc
 *
 * Implementation of BlocksMUL Class.
 * This class resembles a single output in the Blocks architecture.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#include "BlocksMUL.hh"

using namespace TTAMachine;

/**
 * Constructor BlocksMUL
 *
 * @param mach The TTA machine where the MUL needs to be added.
 * @param name The name of the MUL.
 * @param sources A list of sources that are attached to this unit's input.
 * @param in1sock (shared) pointer to the TTA socket for in1 input port.
 * @param in2sock (shared) pointer to the TTA socket for in2 input port.
 */
BlocksMUL::BlocksMUL(
    Machine& mach, const std::string& name, std::list<std::string> sources,
    std::shared_ptr<TTAMachine::Socket> in1sock,
    std::shared_ptr<TTAMachine::Socket> in2sock)
    : BlocksFU::BlocksFU(mach, name, sources, in1sock, in2sock) {
    // Function unit and ports
    mul = this->fu;

    // HW ops
    HWOperation* mulOp = CreateHWOp("mul", 3);
    ops.push_back(mulOp);
    HWOperation* copyOp = CreateHWOp("copy", 2);
    ops.push_back(copyOp);
}

/**
 * Constructor BlocksMULPair
 *
 * @param mach The TTA machine where the MUL pair needs to be added.
 * @param name The name of the MUL pair.
 * @param sources A list of sources that are attached to this unit's input.
 * @param usesOut0 A boolean that indicates whether output port 0 of the FU is
 * used in the Blocks.
 * @param usesOut1 A boolean that indicates whether output port 1 of the FU is
 * used in the Blocks.
 */
BlocksMULPair::BlocksMULPair(
    TTAMachine::Machine& mach, const std::string& name,
    std::list<std::string> sources, bool usesOut0, bool usesOut1)
    : sources(sources) {
    in1sock = std::make_shared<TTAMachine::Socket>(name + "_in1t");
    in2sock = std::make_shared<TTAMachine::Socket>(name + "_in2");
    mach.addSocket(*(in1sock.get()));
    mach.addSocket(*(in2sock.get()));
    if (usesOut0)
        mul0.reset(
            new BlocksMUL(mach, name + "_out0", sources, in1sock, in2sock));
    if (usesOut1)
        mul1.reset(
            new BlocksMUL(mach, name + "_out1", sources, in1sock, in2sock));
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
BlocksMUL::CreateHWOp(const std::string& name, int numOfOperands) {
    HWOperation* op = new HWOperation(name, *mul);
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
BlocksMUL::BindPorts(HWOperation* hwOp, int numOfOperands) {
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
            "Trying to add an MUL operation with a different number of "
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
BlocksMUL::ConfigurePipeline(ExecutionPipeline* pipeline, int numOfOperands) {
    if (numOfOperands < 3) {
        pipeline->addPortRead(1, 0, 1);
        pipeline->addPortWrite(2, 0, 1);
    } else {
        pipeline->addPortRead(1, 0, 1);
        pipeline->addPortRead(2, 0, 1);
        pipeline->addPortWrite(3, 0, 1);
    }
}
