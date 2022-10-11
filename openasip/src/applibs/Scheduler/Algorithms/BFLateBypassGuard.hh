#ifndef BF_LATE_BYPASS_GUARD_HH
#define BF_LATE_BYPASS_GUARD_HH

#include "BFLateBypass.hh"
namespace TTAMachine {
    class Guard;
}
class BFLateBypassGuard: public BFLateBypass {
public:
    BFLateBypassGuard(BF2Scheduler& sched, MoveNode& src, MoveNode& dst, int lc) :
        BFLateBypass(sched, src, dst, lc) {}

    virtual bool operator()();
    virtual void undoOnlyMe();
private:
    const TTAMachine::Guard* originalGuard_;
};

#endif
