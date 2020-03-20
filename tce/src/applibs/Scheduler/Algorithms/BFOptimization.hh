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
 * @file BFOptimization.hh
 *
 * Declaration of BFOptimization class.
 *
 * Base class for all optimizations and scheudling operations the
 * BF2 instruction scheduler can perform. Contains an undo-mechanism
 * To undo everything, and contains handling for scheudling mirror move
 * to prolog/epilog in case of loop scheduling.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BFOPTIMIZATION_HH
#define BFOPTIMIZATION_HH

class DataDependenceGraph;
class SimpleResourceManager;
class BF2Scheduler;
class BUMoveNodeSelector;
class MoveNode;
class Operation;

#include <cassert>
#include <map>
#include "MoveNode.hh"
#include "Reversible.hh"
#include "TCEString.hh"

namespace TTAMachine {
    class Machine;
    class FunctionUnit;
    class Bus;
    class ImmediateUnit;
    class RegisterFile;
}

namespace TTAProgram {
    class Terminal;
}

class MoveNodeDuplicator;

// debugging defines.
//#define CHECK_DDG_EQUALITY 1
//#define DEBUG_BUBBLEFISH_SCHEDULER

class BFOptimization : public Reversible {
public:
    BFOptimization(BF2Scheduler& sched) : sched_(sched) {
#ifdef CHECK_DDG_EQUALITY
        getDDGSnapshot();
#endif
}

    virtual bool isFinishFront() { return false; }
    static void clearPrologMoves();
    static MoveNode* getSisterTrigger(
        const MoveNode& mn, const TTAMachine::Machine& mach);
    // TODO: this list may contain moves that are demoved. egt rid of them!
    virtual void mightBeReady(MoveNode& mn);

#ifdef CHECK_DDG_EQUALITY
    virtual void undo() override;
#endif

protected:
    DataDependenceGraph& ddg();
    DataDependenceGraph* rootDDG();
    const DataDependenceGraph& ddg() const;
    DataDependenceGraph* prologDDG();
    SimpleResourceManager& rm() const;
    SimpleResourceManager* prologRM() const;
    BUMoveNodeSelector& selector();
    const TTAMachine::Machine& targetMachine() const;
    unsigned int ii() const;
    MoveNodeDuplicator& duplicator() const;

    BF2Scheduler& sched_;

    // wrappers over RM for loop scheduling
    virtual bool assign(int cycle, MoveNode&,
                        const TTAMachine::Bus* bus = nullptr,
                        const TTAMachine::FunctionUnit* srcFU_ = nullptr,
                        const TTAMachine::FunctionUnit* dstFU = nullptr,
                        const TTAMachine::Bus* prologBus = nullptr,
                        int immWriteCycle = -1,
                        int prologImmWriteCycle = -1,
                        const TTAMachine::ImmediateUnit* immu = nullptr,
                        int immRegIndex = -1,
                        bool ignoreGuardWriteCycle = false);
    virtual void unassign(MoveNode& mn,
                          bool disposePrologCopy = true);
    virtual int rmEC(int cycle, MoveNode& mn,
                     const TTAMachine::Bus* bus = nullptr,
                     const TTAMachine::FunctionUnit* srcFU = nullptr,
                     const TTAMachine::FunctionUnit* dstFU = nullptr,
                     const TTAMachine::Bus* prologBus = nullptr,
                     int immWriteCycle = -1,
                     int prologImmWriteCycle = -1,
                     const TTAMachine::ImmediateUnit* immu = nullptr,
                     int immRegIndex = -1);
    virtual int rmLC(int cycle, MoveNode& mn,
                     const TTAMachine::Bus* bus = nullptr,
                     const TTAMachine::FunctionUnit* srcFU = nullptr,
                     const TTAMachine::FunctionUnit* dstFU = nullptr,
                     const TTAMachine::Bus* prologBus = nullptr,
                     int immWriteCycle = -1,
                     int prologImmWriteCycle = -1,
                     const TTAMachine::ImmediateUnit* immu = nullptr,
                     int immRegIndex = -1);
    virtual bool canAssign(int cycle, MoveNode& mn,
                           const TTAMachine::Bus* bus = nullptr,
                           const TTAMachine::FunctionUnit* srcFU = nullptr,
                           const TTAMachine::FunctionUnit* dstFU = nullptr,
                           const TTAMachine::Bus* prologBus = nullptr,
                           int immWriteCycle = -1,
                           int prologImmWriteCycle = -1,
                           const TTAMachine::ImmediateUnit* immu = nullptr,
                           int immRegIndex = -1,
                           bool ignoreGWN = false);
    static std::map<MoveNode*, MoveNode*, MoveNode::Comparator> prologMoves_;

    bool putAlsoToPrologEpilog(int cycle, MoveNode& mn);

    void setPrologSrcFUAnno(MoveNode& prologMN, MoveNode& loopMN);
    void setPrologDstFUAnno(MoveNode& prologMN, MoveNode& loopMN);
    void setJumpGuard(MoveNode& mn);
    void unsetJumpGuard(MoveNode& mn);
    bool needJumpGuard(const MoveNode& mn, int cycle);
    int jumpGuardAvailableCycle(const MoveNode& mn);
    bool canBeSpeculated(const Operation& op);
    bool canBeSpeculated(const MoveNode& mn);
    bool usePrologMove(const MoveNode& mn);
    bool canBeScheduled(const MoveNode& mn);

    const TTAMachine::RegisterFile* RFReadPortCountPreventsScheduling(
        const MoveNode& mn);

    bool immCountPreventsScheduling(const MoveNode& mn);

#ifdef CHECK_DDG_EQUALITY

    void getDDGSnapshot();
    void checkDDGEquality();

    TCEString ddgString_;
    TCEString prologDDGString_;
    int ddgECount_;
    int prologDDGECount_;
    int ddgNCount_;
    int prologDDGNCount_;
#endif

private:
    void unsetJumpGuardIfNeeded(MoveNode& mn, int cycle);
    void assignCopyToPrologEpilog(
        int cycle, MoveNode& mn, MoveNode& loopMN,
        const TTAMachine::Bus* prologBus,
        int prologImmWriteCycle);
    void unassignCopyFromPrologEpilog(
        MoveNode& mh, bool disposePrologCopy = true);
    std::pair<MoveNode*,bool> createCopyForPrologEpilog(MoveNode& mn);
    bool addJumpGuardIfNeeded(
        MoveNode& mn, int cycle, bool ignoreGuardWriteCycle = false);
    void setPrologFUAnnos(MoveNode& prologMN, MoveNode& loopMN);
    bool hasAmbiguousResources(MoveNode& mn) const;

    const TTAMachine::FunctionUnit* sourceFU(const MoveNode& mn);
    const TTAMachine::FunctionUnit* destinationFU(const MoveNode& mn);
    const TTAMachine::FunctionUnit* fuOfTerminal(
        const TTAProgram::Terminal& t);

    void checkPrologDDG(MoveNode& prologEpilogMN);
};

#endif
