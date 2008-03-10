/**
 * @file SequentialFirstFitResourceAllocator.hh
 *
 * Declaration of SequentialFirstFitResourceAllocator class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PORTED_RESOURCE_ALLOCATOR_HH
#define TTA_PORTED_RESOURCE_ALLOCATOR_HH

#include <set>
#include <list>
#include "StartableSchedulerModule.hh"
#include "Exception.hh"
#include "SimpleResourceManager.hh"

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
 * Schedules the given program for the target architecture without any
 * parallelisation or other optimisations. Simply maps the program on to
 * the target machine.
 */
class SequentialFirstFitResourceAllocator : public StartableSchedulerModule {
public:
    SequentialFirstFitResourceAllocator();
    virtual ~SequentialFirstFitResourceAllocator();

    virtual void start()
        throw (Exception);
    virtual bool needsTarget() const;
    virtual bool needsProgram() const;

private:
    typedef std::list<MoveNode*> MoveNodeList;

    void findReservedRegisters();
    void assignResources();
    MoveNodeList createMoveNodes();
    SimpleResourceManager* resMan_;
};

#endif
