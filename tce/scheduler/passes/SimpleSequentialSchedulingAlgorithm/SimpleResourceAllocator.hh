/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file SimpleResourceAllocator.hh
 *
 * Declaration of SimpleResourceAllocator class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
