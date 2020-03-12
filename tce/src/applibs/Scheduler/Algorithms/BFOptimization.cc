/*
    Copyright (c) 2002-2014 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */

/**
 * @file BFOptimization.cc
 *
 * Definition of BFOptimization class.
 *
 * Base class for all optimizations and scheudling operations the
 * BF2 instruction scheduler can perform. Contains an undo-mechanism
 * To undo everything, and contains handling for scheudling mirror move
 * to prolog/epilog in case of loop scheduling.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFOptimization.hh"
#include "BF2Scheduler.hh"
#include "SimpleResourceManager.hh"
#include "Move.hh"
#include "MoveGuard.hh"
#include "CodeGenerator.hh"
#include "Guard.hh"
#include "RegisterFile.hh"
#include "MoveNodeDuplicator.hh"
#include "Terminal.hh"
#include "BasicBlockScheduler.hh"
#include "Operation.hh"
#include "UniversalMachine.hh"
#include "ControlUnit.hh"
#include "Instruction.hh"
#include "Bus.hh"
#include "BF2ScheduleFront.hh"
#include "MachineConnectivityCheck.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "TerminalImmediate.hh"

//#define DEBUG_BUBBLEFISH_SCHEDULER
//#define DEBUG_LOOP_SCHEDULER
//#define CHECK_PROLOG_DDG

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
#include "POMDisassembler.hh"
#define DEBUG_LOOP_SCHEDULER
#endif

DataDependenceGraph& BFOptimization::ddg() { return sched_.ddg(); }
DataDependenceGraph* BFOptimization::rootDDG() {
    return static_cast<DataDependenceGraph*>(ddg().rootGraph());
}
const DataDependenceGraph& BFOptimization::ddg() const { return sched_.ddg();}
DataDependenceGraph* BFOptimization::prologDDG() { return sched_.prologDDG();}
SimpleResourceManager& BFOptimization::rm() const { return sched_.rm(); }
SimpleResourceManager* BFOptimization::prologRM() const {
    return sched_.prologRM();
}
BUMoveNodeSelector& BFOptimization::selector() { return sched_.selector(); }
const TTAMachine::Machine& BFOptimization::targetMachine() const  {
    return sched_.targetMachine();
}

unsigned int BFOptimization::ii() const { return rm().initiationInterval(); }

MoveNodeDuplicator& BFOptimization::duplicator() const {
    return sched_.duplicator();
}

bool BFOptimization::usePrologMove(const MoveNode& mn) {
    if (prologRM() == NULL) {
        return false;
    }
    // TODO: better way to no prolog move for lbufc op
    if (mn.move().isControlFlowMove() &&
        !mn.move().isJump()) {
        return false;
    }
    return true;
}

bool BFOptimization::assign(int cycle, MoveNode& mn,
                            const TTAMachine::Bus* bus,
                            const TTAMachine::FunctionUnit* srcFU,
                            const TTAMachine::FunctionUnit* dstFU,
                            const TTAMachine::Bus* prologBus,
                            int immWriteCycle,
                            int prologImmWriteCycle,
                            const TTAMachine::ImmediateUnit* immu,
                            int immRegIndex,
                            bool ignoreGWC) {

    bool createdPrologCopy = false;
#ifdef DEBUG_LOOP_SCHEDULER
    if (ii() != 0) {
        std::cerr << "\t\t\t\tAssigning in loop sched: " << mn.toString()
                  << " to cycle: " << cycle << " imm write cycle: "
                  << immWriteCycle << std::endl;
    }
#else
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\t\tAssigning mn: " << mn.toString()
              << " imm write cycle: " << immWriteCycle << std::endl;
#endif
#endif
    bool usePrologMN = usePrologMove(mn);
    MoveNode* prologEpilogMN = NULL;
    // TODO: why trying to assign all to prolog fails???
    if (usePrologMN) {
        auto a = createCopyForPrologEpilog(mn);
        prologEpilogMN = a.first;
        createdPrologCopy = a.second;
    }

    // TODO: assert if too early due the integrated epilog and guard.
    if (!(addJumpGuardIfNeeded(mn, cycle, ignoreGWC))) {
        std::cerr << "Adding jump guard failed for node: "
                  << mn.toString() << std::endl;
        ddg().writeToDotFile("adding_jump_guard_fail.dot");
        assert(false);
    }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    if (!rm().canAssign(
	    cycle, mn, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex)) {
        std::cerr << "cannot assign move in assign: " << mn.toString()
                  << " cycle: " << cycle << std::endl;
        TTAProgram::Move& m = mn.move();
        std::cerr << "set bus: " << m.bus().name() << std::endl;
        if (bus != NULL) {
            std::cerr << "bus: " << bus->name() << std::endl;
        }
        if (srcFU != NULL) {
            std::cerr << "src FU: " << srcFU->name() << std::endl;
        }
        if (dstFU != NULL) {
            std::cerr << "dst FU: " << dstFU->name() << std::endl;
        }

        std::cerr << "annotations:" << std::endl;
        for (int i = 0 ; i < m.annotationCount(); i++) {
            const TTAProgram::ProgramAnnotation& anno = m.annotation(i);
            std::cerr << "\thas anno, id: " << anno.id()
                      << " data: " << anno.stringValue() << std::endl;
        }
        if (mn.isSourceOperation()) {
            std::cerr << "Source op: " << mn.sourceOperation().toString()
                      << std::endl;
        }
        if (mn.isDestinationOperation()) {
            std::cerr << "Destination op: "
                      << mn.destinationOperation().toString() << std::endl;
        }
        ddg().writeToDotFile("cannot_assign_on_assign.dot");
        assert(false);
    }
#endif

    rm().assign(
	cycle, mn, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\t\tAssigned mn: " << mn.toString() << std::endl;
#endif
    assert(bus == NULL || bus == &mn.move().bus());
    if (usePrologMN) {
        assignCopyToPrologEpilog(cycle, *prologEpilogMN, mn, prologBus,
                                 prologImmWriteCycle);
#ifdef CHECK_PROLOG_DDG
        checkPrologDDG(*prologEpilogMN);
#endif
    }
    return createdPrologCopy;
}

void BFOptimization::unassign(MoveNode& mn, bool disposePrologCopy) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\t\tUnassigning mn: " << mn.toString()
              << "dispose: " << disposePrologCopy << std::endl;
#endif
    int cycle = mn.cycle();
    rm().unassign(mn);
    bool usePrologMN = usePrologMove(mn);
    if (usePrologMN) {
        unassignCopyFromPrologEpilog(mn, disposePrologCopy);
    }
    unsetJumpGuardIfNeeded(mn, cycle);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\t\tUnassignined mn: " << mn.toString() << std::endl;
#endif
}

int BFOptimization::rmEC(int cycle, MoveNode& mn,
                         const TTAMachine::Bus* bus,
                         const TTAMachine::FunctionUnit* srcFU,
                         const TTAMachine::FunctionUnit* dstFU,
                         const TTAMachine::Bus* prologBus,
                         int immWriteCycle,
                         int prologImmWriteCycle,
                         const TTAMachine::ImmediateUnit* immu,
                         int immRegIndex) {
#ifdef DEBUG_LOOP_SCHEDULER
    if (ii()) {
        std::cerr << "\t\t\t\t\tbfopt::rmec called, cycle: " << cycle
                  << " mn: " << mn.toString() <<
            std::endl;

    }
#else
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\t\tCalling rmec for: " << mn.toString()
              << " cycle: " << cycle <<  "dispose: "
              << disposePrologCopy << std::endl;
#endif
#endif
    if (!ii()) {
        return rm().earliestCycle(
	    cycle, mn, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
    }
    // cannot add guard if already has guard. so limit cycle to 2nd round.
    if (!mn.move().isUnconditional() && needJumpGuard(mn, cycle)) {
        cycle = ii();
    }

    MoveNode* prologMN = nullptr;
    bool createdCopy = false;
    bool usePrologMN = usePrologMove(mn);
    if (usePrologMN) {
        auto a = duplicator().duplicateMoveNode(mn, false, false);
        prologMN = a.first;
        createdCopy = a.second;
    }

    bool setGuard = false;
    int ec = rm().earliestCycle(
        cycle, mn, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
    if (ec == INT_MAX || ec == -1) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\t\trmec over with -1" << std::endl;
#endif
        if (createdCopy) {
            duplicator().disposeMoveNode(prologMN);
        }
        return ec;
    }

    while (ec < (signed)(2*ii())) {
        if (needJumpGuard(mn, ec)) {
            if (ec < jumpGuardAvailableCycle(mn)) {
                ec++;
                continue;
            }
            if (!setGuard) {
                assert(mn.move().isUnconditional());
                setJumpGuard(mn);
                setGuard = true;
                continue;
            }
        } else { // no longer need jump guard.
            if (setGuard) {
                assert(!mn.move().isUnconditional());
                unsetJumpGuard(mn);
                setGuard = false;
                continue;
            }
        }

        if (!setGuard) {
            ec = rm().earliestCycle(
		ec, mn, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
            if (ec == -1) {
                break;
            }
        } else {
            int newEC = rm().earliestCycle(
                ec, mn, bus, srcFU, dstFU, immWriteCycle);
            // if set guard, guard may reduce too much..
            if (newEC > ec) {
                ec = std::min(newEC, (signed)(ii()));
                continue;
            }
            if (newEC == -1) {
                ec = -1;
                break;
            }
        }
        if (ec == INT_MAX) {
            break;
        }


        if (usePrologMN) {
            bool assignedMN = false;
            auto mySrcFU = srcFU;
            auto myDstFU = dstFU;
            auto myImmu = immu;
            auto myImmReg = immRegIndex;
            if (hasAmbiguousResources(mn)) {
                rm().assign(ec, mn, bus, srcFU, dstFU, immWriteCycle, immu,
			    immRegIndex);
                mySrcFU = sourceFU(mn);
                myDstFU = destinationFU(mn);
                if (mn.isSourceImmediateRegister()) {
                    myImmReg = mn.move().source().index();
                    myImmu = &mn.move().source().immediateUnit();
                }
                assignedMN = true;
            }
            bool ok = prologRM()->canAssign(
                ec + BF2Scheduler::PROLOG_CYCLE_BIAS, *prologMN, prologBus,
                mySrcFU, myDstFU, prologImmWriteCycle, myImmu, myImmReg);
            if (assignedMN) {
                rm().unassign(mn);
            }
            if (!ok) {
                ec++;
                continue;
            } else {
                break;
            }
        } else { // no prolog
            if (createdCopy) {
                duplicator().disposeMoveNode(prologMN);
            }
            if (setGuard) {
                unsetJumpGuard(mn);
            }
            return ec;
        }
    }
    if (createdCopy) {
        duplicator().disposeMoveNode(prologMN);
    }
    if (setGuard) {
        unsetJumpGuard(mn);
    }
#ifdef DEBUG_LOOP_SCHEDULER
    std::cerr << "\t\t\t\t\trmec over" << std::endl;
#endif
    return ec;
}

int BFOptimization::rmLC(int cycle, MoveNode& mn,
                         const TTAMachine::Bus* bus,
                         const TTAMachine::FunctionUnit* srcFU,
                         const TTAMachine::FunctionUnit* dstFU,
                         const TTAMachine::Bus* prologBus,
                         int immWriteCycle,
                         int prologImmWriteCycle,
                         const TTAMachine::ImmediateUnit* immu,
                         int immRegIndex) {
#ifdef DEBUG_LOOP_SCHEDULER
    if (ii()) {
        std::cerr << "\t\t\t\t\tbfopt::rmlc called, cycle: " << cycle
                  << " mn: " << mn.toString() << std::endl;
    }
#else
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\t\tbfopt::rmlc called, cycle: " << cycle
              << " mn: " << mn.toString() << std::endl;
#endif
#endif
    int lc = cycle;
    bool setGuard = false;
    MoveNode* prologMN = NULL;
    bool createdCopy = false;
    bool usePrologMN = usePrologMove(mn);
    if (usePrologMN) {
        auto a = duplicator().duplicateMoveNode(mn, false, false);
        prologMN = a.first;
        createdCopy = a.second;
    }

    while (lc >= 0) {
        lc = rm().latestCycle(lc,mn, bus, srcFU, dstFU, immWriteCycle);
#ifdef DEBUG_LOOP_SCHEDULER
        if (ii()) {
            std::cerr << "\t\t\t\t\tgot lc from rm: " << lc << std::endl;
        }
#else
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\t\tgot lc from rm: " << lc << std::endl;
#endif
#endif
        if (lc == -1) {
            break;
        }
        if (needJumpGuard(mn, lc)) {
#ifdef DEBUG_LOOP_SCHEDULER
            std::cerr << "\t\t\t\t\tNeed jump guard for mn: "
                      << mn.toString() << " cycle: " << lc << std::endl;
#endif
            if (lc < jumpGuardAvailableCycle(mn)) {
#ifdef DEBUG_LOOP_SCHEDULER
                std::cerr << "\t\t\t\t\tjump guard not yet available so "
                          << "cannot schedule this ever" << std::endl;
#endif
                if (createdCopy) {
                    duplicator().disposeMoveNode(prologMN);
                }
                if (setGuard) {
                    unsetJumpGuard(mn);
                }
                return -1;
            }
            if (!setGuard) {
                // cannot add guard if already has guard.
                if (!mn.move().isUnconditional()) {
                    if (createdCopy) {
                        duplicator().disposeMoveNode(prologMN);
                    }
                    return -1;
                }
                setJumpGuard(mn);
                setGuard = true;
                continue;
            }
        }
        if (usePrologMN) {
            // then check that this can be assigned to prolog/epilog
            // need to use the correct FU - only way to get it is to
            // assign loop move first
            bool assignedMN = false;
            auto mySrcFU = srcFU;
            auto myDstFU = dstFU;
            auto myImmu = immu;
            auto myImmReg = immRegIndex;

            if (hasAmbiguousResources(mn)) {
                rm().assign(lc, mn, bus, srcFU, dstFU, immWriteCycle);
                mySrcFU = sourceFU(mn);
                myDstFU = destinationFU(mn);
                if (mn.isSourceImmediateRegister()) {
                    myImmReg = mn.move().source().index();
                    myImmu = &mn.move().source().immediateUnit();
                }
                assignedMN = true;
            }
            bool ok = prologRM()->canAssign(
                lc + BF2Scheduler::PROLOG_CYCLE_BIAS, *prologMN, prologBus,
                mySrcFU, myDstFU, prologImmWriteCycle, myImmu, myImmReg);
            if (assignedMN) {
                rm().unassign(mn);
            }
            if (!ok) {
#ifdef DEBUG_LOOP_SCHEDULER
                std::cerr << "prolog RM cannot assign to cycle: "
                          << lc << std::endl;
                std::cerr << "instr in prolog rm: " <<
                    POMDisassembler::disassemble(
                        *prologRM()->instruction(
                            lc + BF2Scheduler::PROLOG_CYCLE_BIAS));
#endif
                lc--;
                continue;
            } else {
                break;
            }
        } else { // not in a prolog.
            if (createdCopy) {
                duplicator().disposeMoveNode(prologMN);
            }
            return lc;
        }
    }
    if (createdCopy) {
        duplicator().disposeMoveNode(prologMN);
    }
    if (setGuard) {
        unsetJumpGuard(mn);
    }
    return lc;
}

bool BFOptimization::canAssign(int cycle, MoveNode& mn,
                               const TTAMachine::Bus* bus,
                               const TTAMachine::FunctionUnit* srcFU,
                               const TTAMachine::FunctionUnit* dstFU,
                               const TTAMachine::Bus* prologBus,
                               int immWriteCycle,
                               int prologImmWriteCycle,
                               const TTAMachine::ImmediateUnit* immu,
                               int immRegIndex,
                               bool ignoreGuardWriteNode) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    if (ii()) {
        std::cerr << "\t\t\t\t\tCalling BFOpt::canassign for: "
                  << mn.toString() << " cycle: " << cycle << std::endl;
    } else {
        std::cerr << "\t\t\t\t\tCalling BFOpt::canassign for: "
                  << mn.toString() << " cycle: " << cycle << std::endl;
    }
    if (bus) {
        std::cerr << "\t\t\t\t\t\tBus: " << bus->name() << std::endl;
    }
    if (prologBus) {
        std::cerr << "\t\t\t\t\t\tProlog bus: " << prologBus->name()
                  << std::endl;
    }
    if (srcFU) {
        std::cerr << "\t\t\t\t\t\tSrc FU: " << srcFU->name() << std::endl;
    }
    if (dstFU) {
        std::cerr << "\t\t\t\t\t\tDst FU: " << dstFU->name() << std::endl;
    }
    if (mn.isDestinationOperation()) {
        std::cerr << "\t\t\t\t\t\tDst po: "
                  << mn.destinationOperation().toString() << std::endl;
    }
#endif
    bool addGuard = needJumpGuard(mn, cycle);
    if (addGuard) {
#ifdef DEBUG_LOOP_SCHEDULER
        std::cerr << "\t\t\t\t\t\tNeed jump guard, cycle: " << cycle
                  << "ii: " << ii() << std::endl;
#endif
        // cannot add guard if already has guard.
        if (!mn.move().isUnconditional()) {
#ifdef DEBUG_LOOP_SCHEDULER
            std::cerr << "\t\t\t\t\tcanassin over with uncond" << std::endl;
#endif
            return false;
        }
        if (cycle < jumpGuardAvailableCycle(mn) && !ignoreGuardWriteNode) {
#ifdef DEBUG_LOOP_SCHEDULER
            std::cerr << "\t\t\t\t\tjump guard not yet available so"
                      << " cannot scheudle this." << std::endl;
            std::cerr << "\t\t\t\t\t\tAvailable on cycle: "
                      << jumpGuardAvailableCycle(mn) << std::endl;
#endif
            return false;
        }
        setJumpGuard(mn);
    }
    bool ok =  rm().canAssign(
        cycle, mn, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex);
#ifdef DEBUG_LOOP_SCHEDULER
    if (ii()) {
        std::cerr << "\t\t\t\t\t for this: " << ok << " , what about prolog?"
                  << std::endl;
    }
#endif

    // then test assigning to prolog
    bool usePrologMN = usePrologMove(mn);
    if (ok && usePrologMN) {
        // may not have the added jump guard when duplicating.
        if (addGuard) unsetJumpGuard(mn);
        auto a = duplicator().duplicateMoveNode(mn, false, false);
        if (addGuard) setJumpGuard(mn);
        MoveNode* prologMN = a.first;
#ifdef DEBUG_LOOP_SCHEDULER
        std::cerr << "\t\t\t\t\t\tProlog mn: " << prologMN->toString()
                  << std::endl;
#endif
        bool assignedMN = false;
        if (hasAmbiguousResources(mn)) {
            rm().assign(cycle, mn, bus, srcFU, dstFU, immWriteCycle, immu,
                        immRegIndex);
            srcFU = sourceFU(mn);
            dstFU = destinationFU(mn);
            if (mn.isSourceImmediateRegister()) {
                immRegIndex = mn.move().source().index();
                immu = &mn.move().source().immediateUnit();
            }
            assignedMN = true;
        }
        ok &= prologRM()->canAssign(
            BF2Scheduler::PROLOG_CYCLE_BIAS + cycle, *prologMN, prologBus,
            srcFU, dstFU, prologImmWriteCycle, immu, immRegIndex);
        if (assignedMN) {
            rm().unassign(mn);
        }
        if (a.second) { //disposePrologCopy) {
            duplicator().disposeMoveNode(prologMN);
        }
    }
    unsetJumpGuardIfNeeded(mn, cycle);

#ifdef DEBUG_LOOP_SCHEDULER
    if (ii()) {
        std::cerr << "\t\t\t\t\tcanassin over:" << ok << std::endl;
    }
#else
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\t\tcanassin over:" << ok << std::endl;
#endif
#endif
    return ok;
}

void BFOptimization::setJumpGuard(MoveNode& mn) {
#ifdef DEBUG_LOOP_SCHEDULER
    std::cerr << "\t\t\t\t\tsetting jump guard to: " << mn.toString()
              << std::endl;
#endif

    if (!mn.move().isUnconditional()) {
        std::cerr << "Cannot set jump guard because already conditional: "
                  << mn.toString() << std::endl;
    }
    assert(mn.move().isUnconditional());

    assert(!dynamic_cast<const TTAMachine::PortGuard*>(
               &sched_.jumpGuard()->guard()));

    TTAProgram::MoveGuard* newGuard = sched_.jumpGuard()->copy();
//        TTAProgram::CodeGenerator::createInverseGuard(*sched_.jumpGuard());

    mn.move().setGuard(newGuard);

/*
    TTAMachine::Guard& g = newGuard->guard();
    TTAMachine::RegisterGuard* rg =
        dynamic_cast<TTAMachine::RegisterGuard*>(&g);




    TCEString regName;
    regName << rg->registerFile()->name() << "." <<
        rg->registerIndex();

    DataDependenceEdge* e = new DataDependenceEdge(
        DataDependenceEdge::EDGE_REGISTER,
        DataDependenceEdge::DEP_RAW,
        regName,
        true, // guard
        false, // true_alias mem dep
        false, // tail pseudo
        false, // head pseudo
        1); // loop depth
    ddg().connectNodes(*sched_.guardWriteNode(), mn,*e);

*/
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\t\tset jump guard: " << mn.toString() << std::endl;
#endif

}

