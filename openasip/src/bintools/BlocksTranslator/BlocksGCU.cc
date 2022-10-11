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
 * @file BlocksGCU.cc
 *
 * Implementation of BlocksGCU Class.
 * This class describes the GCU unit of the Blocks (ABU of the TTA).
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#include "BlocksGCU.hh"

using namespace TTAMachine;
/**
 * BlocksGCU with default instructions
 *
 * @param mach The TTA machine where the GCU(ABU) needs to be added.
 * @param name The name of the GCU.
 * @param asInstr The address space containing the program instructions.
 */
BlocksGCU::BlocksGCU(
    Machine& mach, const std::string& name, AddressSpace& asInstr) {
    int delaySlots = 2;
    int globalGuardLatency = 1;
    gcu = new ControlUnit(name, delaySlots, globalGuardLatency);
    mach.setGlobalControl(*gcu);
    gcu->setAddressSpace(&asInstr);

    // TODO(mm): add check if there is not already a GCU present
    pc = new FUPort("pc", 32, *gcu, true, true, true);
    val = new FUPort("value", 32, *gcu, false, false, true);
    ra = new SpecialRegisterPort("ra", 32, *gcu);

    // Create in and ouput sockets for 'special RA port'
    raIn = new Socket("raIn");
    raOut = new Socket("abu_out0");
    valIn = new Socket("value");
    pcIn = new Socket("pc");
    mach.addSocket(*raIn);
    mach.addSocket(*raOut);
    mach.addSocket(*valIn);
    mach.addSocket(*pcIn);
    ra->attachSocket(*raIn);
    ra->attachSocket(*raOut);
    pc->attachSocket(*pcIn);
    val->attachSocket(*valIn);

    // Connect in and out socket to bus
    const int busWidth = 32;
    const int immWidth = 0;
    Machine::Extension busExt = Machine::Extension::ZERO;
    Bus* raBus = new Bus("ra_out_to_ra_in", busWidth, immWidth, busExt);
    mach.addBus(*raBus);
    new UnconditionalGuard(false, *raBus);
    new Segment("seg1", *raBus);
    raIn->attachBus(*raBus);
    raOut->attachBus(*raBus);
    pcIn->attachBus(*raBus);
    raOut->setDirection(Socket::Direction::OUTPUT);

    // set RA port
    gcu->setReturnAddressPort(*ra);

    // Add HWops
    ops.push_back(CreateHWOp("bnz", 2));
    ops.push_back(CreateHWOp("bz", 2));
    ops.push_back(CreateHWOp("call", 1));
    ops.push_back(CreateHWOp("jump", 1));
}

/**
 * Configure the operation pipeline.
 *
 * @param pipeline A pointer to the execution pipeline of the operation of
 * which the pipeline needs to be configured.
 * @param numOfOperands The number of operands of the given hwOp.
 */
void
BlocksGCU::ConfigurePipeline(
    TTAMachine::ExecutionPipeline* pipeline, int numOfOperands) {
    if (numOfOperands == 1) {
        pipeline->addPortRead(1, 0, 1);
    } else if (numOfOperands == 2) {
        pipeline->addPortRead(1, 0, 1);
        pipeline->addPortRead(2, 0, 1);
    } else {
        // Currently no operation known that uses a different numbers of
        // operands.
        assert(
            false &&
            "Trying to add a GCU operation with a different number of "
            "operands than 1 or 2.");
    }
}

/**
 * Bind the operation's operands to GCU ports.
 *
 * @param hwOp A pointer to the hardware operation of which the operands need
 * to be binded to GCU ports.
 * @param numOfOperands The number of operands of the given hwOp.
 */
void
BlocksGCU::BindPorts(TTAMachine::HWOperation* hwOp, int numOfOperands) {
    if (numOfOperands < 2) {
        hwOp->bindPort(1, *pc);
    } else {
        hwOp->bindPort(2, *pc);  // Program counter is operand 2, therfore
                                 // port binding is switched.
        hwOp->bindPort(1, *val);
    }
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
BlocksGCU::CreateHWOp(const std::string& name, int numOfOperands) {
    HWOperation* op = new HWOperation(name, *gcu);
    BindPorts(op, numOfOperands);
    ConfigurePipeline(op->pipeline(), numOfOperands);
    return op;
}
