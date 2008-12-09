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
 * @file SimpleResourceAllocator.hh
 *
 * Declaration of SimpleResourceAllocator class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_RESOURCE_ALLOCATOR_HH
#define TTA_SIMPLE_RESOURCE_ALLOCATOR_HH

#include <set>
#include "StartableSchedulerModule.hh"
#include "Exception.hh"

namespace TTAProgram {
    class Program;
    class Move;
    class Instruction;
    class TerminalRegister;
}

namespace TTAMachine {
    class FunctionUnit;
    class Bus;
    class RegisterFile;
    class Port;
}

class Operation;

/**
 * Keeps track of free and reserved resources in the machine, and
 * assigns and helps assigning resources to moves. Provides
 * information whether a certain move can be carried out using
 * currently assigned resources.
 */
class SimpleResourceManager {
public:

    SimpleResourceManager(const TTAMachine::Machine& mach);
    virtual ~SimpleResourceManager();

    void reserveRegister(TTAProgram::TerminalRegister& reg);
    const TTAMachine::FunctionUnit& reserveFunctionUnit(
        Operation& op);
    void assignBuses(TTAProgram::Instruction& ins);

    struct TempRegister {
        TTAMachine::Port* inPort;
        TTAMachine::Port* outPort;
        int index;
    };

private:

    TTAMachine::Bus& findBus(
        TTAProgram::Move& move,
        TTAProgram::Instruction& parent);

    TempRegister findTempRegConnectedTo(
        std::set<TTAMachine::Bus*>& srcBuses,
        std::set<TTAMachine::Bus*>& dstBuses);

    int findFreeIndex(TTAMachine::RegisterFile& rf);

    const TTAMachine::Machine& mach_;
    std::vector<TTAProgram::TerminalRegister*> reservedRegs_;
};

/**
 * Schedules the given program for the target architecture without any
 * parallelisation or other optimisations. Simply maps the program on to
 * the target machine.
 */
class SimpleResourceAllocator : public StartableSchedulerModule {
public:
    SimpleResourceAllocator();
    virtual ~SimpleResourceAllocator();

    virtual void start()
        throw (Exception);
    virtual bool needsTarget() const;
    virtual bool needsProgram() const;

private:
    void findReservedRegisters();
    void assignResources(
        TTAProgram::Program& prog, const TTAMachine::Machine& mach);
    SimpleResourceManager* resMan_;
};

#endif
