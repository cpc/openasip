/**
 * @file SimpleRegisterAllocator.hh
 *
 * Declaration of SimpleRegisterAllocator class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
