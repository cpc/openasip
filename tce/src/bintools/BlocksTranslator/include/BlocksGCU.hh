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
 * @file BlocksGCU.hh
 *
 * This class describes the GCU unit of the Blocks (ABU of the TTA).
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#ifndef BLOCKS_GCU_HH
#define BLOCKS_GCU_HH

#include "ControlUnit.hh"
#include "ExecutionPipeline.hh"
#include "FUPort.hh"
#include "Guard.hh"
#include "HWOperation.hh"
#include "Machine.hh"
#include "Segment.hh"
#include "Socket.hh"
#include "SpecialRegisterPort.hh"

class BlocksGCU {
public:
    TTAMachine::ControlUnit* gcu;
    BlocksGCU(
        TTAMachine::Machine& mach, const std::string& name,
        TTAMachine::AddressSpace& asInstr);
    TTAMachine::FUPort* pc;
    TTAMachine::FUPort* val;
    TTAMachine::SpecialRegisterPort* ra;
    TTAMachine::Socket* raIn;
    TTAMachine::Socket* raOut;
    TTAMachine::Socket* pcIn;
    TTAMachine::Socket* valIn;
    TTAMachine::Bus* raBus;
    std::list<std::string> sources;
    ~BlocksGCU() {
        delete pc;
        delete val;
        delete ra;
        while (!ops.empty()) {
            delete ops.back();
            ops.pop_back();
        }
        delete raIn;
        delete raOut;
        delete valIn;
        delete pcIn;
        delete gcu;
    }

private:
    std::vector<TTAMachine::HWOperation*> ops;
    void ConfigurePipeline(
        TTAMachine::ExecutionPipeline* pipeline, int numOfOperands);
    void BindPorts(TTAMachine::HWOperation* hwOp, int numOfOperands);
    TTAMachine::HWOperation* CreateHWOp(
        const std::string& name, int numOfOperands);
};
#endif
