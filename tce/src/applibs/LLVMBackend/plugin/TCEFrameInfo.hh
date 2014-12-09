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
    class TCEFrameInfo : public TargetFrameLowering {
    public:

    /** !! Important !! *************
     * If the last boolean parameter of the constructor is set to false, 
     * stack realignment is not allowed. This means, that every stack object
     * having a bigger alignment than the stack's own alignment, will be 
     * reduced to have the stack's alignment.
     */
        TCEFrameInfo(const TCERegisterInfo* tri, int stackAlignment) : 
#ifdef LLVM_3_2
        TargetFrameLowering(
            TargetFrameLowering::StackGrowsDown, 
            stackAlignment, 
            -stackAlignment, 
            1), 
        tri_(tri) {}
#else
        TargetFrameLowering(
            TargetFrameLowering::StackGrowsDown, 
            stackAlignment, 
            -stackAlignment,
            1,
            true /*false*/),
        tri_(tri) {}

        void eliminateCallFramePseudoInstr(
            MachineFunction &MF,
            MachineBasicBlock &MBB,
            MachineBasicBlock::iterator I) const override;
#endif

	void emitPrologue(MachineFunction &mf) const override;
	void emitEpilogue(MachineFunction &mf, MachineBasicBlock &MBB) const override;
	bool hasFP(const MachineFunction &MF) const override { return false; }

    private:
	const TCERegisterInfo* tri_;
    };
} // /namespace

#endif
