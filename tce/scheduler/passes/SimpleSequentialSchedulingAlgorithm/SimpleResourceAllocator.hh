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