void BFOptimization::unsetJumpGuard(MoveNode& mn) {
#ifdef DEBUG_LOOP_SCHEDULER
    std::cerr << "\t\t\t\t\tunsetting jump guard from: "
              << mn.toString() << std::endl;
#endif
    ddg().removeIncomingGuardEdges(mn);
    mn.move().setGuard(NULL);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\t\tunset jump guard: " << mn.toString() << std::endl;
#endif
}

std::pair<MoveNode*,bool> BFOptimization::createCopyForPrologEpilog(
    MoveNode& mn) {
    auto a = duplicator().duplicateMoveNode(mn, true, true);
    prologMoves_[&mn] = a.first;
    return a; //prologMN;
}

void BFOptimization::setPrologSrcFUAnno(MoveNode& prologMN, MoveNode& loopMN){
    TTAProgram::Move& m = prologMN.move();

    if (loopMN.isSourceOperation()) {
        m.removeAnnotations(
            TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC);
        TTAProgram::Terminal& source = loopMN.move().source();
        assert(source.isFUPort());
        std::string fuName = source.functionUnit().name();
#ifdef DEBUG_LOOP_SCHEDULER
        std::cerr << "\t\tSetting src Fu anno, loop MN: "
                  << loopMN.toString() << " prolog MN: "
                  << prologMN.toString() << std::endl;
#endif
	    //TODO: which is the correct annotation here?
        TTAProgram::ProgramAnnotation srcUnit(
            TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC,
            fuName);
        m.setAnnotation(srcUnit);
    }
}

