/**
 * @file SimpleGuardAllocatorCore.hh
 *
 * Declaration of SimpleGuardAllocatorCore class.
 * 
 * Stupid guard allocator. does not use more than one guard register,
 * Does not do any if-conversion etc.
 *
 * @author Heikki Kultala 2007 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_GUARD_ALLOCATOR_CORE_HH
#define TTA_SIMPLE_GUARD_ALLOCATOR_CORE_HH

#include "Exception.hh"

namespace TTAProgram {
    class Move;
    class Procedure;
}

namespace TTAMachine {
    class Machine;
}

class RegisterMap;
class InterPassData;

class SimpleGuardAllocatorCore {

//public:
    SimpleGuardAllocatorCore();
    virtual ~SimpleGuardAllocatorCore();

public:
    static void allocateGuards(
        TTAProgram::Procedure& proc, const TTAMachine::Machine& mach, 
        InterPassData& interPassData) 
        throw (Exception);

private:
    static void allocateGuard(
        TTAProgram::Move& move, RegisterMap& regMap, 
        const TTAMachine::Machine& mach) throw (Exception);
};

#endif
