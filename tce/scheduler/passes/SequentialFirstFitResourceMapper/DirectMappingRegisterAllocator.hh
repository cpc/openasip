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
 * @file DirectMappingRegisterAllocator.hh
 *
 * Declaration of DirectMappingRegisterAllocator class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme-no.spam-tut.fi)
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
    class Move;
}

namespace TTAMachine {
    class Machine;
    class RegisterFile;
    class RegisterGuard;
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
class DirectMappingRegisterAllocator : public StartableSchedulerModule {
public:
    DirectMappingRegisterAllocator();
    virtual ~DirectMappingRegisterAllocator();

    virtual void start()
        throw (Exception);
    virtual bool needsTarget() const;
    virtual bool needsProgram() const;
    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

private:
    void addSrcAnnotation(const std::string& var, TTAProgram::Move& move);
    void addDstAnnotation(const std::string& var, TTAProgram::Move& move);
    void createStackInitialization();
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
    bool hasGuard(TTAMachine::RegisterFile& rf, int index) const;
    std::vector<TTAMachine::RegisterGuard*> guards_;
};

#endif