void BFOptimization::setPrologDstFUAnno(MoveNode& prologMN, MoveNode& loopMN){
    TTAProgram::Move& m = prologMN.move();

    if (loopMN.isDestinationOperation()) {
        m.removeAnnotations(
            TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST);
        TTAProgram::Terminal& dest = loopMN.move().destination();
        assert(dest.isFUPort());
        std::string fuName = dest.functionUnit().name();
#ifdef DEBUG_LOOP_SCHEDULER
        std::cerr << "\t\tSetting dst Fu anno, loop MN: "
                  << loopMN.toString() << " prolog MN: "
                  << prologMN.toString() << std::endl;
#endif
	    //TODO: which is the correct annotation here?
        TTAProgram::ProgramAnnotation dstUnit(
            TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST,
            fuName);
        m.setAnnotation(dstUnit);
    }
}

const TTAMachine::FunctionUnit*
BFOptimization::sourceFU(const MoveNode& mn) {
    return fuOfTerminal(mn.move().source());
}

const TTAMachine::FunctionUnit*
BFOptimization::destinationFU(const MoveNode& mn) {
    return fuOfTerminal(mn.move().destination());
}

const TTAMachine::FunctionUnit*
BFOptimization::fuOfTerminal(const TTAProgram::Terminal& t) {
    if (!t.isFUPort())
        return NULL;
    return &t.functionUnit();
}

