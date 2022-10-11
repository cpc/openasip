#include "BFLateBypassGuard.hh"
#include "BFScheduleBU.hh"
#include "BFScheduleExact.hh"

#include "SimpleResourceManager.hh"
#include "DataDependenceGraph.hh"
#include "MoveNode.hh"
#include "Move.hh"

#include "Bus.hh"
#include "BUMoveNodeSelector.hh"
#include "Terminal.hh"
#include "FUPort.hh"
#include "Guard.hh"
#include "Operation.hh"
#include "MoveGuard.hh"

bool
BFLateBypassGuard::operator()() {

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tPerforming late bypass guard: " << dst_.toString() << std::endl;
#endif

    // Only allow guard bypass if it can save clock cycles on guard path.
    if (dst_.cycle() > lc_ + dst_.guardLatency()-1) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tAborting guard bypass due too long guard distance" << std::endl;
#endif

        return false;
    }
    
    if (!src_.isSourceOperation()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tsrc src not op: "<< src_.toString() << std::endl;
#endif
        return false;
    }


    if (dst_.isSourceOperation()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tsrc not constant: " << dst_.toString() << std::endl;
#endif
        return false;
    }


    ProgramOperation &po = src_.sourceOperation();
    const Operation& op = po.operation();

    // TODO: might need set of these for annotations.
    const TTAMachine::FunctionUnit* fu = NULL;
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& mn = po.inputMove(i);
        if (mn.isScheduled()) {
            fu = &mn.move().destination().functionUnit();
        }
        // TODO: fu annotations can forbid this?
    }
    for (int i = 0; i < po.outputMoveCount(); i++) {
        MoveNode& mn = po.outputMove(i);
        if (mn.isScheduled()) {
            fu = &mn.move().source().functionUnit();
        }
        // TODO: fu annotations can forbid this?
    }

    if (dst_.move().isUnconditional()) {
        ddg().writeToDotFile("guard_edge_dest_uncond.dot");
    }
    assert(!dst_.move().isUnconditional());
    const TTAMachine::Bus& bus = dst_.move().bus();
    for (int i = 0; i < bus.guardCount(); i++) {
        
        TTAMachine::Guard* guard = bus.guard(i);
        TTAMachine::PortGuard* pg = 
            dynamic_cast<TTAMachine::PortGuard*>(guard);
        if (pg == NULL) {
            continue;
        }
        if (pg->isInverted() != dst_.move().guard().isInverted()) {
            continue;
        }
        const TTAMachine::FUPort* port = pg->port();
        TTAMachine::FunctionUnit* gfu = port->parentUnit();
        if (gfu->hasOperation(op.name()) &&
            po.outputIndexFromGuard(*pg) ==
            src_.move().source().operationIndex() &&
            (fu == NULL || fu == gfu)) {
            
            if (!po.isLegalFU(*gfu)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\t\tFU: " << gfu->name() << " not allowed for operation: " << po.toString() << std::endl;
#endif
                continue;
            }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\t\tFound suitable guard & FU" << std::endl;
#endif
            originalCycle_ = dst_.cycle();
            originalBus_ = &dst_.move().bus();
            originalGuard_ = &dst_.move().guard().guard();

            unassign(dst_, false);
            
            // TODO: backup old guard.
            dst_.move().setGuard(new TTAProgram::MoveGuard(*guard));

            ddg().guardConverted(src_, dst_);

            BFOptimization* sched = new BFScheduleExact(sched_, dst_, originalCycle_);
            if ((*sched)()) {
                postChildren_.push(sched);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\tGuard conversion ok: " << dst_.toString() << std::endl;
#endif
                return true;
            } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\tGuard conversion sched fail.: " << dst_.toString() << std::endl;
#endif
                delete sched;

                ddg().guardRestored(src_, dst_);

                dst_.move().setGuard(new TTAProgram::MoveGuard(*originalGuard_));
                dst_.move().setBus(*originalBus_);

                assert(canAssign(originalCycle_, dst_));
                assign(originalCycle_, dst_);
            }
        }
    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tDid not find a bus with suitable guard?" << std::endl;
#endif
    return false;
}




void
BFLateBypassGuard::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tUndoing guard conversion: " << dst_.toString() << std::endl;
#endif

    ddg().guardRestored(src_, dst_);

    dst_.move().setGuard(new TTAProgram::MoveGuard(*originalGuard_));

    dst_.move().setBus(*originalBus_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tReassigning move: " << dst_.toString() << " to cycle: "
	      << originalCycle_ << " bus: " << originalBus_->name() << std::endl;
#endif
    assert(canAssign(originalCycle_, dst_));
    assign(originalCycle_, dst_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tReassigned move:" << dst_.toString() << std::endl;
#endif
}
