/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ResourceManager.hh
 *
 * Declaration of ResourceManager class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_MANAGER_HH
#define TTA_RESOURCE_MANAGER_HH

namespace TTAMachine {
    class Machine;
    class FunctionUnit;
    class Bus;
    class ImmediateUnit;
}

namespace TTAProgram {
    class Instruction;
}

class MoveNode;
class MoveNodeSet;

/**
 * Base interface for resource managers.
 */
class ResourceManager {
public:
    ResourceManager(const TTAMachine::Machine& machine);
    virtual ~ResourceManager();

    virtual bool canAssign(
        int cycle, MoveNode& node,
        const TTAMachine::Bus* bus = nullptr,
        const TTAMachine::FunctionUnit* srcFU = nullptr,
        const TTAMachine::FunctionUnit* dstFU = nullptr,
        int immWriteCycle = -1,
        const TTAMachine::ImmediateUnit* immu = nullptr,
        int immRegIndex = -1) const = 0;
    virtual void assign(
        int cycle, MoveNode& node,
        const TTAMachine::Bus* bus = nullptr,
        const TTAMachine::FunctionUnit* srcFU = nullptr,
        const TTAMachine::FunctionUnit* dstFU = nullptr,
        int immWriteCycle = -1,
        const TTAMachine::ImmediateUnit* immu = nullptr,
        int immRegIndex = -1) = 0;
    virtual void unassign(MoveNode& node) = 0;
    virtual int earliestCycle(
        MoveNode& node,
        const TTAMachine::Bus* bus = nullptr,
        const TTAMachine::FunctionUnit* srcFU = nullptr,
        const TTAMachine::FunctionUnit* dstFU = nullptr,
        int immWriteCycle = -1,
        const TTAMachine::ImmediateUnit* immu = nullptr,
        int immRegIndex = -1) const = 0;
    virtual int earliestCycle(
        int cycle, MoveNode& node,
        const TTAMachine::Bus* bus = nullptr,
        const TTAMachine::FunctionUnit* srcFU = nullptr,
        const TTAMachine::FunctionUnit* dstFU = nullptr,
        int immWriteCycle = -1,
        const TTAMachine::ImmediateUnit* immu = nullptr,
        int immRegIndex = -1) const = 0;
    virtual int latestCycle(
        MoveNode& node,
        const TTAMachine::Bus* bus = nullptr,
        const TTAMachine::FunctionUnit* srcFU = nullptr,
        const TTAMachine::FunctionUnit* dstFU = nullptr,
        int immWriteCycle = -1,
        const TTAMachine::ImmediateUnit* immu = nullptr,
        int immRegIndex = -1) const = 0;
    virtual int latestCycle(
        int cycle, MoveNode& node,
        const TTAMachine::Bus* bus = nullptr,
        const TTAMachine::FunctionUnit* srcFU = nullptr,
        const TTAMachine::FunctionUnit* dstFU = nullptr,
        int immWriteCycle = -1,
        const TTAMachine::ImmediateUnit* immu = nullptr,
        int immRegIndex = -1) const = 0;
    virtual bool supportsExternalAssignments() const = 0;
    virtual int largestCycle() const = 0;
    virtual int smallestCycle() const = 0;
    const TTAMachine::Machine& machine() const;
    virtual TTAProgram::Instruction* instruction(int cycle) const =0;
private:
    /// Target machine.
    const TTAMachine::Machine* machine_;
};

//#define DEBUG_RM

#ifdef DEBUG_RM
#include "Application.hh"
#define debugLogRM(__X) debugLog(__X)
#else
#define debugLogRM(__X)
#endif

#endif
