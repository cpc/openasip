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
 * @file SimpleRegisterAllocator.hh
 *
 * Declaration of SimpleRegisterAllocator class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_REGISTER_ALLOCATOR_HH
#define TTA_SIMPLE_REGISTER_ALLOCATOR_HH

#include <string>
#include <vector>
#include <map>

#include "StartableSchedulerModule.hh"

class RegisterBookkeeper;
class UniversalMachine;

namespace TTAProgram {
    class Terminal;
    class TerminalRegister;
    class CodeSnippet;
    class Instruction;
}

namespace TTAMachine {
    class Machine;
    class RegisterFile;
}

/**
 * A helper class that keeps a record of allocated registers. Provides
 * a service to assign "sequential variables" to real registers in the
 * machine.
 */
class RegisterBookkeeper {
public:
    RegisterBookkeeper();
    virtual ~RegisterBookkeeper();

    void reserveRegister(
        TTAProgram::TerminalRegister& reg, std::string var);
    bool isReserved(TTAProgram::TerminalRegister& reg) const;
    bool isReserved(std::string var) const;
    TTAProgram::TerminalRegister& regForVar(const std::string& var) const;
    void freeRegister(TTAProgram::TerminalRegister& reg);
    int numReservedRegisters() const;

private:
    typedef std::map<TTAProgram::TerminalRegister*, std::string> RegMap;
    RegMap reservedRegisters_;
};


/**
 * Performs register allocation and assignment.
 */
class SimpleRegisterAllocator : public StartableSchedulerModule {
public:
    SimpleRegisterAllocator();
    virtual ~SimpleRegisterAllocator();

    virtual void start()
        throw (Exception);
    virtual bool needsTarget() const;
    virtual bool needsProgram() const;

private:
    virtual void allocateRegisters(TTAProgram::Instruction& ins);
    void allocateGuards(TTAProgram::Instruction& ins);
    std::string getVariableName(
        const TTAMachine::RegisterFile* regFile, int index) const;
    TTAProgram::TerminalRegister& regForVar(const std::string& var) const;
    TTAProgram::TerminalRegister* findFreeRegister(
        const std::string& var) const;

    RegisterBookkeeper regBookkeeper_;
    UniversalMachine* um_;
    std::vector<TTAProgram::TerminalRegister*> registers_;
};

SCHEDULER_PASS(SimpleRegisterAllocator)

#endif