void BFOptimization::setPrologFUAnnos(MoveNode& prologMN, MoveNode& loopMN) {
    setPrologSrcFUAnno(prologMN, loopMN);
    setPrologDstFUAnno(prologMN, loopMN);
}

void BFOptimization::assignCopyToPrologEpilog(
    int cycle, MoveNode& prologMN, MoveNode& oldMN,
    const TTAMachine::Bus* prologBus,
    int prologImmWriteCycle) {

    const TTAMachine::ImmediateUnit* immu = nullptr;
    int immRegIndex = -1;
    if (oldMN.isSourceImmediateRegister()) {
        immRegIndex = oldMN.move().source().index();
        immu = &oldMN.move().source().immediateUnit();
    }

#ifdef DEBUG_LOOP_SCHEDULER
    std::cerr << "\t\t\t\t\tAssigning copy to prolog: " << cycle
              << " " << prologMN.toString() <<
        " prolog cycle: " << cycle + BF2Scheduler::PROLOG_CYCLE_BIAS
              << std::endl;

    if (!prologRM()->canAssign(
            BF2Scheduler::PROLOG_CYCLE_BIAS + cycle, prologMN, prologBus,
            sourceFU(oldMN), destinationFU(oldMN), prologImmWriteCycle,
            immu, immRegIndex)) {
        TTAProgram::Move& m = prologMN.move();

        std::cerr << std::endl << "Cannot assign move to prolog! "
                  << prologMN.toString() << std::endl << std::endl;
        std::cerr << "Preassigned bus: " << m.bus().name() << std::endl;
        if (prologBus != nullptr) {
            std::cerr << "prolog bus: " << prologBus->name() << std::endl;
        }
        std::cerr << "Cycle: " << cycle << " prolog cycle: "
                  << cycle + BF2Scheduler::PROLOG_CYCLE_BIAS << std::endl;

        if (prologMN.isDestinationOperation()) {
            std::cerr << "destpo: "
                      << prologMN.destinationOperation().toString()
                      << std::endl;
        }

        ddg().writeToDotFile("cannot_assign_prolog.dot");
        prologDDG()->writeToDotFile("prologddg.dot");
        int sc = prologRM()->smallestCycle();
        int lc = prologRM()->largestCycle();
        for (int i = sc; i <= lc; i++) {
            std::cerr << "\t" << i << "\t"
                      << prologRM()->instruction(i)->toString() << std::endl;
        }

        assert(false && "Cannot asign copy to prolog");
    }
#endif

    prologRM()->assign(
        BF2Scheduler::PROLOG_CYCLE_BIAS + cycle, prologMN, prologBus,
        sourceFU(oldMN), destinationFU(oldMN), prologImmWriteCycle,
        immu, immRegIndex);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tAassigned copy to prolog, original mn: "
              << oldMN.toString()
              << " copy: " << prologMN.toString() << std::endl;
#endif
}

