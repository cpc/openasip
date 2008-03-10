/**
 * @file SimpleGuardAllocatorPass.hh
 *
 * Declaration of SimpleGuardAllocator class.
 * 
 * Stupid guard allocator
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_GUARD_ALLOCATOR_PASS_HH
#define TTA_SIMPLE_GUARD_ALLOCATOR_PASS_HH

#include "StartableSchedulerModule.hh"

#include "Exception.hh"

class SimpleGuardAllocatorPass : public StartableSchedulerModule {

public:
    SimpleGuardAllocatorPass();
    virtual ~SimpleGuardAllocatorPass();

    virtual void start()        
        throw (Exception);

    bool needsProgram() const { return true; }
    bool needsTarget() const { return true; }
};


#endif
