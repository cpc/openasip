#include "BFOptimization.hh"

namespace TTAMachine {
    class Guard;
}

class BFEarlyGuardBypass: public BFOptimization {

public:
    BFEarlyGuardBypass(BF2Scheduler& sched, MoveNode& mn) :
        BFOptimization(sched), mn_(mn) {}
        
    void undoOnlyMe() override;
    bool operator()() override;
private:
    MoveNode& mn_;
    MoveNode* src_;
    const TTAMachine::Guard* originalGuard_;
};
