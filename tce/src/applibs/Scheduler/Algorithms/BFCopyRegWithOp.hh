#ifndef BF_COPY_REG_WITH_OP_HH
#define BF_COPY_REG_WITH_OP_HH

#include "BFRegCopy.hh"
#include "TCEString.hh"
#include "MachineConnectivityCheck.hh"

class BFCopyRegWithOp : public BFRegCopy {
public:
    BFCopyRegWithOp(BF2Scheduler& sched, MoveNode& mn, int lc,
                    MachineConnectivityCheck::FUSet candidateFUs) :
        BFRegCopy(sched,mn,lc), candidateFUs_(candidateFUs) {}
    virtual ~BFCopyRegWithOp() {}
protected:
    bool splitMove(BasicBlockNode& bbn);
    void undoSplit();
    ProgramOperationPtr pop_;
    MachineConnectivityCheck::FUSet candidateFUs_;
};

#endif
