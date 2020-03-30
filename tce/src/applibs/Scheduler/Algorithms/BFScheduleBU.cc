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
 * @file BFScheduleBU.cc
 *
 * Definition of BFScheduleBU class
 *
 * Tries to schedule a move in bottom-up order. Before the actual scheduling,
 * Tries to call many other optimization classes to do things like
 * bypassing, operand sharing etc.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFScheduleBU.hh"
#include "BFRegCopyBefore.hh"
#include "BFRegCopyAfter.hh"
#include "BFEarlyBypasser.hh"
#include "BFLateBypasses.hh"
#include "BFPushAntidepsDown.hh"
#include "MachineConnectivityCheck.hh"
#include "SimpleResourceManager.hh"
#include "MoveNode.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "Move.hh"
#include "UniversalMachine.hh"
#include "BFScheduleTD.hh"
#include "BF2ScheduleFront.hh"
#include "BFShareOperandsLate.hh"
#include "BFShareOperands.hh"
#include "BFPushDepsUp.hh"
#include "BFRemoveGuardsFromSuccs.hh"
#include "Terminal.hh"
#include "BFRescheduleResultsClose.hh"
#include "BFTryRemoveGuard.hh"
#include "BFRenameSource.hh"

//#define DEBUG_BUBBLEFISH_SCHEDULER

bool
BFScheduleBU::operator()() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\tPerforming BFSchduleBU for: " << mn_.toString() <<
        " lc limit: " << lc_ << std::endl;
#endif
    assert(!mn_.isScheduled());
    if (&mn_.move().bus() != &UniversalMachine::instance().universalBus()) {
        mn_.move().setBus(UniversalMachine::instance().universalBus());
    }
    // TODO: what about constants!

    if (mn_.isDestinationOperation() && mn_.isSourceVariable() &&
        allowEarlySharing_) {
        /** Another op may not come between these, so if this does
            not work, try without this */
        BFShareOperands* shareOpers = new BFShareOperands(sched_, mn_, lc_);
        runPreChild(shareOpers);
    }

    if (mn_.isSourceVariable()) {
        bool forceBypass = BF2Scheduler::isSourceUniversalReg(mn_);
        if (forceBypass && !allowEarlyBypass_) {
            return false;
        }
        if (allowEarlyBypass_) {
            BFEarlyBypasser* ebp = new BFEarlyBypasser(sched_, mn_);
            if (!runPreChild(ebp)) {
                if (forceBypass) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    std::cerr << "\t\tForced bypass failed" << std::endl;
                    ddg().writeToDotFile("forced_bypass_fail.dot");
#endif
                    return false;
                }
            }
            if (mn_.isScheduled()) {
                bypasserScheduled_ = true;
                return true;
            }
        }
    }

    if (mn_.isDestinationVariable() && allowLateBypass_) {
        BFLateBypasses* lbp = new BFLateBypasses(sched_, mn_, lc_);
        if (runPreChild(lbp)) {
            if (lbp->removedSource()) {
                return true;
            } else {
                // late bypass without DRE .. we may want to do TD instead?
                int prefCycle = BF2ScheduleFront::prefResultCycle(mn_);
                if (prefCycle != INT_MAX) {
                    BFScheduleTD* td =
                        new BFScheduleTD(sched_, mn_, prefCycle, false);
                    if (runPostChild(td)) {
                        return true;
                    }
                }
            }
        }
    }

    BFRegCopy* regCopy = NULL;
    BFRegCopy* regCopyBefore = NULL;
    BFRegCopy* regCopyAfter = NULL;
    if (!canBeScheduled(mn_)) {

        // can this be solved by dropping guard?
        if (!runPreChild(new BFTryRemoveGuard(sched_, mn_))) {

            // no. need to add a regcopy.
            if (mn_.isSourceOperation()) {
                regCopy = regCopyAfter = new BFRegCopyAfter(sched_,mn_, lc_);
            } else {
                BFRenameSource* renSrc =
                    new BFRenameSource(sched_, mn_, lc_, lc_);
                if (runPreChild(renSrc)) {
                    auto forbiddenRF =
                        RFReadPortCountPreventsScheduling(mn_);
                    if (forbiddenRF) {
                        renSrc->undo();
                        preChildren_.pop();
                        forbiddenRF = RFReadPortCountPreventsScheduling(mn_);
                        regCopy = regCopyBefore =
                            new BFRegCopyBefore(sched_, mn_, lc_,forbiddenRF);
                    }
                } else {
                    // could not use renaming to get rid of regcopy.
                    auto forbiddenRF = RFReadPortCountPreventsScheduling(mn_);
                    regCopy = regCopyBefore =
                        new BFRegCopyBefore(sched_, mn_, lc_, forbiddenRF);
                }
            }
            if (regCopy && !runPreChild(regCopy)) {
                undoAndRemovePreChildren();
                return false;
            }
        }
    }
    // ignore unsched successors
    int ddglc = std::min(lc_,ddg().latestCycle(mn_, ii(), false, true));
    int ddgec = ddg().earliestCycle(mn_, ii(), false, false);
    int rmlc = rmLC(ddglc, mn_);

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tddg lc: " << ddglc << std::endl;
    std::cerr << "\t\tddg ec: " << ddgec << std::endl;
