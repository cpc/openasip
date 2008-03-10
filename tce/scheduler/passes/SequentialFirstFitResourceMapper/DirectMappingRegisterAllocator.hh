/**
 * @file DirectMappingRegisterAllocator.hh
 *
 * Declaration of DirectMappingRegisterAllocator class.
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
