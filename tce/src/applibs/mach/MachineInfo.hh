/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file MachineInfo.hh
 *
 * Declaration of MachineInfo class.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_INFO_HH
#define TTA_MACHINE_INFO_HH

#include "OperationDAGSelector.hh"
#include "InstructionTemplate.hh"

#include <set>

namespace TTAMachine {
    class AddressSpace;
    class Machine;
    class HWOperation;
    class FUPort;
    class FunctionUnit;
}

class Operand;
class Operation;

class MachineInfo {
public:
    static OperationDAGSelector::OperationSet getOpset(
        const TTAMachine::Machine& mach);
    static bool supportsOperation(
        const TTAMachine::Machine& mach, TCEString operation);
    static TTAMachine::AddressSpace* defaultDataAddressSpace(
        const TTAMachine::Machine& mach);
    static int longestGuardLatency(
        const TTAMachine::Machine& mach);
    static Operand& operandFromPort(
        const TTAMachine::HWOperation& hwOp,
        const TTAMachine::FUPort& port);
    static bool templatesUsesSlot(
        const TTAMachine::Machine& mach,
        const std::string& slotName);
    static std::set<TTAMachine::InstructionTemplate*> templatesUsingSlot(
        const TTAMachine::Machine& mach,
        const std::string& slotName);
    static bool canEncodeImmediateInteger(
        const TTAMachine::Machine& mach, int64_t imm,
        unsigned destWidth=UINT_MAX);
    static int triggerIndex(
        const TTAMachine::Machine& machine, const Operation& op);
    static int triggerIndex(
        const TTAMachine::FunctionUnit& fu, const Operation& op);
    static unsigned findWidestOperand(
        const TTAMachine::Machine& machine, bool vector) ;
    static unsigned numberOfRegisters(
        const TTAMachine::Machine& machine, unsigned width);

private:
    MachineInfo();
};

#endif
