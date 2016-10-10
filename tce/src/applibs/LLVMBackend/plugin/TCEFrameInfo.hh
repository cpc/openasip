/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file TCEFrameInfo.hh
 *
 * Declaration of TCEFrameInfo class.
 *
 * @author Heikki Kultala 2010 (hkultala-no.spam-cs.tut.fi)
 */

#ifndef TCE_FRAME_INFO_H
#define TCE_FRAME_INFO_H

#include "tce_config.h"

#include <llvm/Support/ErrorHandling.h>
#include <llvm/Target/TargetFrameLowering.h>
#include "TCERegisterInfo.hh"
#include "TCEInstrInfo.hh"

namespace llvm {

    /** !! Important !! *************
     * ON EVERY LLVM UPDATE CHECK THESE INTERFACES VERY CAREFULLY
     * FROM include/llvm/Target/TargetFrameInfo.h
     *
     * Compiler doesn warn or give error if parameter lists are changed.
     * Many times also base class implementation works, but does not do
     * very good job.
     */

//  Frame info:
// -------------
// Grows down, alignment at least 4 bytes.
//

#ifndef LLVM_OLDER_THAN_3_7
// LLVM 3.7 tdgen expects the name class TCEFrameLowering
#define TCEFrameInfo TCEFrameLowering 
#endif

    class TCEFrameInfo : public TargetFrameLowering {
    public:

    /** !! Important !! *************
     * If the last boolean parameter of the constructor is set to false, 
     * stack realignment is not allowed. This means, that every stack object
     * having a bigger alignment than the stack's own alignment, will be 
     * reduced to have the stack's alignment.
     */
        TCEFrameInfo(TCERegisterInfo* tri, const TCEInstrInfo* tii, int stackAlignment) :
        TargetFrameLowering(
            TargetFrameLowering::StackGrowsDown, 
            stackAlignment, 
            -stackAlignment,
            1,
            true /*false*/),
        tri_(tri), tii_(*tii), stackAlignment_(stackAlignment) {
            tri->setTFI(this); }

#ifdef LLVM_OLDER_THAN_3_9
        void
#else
        MachineBasicBlock::iterator
#endif
        eliminateCallFramePseudoInstr(
            MachineFunction &MF,
            MachineBasicBlock &MBB,
            MachineBasicBlock::iterator I) const override;

#ifdef LLVM_OLDER_THAN_3_7
	void emitPrologue(MachineFunction &mf) const override;
#else
    void emitPrologue(MachineFunction &mf, MachineBasicBlock &MBB) const override;
#endif
        void emitEpilogue(MachineFunction &mf, MachineBasicBlock &MBB) const override;
        bool hasFP(const MachineFunction &MF) const override;
        int stackAlignment() const { return stackAlignment_; }
        bool containsCall(MachineFunction& mf) const;
    private:
        int stackAlignment_;
        const TCERegisterInfo* tri_;
        const TCEInstrInfo& tii_;
    };
} // /namespace

#endif
