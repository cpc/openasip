//===-- llvm/CodeGen/Spiller.h - Spiller -*- C++ -*------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_SPILLER_H
#define LLVM_CODEGEN_SPILLER_H

#include "tce_config.h"

#include "llvm/ADT/SmallVector.h"

namespace llvm {

  class LiveInterval;
  class MachineFunction;
  class MachineFunctionPass;
  class SlotIndex;
  class VirtRegMap;

  /// Spiller interface.
  ///
  /// Implementations are utility classes which insert spill or remat code on
  /// demand.
  class Spiller {
  public:
    virtual ~Spiller() = 0;

    /// spill - Spill the given live interval. The method used will depend on
    /// the Spiller implementation selected.
    ///
    /// @param li            The live interval to be spilled.
    /// @param spillIs       A list of intervals that are about to be spilled,
    ///                      and so cannot be used for remat etc.
    /// @param newIntervals  The newly created intervals will be appended here.
#if (defined(LLVM_2_9))
    virtual void spill(LiveInterval *li,
                       SmallVectorImpl<LiveInterval*> &newIntervals,
                       SmallVectorImpl<LiveInterval*> &spillIs) = 0;

#else
      virtual void spill(LiveRangeEdit& LRE) = 0;
#endif
  };

  /// Create and return a spiller object, as specified on the command line.
  Spiller* createSpiller(MachineFunctionPass &pass,
                         MachineFunction &mf,
                         VirtRegMap &vrm);

#if (!(defined(LLVM_2_9)))
  /// Create and return a spiller that will insert spill code directly instead
  /// of deferring though VirtRegMap.
  Spiller *createInlineSpiller(MachineFunctionPass &pass,
                               MachineFunction &mf,
                               VirtRegMap &vrm);
#endif
}

#endif
