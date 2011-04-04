//===-- RegAllocLinearScan.cpp - Linear Scan register allocator -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements a linear scan register allocator.
// TCE version is optimized for improving ILP found by a post-pass instruction 
// scheduler (by pekka jaaskelainen[at]tut fi 2009). Starting point was the
// LLVM 2.4 linear scan register allocator, but the code was re-brangeed-merged
// from llvm 2.6 and later 2.7 linear scan allocator.
//
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "regalloc"
#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
#include "LiveDebugVariables.h"
#endif
#include "VirtRegMap.h"
#include "VirtRegRewriter.h"
#include "Spiller.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/CalcSpillWeights.h"
#include "llvm/CodeGen/LiveIntervalAnalysis.h"
#include "llvm/CodeGen/LiveStackAnalysis.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/RegAllocRegistry.h"
#include "llvm/CodeGen/RegisterCoalescer.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/ADT/EquivalenceClasses.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <set>
#include <queue>
#include <memory>
#include <cmath>

// only one of these should be enabled at a time:
#define ROUND_ROBIN_REG_ASSIGN
// false dep prevention heuristics that uses only DDG
// #define DDG_FDP
// false dep prevention graph based reg assignment heuristics
//#define FDPG_REG_ASSIGN

// TCE
#ifdef FDPG_REG_ASSIGN
#include "VirtRegIndependenceGraph.hh"
#endif
#include "TCETargetMachine.hh"
// /TCE

#include "tce_config.h"

#ifndef LLVM_2_7
namespace llvm {
    void initializeRALinScanILPPass(llvm::PassRegistry&);
}
#endif

using namespace llvm;


STATISTIC(NumIters     , "Number of iterations performed");
STATISTIC(NumBacktracks, "Number of times we had to backtrack");
static cl::opt<bool>
AvoidRegisterReuse("avoid-register-reuse",
             cl::desc("Avoid register reuse to improve ILP for post-pass scheduling"),
             cl::init(true), cl::Hidden);

STATISTIC(NumCoalesce,   "Number of copies coalesced");
STATISTIC(NumDowngrade,  "Number of registers downgraded");

static cl::opt<bool>
NewHeuristic("ilpra-new-spilling-heuristic",
             cl::desc("Use new spilling heuristic"),
             cl::init(false), cl::Hidden);

static cl::opt<bool>
PreSplitIntervals("ilpra-pre-alloc-split",
                  cl::desc("Pre-register allocation live interval splitting"),
                  cl::init(false), cl::Hidden);

static cl::opt<bool>
TrivCoalesceEnds("ilpra-trivial-coalesce-ends",
                  cl::desc("Attempt trivial coalescing of interval ends"),
                  cl::init(false), cl::Hidden);
#if 0
static RegisterRegAlloc
linearscanRegAlloc("linearscan-ilp", "ILP-optimizer linear scan register allocator",
                   createILPLinearScanRegisterAllocator);
#endif

#include <iostream>
#include "Application.hh"
#include "AssocTools.hh"
// #define DOUT Application::logStream()

namespace {
  // When we allocate a register, add it to a fixed-size queue of
  // registers to skip in subsequent allocations. This trades a small
  // amount of register pressure and increased spills for flexibility in
  // the post-pass scheduler.
  //
  // Note that in a the number of registers used for reloading spills
  // will be one greater than the value of this option.
  //
  // One big limitation of this is that it doesn't differentiate between
  // different register classes. So on x86-64, if there is xmm register
  // pressure, it can caused fewer GPRs to be held in the queue.
  static cl::opt<unsigned>
  NumRecentlyUsedRegs("ilpra-linearscan-skip-count",
                      cl::desc("Number of registers for linearscan to remember to skip."),
                      cl::init(0),
                      cl::Hidden);
 