void BFOptimization::unassignCopyFromPrologEpilog(
    MoveNode& mn, bool disposePrologCopy) {
#ifdef DEBUG_LOOP_SCHEDULER
    std::cerr << "\t\tUnassigning copy from prolog, original mn: "
              << mn.toString() << "dispose: "
              << disposePrologCopy << std::endl;
#endif
    MoveNode* copy = prologMoves_[&mn];
    if (copy != NULL) {
        prologRM()->unassign(*copy);
        prologMoves_.erase(&mn);
#ifdef DEBUG_LOOP_SCHEDULER
        std::cerr << "\t\t\tcopy after unassign: " << copy->toString()
                  << std::endl;
#endif
        if (disposePrologCopy) {
            duplicator().disposeMoveNode(copy);
        }
        // TODO: program ops memory leak now
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    } else {
        std::cerr << "\t\t\tCopy not found." << std::endl;
#endif
    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tUnassigned copy from prolog, original mn: "
              << mn.toString() << std::endl;
#endif
    prologMoves_.erase(&mn);
}

std::map<MoveNode*, MoveNode*, MoveNode::Comparator>
BFOptimization::prologMoves_;

void BFOptimization::clearPrologMoves() {
    prologMoves_.clear();
}

MoveNode* BFOptimization::getSisterTrigger(
    const MoveNode& mn, const TTAMachine::Machine& mach) {
    if (!mn.isDestinationOperation()) {
        return NULL;
    }
    ProgramOperation& po = mn.destinationOperation();
    return BasicBlockScheduler::findTrigger(po, mach);
}

bool BFOptimization::canBeSpeculated(const Operation& op) {
    return
        !op.usesMemory() &&
        !op.hasSideEffects() &&
        !op.isControlFlowOperation() &&
        op.affectsCount() == 0;
}

bool BFOptimization::canBeSpeculated(const MoveNode& mn) {
    if (!mn.isDestinationOperation()) {
        return false;
    }
    const MoveNode* trig = getSisterTrigger(mn, targetMachine());
    // operand writes can be always speculated
    if (&mn != trig && trig != NULL) {
        return true;
    }
    const Operation& op = mn.destinationOperation().operation();
    return canBeSpeculated(op);
}

bool BFOptimization::needJumpGuard(const MoveNode& mn, int cycle){
    // TODO: different modes for prolog/epilog.
    if (sched_.hasEpilog()) {
        return false;
    }

    if (cycle >= (int)(ii())) {
        return false;
    }

    return !canBeSpeculated(mn);
}

/**
 * Returns false if needs guard but guard not available yet
 */
bool BFOptimization::addJumpGuardIfNeeded(MoveNode& mn, int cycle,
                                          bool ignoreGuardWriteCycle) {
    if (needJumpGuard(mn, cycle)) {
        if (jumpGuardAvailableCycle(mn) > cycle && !ignoreGuardWriteCycle) {
#ifdef DEBUG_LOOP_SCHEDULER
            std::cerr << "jump guard available at "
                      << jumpGuardAvailableCycle(mn)
                      << " , not in cycle: " << cycle << std::endl;
#endif
            return false;
        }
        if (!mn.move().isUnconditional()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Cannot add guard to already-conditional move "
                      << std::endl;
#endif
            return false;
        }
        setJumpGuard(mn);
    }
    return true;
}

void BFOptimization::unsetJumpGuardIfNeeded(MoveNode& mn, int cycle) {
    if (needJumpGuard(mn, cycle)) {
        unsetJumpGuard(mn);
    }
}

int BFOptimization::jumpGuardAvailableCycle(const MoveNode& mn) {

    if (!sched_.jumpNode()) {
#ifdef DEBUG_LOOP_SCHEDULER
        std::cerr << "jump node is null!" << std::endl;
#endif
        return INT_MAX;
    }

    if (!sched_.guardPrologMoves()) {
        return INT_MAX;
    }

    if (sched_.scheduleJumpGuardBetweenIters())
        return 0;

    if (sched_.guardWriteNode() == NULL) {
#ifdef DEBUG_LOOP_SCHEDULER
        std::cerr << "Guard write node not known!" << std::endl;
#endif
        return INT_MAX;
    }
    if (&mn == sched_.guardWriteNode()) {
#ifdef DEBUG_LOOP_SCHEDULER
        std::cerr << "I AM guard write node!" << std::endl << std::endl;
#endif
        return 0;
    }
    if (!sched_.guardWriteNode()->isScheduled()) {
#ifdef DEBUG_LOOP_SCHEDULER
        std::cerr << "guardwrite node is null or not sched:"
                  << sched_.guardWriteNode()->toString() << std::endl;
#endif
        return INT_MAX;
    }

    int glat = sched_.guardWriteNode()->move().destination().registerFile().
        guardLatency()+
        targetMachine().controlUnit()->globalGuardLatency();

    return glat+ sched_.guardWriteNode()->cycle() - ii();
}

void BFOptimization::mightBeReady(MoveNode& n) {
    sched_.currentFront()->mightBeReady(n);
}

bool BFOptimization::hasAmbiguousResources(MoveNode& mn) const {
    if (mn.isSourceOperation() &&
        (mn.move().annotationCount(
             TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC) !=1)
        && (mn.move().annotationCount(
                TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC) !=1)) {
        return true;
    }
    if (mn.isDestinationOperation() &&
        (mn.move().annotationCount(
             TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST) !=1)
        && (mn.move().annotationCount(
                TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC) !=1)) {
        return true;
    }
    // LIMM unit may be ambiguous resource
    if (mn.isSourceConstant())
        return true;

    return false;
}

bool BFOptimization::immCountPreventsScheduling(const MoveNode& mn) {

    auto& move = mn.move();
    if (!move.source().isImmediate()) {
        return false;
    }

    if (!mn.isDestinationOperation()) {
        return false;
    }

    int simmCount = MachineConnectivityCheck::maxSIMMCount(targetMachine());
    int limmCount = MachineConnectivityCheck::maxLIMMCount(targetMachine());

    ProgramOperation& po = mn.destinationOperation();
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& input = po.inputMove(i);
        auto& inputMove = input.move();
        if (input.move().destination().isFUPort() && input.isScheduled()) {
            auto& fu = inputMove.destination().functionUnit();
            auto hwop = fu.operation(po.operation().name());
            auto port = hwop->port(move.destination().operationIndex());
            if (!port->noRegister()) {
                return false;
            }
            if (inputMove.source().isImmediate()) {
                simmCount--;
            }
            if (inputMove.source().isImmediateRegister()) {
                limmCount--;
            }
            // all ways to transport immediate used.
            if (simmCount + limmCount < 1) {
                return true;
            }
            // needs LIMM but no LIMM available, only too narrow SIMMs available.
            if (limmCount < 1 &&
                !MachineConnectivityCheck::canTransportImmediate(
                    static_cast<TTAProgram::TerminalImmediate&>(
                        move.source()), *port,
                    move.isUnconditional()
                    ? nullptr
                    : &move.guard().guard())) {
                return true;
            }
        }
    }
    return false;
}

