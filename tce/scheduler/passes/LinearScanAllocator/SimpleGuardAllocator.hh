/**
 * @file SimpleGuardAllocator.hh
 *
 * Declaration of SimpleGuardAllocator class.
 * 
 * Stupid guard allocator
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_GUARD_ALLOCATOR_HH
#define TTA_SIMPLE_GUARD_ALLOCATOR_HH

#include "StartableSchedulerModule.hh"

#include "Exception.hh"

namespace TTAProgram {
    class Move;
}

class RegisterMap;

class SimpleGuardAllocator : public StartableSchedulerModule {

public:
    SimpleGuardAllocator();
    virtual ~SimpleGuardAllocator();

    virtual void start()        
        throw (Exception);

    bool needsProgram() const { return true; }
    bool needsTarget() const { return true; }
    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

private:
    void allocateGuard(TTAProgram::Move& move, RegisterMap& regMap);
};

SCHEDULER_PASS(SimpleGuardAllocator)

#endif