#endif

    int latestIfRenamed =
	std::min(lc_,
		 ddg().latestCycle(mn_, ii(), true, false));
//    ddgReplyLimit = std::min(latestIfRenamed+3, ddgReplyLimit);
    if (latestIfRenamed > ddglc && !mn_.move().isControlFlowMove()) {
        latestIfRenamed = rmLC(latestIfRenamed, mn_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\tRenaming could benefit move: " << mn_.toString() <<
            " lc: " << ddglc << " renamed lc: " << latestIfRenamed
                  << std::endl;
#endif
        BFOptimization* adepPush = new BFPushAntidepsDown(
            sched_, mn_, ddglc, latestIfRenamed);
        if (runPreChild(adepPush)) {
            ddglc = std::min(
                lc_, ddg().latestCycle(mn_, ii(), false, false));
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\tPushed antidep dests down, new lc: "
                      << ddglc << std::endl;
#endif

            latestIfRenamed = std::min(lc_,
                                       ddg().latestCycle(
                                           mn_, ii(), true, false));
            latestIfRenamed = rmLC(latestIfRenamed, mn_);

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        } else {

            std::cerr << "\t\tCould not push antidep dests down" << std::endl;
#endif
        }
        rmlc = rmLC(ddglc, mn_);

        if (latestIfRenamed > ddglc &&
            !mn_.move().isControlFlowMove() &&
            mn_.isSourceVariable()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Antideps still limit scheduling of: "
                      << mn_.toString() << " even after pushing? them down!"
                      << std::endl;
#endif
            BFOptimization* renameSrc =
                new BFRenameSource(sched_, mn_, ddglc, latestIfRenamed);
            if (runPreChild(renameSrc)) {
                int renamedDDGLC =
                    std::min(
                        lc_, ddg().latestCycle(mn_, ii(), false, false));

                int renamedRMLC = rmLC(renamedDDGLC, mn_);
                if (renamedRMLC <= rmlc) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    std::cerr << "\t\tRenaming did not help rmlc, undoing"
                              << std::endl;
#endif
                    preChildren_.pop();
                    renameSrc->undo();
                } else {
                    ddglc = renamedDDGLC;
                    rmlc = renamedRMLC;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\tRenamed Source Reg, new lc: "
                          << ddglc << std::endl;
#endif
                }
            }
        }
    }

    if ((ddglc < ddgec || rmlc < ddgec) && ddglc >=0 && ii()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\tPushing up deps could benefit move: "
                  << mn_.toString() << " lc: " << ddglc
                  << " ec: " << ddgec << std::endl;
#endif
        if (runPreChild(new BFPushDepsUp(sched_, mn_, ddglc))) {
            ddgec = ddg().earliestCycle(mn_, ii(), false, false);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "pushed antidep something up, ec now: "
                      << ddgec << std::endl;
#endif
        }
    }


    if (mn_.move().destination().isGPR() &&
        mn_.move().destination().registerFile().width() == 1) {
        int latestNoGuard =
            std::min(lc_,
                     ddg().latestCycle(mn_, ii(), false, false, true));
        if (latestNoGuard > ddglc) {
            BFOptimization* removeGuardsFromSuccs =
                new BFRemoveGuardsFromSuccs(sched_, mn_, ddglc,latestNoGuard);
            if (runPreChild(removeGuardsFromSuccs)) {
                ddglc = std::min(lc_,
                                 ddg().latestCycle(mn_, ii(), false, false));
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\tRemoved guards from dests, new lc: "
                          << ddglc << std::endl;
#endif
            } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\tCould not remove guards from dests"
                          << std::endl;
#endif
            }
        }
    }

    rmlc = rmLC(ddglc, mn_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tbfschdbu: rmlc: " << rmlc << std::endl;
#endif

    if (rmlc < ddgec && ii() != 0) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tDDG ec " << ddgec << " on loop(II:"
                  << ii() << ") limits scheudling of: " << mn_.toString();
#endif
        if (ddg().earliestCycle(mn_, INT_MAX/2, false, false) < rmlc) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Limiter is a loop edge" << std::endl;
            std::cerr << "\t\t\t\tddgec was: " << ddgec << " ddglc was: "
                      << ddgec << " rmlc was: " << rmlc << std::endl;
#endif
            BFOptimization* loopDepPush = new BFPushDepsUp(sched_, mn_, rmlc);
            if (runPreChild(loopDepPush)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "Pushed loop deps up." << std::endl;
#endif
                ddglc = std::min(lc_,ddg().latestCycle(mn_, ii(), false, false));
                ddgec = ddg().earliestCycle(mn_, ii(), false, false);
                rmlc = rmLC(ddglc, mn_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\t\t\tddgec now: " << ddgec << "ddglc now: "
                          << ddgec << "rmlc now: " << rmlc << std::endl;
#endif
            } else {
                //                delete loopDepPush;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "loop dep psuh failed." << std::endl;
#endif
            }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        } else {
            std::cerr << "Limiter not a loop edge." << std::endl;
#endif
        }
    }
    rmlc = rmLC(ddglc, mn_);
    if (rmlc >= ddgec && rmlc != INT_MAX) {

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tassigning to: " << rmlc << std::endl;
#endif

        createdCopy_ = assign(rmlc, mn_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\tAssigned ok:" << mn_.toString()
                  << " bus: " << mn_.move().bus().name() << std::endl;
#endif
        if (regCopyBefore != NULL) {
            MoveNode* regCopy = regCopyBefore->getRegCopy();
            BFScheduleBU* regCopySched =
                new BFScheduleBU(sched_, *regCopy, lc_, false, false, false);
            if (!runPostChild(regCopySched)) {
                undo();
                return false;
            }
        }

        BFShareOperandsLate* sol = new BFShareOperandsLate(sched_, mn_);
        runPostChild(sol);

#if 0
        if (mn_.move().destination().isFUPort() &&
            mn_.move().destination().isTriggering()) {
            runPostChild(new BFRescheduleResultsClose(
                             sched_, mn_.destinationOperation()));
        }
#endif
        return true;
    } else {
        undoAndRemovePreChildren();
        return false;
    }
}

void
BFScheduleBU::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tUndoing BU schedule of: " << mn_.toString() <<std::endl;
#endif

    if (!bypasserScheduled_) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tunassigning here." << std::endl;
#endif
        if (mn_.isScheduled()) {
            unassign(mn_, createdCopy_);
        }
        if (&mn_.move().bus() !=
            &UniversalMachine::instance().universalBus()) {
            mn_.move().setBus(UniversalMachine::instance().universalBus());
        }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    } else {
        std::cerr << "\t\t\tbypasser scheduled this, no need to undo here"
                  << std::endl;
#endif
    }
}