  struct RALinScanILP : public MachineFunctionPass {
    static char ID;
#ifdef LLVM_2_7
    RALinScanILP() : MachineFunctionPass(&ID) {
#else
    RALinScanILP() : MachineFunctionPass(ID) {
#endif

// TODO: why is this here? works without?
#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
      initializeLiveDebugVariablesPass(*PassRegistry::getPassRegistry());
      initializeLiveIntervalsPass(*PassRegistry::getPassRegistry());
      initializeStrongPHIEliminationPass(*PassRegistry::getPassRegistry());
      initializeRegisterCoalescerAnalysisGroup(
        *PassRegistry::getPassRegistry());
      initializeCalculateSpillWeightsPass(*PassRegistry::getPassRegistry());
      initializePreAllocSplittingPass(*PassRegistry::getPassRegistry());
      initializeLiveStacksPass(*PassRegistry::getPassRegistry());
      initializeMachineDominatorTreePass(*PassRegistry::getPassRegistry());
      initializeMachineLoopInfoPass(*PassRegistry::getPassRegistry());
      initializeVirtRegMapPass(*PassRegistry::getPassRegistry());
      initializeMachineDominatorTreePass(*PassRegistry::getPassRegistry());
#endif

      // Initialize the queue to record recently-used registers.
      if (NumRecentlyUsedRegs > 0)
        RecentRegs.resize(NumRecentlyUsedRegs, 0);
      RecentNext = RecentRegs.begin();
    }

    typedef std::pair<LiveInterval*, LiveInterval::iterator> IntervalPtr;
    typedef SmallVector<IntervalPtr, 32> IntervalPtrs;
  private:
    /// RelatedRegClasses - This structure is built the first time a function is
    /// compiled, and keeps track of which register classes have registers that
    /// belong to multiple classes or have aliases that are in other classes.
    EquivalenceClasses<const TargetRegisterClass*> RelatedRegClasses;
    DenseMap<unsigned, const TargetRegisterClass*> OneClassForEachPhysReg;

    // NextReloadMap - For each register in the map, it maps to the another
    // register which is defined by a reload from the same stack slot and
    // both reloads are in the same basic block.
    DenseMap<unsigned, unsigned> NextReloadMap;

    // DowngradedRegs - A set of registers which are being "downgraded", i.e.
    // un-favored for allocation.
    SmallSet<unsigned, 8> DowngradedRegs;

    // DowngradeMap - A map from virtual registers to physical registers being
    // downgraded for the virtual registers.
    DenseMap<unsigned, unsigned> DowngradeMap;

    MachineFunction* mf_;
    MachineRegisterInfo* mri_;
    const TargetMachine* tm_;
    const TargetRegisterInfo* tri_;
    const TargetInstrInfo* tii_;
    BitVector allocatableRegs_;
#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
    BitVector reservedRegs_;
#else
    LiveStacks* ls_;
#endif
    LiveIntervals* li_;
    const MachineLoopInfo *loopInfo;

    /// handled_ - Intervals are added to the handled_ set in the order of their
    /// start value.  This is uses for backtracking.
    std::vector<LiveInterval*> handled_;

    /// fixed_ - Intervals that correspond to machine registers.
    ///
    IntervalPtrs fixed_;

    /// active_ - Intervals that are currently being processed, and which have a
    /// live range active for the current point.
    IntervalPtrs active_;

    /// inactive_ - Intervals that are currently being processed, but which have
    /// a hold at the current point.
    IntervalPtrs inactive_;

    typedef std::priority_queue<LiveInterval*,
                                SmallVector<LiveInterval*, 64>,
                                greater_ptr<LiveInterval> > IntervalHeap;
    IntervalHeap unhandled_;

    /// regUse_ - Tracks register usage.
    SmallVector<unsigned, 32> regUse_;
    SmallVector<unsigned, 32> regUseBackUp_;

    /// TCE
    SmallVector<bool, 32> regEverUsed_;
    SmallVector<bool, 32> regEverUsedBackUp_;
#ifdef FDPG_REG_ASSIGN
    VirtRegIndependenceGraph* fdpg_;
#endif

#ifdef DDG_FDP
    MachineInstrDDG* ddg_;
#endif

    /// /TCE

    /// vrm_ - Tracks register assignments.
    VirtRegMap* vrm_;

    std::auto_ptr<VirtRegRewriter> rewriter_;

    std::auto_ptr<Spiller> spiller_;

    // The queue of recently-used registers.
    SmallVector<unsigned, 4> RecentRegs;
    SmallVector<unsigned, 4>::iterator RecentNext;

    // Record that we just picked this register.
    void recordRecentlyUsed(unsigned reg) {
      assert(reg != 0 && "Recently used register is NOREG!");
      if (!RecentRegs.empty()) {
        *RecentNext++ = reg;
        if (RecentNext == RecentRegs.end())
          RecentNext = RecentRegs.begin();
      }
    }

  public:
    virtual const char* getPassName() const {
      return "Linear Scan Register Allocator";
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesCFG();
#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
      AU.addRequired<AliasAnalysis>();
      AU.addPreserved<AliasAnalysis>();
#endif
      AU.addRequired<LiveIntervals>();
      AU.addPreserved<SlotIndexes>();
      if (StrongPHIElim)
        AU.addRequiredID(StrongPHIEliminationID);
      // Make sure PassManager knows which analyses to make available
      // to coalescing and which analyses coalescing invalidates.
      AU.addRequiredTransitive<RegisterCoalescer>();
      AU.addRequired<CalculateSpillWeights>();
      if (PreSplitIntervals)
        AU.addRequiredID(PreAllocSplittingID);

//#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
//      AU.addRequired(LiveStacksID);
//      AU.addPreserved(LiveStacksID);
//#else
      AU.addRequired<LiveStacks>();
      AU.addPreserved<LiveStacks>();
//#endif
      AU.addRequired<MachineLoopInfo>();
      AU.addPreserved<MachineLoopInfo>();
      AU.addRequired<VirtRegMap>();
      AU.addPreserved<VirtRegMap>();
#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
      AU.addRequired<LiveDebugVariables>();
      AU.addPreserved<LiveDebugVariables>();
      AU.addRequiredID(MachineDominatorsID);
#endif
      AU.addPreservedID(MachineDominatorsID);
      MachineFunctionPass::getAnalysisUsage(AU);
    }

    /// runOnMachineFunction - register allocate the whole function
    bool runOnMachineFunction(MachineFunction&);

    // Determine if we skip this register due to its being recently used.
    bool isRecentlyUsed(unsigned reg) const {
      return std::find(RecentRegs.begin(), RecentRegs.end(), reg) !=
             RecentRegs.end();
    }

  private:
    /// linearScan - the linear scan algorithm
    void linearScan();

    /// initIntervalSets - initialize the interval sets.
    ///
    void initIntervalSets();

    /// processActiveIntervals - expire old intervals and move non-overlapping
    /// ones to the inactive list.
    void processActiveIntervals(SlotIndex CurPoint);

    /// processInactiveIntervals - expire old intervals and move overlapping
    /// ones to the active list.
    void processInactiveIntervals(SlotIndex CurPoint);

    /// hasNextReloadInterval - Return the next liveinterval that's being
    /// defined by a reload from the same SS as the specified one.
    LiveInterval *hasNextReloadInterval(LiveInterval *cur);

    /// DowngradeRegister - Downgrade a register for allocation.
    void DowngradeRegister(LiveInterval *li, unsigned Reg);

    /// UpgradeRegister - Upgrade a register for allocation.
    void UpgradeRegister(unsigned Reg);

    /// assignRegOrStackSlotAtInterval - assign a register if one
    /// is available, or spill.
    void assignRegOrStackSlotAtInterval(LiveInterval* cur);

    void updateSpillWeights(std::vector<float> &Weights,
                            unsigned reg, float weight,
                            const TargetRegisterClass *RC);

    /// findIntervalsToSpill - Determine the intervals to spill for the
    /// specified interval. It's passed the physical registers whose spill
    /// weight is the lowest among all the registers whose live intervals
    /// conflict with the interval.
    void findIntervalsToSpill(LiveInterval *cur,
                            std::vector<std::pair<unsigned,float> > &Candidates,
                            unsigned NumCands,
                            SmallVector<LiveInterval*, 8> &SpillIntervals);

    /// attemptTrivialCoalescing - If a simple interval is defined by a copy,
    /// try allocate the definition the same register as the source register
    /// if the register is not defined during live time of the interval. This
    /// eliminate a copy. This is used to coalesce copies which were not
    /// coalesced away before allocation either due to dest and src being in
    /// different register classes or because the coalescer was overly
    /// conservative.
    unsigned attemptTrivialCoalescing(LiveInterval &cur, unsigned Reg);

    ///
    /// Register usage / availability tracking helpers.
    ///

    void initRegUses() {
      regUse_.resize(tri_->getNumRegs(), 0);
      regUseBackUp_.resize(tri_->getNumRegs(), 0);
      regEverUsed_.resize(tri_->getNumRegs(), 0);
      regEverUsedBackUp_.resize(tri_->getNumRegs(), 0);
    }

    void finalizeRegUses() {
#ifndef NDEBUG
      // Verify all the registers are "freed".
      bool Error = false;
      for (unsigned i = 0, e = tri_->getNumRegs(); i != e; ++i) {
        if (regUse_[i] != 0) {
          dbgs() << tri_->getName(i) << " is still in use!\n";
          Error = true;
        }
      }
      if (Error)
        llvm_unreachable(0);
#endif
      regUse_.clear();
      regUseBackUp_.clear();
      regEverUsedBackUp_.clear();
    }

    void addRegUse(unsigned physReg) {
      assert(TargetRegisterInfo::isPhysicalRegister(physReg) &&
             "should be physical register!");
      ++regUse_[physReg];
      for (const unsigned* as = tri_->getAliasSet(physReg); *as; ++as)
        ++regUse_[*as];
    }

    void delRegUse(unsigned physReg) {
      assert(TargetRegisterInfo::isPhysicalRegister(physReg) &&
             "should be physical register!");
      assert(regUse_[physReg] != 0);
      --regUse_[physReg];
      for (const unsigned* as = tri_->getAliasSet(physReg); *as; ++as) {
        assert(regUse_[*as] != 0);
        --regUse_[*as];
      }
    }

    bool isRegAvail(unsigned physReg) const {
      assert(TargetRegisterInfo::isPhysicalRegister(physReg) &&
             "should be physical register!");
      return regUse_[physReg] == 0;
    }

    void backUpRegUses() {
      regUseBackUp_ = regUse_;
      regEverUsedBackUp_ = regEverUsed_;
    }

    void restoreRegUses() {
      regUse_ = regUseBackUp_;
      regEverUsed_ = regEverUsedBackUp_;
    }
    /// TCE
    void resetFreshness();

    void setRegFresh(unsigned physReg, bool isFresh) {
        regEverUsed_[physReg] = !isFresh;
        for (const unsigned* as = tri_->getAliasSet(physReg); *as; ++as) {
            regEverUsed_[*as] = !isFresh;
        }
    }

    bool isRegFresh(unsigned physReg) const {
        return !regEverUsed_[physReg];
    }
    
    /// /TCE

    ///
    /// Register handling helpers.
    ///

    /// getFreePhysReg - return a free physical register for this virtual
    /// register interval if we have one, otherwise return 0.
    unsigned getFreePhysReg(LiveInterval* cur);
    unsigned getFreePhysReg(LiveInterval* cur,
                            const TargetRegisterClass *RC,
                            unsigned MaxInactiveCount,
                            SmallVector<unsigned, 256> &inactiveCounts,
                            bool SkipDGRegs);

#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
    /// getFirstNonReservedPhysReg - return the first non-reserved physical
    /// register in the register class.
    unsigned getFirstNonReservedPhysReg(const TargetRegisterClass *RC) {
        TargetRegisterClass::iterator aoe = RC->allocation_order_end(*mf_);
        TargetRegisterClass::iterator i = RC->allocation_order_begin(*mf_);
        while (i != aoe && reservedRegs_.test(*i))
          ++i;
        assert(i != aoe && "All registers reserved?!");
        return *i;
      }
#endif

    void ComputeRelatedRegClasses();

    template <typename ItTy>
    void printIntervals(const char* const str, ItTy i, ItTy e) const {
      DEBUG({
          if (str)
            dbgs() << str << " intervals:\n";

          for (; i != e; ++i) {
            dbgs() << "\t" << *i->first << " -> ";

            unsigned reg = i->first->reg;
            if (TargetRegisterInfo::isVirtualRegister(reg))
              reg = vrm_->getPhys(reg);

            dbgs() << tri_->getName(reg) << '\n';
          }
        });
    }
  };
  char RALinScanILP::ID = 0;
}

#if !(defined(LLVM_2_7) || defined(LLVM_2_8))

INITIALIZE_PASS_BEGIN(RALinScanILP, "linearscan-regalloc",
                "Linear Scan Register Allocator", false, false)
INITIALIZE_PASS_DEPENDENCY(LiveIntervals)
INITIALIZE_PASS_DEPENDENCY(StrongPHIElimination)
INITIALIZE_PASS_DEPENDENCY(CalculateSpillWeights)
INITIALIZE_PASS_DEPENDENCY(PreAllocSplitting)
INITIALIZE_PASS_DEPENDENCY(LiveStacks)
INITIALIZE_PASS_DEPENDENCY(MachineLoopInfo)
INITIALIZE_PASS_DEPENDENCY(VirtRegMap)
INITIALIZE_AG_DEPENDENCY(RegisterCoalescer)
INITIALIZE_AG_DEPENDENCY(AliasAnalysis)
INITIALIZE_PASS_END(RALinScanILP, "linearscan-regalloc",
                "Linear Scan Register Allocator", false, false)

#endif

#if 0
static RegisterPass<RALinScanILP>
X("linearscan-ilp-regalloc", "ILP-optimized Linear Scan Register Allocator");
#endif

void RALinScanILP::ComputeRelatedRegClasses() {
  // First pass, add all reg classes to the union, and determine at least one
  // reg class that each register is in.
  bool HasAliases = false;
  for (TargetRegisterInfo::regclass_iterator RCI = tri_->regclass_begin(),
       E = tri_->regclass_end(); RCI != E; ++RCI) {
    RelatedRegClasses.insert(*RCI);
    for (TargetRegisterClass::iterator I = (*RCI)->begin(), E = (*RCI)->end();
         I != E; ++I) {
      HasAliases = HasAliases || *tri_->getAliasSet(*I) != 0;
      
      const TargetRegisterClass *&PRC = OneClassForEachPhysReg[*I];
      if (PRC) {
        // Already processed this register.  Just make sure we know that
        // multiple register classes share a register.
        RelatedRegClasses.unionSets(PRC, *RCI);
      } else {
        PRC = *RCI;
      }
    }
  }
  
  // Second pass, now that we know conservatively what register classes each reg
  // belongs to, add info about aliases.  We don't need to do this for targets
  // without register aliases.
#if !(defined(LLVM_2_7) || defined(LLVM_2_8))
  if (HasAliases)
    for (DenseMap<unsigned, const TargetRegisterClass*>::iterator
         I = OneClassForEachPhysReg.begin(), E = OneClassForEachPhysReg.end();
         I != E; ++I)
      for (const unsigned *AS = tri_->getAliasSet(I->first); *AS; ++AS)
        RelatedRegClasses.unionSets(I->second, OneClassForEachPhysReg[*AS]);
#else
  if (HasAliases)
    for (DenseMap<unsigned, const TargetRegisterClass*>::iterator
         I = OneClassForEachPhysReg.begin(), E = OneClassForEachPhysReg.end();
         I != E; ++I)
      for (const unsigned *AS = tri_->getAliasSet(I->first); *AS; ++AS) {
        const TargetRegisterClass *AliasClass = 
          OneClassForEachPhysReg.lookup(*AS);
        if (AliasClass)
          RelatedRegClasses.unionSets(I->second, AliasClass);
      }
#endif
}

/// attemptTrivialCoalescing - If a simple interval is defined by a copy, try
/// allocate the definition the same register as the source register if the
/// register is not defined during live time of the interval. If the interval is
/// killed by a copy, try to use the destination register. This eliminates a
/// copy. This is used to coalesce copies which were not coalesced away before
/// allocation either due to dest and src being in different register classes or
/// because the coalescer was overly conservative.
unsigned RALinScanILP::attemptTrivialCoalescing(LiveInterval &cur, unsigned Reg) {
  unsigned Preference = vrm_->getRegAllocPref(cur.reg);
  if ((Preference && Preference == Reg) || !cur.containsOneValue())
    return Reg;

  // We cannot handle complicated live ranges. Simple linear stuff only.
  if (cur.ranges.size() != 1)
    return Reg;

  const LiveRange &range = cur.ranges.front();

  VNInfo *vni = range.valno;
#if !(defined(LLVM_2_7) || defined(LLVM_2_8))
  if (vni->isUnused() || !vni->def.isValid())
    return Reg;
#else
  if (vni->isUnused())
    return Reg;
#endif
  unsigned CandReg;
  {
    MachineInstr *CopyMI;
#ifdef LLVM_2_7
    unsigned SrcReg, DstReg, SrcSubReg, DstSubReg;
    if (vni->def != SlotIndex() && vni->isDefAccurate() &&
        (CopyMI = li_->getInstructionFromIndex(vni->def)) &&
        tii_->isMoveInstr(*CopyMI, SrcReg, DstReg, SrcSubReg, DstSubReg))
      // Defined by a copy, try to extend SrcReg forward
      CandReg = SrcReg;
    else if (TrivCoalesceEnds &&
             (CopyMI =
              li_->getInstructionFromIndex(range.end.getBaseIndex())) &&
             tii_->isMoveInstr(*CopyMI, SrcReg, DstReg, SrcSubReg, DstSubReg) &&
             cur.reg == SrcReg)
      // Only used by a copy, try to extend DstReg backwards
      CandReg = DstReg;
#else
#ifdef LLVM_2_8
    if (vni->def != SlotIndex() && vni->isDefAccurate() &&
        (CopyMI = li_->getInstructionFromIndex(vni->def)) && CopyMI->isCopy())
#else // LLVM_2_9-svn
    if ((CopyMI = li_->getInstructionFromIndex(vni->def)) && CopyMI->isCopy())
#endif
      // Defined by a copy, try to extend SrcReg forward
      CandReg = CopyMI->getOperand(1).getReg();
    else if (TrivCoalesceEnds &&
            (CopyMI = li_->getInstructionFromIndex(range.end.getBaseIndex())) &&
             CopyMI->isCopy() && cur.reg == CopyMI->getOperand(1).getReg())
      // Only used by a copy, try to extend DstReg backwards
      CandReg = CopyMI->getOperand(0).getReg();
#endif
    else
      return Reg;

#if !(defined(LLVM_2_7) || defined(LLVM_2_8))
    // If the target of the copy is a sub-register then don't coalesce.
    if(CopyMI->getOperand(0).getSubReg())
      return Reg;
#endif
  }

  if (TargetRegisterInfo::isVirtualRegister(CandReg)) {
    if (!vrm_->isAssignedReg(CandReg))
      return Reg;
    CandReg = vrm_->getPhys(CandReg);
  }
  if (Reg == CandReg)
    return Reg;

  const TargetRegisterClass *RC = mri_->getRegClass(cur.reg);
  if (!RC->contains(CandReg))
    return Reg;

  if (li_->conflictsWithPhysReg(cur, *vrm_, CandReg))
    return Reg;

  // Try to coalesce.
  DEBUG(dbgs() << "Coalescing: " << cur << " -> " << tri_->getName(CandReg)
        << '\n');
  vrm_->clearVirt(cur.reg);
  vrm_->assignVirt2Phys(cur.reg, CandReg);

  ++NumCoalesce;
  return CandReg;
}

bool RALinScanILP::runOnMachineFunction(MachineFunction &fn) {
  mf_ = &fn;
  mri_ = &fn.getRegInfo();
  tm_ = &fn.getTarget();
  tri_ = tm_->getRegisterInfo();
  tii_ = tm_->getInstrInfo();
  allocatableRegs_ = tri_->getAllocatableSet(fn);
#if !(defined(LLVM_2_7) || defined(LLVM_2_8))
  reservedRegs_ = tri_->getReservedRegs(fn);
#endif
  li_ = &getAnalysis<LiveIntervals>();
#if (defined(LLVM_2_7) || defined(LLVM_2_8))
  ls_ = &getAnalysis<LiveStacks>();
#endif
  loopInfo = &getAnalysis<MachineLoopInfo>();

  // We don't run the coalescer here because we have no reason to
  // interact with it.  If the coalescer requires interaction, it
  // won't do anything.  If it doesn't require interaction, we assume
  // it was run as a separate pass.

  // If this is the first function compiled, compute the related reg classes.
  if (RelatedRegClasses.empty())
    ComputeRelatedRegClasses();

  // Also resize register usage trackers.
  initRegUses();

  vrm_ = &getAnalysis<VirtRegMap>();


  if (AvoidRegisterReuse) {
#ifdef ROUND_ROBIN_REG_ASSIGN
      resetFreshness();
#endif

#ifdef FDPG_REG_ASSIGN
      fdpg_ = new VirtRegIndependenceGraph(*mf_, *vrm_);
#endif

#ifdef DDG_FDP
      ddg_ = new MachineInstrDDG(*mf_, false);
#endif
  }

  if (!rewriter_.get()) rewriter_.reset(createVirtRegRewriter());
  
#ifdef LLVM_2_7
  spiller_.reset(createSpiller(mf_, li_, loopInfo, vrm_));
#else
  spiller_.reset(createSpiller(*this, *mf_, *vrm_));
#endif
  initIntervalSets();

  linearScan();

  // Rewrite spill code and update the PhysRegsUsed set.
  rewriter_->runOnMachineFunction(*mf_, *vrm_, li_);

  // TCE
  vrm_->markSpillInstructions();
  // /TCE

#if !(defined(LLVM_2_7) || defined(LLVM_2_8))
  // Write out new DBG_VALUE instructions.
  getAnalysis<LiveDebugVariables>().emitDebugValues(vrm_);
#endif

  assert(unhandled_.empty() && "Unhandled live intervals remain!");

  finalizeRegUses();

  fixed_.clear();
  active_.clear();
  inactive_.clear();
  handled_.clear();
  NextReloadMap.clear();
  DowngradedRegs.clear();
  DowngradeMap.clear();
  spiller_.reset(0);

#ifdef FDPG_REG_ASSIGN
  delete fdpg_;
  fdpg_ = NULL;
#endif 

#ifdef DDG_FDP
  delete ddg_;
  ddg_ = NULL;
#endif

  return true;
}

/// initIntervalSets - initialize the interval sets.
///
void RALinScanILP::initIntervalSets()
{
  assert(unhandled_.empty() && fixed_.empty() &&
         active_.empty() && inactive_.empty() &&
         "interval sets should be empty on initialization");

  handled_.reserve(li_->getNumIntervals());

  for (LiveIntervals::iterator i = li_->begin(), e = li_->end(); i != e; ++i) {
    if (TargetRegisterInfo::isPhysicalRegister(i->second->reg)) {
      if (!i->second->empty()) {
        mri_->setPhysRegUsed(i->second->reg);
        fixed_.push_back(std::make_pair(i->second, i->second->begin()));
      }
    } else {
      if (i->second->empty()) {
        assignRegOrStackSlotAtInterval(i->second);
      }
      else
        unhandled_.push(i->second);
    }
  }
}

void RALinScanILP::linearScan() {
  // linear scan algorithm
  DEBUG({
      dbgs() << "********** LINEAR SCAN **********\n"
             << "********** Function: " 
             << mf_->getFunction()->getName() << '\n';
      printIntervals("fixed", fixed_.begin(), fixed_.end());
    });

  if (AvoidRegisterReuse) {
      resetFreshness();
  }

  while (!unhandled_.empty()) {
    // pick the interval with the earliest start point
    LiveInterval* cur = unhandled_.top();
    unhandled_.pop();
    ++NumIters;
    DEBUG(dbgs() << "\n*** CURRENT ***: " << *cur << '\n');

    assert(!cur->empty() && "Empty interval in unhandled set.");

    processActiveIntervals(cur->beginIndex());
    processInactiveIntervals(cur->beginIndex());

    assert(TargetRegisterInfo::isVirtualRegister(cur->reg) &&
           "Can only allocate virtual registers!");

    // Allocating a virtual register. try to find a free
    // physical register or spill an interval (possibly this one) in order to
    // assign it one.
    assignRegOrStackSlotAtInterval(cur);

    DEBUG({
        printIntervals("active", active_.begin(), active_.end());
        printIntervals("inactive", inactive_.begin(), inactive_.end());
      });
  }

  // Expire any remaining active intervals
  while (!active_.empty()) {
    IntervalPtr &IP = active_.back();
    unsigned reg = IP.first->reg;
    DEBUG(dbgs() << "\tinterval " << *IP.first << " expired\n");
    assert(TargetRegisterInfo::isVirtualRegister(reg) &&
           "Can only allocate virtual registers!");
    reg = vrm_->getPhys(reg);
    delRegUse(reg);
    active_.pop_back();
  }

  // Expire any remaining inactive intervals
  DEBUG({
      for (IntervalPtrs::reverse_iterator
             i = inactive_.rbegin(); i != inactive_.rend(); ++i)
        dbgs() << "\tinterval " << *i->first << " expired\n";
    });
  inactive_.clear();

  // Add live-ins to every BB except for entry. Also perform trivial coalescing.
  MachineFunction::iterator EntryMBB = mf_->begin();
  SmallVector<MachineBasicBlock*, 8> LiveInMBBs;
  for (LiveIntervals::iterator i = li_->begin(), e = li_->end(); i != e; ++i) {
    LiveInterval &cur = *i->second;
    unsigned Reg = 0;
    bool isPhys = TargetRegisterInfo::isPhysicalRegister(cur.reg);
    if (isPhys)
      Reg = cur.reg;
    else if (vrm_->isAssignedReg(cur.reg))
      Reg = attemptTrivialCoalescing(cur, vrm_->getPhys(cur.reg));
    if (!Reg)
      continue;
    // Ignore splited live intervals.
    if (!isPhys && vrm_->getPreSplitReg(cur.reg))
      continue;

    for (LiveInterval::Ranges::const_iterator I = cur.begin(), E = cur.end();
         I != E; ++I) {
      const LiveRange &LR = *I;
      if (li_->findLiveInMBBs(LR.start, LR.end, LiveInMBBs)) {
        for (unsigned i = 0, e = LiveInMBBs.size(); i != e; ++i)
          if (LiveInMBBs[i] != EntryMBB) {
            assert(TargetRegisterInfo::isPhysicalRegister(Reg) &&
                   "Adding a virtual register to livein set?");
            LiveInMBBs[i]->addLiveIn(Reg);
          }
        LiveInMBBs.clear();
      }
    }
  }

  DEBUG(dbgs() << *vrm_);

  // Look for physical registers that end up not being allocated even though
  // register allocator had to spill other registers in its register class.
#if (defined(LLVM_2_7) || defined(LLVM_2_8))
  if (ls_->getNumIntervals() == 0)
    return;
#endif
  if (!vrm_->FindUnusedRegisters(li_))
    return;
}

/// processActiveIntervals - expire old intervals and move non-overlapping ones
/// to the inactive list.
void RALinScanILP::processActiveIntervals(SlotIndex CurPoint)
{
  DEBUG(dbgs() << "\tprocessing active intervals:\n");

  for (unsigned i = 0, e = active_.size(); i != e; ++i) {
    LiveInterval *Interval = active_[i].first;
    LiveInterval::iterator IntervalPos = active_[i].second;
    unsigned reg = Interval->reg;

    IntervalPos = Interval->advanceTo(IntervalPos, CurPoint);

    if (IntervalPos == Interval->end()) {     // Remove expired intervals.
      DEBUG(dbgs() << "\t\tinterval " << *Interval << " expired\n");
      assert(TargetRegisterInfo::isVirtualRegister(reg) &&
             "Can only allocate virtual registers!");
      reg = vrm_->getPhys(reg);
      delRegUse(reg);

      // Pop off the end of the list.
      active_[i] = active_.back();
      active_.pop_back();
      --i; --e;

    } else if (IntervalPos->start > CurPoint) {
      // Move inactive intervals to inactive list.
      DEBUG(dbgs() << "\t\tinterval " << *Interval << " inactive\n");
      assert(TargetRegisterInfo::isVirtualRegister(reg) &&
             "Can only allocate virtual registers!");
      reg = vrm_->getPhys(reg);
      delRegUse(reg);
      // add to inactive.
      inactive_.push_back(std::make_pair(Interval, IntervalPos));

      // Pop off the end of the list.
      active_[i] = active_.back();
      active_.pop_back();
      --i; --e;
    } else {
      // Otherwise, just update the iterator position.
      active_[i].second = IntervalPos;
    }
  }
}

/// processInactiveIntervals - expire old intervals and move overlapping
/// ones to the active list.
void RALinScanILP::processInactiveIntervals(SlotIndex CurPoint)
{
  DEBUG(dbgs() << "\tprocessing inactive intervals:\n");

  for (unsigned i = 0, e = inactive_.size(); i != e; ++i) {
    LiveInterval *Interval = inactive_[i].first;
    LiveInterval::iterator IntervalPos = inactive_[i].second;
    unsigned reg = Interval->reg;

    IntervalPos = Interval->advanceTo(IntervalPos, CurPoint);

    if (IntervalPos == Interval->end()) {       // remove expired intervals.
      DEBUG(dbgs() << "\t\tinterval " << *Interval << " expired\n");

      // Pop off the end of the list.
      inactive_[i] = inactive_.back();
      inactive_.pop_back();
      --i; --e;
    } else if (IntervalPos->start <= CurPoint) {
      // move re-activated intervals in active list
      DEBUG(dbgs() << "\t\tinterval " << *Interval << " active\n");
      assert(TargetRegisterInfo::isVirtualRegister(reg) &&
             "Can only allocate virtual registers!");
      reg = vrm_->getPhys(reg);
      addRegUse(reg);
      // add to active
      active_.push_back(std::make_pair(Interval, IntervalPos));

      // Pop off the end of the list.
      inactive_[i] = inactive_.back();
      inactive_.pop_back();
      --i; --e;
    } else {
      // Otherwise, just update the iterator position.
      inactive_[i].second = IntervalPos;
    }
  }
}

/// updateSpillWeights - updates the spill weights of the specifed physical
/// register and its weight.
void RALinScanILP::updateSpillWeights(std::vector<float> &Weights,
                                   unsigned reg, float weight,
                                   const TargetRegisterClass *RC) {
  SmallSet<unsigned, 4> Processed;
  SmallSet<unsigned, 4> SuperAdded;
  SmallVector<unsigned, 4> Supers;
  Weights[reg] += weight;
  Processed.insert(reg);
  for (const unsigned* as = tri_->getAliasSet(reg); *as; ++as) {
    Weights[*as] += weight;
    Processed.insert(*as);
    if (tri_->isSubRegister(*as, reg) &&
        SuperAdded.insert(*as) &&
        RC->contains(*as)) {
      Supers.push_back(*as);
    }
  }

  // If the alias is a super-register, and the super-register is in the
  // register class we are trying to allocate. Then add the weight to all
  // sub-registers of the super-register even if they are not aliases.
  // e.g. allocating for GR32, bh is not used, updating bl spill weight.
  //      bl should get the same spill weight otherwise it will be choosen
  //      as a spill candidate since spilling bh doesn't make ebx available.
  for (unsigned i = 0, e = Supers.size(); i != e; ++i) {
    for (const unsigned *sr = tri_->getSubRegisters(Supers[i]); *sr; ++sr)
      if (!Processed.count(*sr))
        Weights[*sr] += weight;
  }
}

static
RALinScanILP::IntervalPtrs::iterator
FindIntervalInVector(RALinScanILP::IntervalPtrs &IP, LiveInterval *LI) {
  for (RALinScanILP::IntervalPtrs::iterator I = IP.begin(), E = IP.end();
       I != E; ++I)
    if (I->first == LI) return I;
  return IP.end();
}

static void RevertVectorIteratorsTo(RALinScanILP::IntervalPtrs &V,
                                    SlotIndex Point){
  for (unsigned i = 0, e = V.size(); i != e; ++i) {
    RALinScanILP::IntervalPtr &IP = V[i];
    LiveInterval::iterator I = std::upper_bound(IP.first->begin(),
                                                IP.second, Point);
    if (I != IP.first->begin()) --I;
    IP.second = I;
  }
}

#if (defined(LLVM_2_7) || defined(LLVM_2_8))
/// addStackInterval - Create a LiveInterval for stack if the specified live
/// interval has been spilled.
static void addStackInterval(LiveInterval *cur, LiveStacks *ls_,
                             LiveIntervals *li_,
                             MachineRegisterInfo* mri_, VirtRegMap &vrm_) {
  int SS = vrm_.getStackSlot(cur->reg);
  if (SS == VirtRegMap::NO_STACK_SLOT)
    return;

  const TargetRegisterClass *RC = mri_->getRegClass(cur->reg);
  LiveInterval &SI = ls_->getOrCreateInterval(SS, RC);

  VNInfo *VNI;
  if (SI.hasAtLeastOneValue())
    VNI = SI.getValNumInfo(0);
  else
#if defined(LLVM_2_7) || defined(LLVM_2_8)
    VNI = SI.getNextValue(SlotIndex(), 0, false,
                          ls_->getVNInfoAllocator());
#else
    VNI = SI.getNextValue(SlotIndex(), 0,
                          ls_->getVNInfoAllocator());
#endif
  LiveInterval &RI = li_->getInterval(cur->reg);
  // FIXME: This may be overly conservative.
  SI.MergeRangesInAsValue(RI, VNI);
}
#endif

/// getConflictWeight - Return the number of conflicts between cur
/// live interval and defs and uses of Reg weighted by loop depthes.
static
float getConflictWeight(LiveInterval *cur, unsigned Reg, LiveIntervals *li_,
                        MachineRegisterInfo *mri_,
                        const MachineLoopInfo *loopInfo) {
  float Conflicts = 0;
  for (MachineRegisterInfo::reg_iterator I = mri_->reg_begin(Reg),
         E = mri_->reg_end(); I != E; ++I) {
    MachineInstr *MI = &*I;
    if (cur->liveAt(li_->getInstructionIndex(MI))) {
      unsigned loopDepth = loopInfo->getLoopDepth(MI->getParent());
      Conflicts += powf(10.0f, (float)loopDepth);
    }
  }
  return Conflicts;
}

/// findIntervalsToSpill - Determine the intervals to spill for the
/// specified interval. It's passed the physical registers whose spill
/// weight is the lowest among all the registers whose live intervals
/// conflict with the interval.
void RALinScanILP::findIntervalsToSpill(LiveInterval *cur,
                            std::vector<std::pair<unsigned,float> > &Candidates,
                            unsigned NumCands,
                            SmallVector<LiveInterval*, 8> &SpillIntervals) {
  // We have figured out the *best* register to spill. But there are other
  // registers that are pretty good as well (spill weight within 3%). Spill
  // the one that has fewest defs and uses that conflict with cur.
  float Conflicts[3] = { 0.0f, 0.0f, 0.0f };
  SmallVector<LiveInterval*, 8> SLIs[3];

  DEBUG({
      dbgs() << "\tConsidering " << NumCands << " candidates: ";
      for (unsigned i = 0; i != NumCands; ++i)
        dbgs() << tri_->getName(Candidates[i].first) << " ";
      dbgs() << "\n";
    });
  
  // Calculate the number of conflicts of each candidate.
  for (IntervalPtrs::iterator i = active_.begin(); i != active_.end(); ++i) {
    unsigned Reg = i->first->reg;
    unsigned PhysReg = vrm_->getPhys(Reg);
    if (!cur->overlapsFrom(*i->first, i->second))
      continue;
    for (unsigned j = 0; j < NumCands; ++j) {
      unsigned Candidate = Candidates[j].first;
      if (tri_->regsOverlap(PhysReg, Candidate)) {
        if (NumCands > 1)
          Conflicts[j] += getConflictWeight(cur, Reg, li_, mri_, loopInfo);
        SLIs[j].push_back(i->first);
      }
    }
  }

  for (IntervalPtrs::iterator i = inactive_.begin(); i != inactive_.end(); ++i){
    unsigned Reg = i->first->reg;
    unsigned PhysReg = vrm_->getPhys(Reg);
    if (!cur->overlapsFrom(*i->first, i->second-1))
      continue;
    for (unsigned j = 0; j < NumCands; ++j) {
      unsigned Candidate = Candidates[j].first;
      if (tri_->regsOverlap(PhysReg, Candidate)) {
        if (NumCands > 1)
          Conflicts[j] += getConflictWeight(cur, Reg, li_, mri_, loopInfo);
        SLIs[j].push_back(i->first);
      }
    }
  }

  // Which is the best candidate?
  unsigned BestCandidate = 0;
  float MinConflicts = Conflicts[0];
  for (unsigned i = 1; i != NumCands; ++i) {
    if (Conflicts[i] < MinConflicts) {
      BestCandidate = i;
      MinConflicts = Conflicts[i];
    }
  }

  std::copy(SLIs[BestCandidate].begin(), SLIs[BestCandidate].end(),
            std::back_inserter(SpillIntervals));
}

namespace {
  struct WeightCompare {
  private:
    const RALinScanILP &Allocator;