const TTAMachine::RegisterFile*
BFOptimization::RFReadPortCountPreventsScheduling(const MoveNode& mn) {

    auto& move = mn.move();
    if (!move.source().isGPR()) {
        return nullptr;
    }

    auto& rf = move.source().registerFile();
    int freeRFPorts = rf.maxReads();

    if (!move.destination().isFUPort() || !mn.isDestinationOperation()) {
        return nullptr;
    }

    ProgramOperation& po = mn.destinationOperation();
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& input = po.inputMove(i);
        auto& inputMove = input.move();
        if (input.move().destination().isFUPort() && input.isScheduled()) {
            auto& fu = inputMove.destination().functionUnit();
            auto hwop = fu.operation(po.operation().name());
            auto port = hwop->port(move.destination().operationIndex());
            if (!port->noRegister()) {
                return nullptr;
            }
            if (inputMove.source().isGPR() &&
                &inputMove.source().registerFile() == &rf) {
                freeRFPorts--;
                if (freeRFPorts < 1) {
                    return &rf;
                }
            }
        }
    }
    return nullptr;
}

bool BFOptimization::canBeScheduled(const MoveNode& mn) {
    if (!MachineConnectivityCheck::canTransportMove(mn, rm().machine()))
        return false;
    if (RFReadPortCountPreventsScheduling(mn)) {
        return false;
    }
    if (immCountPreventsScheduling(mn)) {
        return false;
    }
    return true;
}