  public:
    WeightCompare(const RALinScanILP &Alloc) : Allocator(Alloc) {}

    typedef std::pair<unsigned, float> RegWeightPair;
    bool operator()(const RegWeightPair &LHS, const RegWeightPair &RHS) const {
      return LHS.second < RHS.second && !Allocator.isRecentlyUsed(LHS.first);
    }
  };
}

static bool weightsAreClose(float w1, float w2) {
  if (!NewHeuristic)
    return false;

  float diff = w1 - w2;
  if (diff <= 0.02f)  // Within 0.02f
    return true;
  return (diff / w2) <= 0.05f;  // Within 5%.
}

LiveInterval *RALinScanILP::hasNextReloadInterval(LiveInterval *cur) {
  DenseMap<unsigned, unsigned>::iterator I = NextReloadMap.find(cur->reg);
  if (I == NextReloadMap.end())
    return 0;
  return &li_->getInterval(I->second);
}

void RALinScanILP::DowngradeRegister(LiveInterval *li, unsigned Reg) {
#if defined(LLVM_2_7) || defined(LLVM_2_8)
  bool isNew = DowngradedRegs.insert(Reg);
  isNew = isNew; // Silence compiler warning.
  assert(isNew && "Multiple reloads holding the same register?");
  DowngradeMap.insert(std::make_pair(li->reg, Reg));
  for (const unsigned *AS = tri_->getAliasSet(Reg); *AS; ++AS) {
    isNew = DowngradedRegs.insert(*AS);
    isNew = isNew; // Silence compiler warning.
    assert(isNew && "Multiple reloads holding the same register?");
    DowngradeMap.insert(std::make_pair(li->reg, *AS));
  }
  ++NumDowngrade;
#else
  for (const unsigned *AS = tri_->getOverlaps(Reg); *AS; ++AS) {
    bool isNew = DowngradedRegs.insert(*AS);
    (void)isNew; // Silence compiler warning.
    assert(isNew && "Multiple reloads holding the same register?");
    DowngradeMap.insert(std::make_pair(li->reg, *AS));
  }
  ++NumDowngrade;
#endif
}

void RALinScanILP::UpgradeRegister(unsigned Reg) {
  if (Reg) {
    DowngradedRegs.erase(Reg);
    for (const unsigned *AS = tri_->getAliasSet(Reg); *AS; ++AS)
      DowngradedRegs.erase(*AS);
  }
}

namespace {
  struct LISorter {
    bool operator()(LiveInterval* A, LiveInterval* B) {
      return A->beginIndex() < B->beginIndex();
    }
  };
}

/// assignRegOrStackSlotAtInterval - assign a register if one is available, or
/// spill.
void RALinScanILP::assignRegOrStackSlotAtInterval(LiveInterval* cur) {
  const TargetRegisterClass *RC = mri_->getRegClass(cur->reg);
  DEBUG(dbgs() << "\tallocating current interval from "
               << RC->getName() << ": ");

  // This is an implicitly defined live interval, just assign any register.
  if (cur->empty()) {
    unsigned physReg = vrm_->getRegAllocPref(cur->reg);
    if (!physReg)
#if defined(LLVM_2_7) || defined(LLVM_2_8)
      physReg = *RC->allocation_order_begin(*mf_);
#else
      physReg = getFirstNonReservedPhysReg(RC);
#endif
    DEBUG(dbgs() <<  tri_->getName(physReg) << '\n');
    // Note the register is not really in use.
    vrm_->assignVirt2Phys(cur->reg, physReg);
    return;
  }

  backUpRegUses();

  std::vector<std::pair<unsigned, float> > SpillWeightsToAdd;
  SlotIndex StartPosition = cur->beginIndex();
  const TargetRegisterClass *RCLeader = RelatedRegClasses.getLeaderValue(RC);

  // If start of this live interval is defined by a move instruction and its
  // source is assigned a physical register that is compatible with the target
  // register class, then we should try to assign it the same register.
  // This can happen when the move is from a larger register class to a smaller
  // one, e.g. X86::mov32to32_. These move instructions are not coalescable.
  if (!vrm_->getRegAllocPref(cur->reg) && cur->hasAtLeastOneValue()) {
    VNInfo *vni = cur->begin()->valno;
#if defined(LLVM_2_7) || defined(LLVM_2_8)
    if ((vni->def != SlotIndex()) && !vni->isUnused() &&
         vni->isDefAccurate()) {
#else // LLVM_2_9
    if (!vni->isUnused() && vni->def.isValid()) {
#endif
      MachineInstr *CopyMI = li_->getInstructionFromIndex(vni->def);
#ifdef LLVM_2_7
      unsigned SrcReg, DstReg, SrcSubReg, DstSubReg;
      if (CopyMI &&
          tii_->isMoveInstr(*CopyMI, SrcReg, DstReg, SrcSubReg, DstSubReg)) {
#else
      if (CopyMI && CopyMI->isCopy()) {
        unsigned DstSubReg = CopyMI->getOperand(0).getSubReg();
        unsigned SrcReg = CopyMI->getOperand(1).getReg();
        unsigned SrcSubReg = CopyMI->getOperand(1).getSubReg();
#endif
        unsigned Reg = 0;
        if (TargetRegisterInfo::isPhysicalRegister(SrcReg))
          Reg = SrcReg;
        else if (vrm_->isAssignedReg(SrcReg))
          Reg = vrm_->getPhys(SrcReg);
        if (Reg) {
          if (SrcSubReg)
            Reg = tri_->getSubReg(Reg, SrcSubReg);
          if (DstSubReg)
            Reg = tri_->getMatchingSuperReg(Reg, DstSubReg, RC);
          if (Reg && allocatableRegs_[Reg] && RC->contains(Reg))
            mri_->setRegAllocationHint(cur->reg, 0, Reg);
        }
      }
    }
  }

  // For every interval in inactive we overlap with, mark the
  // register as not free and update spill weights.
  for (IntervalPtrs::const_iterator i = inactive_.begin(),
         e = inactive_.end(); i != e; ++i) {
    unsigned Reg = i->first->reg;
    assert(TargetRegisterInfo::isVirtualRegister(Reg) &&
           "Can only allocate virtual registers!");
    const TargetRegisterClass *RegRC = mri_->getRegClass(Reg);
    // If this is not in a related reg class to the register we're allocating,
    // don't check it.
    if (RelatedRegClasses.getLeaderValue(RegRC) == RCLeader &&
        cur->overlapsFrom(*i->first, i->second-1)) {
      Reg = vrm_->getPhys(Reg);
      addRegUse(Reg);
      SpillWeightsToAdd.push_back(std::make_pair(Reg, i->first->weight));
    }
  }
  
  // Speculatively check to see if we can get a register right now.  If not,
  // we know we won't be able to by adding more constraints.  If so, we can
  // check to see if it is valid.  Doing an exhaustive search of the fixed_ list
  // is very bad (it contains all callee clobbered registers for any functions
  // with a call), so we want to avoid doing that if possible.
  unsigned physReg = getFreePhysReg(cur);
  unsigned BestPhysReg = physReg;
  if (physReg) {
    // We got a register.  However, if it's in the fixed_ list, we might
    // conflict with it.  Check to see if we conflict with it or any of its
    // aliases.
    SmallSet<unsigned, 8> RegAliases;
    for (const unsigned *AS = tri_->getAliasSet(physReg); *AS; ++AS)
      RegAliases.insert(*AS);
    
    bool ConflictsWithFixed = false;
    for (unsigned i = 0, e = fixed_.size(); i != e; ++i) {
      IntervalPtr &IP = fixed_[i];
      if (physReg == IP.first->reg || RegAliases.count(IP.first->reg)) {
        // Okay, this reg is on the fixed list.  Check to see if we actually
        // conflict.
        LiveInterval *I = IP.first;
        if (I->endIndex() > StartPosition) {
          LiveInterval::iterator II = I->advanceTo(IP.second, StartPosition);
          IP.second = II;
          if (II != I->begin() && II->start > StartPosition)
            --II;
          if (cur->overlapsFrom(*I, II)) {
            ConflictsWithFixed = true;
            break;
          }
        }
      }
    }
    
    // Okay, the register picked by our speculative getFreePhysReg call turned
    // out to be in use.  Actually add all of the conflicting fixed registers to
    // regUse_ so we can do an accurate query.
    if (ConflictsWithFixed) {
      // For every interval in fixed we overlap with, mark the register as not
      // free and update spill weights.
      for (unsigned i = 0, e = fixed_.size(); i != e; ++i) {
        IntervalPtr &IP = fixed_[i];
        LiveInterval *I = IP.first;

        const TargetRegisterClass *RegRC = OneClassForEachPhysReg[I->reg];
        if (RelatedRegClasses.getLeaderValue(RegRC) == RCLeader &&
            I->endIndex() > StartPosition) {
          LiveInterval::iterator II = I->advanceTo(IP.second, StartPosition);
          IP.second = II;
          if (II != I->begin() && II->start > StartPosition)
            --II;
          if (cur->overlapsFrom(*I, II)) {
            unsigned reg = I->reg;
            addRegUse(reg);
            SpillWeightsToAdd.push_back(std::make_pair(reg, I->weight));
          }
        }
      }

      // Using the newly updated regUse_ object, which includes conflicts in the
      // future, see if there are any registers available.
      physReg = getFreePhysReg(cur);
    }
  }
    
  // Restore the physical register tracker, removing information about the
  // future.
  restoreRegUses();
  
  // If we find a free register, we are done: assign this virtual to
  // the free physical register and add this interval to the active
  // list.
  if (physReg) {
    DEBUG(dbgs() <<  tri_->getName(physReg) << '\n');
    vrm_->assignVirt2Phys(cur->reg, physReg);
    addRegUse(physReg);
    active_.push_back(std::make_pair(cur, cur->begin()));
    handled_.push_back(cur);

    /// /TCE
    if (AvoidRegisterReuse) {

#ifdef ROUND_ROBIN_REG_ASSIGN

        if (regEverUsed_[physReg]) {
            // we had to reuse a non-fresh register because the
            // fresh regs and free regs set did not produce non-zero union set
            // reset the freshness to produce more allocation freedom
            TargetRegisterClass::iterator I = RC->allocation_order_begin(*mf_);
            TargetRegisterClass::iterator E = RC->allocation_order_end(*mf_);
            int freeCount = 0;
            int freshCount = 0;
            int freshAndFreeCount = 0;
            for (; I != E; ++I) {
                if (isRegAvail(*I)) freeCount++;
                if (!regEverUsed_[*I]) freshCount++;
                if (isRegAvail(*I) && !regEverUsed_[*I]) freshAndFreeCount++;
            }

            if (freshAndFreeCount == 0) {
		DEBUG(dbgs() << "[reset reg freshness]" << '\n');
                resetFreshness();
            } else {
		DEBUG(dbgs() << "Allocated non-fresh reg " << physReg 
		      << " for some reason even though there were "
		      << freshAndFreeCount << " fresh and free regs." 
		      << '\n'
		      << "The coalescer, phi-node or special register usage?" 
		      << '\n');
            }
        } 
        setRegFresh(physReg, false);
#endif

#ifdef DDG_FDP
        ddg_->assignPhysReg(cur->reg, physReg);
#endif
    }
    /// /TCE

    // "Upgrade" the physical register since it has been allocated.
    UpgradeRegister(physReg);
    if (LiveInterval *NextReloadLI = hasNextReloadInterval(cur)) {
      // "Downgrade" physReg to try to keep physReg from being allocated until
      // the next reload from the same SS is allocated.
      mri_->setRegAllocationHint(NextReloadLI->reg, 0, physReg);
      DowngradeRegister(cur, physReg);
    }
    return;
  }
  DEBUG(dbgs() << "no free registers\n");

  // Compile the spill weights into an array that is better for scanning.
  std::vector<float> SpillWeights(tri_->getNumRegs(), 0.0f);
  for (std::vector<std::pair<unsigned, float> >::iterator
       I = SpillWeightsToAdd.begin(), E = SpillWeightsToAdd.end(); I != E; ++I)
    updateSpillWeights(SpillWeights, I->first, I->second, RC);
  
  // for each interval in active, update spill weights.
  for (IntervalPtrs::const_iterator i = active_.begin(), e = active_.end();
       i != e; ++i) {
    unsigned reg = i->first->reg;
    assert(TargetRegisterInfo::isVirtualRegister(reg) &&
           "Can only allocate virtual registers!");
    reg = vrm_->getPhys(reg);
    updateSpillWeights(SpillWeights, reg, i->first->weight, RC);
  }
 
  DEBUG(dbgs() << "\tassigning stack slot at interval "<< *cur << ":\n");

  // Find a register to spill.
  float minWeight = HUGE_VALF;
  unsigned minReg = 0;

  bool Found = false;
  std::vector<std::pair<unsigned,float> > RegsWeights;
  if (!minReg || SpillWeights[minReg] == HUGE_VALF)
    for (TargetRegisterClass::iterator i = RC->allocation_order_begin(*mf_),
           e = RC->allocation_order_end(*mf_); i != e; ++i) {
      unsigned reg = *i;
      float regWeight = SpillWeights[reg];
#if !(defined(LLVM_2_7) || defined(LLVM_2_8))
      // Don't even consider reserved regs.
      if (reservedRegs_.test(reg))
        continue;
#endif
      // Skip recently allocated registers (and reserved registers)
      if (minWeight > regWeight && !isRecentlyUsed(reg))
        Found = true;
      RegsWeights.push_back(std::make_pair(reg, regWeight));
    }
  
  // If we didn't find a register that is spillable, try aliases?
  if (!Found) {
    for (TargetRegisterClass::iterator i = RC->allocation_order_begin(*mf_),
           e = RC->allocation_order_end(*mf_); i != e; ++i) {
      unsigned reg = *i;
#if !(defined(LLVM_2_7) || defined(LLVM_2_8))
      if (reservedRegs_.test(reg))
        continue;
#endif
      // No need to worry about if the alias register size < regsize of RC.
      // We are going to spill all registers that alias it anyway.
      for (const unsigned* as = tri_->getAliasSet(reg); *as; ++as)
        RegsWeights.push_back(std::make_pair(*as, SpillWeights[*as]));
    }
  }

  // Sort all potential spill candidates by weight.
  std::sort(RegsWeights.begin(), RegsWeights.end(), WeightCompare(*this));
  minReg = RegsWeights[0].first;
  minWeight = RegsWeights[0].second;
  if (minWeight == HUGE_VALF) {
    // All registers must have inf weight. Just grab one!
#if !(defined(LLVM_2_7) || defined(LLVM_2_8))
    minReg = BestPhysReg ? BestPhysReg : getFirstNonReservedPhysReg(RC);
#else
    minReg = BestPhysReg ? BestPhysReg : *RC->allocation_order_begin(*mf_);
#endif
    if (cur->weight == HUGE_VALF ||
        li_->getApproximateInstructionCount(*cur) == 0) {
      // Spill a physical register around defs and uses.
      if (li_->spillPhysRegAroundRegDefsUses(*cur, minReg, *vrm_)) {
        // spillPhysRegAroundRegDefsUses may have invalidated iterator stored
        // in fixed_. Reset them.
        for (unsigned i = 0, e = fixed_.size(); i != e; ++i) {
          IntervalPtr &IP = fixed_[i];
          LiveInterval *I = IP.first;
          if (I->reg == minReg || tri_->isSubRegister(minReg, I->reg))
            IP.second = I->advanceTo(I->begin(), StartPosition);
        }

        DowngradedRegs.clear();
        assignRegOrStackSlotAtInterval(cur);
      } else {
        assert(false && "Ran out of registers during register allocation!");
#ifdef LLVM_2_7
        llvm_report_error("Ran out of registers during register allocation!");
#else
        report_fatal_error("Ran out of registers during register allocation!");
#endif
      }
      return;
    }
  }

  // Find up to 3 registers to consider as spill candidates.
  unsigned LastCandidate = RegsWeights.size() >= 3 ? 3 : 1;
  while (LastCandidate > 1) {
    if (weightsAreClose(RegsWeights[LastCandidate-1].second, minWeight))
      break;
    --LastCandidate;
  }

  DEBUG({
      dbgs() << "\t\tregister(s) with min weight(s): ";

      for (unsigned i = 0; i != LastCandidate; ++i)
        dbgs() << tri_->getName(RegsWeights[i].first)
               << " (" << RegsWeights[i].second << ")\n";
    });

  // If the current has the minimum weight, we need to spill it and
  // add any added intervals back to unhandled, and restart
  // linearscan.
  if (cur->weight != HUGE_VALF && cur->weight <= minWeight) {
    DEBUG(dbgs() << "\t\t\tspilling(c): " << *cur << '\n');
#ifdef LLVM_2_7    
    SmallVector<LiveInterval*, 8> spillIs;
    std::vector<LiveInterval*> added;
    added = spiller_->spill(cur, spillIs); 
#else
    SmallVector<LiveInterval*, 8> spillIs, added;
    spiller_->spill(cur, added, spillIs);
#endif
    std::sort(added.begin(), added.end(), LISorter());
#if (defined(LLVM_2_7) || defined(LLVM_2_8))
    addStackInterval(cur, ls_, li_, mri_, *vrm_);
#endif
    if (added.empty())
      return;  // Early exit if all spills were folded.

    // Merge added with unhandled.  Note that we have already sorted
    // intervals returned by addIntervalsForSpills by their starting
    // point.
    // This also update the NextReloadMap. That is, it adds mapping from a
    // register defined by a reload from SS to the next reload from SS in the
    // same basic block.
    MachineBasicBlock *LastReloadMBB = 0;
    LiveInterval *LastReload = 0;
    int LastReloadSS = VirtRegMap::NO_STACK_SLOT;
    for (unsigned i = 0, e = added.size(); i != e; ++i) {
      LiveInterval *ReloadLi = added[i];
      if (ReloadLi->weight == HUGE_VALF &&
          li_->getApproximateInstructionCount(*ReloadLi) == 0) {
        SlotIndex ReloadIdx = ReloadLi->beginIndex();
        MachineBasicBlock *ReloadMBB = li_->getMBBFromIndex(ReloadIdx);
        int ReloadSS = vrm_->getStackSlot(ReloadLi->reg);
        if (LastReloadMBB == ReloadMBB && LastReloadSS == ReloadSS) {
          // Last reload of same SS is in the same MBB. We want to try to
          // allocate both reloads the same register and make sure the reg
          // isn't clobbered in between if at all possible.
          assert(LastReload->beginIndex() < ReloadIdx);
          NextReloadMap.insert(std::make_pair(LastReload->reg, ReloadLi->reg));
        }
        LastReloadMBB = ReloadMBB;
        LastReload = ReloadLi;
        LastReloadSS = ReloadSS;
      }
      unhandled_.push(ReloadLi);
    }
    return;
  }

  ++NumBacktracks;

  // Push the current interval back to unhandled since we are going
  // to re-run at least this iteration. Since we didn't modify it it
  // should go back right in the front of the list
  unhandled_.push(cur);

  assert(TargetRegisterInfo::isPhysicalRegister(minReg) &&
         "did not choose a register to spill?");

  // We spill all intervals aliasing the register with
  // minimum weight, rollback to the interval with the earliest
  // start point and let the linear scan algorithm run again
  SmallVector<LiveInterval*, 8> spillIs;

  // Determine which intervals have to be spilled.
  findIntervalsToSpill(cur, RegsWeights, LastCandidate, spillIs);

  // Set of spilled vregs (used later to rollback properly)
  SmallSet<unsigned, 8> spilled;

  // The earliest start of a Spilled interval indicates up to where
  // in handled we need to roll back
  assert(!spillIs.empty() && "No spill intervals?");
  SlotIndex earliestStart = spillIs[0]->beginIndex();
  
  // Spill live intervals of virtual regs mapped to the physical register we
  // want to clear (and its aliases).  We only spill those that overlap with the
  // current interval as the rest do not affect its allocation. we also keep
  // track of the earliest start of all spilled live intervals since this will
  // mark our rollback point.
#ifdef LLVM_2_7
  std::vector<LiveInterval*> added;
#else
  SmallVector<LiveInterval*, 8> added;
#endif
  while (!spillIs.empty()) {
    LiveInterval *sli = spillIs.back();
    spillIs.pop_back();
    DEBUG(dbgs() << "\t\t\tspilling(a): " << *sli << '\n');
    if (sli->beginIndex() < earliestStart)
      earliestStart = sli->beginIndex();
       
    std::vector<LiveInterval*> newIs;
#ifdef LLVM_2_7
    newIs = spiller_->spill(sli, spillIs, &earliestStart);
    addStackInterval(sli, ls_, li_, mri_, *vrm_);
    std::copy(newIs.begin(), newIs.end(), std::back_inserter(added));
#else
    spiller_->spill(sli, added, spillIs);
#ifdef LLVM_2_8
    addStackInterval(sli, ls_, li_, mri_, *vrm_);
#endif
#endif
    spilled.insert(sli->reg);
  }

  DEBUG(dbgs() << "\t\trolling back to: " << earliestStart << '\n');

  // Scan handled in reverse order up to the earliest start of a
  // spilled live interval and undo each one, restoring the state of
  // unhandled.
  while (!handled_.empty()) {
    LiveInterval* i = handled_.back();
    // If this interval starts before t we are done.
    if (!i->empty() && i->beginIndex() < earliestStart)
      break;
    DEBUG(dbgs() << "\t\t\tundo changes for: " << *i << '\n');
    handled_.pop_back();

    // When undoing a live interval allocation we must know if it is active or
    // inactive to properly update regUse_ and the VirtRegMap.
    IntervalPtrs::iterator it;
    if ((it = FindIntervalInVector(active_, i)) != active_.end()) {
      active_.erase(it);
      assert(!TargetRegisterInfo::isPhysicalRegister(i->reg));
      if (!spilled.count(i->reg))
        unhandled_.push(i);
      delRegUse(vrm_->getPhys(i->reg));
      vrm_->clearVirt(i->reg);
    } else if ((it = FindIntervalInVector(inactive_, i)) != inactive_.end()) {
      inactive_.erase(it);
      assert(!TargetRegisterInfo::isPhysicalRegister(i->reg));
      if (!spilled.count(i->reg))
        unhandled_.push(i);
      vrm_->clearVirt(i->reg);
    } else {
      assert(TargetRegisterInfo::isVirtualRegister(i->reg) &&
             "Can only allocate virtual registers!");
      vrm_->clearVirt(i->reg);
      unhandled_.push(i);
    }

    DenseMap<unsigned, unsigned>::iterator ii = DowngradeMap.find(i->reg);
    if (ii == DowngradeMap.end())
      // It interval has a preference, it must be defined by a copy. Clear the
      // preference now since the source interval allocation may have been
      // undone as well.
      mri_->setRegAllocationHint(i->reg, 0, 0);
    else {
      UpgradeRegister(ii->second);
    }
  }

  // Rewind the iterators in the active, inactive, and fixed lists back to the
  // point we reverted to.
  RevertVectorIteratorsTo(active_, earliestStart);
  RevertVectorIteratorsTo(inactive_, earliestStart);
  RevertVectorIteratorsTo(fixed_, earliestStart);

  // Scan the rest and undo each interval that expired after t and
  // insert it in active (the next iteration of the algorithm will
  // put it in inactive if required)
  for (unsigned i = 0, e = handled_.size(); i != e; ++i) {
    LiveInterval *HI = handled_[i];
    if (!HI->expiredAt(earliestStart) &&
        HI->expiredAt(cur->beginIndex())) {
      DEBUG(dbgs() << "\t\t\tundo changes for: " << *HI << '\n');
      active_.push_back(std::make_pair(HI, HI->begin()));
      assert(!TargetRegisterInfo::isPhysicalRegister(HI->reg));
      addRegUse(vrm_->getPhys(HI->reg));
    }
  }

  // Merge added with unhandled.
  // This also update the NextReloadMap. That is, it adds mapping from a
  // register defined by a reload from SS to the next reload from SS in the
  // same basic block.
  MachineBasicBlock *LastReloadMBB = 0;
  LiveInterval *LastReload = 0;
  int LastReloadSS = VirtRegMap::NO_STACK_SLOT;
  std::sort(added.begin(), added.end(), LISorter());
  for (unsigned i = 0, e = added.size(); i != e; ++i) {
    LiveInterval *ReloadLi = added[i];
    if (ReloadLi->weight == HUGE_VALF &&
        li_->getApproximateInstructionCount(*ReloadLi) == 0) {
      SlotIndex ReloadIdx = ReloadLi->beginIndex();
      MachineBasicBlock *ReloadMBB = li_->getMBBFromIndex(ReloadIdx);
      int ReloadSS = vrm_->getStackSlot(ReloadLi->reg);
      if (LastReloadMBB == ReloadMBB && LastReloadSS == ReloadSS) {
        // Last reload of same SS is in the same MBB. We want to try to
        // allocate both reloads the same register and make sure the reg
        // isn't clobbered in between if at all possible.
        assert(LastReload->beginIndex() < ReloadIdx);
        NextReloadMap.insert(std::make_pair(LastReload->reg, ReloadLi->reg));
      }
      LastReloadMBB = ReloadMBB;
      LastReload = ReloadLi;
      LastReloadSS = ReloadSS;
    }
    unhandled_.push(ReloadLi);
  }
}

unsigned RALinScanILP::getFreePhysReg(LiveInterval* cur,
                                   const TargetRegisterClass *RC,
                                   unsigned MaxInactiveCount,
                                   SmallVector<unsigned, 256> &inactiveCounts,
                                   bool SkipDGRegs) {
  unsigned FreeReg = 0;
  unsigned FreeRegInactiveCount = 0;

  std::pair<unsigned, unsigned> Hint = mri_->getRegAllocationHint(cur->reg);
  // Resolve second part of the hint (if possible) given the current allocation.
  unsigned physReg = Hint.second;
#if (defined(LLVM_2_7) || defined(LLVM_2_8))
  if (physReg &&
      TargetRegisterInfo::isVirtualRegister(physReg) && vrm_->hasPhys(physReg))
#else
  if (TargetRegisterInfo::isVirtualRegister(physReg) && vrm_->hasPhys(physReg))
#endif
    physReg = vrm_->getPhys(physReg);


// TCE
  if (AvoidRegisterReuse) {

#ifdef ROUND_ROBIN_REG_ASSIGN
      // Scan for the first available register that has not been
      // used yet in this function since the last freshness reset
      int freeCount = 0;
      int freshCount = 0;
      TargetRegisterClass::iterator I = RC->allocation_order_begin(*mf_);
      TargetRegisterClass::iterator E = RC->allocation_order_end(*mf_);
      for (; I != E; ++I) {
#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
	  // Skip reserved registers.
	  if (reservedRegs_.test(*I))
	      continue;
#endif
          if (regUse_[*I] == 0) freeCount++;
          if (!regEverUsed_[*I]) freshCount++;
          if (regUse_[*I] == 0 && !regEverUsed_[*I]) {
              FreeReg = *I;
              if (FreeReg < inactiveCounts.size())
                  FreeRegInactiveCount = inactiveCounts[FreeReg];
              else
                  FreeRegInactiveCount = 0;
              break;
          } 
      }
      if (FreeReg == 0) {
	  DEBUG(dbgs() << "no fresh (" << freshCount << ") and free ("
		<< freeCount << ") found" << '\n');
      }
#else

# ifdef FDPG_REG_ASSIGN
      // use false dependency avoidance heuristics
        
      // check if the assignment of a register would introduce a false dep
      // with a previously assigned register, try to choose a register that
      // does not
      TargetRegisterClass::iterator I = RC->allocation_order_begin(*mf_);
      TargetRegisterClass::iterator E = RC->allocation_order_end(*mf_);
      int minFalseDepsFromAssign = INT_MAX;
      for (; I != E; ++I) {
          unsigned physRegCandidate = *I;
#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
	  // Skip reserved registers.
	  if (reservedRegs_.test(physRegCandidate))
	      continue;
#endif
          if (!isRegAvail(physRegCandidate))
              continue;
          if (FreeReg == 0)
              FreeReg = physRegCandidate;

          int falseDeps = fdpg_->newFalseDepsFromAssign(cur, physRegCandidate); 
          // TODO: maybe even spill instead of a false dep in some 
          // cases where 
          // the false dep restricts a big chunk of code moving up

          // check the aliases for the reg
          for (const unsigned* as = tri_->getAliasSet(physRegCandidate); 
               *as; ++as) {
              falseDeps = std::max(
                  falseDeps, fdpg_->newFalseDepsFromAssign(cur, *as));
          }

          if (falseDeps < minFalseDepsFromAssign) {
              FreeReg = physRegCandidate;
              minFalseDepsFromAssign = falseDeps;
#if 0
              Application::logStream()
                  << "assigning " << FreeReg << " to " << cur->reg
                  << " would cause " << minFalseDepsFromAssign << " fdeps"
                  << std::endl;
#endif
              if (falseDeps == 0)
                  break;
          }
      }
#if 0
      if (FreeReg > 0) {
          Application::logStream()
              << "assigning " << FreeReg << " to " << cur->reg
              << " causes " << minFalseDepsFromAssign << " fdeps"
              << std::endl;
          TCETargetMachine& tceMach = 
              *const_cast<TCETargetMachine*>(
                  dynamic_cast<const TCETargetMachine*>(tm_));
          Application::logStream()
              << tceMach.rfName(FreeReg) << "." 
              << tceMach.registerIndex(FreeReg) << std::endl;
      }
#endif

      if (FreeReg < inactiveCounts.size())
          FreeRegInactiveCount = inactiveCounts[FreeReg];
      else
          FreeRegInactiveCount = 0;
# else

#  ifdef DDG_FDP

      /*
        Finds a register assignment that minimizes the "height delta"
        of a false dep added to the DDG.

        Negative hdelta indicates that the false dependence does not
        affect parallelization as there are other more constraining
        (true) edges in the path. 

        Positive hdelta indicates that the added false dep edge increases
        the schedule length as the edge goes upwards in the graph,
        thus constraints nodes that were not previously constrained by
        nodes at that height level.       

        Prefer registers in this order:
        - register already used by the variable defining instruction
        - register that creates close to (negative) zero hdelta
        - register that does not create antideps
        TODO
       */
      

      int minHdeltaFound = INT_MAX;
      /* First try to use the same register that is used as an
         operand register for the instruction defining the variable.
         It should not produce an antidep that increases the longest path and 
         reduces allocation greediness. */
      llvm::MachineInstr* definer = mri_->getVRegDef(cur->reg);      
      for (unsigned operand = 0; 
           definer != NULL && operand < definer->getNumOperands(); 
           ++operand) {
          const llvm::MachineOperand& mo = definer->getOperand(operand);
          if (!mo.isReg() || 
              !TargetRegisterInfo::isPhysicalRegister(mo.getReg()) ||
              !isRegAvail(mo.getReg()))
              continue;

          if (SkipDGRegs && DowngradedRegs.count(mo.getReg()))
              continue;

          // found an operand register that is free for the variable
          FreeReg = mo.getReg();
          minHdeltaFound = ddg_->falseDepHeightDelta(cur->reg, FreeReg);
          Application::logStream() << "using the same reg as used by an operand" << std::endl;
          break;
      }

      TargetRegisterClass::iterator I = RC->allocation_order_begin(*mf_);
      TargetRegisterClass::iterator E = RC->allocation_order_end(*mf_);
      unsigned BestFound = 0;
      for (; I != E && FreeReg == 0; ++I) {
          unsigned physRegCandidate = *I;
#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
	  // Skip reserved registers.
	  if (reservedRegs_.test(physRegCandidate))
	      continue;
#endif

          if (!isRegAvail(physRegCandidate))
              continue;

          if (SkipDGRegs && DowngradedRegs.count(physRegCandidate))
              continue;

          if (BestFound == 0)
              BestFound = physRegCandidate;

          int hdelta = ddg_->falseDepHeightDelta(cur->reg, physRegCandidate);

          // check the aliases for the reg
          for (const unsigned* as = tri_->getAliasSet(physRegCandidate); 
               *as; ++as) {
              hdelta = std::max(
                  hdelta, ddg_->falseDepHeightDelta(cur->reg, *as));
          }
#if 0
          // the hdelta contains the largest hdelta of an introduced
          // antidep, try to find largest negative one 
          if (minHdeltaFound < 0 && hdelta < 0 && hdelta > minHdeltaFound) {
              BestFound = physRegCandidate;
              minHdeltaFound = hdelta;
          } else if (minHdeltaFound >= 0 && hdelta < minHdeltaFound) {
              BestFound = physRegCandidate;
              minHdeltaFound = hdelta;
          } 
#endif
          if (hdelta < minHdeltaFound) {
              BestFound = physRegCandidate;
              minHdeltaFound = hdelta;
          }
          if (minHdeltaFound < 0 && minHdeltaFound > -3)
              break;

#if 0
          Application::logStream()
              << "assigning " << physRegCandidate << " to " 
              << cur->reg << " has "
              << hdelta << " hdelta " << std::endl;
#endif

      }
      if (FreeReg == 0)
          FreeReg = BestFound;
#if 1      
      if (FreeReg > 0) {
          Application::logStream()
              << "assigning " << FreeReg << " to " << cur->reg
              << " causes " << minHdeltaFound << " hdelta"
              << std::endl;
          TCETargetMachine& tceMach = 
              *const_cast<TCETargetMachine*>(
                  dynamic_cast<const TCETargetMachine*>(tm_));
          Application::logStream()
              << tceMach.rfName(FreeReg) << "." 
              << tceMach.registerIndex(FreeReg) << std::endl;
      }
#endif

      if (FreeReg < inactiveCounts.size())
          FreeRegInactiveCount = inactiveCounts[FreeReg];
      else
          FreeRegInactiveCount = 0;


#  endif /* DDG_FPG */



# endif /* FDPG_REG_ASSIGN */

#endif
  } 
  // /TCE
  TargetRegisterClass::iterator I, E;
  tie(I, E) = tri_->getAllocationOrder(RC, Hint.first, physReg, *mf_);
  assert(I != E && "No allocatable register in this register class!");

  // outer if added by ilp code/TCE
  if (!FreeReg) {
  // Scan for the first available register.
  for (; I != E; ++I) {
    unsigned Reg = *I;
    // Ignore "downgraded" registers.
    if (SkipDGRegs && DowngradedRegs.count(Reg))
      continue;
#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
    // Skip reserved registers.
    if (reservedRegs_.test(Reg))
	continue;
#endif
    // Skip recently allocated registers.
    if (isRegAvail(Reg) && !isRecentlyUsed(Reg)) {
      FreeReg = Reg;
      if (FreeReg < inactiveCounts.size())
        FreeRegInactiveCount = inactiveCounts[FreeReg];
      else
        FreeRegInactiveCount = 0;
      break;
    }
    }
  }
  
  // If there are no free regs, or if this reg has the max inactive count,
  // return this register.
  if (FreeReg == 0 || FreeRegInactiveCount == MaxInactiveCount) {
    // Remember what register we picked so we can skip it next time.

      // TODO: this was removed on old trunk - why? 
      // or was this added on this side?
      //if (FreeReg != 0) recordRecentlyUsed(FreeReg);
    return FreeReg;
  }
  
  // Continue scanning the registers, looking for the one with the highest
  // inactive count.  Alkis found that this reduced register pressure very
  // slightly on X86 (in rev 1.94 of this file), though this should probably be
  // reevaluated now.
  for (; I != E; ++I) {
    unsigned Reg = *I;
    // Ignore "downgraded" registers.
    if (SkipDGRegs && DowngradedRegs.count(Reg))
      continue;
#if (!(defined(LLVM_2_7) || defined(LLVM_2_8)))
    // Skip reserved registers.
    if (reservedRegs_.test(Reg))
	continue;
#endif
    if (isRegAvail(Reg) && Reg < inactiveCounts.size() &&
        FreeRegInactiveCount < inactiveCounts[Reg] && !isRecentlyUsed(Reg)) {
      FreeReg = Reg;
      FreeRegInactiveCount = inactiveCounts[Reg];
      if (FreeRegInactiveCount == MaxInactiveCount)
        break;    // We found the one with the max inactive count.
    }
  }
  
  // Remember what register we picked so we can skip it next time.
  recordRecentlyUsed(FreeReg);

  return FreeReg;
  }

/// TCE
/// reset the book keeping for register "freshness" for ILP enhanced
/// allocation
void RALinScanILP::resetFreshness() {
    for (std::size_t i = 0; i < regEverUsed_.size(); ++i) {
        regEverUsed_[i] = false;
    }
}

/// /TCE

/// getFreePhysReg - return a free physical register for this virtual register
/// interval if we have one, otherwise return 0.
unsigned RALinScanILP::getFreePhysReg(LiveInterval *cur) {
  SmallVector<unsigned, 256> inactiveCounts;
  unsigned MaxInactiveCount = 0;
  
  const TargetRegisterClass *RC = mri_->getRegClass(cur->reg);
  const TargetRegisterClass *RCLeader = RelatedRegClasses.getLeaderValue(RC);
 
  for (IntervalPtrs::iterator i = inactive_.begin(), e = inactive_.end();
       i != e; ++i) {
    unsigned reg = i->first->reg;
    assert(TargetRegisterInfo::isVirtualRegister(reg) &&
           "Can only allocate virtual registers!");

    // If this is not in a related reg class to the register we're allocating, 
    // don't check it.
    const TargetRegisterClass *RegRC = mri_->getRegClass(reg);
    if (RelatedRegClasses.getLeaderValue(RegRC) == RCLeader) {
      reg = vrm_->getPhys(reg);
      if (inactiveCounts.size() <= reg)
        inactiveCounts.resize(reg+1);
      ++inactiveCounts[reg];
      MaxInactiveCount = std::max(MaxInactiveCount, inactiveCounts[reg]);
    }
  }

  // If copy coalescer has assigned a "preferred" register, check if it's
  // available first.
  unsigned Preference = vrm_->getRegAllocPref(cur->reg);
  if (Preference) {
    DEBUG(dbgs() << "(preferred: " << tri_->getName(Preference) << ") ");
    if (isRegAvail(Preference) &&
        RC->contains(Preference))
      return Preference;
#if 0
    Application::logStream() 
        << "could not use preferred, avail: " << isRegAvail(Preference) 
        << " RC->contains: " << RC->contains(Preference) << std::endl;
#endif
  }

  if (!DowngradedRegs.empty()) {
    unsigned FreeReg = getFreePhysReg(cur, RC, MaxInactiveCount, inactiveCounts,
                                      true);
    if (FreeReg)
      return FreeReg;
  }
  return getFreePhysReg(cur, RC, MaxInactiveCount, inactiveCounts, false);
}

FunctionPass* createILPLinearScanRegisterAllocator() {
  return new RALinScanILP();
}