void BFOptimization::checkPrologDDG(MoveNode& prologEpilogMN) {
    auto oEdges = prologDDG()->outEdges(prologEpilogMN);
    auto iEdges = prologDDG()->inEdges(prologEpilogMN);

    for (auto e: oEdges) {
        if (!e->isRAW())
            continue;
        MoveNode& head = prologDDG()->headNode(*e);
        if (head.isScheduled() && head.cycle() <= prologEpilogMN.cycle()) {
            prologDDG()->writeToDotFile("illegal_assign_in_prolog.dot");

            ddg().writeToDotFile("mainddg.dot");

            std::cerr << "Illegal assign of: " << prologEpilogMN.toString()
                      << std::endl;
            assert(false);
        }
    }

    for (auto e: iEdges) {
        if (!e->isRAW())
            continue;
        MoveNode& tail = prologDDG()->tailNode(*e);
        if (tail.isScheduled() && tail.cycle() >= prologEpilogMN.cycle()) {
            prologDDG()->writeToDotFile("illegal_assign_in_prolog.dot");

            ddg().writeToDotFile("mainddg.dot");

            std::cerr << "Illegal assign(2) of: " << prologEpilogMN.toString()
                      << std::endl;
            assert(false);
        }
    }
}

#ifdef CHECK_DDG_EQUALITY
void BFOptimization::getDDGSnapshot() {
    if (id() < CHECK_DDG_EQUALITY) {
        return;
    }

    ddgECount_ = ddg().edgeCount();
    ddgString_ = ddg().dotString();
    if (prologDDG() != nullptr) {
        prologDDGString_ = prologDDG()->dotString();
        prologDDGECount_ = prologDDG()->edgeCount();
        prologDDGNCount_ = prologDDG()->nodeCount();
    }
}

/**
 * Checks that DDG edge and node counts are same before the optimization
 * and after reverting it.
 */
void BFOptimization::checkDDGEquality() {

    if (id() < CHECK_DDG_EQUALITY) {
        return;
    }
    int ddgECount = ddg().edgeCount();
    auto ddgst = ddg().dotString();
    if (ddgECount != ddgECount_ ) { //(ddgst != ddgString_) {
        std::cerr << "ddg changed! id:" << id() << std::endl;

        std::ofstream output("before.dot");
        output << ddgString_;
        output.close();

        ddg().writeToDotFile("after.dot");
        assert(false);
    }
    if (prologDDGString_ != "") {
        if (prologDDG()->edgeCount() != prologDDGECount_ ||
            prologDDG()->nodeCount() != prologDDGNCount_) {

            std::cerr << "prolog ddg changed! id: " << id() << std::endl;

            std::ofstream output("before.dot");
            output << prologDDGString_;
            output.close();

            prologDDG()->writeToDotFile("after.dot");
            assert(false);
        }
    }
}

void BFOptimization::undo() {
    Reversible::undo();
    checkDDGEquality();
}
#endif
