/*
    Copyright (c) 2012-2020 Tampere University.

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
 * @file PRegionMarkerAnalyzer.cc
 *
 * @author Pekka Jääskeläinen 2012,2020
 */

#include <climits>

#include "tce_config.h"
#include "TCEString.hh"
#include "Conversion.hh"
#include "PRegionMarkerAnalyzer.hh"

#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")
IGNORE_COMPILER_WARNING("-Wcomment")

#include <llvm/CodeGen/MachineFunction.h>
#include <llvm/IR/Function.h>

//#define DEBUG_PREGIONS

using namespace llvm;

PRegionMarkerAnalyzer::PRegionMarkerAnalyzer(
    const llvm::MachineFunction& MF_) : MF(MF_), markersFound_(false) {

    for (MachineFunction::const_iterator i = MF.begin();
         i != MF.end() && !markersFound_; i++) {        
        for (MachineBasicBlock::const_iterator j = i->begin();
             j != i->end(); j++) {
            const llvm::MachineInstr& mi = *j; 
            if (isPregionStartMarker(mi) || isPregionEndMarker(mi)) {
                markersFound_ = true;
                break;
            }
        }
    }

    if (!markersFound_) return;
    findPregions();
}

void
PRegionMarkerAnalyzer::propagatePregionID(
    const llvm::MachineInstr& start, unsigned id) {
    const llvm::MachineBasicBlock* mbb = start.getParent();

    for (MachineBasicBlock::const_iterator i = &start; 
         i != mbb->end(); 
         ++i) {
#if LLVM_OLDER_THAN_4_0
        const llvm::MachineInstr* mi = i;
#else
        const llvm::MachineInstr* mi = &*i;
#endif
        if (mi != &start && 
            (isPregionEndMarker(*mi) || isPregionStartMarker(*mi)))
            return;

        if (mi->isCall() && !mi->isInlineAsm()) return;

#ifdef DEBUG_PREGIONS
        std::cerr << "### PRA: propagating region id " << id << " to ";
        mi->dump();                
#endif 
        pregionIDs_[mi] = id;
    }
    for (MachineBasicBlock::const_succ_iterator i = mbb->succ_begin();
         i != mbb->succ_end(); ++i) {
        const llvm::MachineBasicBlock* child = *i;
        const llvm::MachineInstr* first = &child->front();
        // avoid looping forever by checking whether the info
        // has been propagated already to the recusrively
        // traversed basic blocks
        if (pregionIDs_.find(first) == pregionIDs_.end() &&
            !isPregionStartMarker(*first) && !isPregionEndMarker(*first))
            propagatePregionID(child->front(), id);
    }
}

/**
 * Finds the parallel region markers and propagates the
 * parallel region ids to the instructions dominated by
 * it.
 */
void
PRegionMarkerAnalyzer::findPregions() {

    for (MachineFunction::const_iterator i = MF.begin();
         i != MF.end(); i++) {        
        for (MachineBasicBlock::const_iterator j = i->begin();
             j != i->end(); j++) {
            const llvm::MachineInstr& mi = *j; 
            if (!isPregionStartMarker(mi))
                continue;
            unsigned pregionId = parsePregionID(mi);
            propagatePregionID(mi, pregionId);
        }
    }
}

bool
PRegionMarkerAnalyzer::markersFound() const {
    return markersFound_;
}

bool
PRegionMarkerAnalyzer::isPregionEndMarker(const llvm::MachineInstr &I) const {
    // Count the number of register definitions.
    if (!I.isInlineAsm()) return false;
    unsigned numDefs = 0;
    while (I.getOperand(numDefs).isReg() &&
           I.getOperand(numDefs).isDef())
        ++numDefs;
    TCEString opName = I.getOperand(numDefs).getSymbolName();
    return opName.startsWith(".pregion_end");
}

bool
PRegionMarkerAnalyzer::isPregionStartMarker(const llvm::MachineInstr &I) const {
    if (!I.isInlineAsm()) return false;
    // Count the number of register definitions.
    unsigned numDefs = 0;
    while (I.getOperand(numDefs).isReg() &&
           I.getOperand(numDefs).isDef())
        ++numDefs;
    TCEString opName = I.getOperand(numDefs).getSymbolName();
    return opName.startsWith(".pregion_start.");
}

unsigned
PRegionMarkerAnalyzer::parsePregionID(const llvm::MachineInstr &I) const {
    if (!isPregionStartMarker(I)) return UINT_MAX;

    unsigned numDefs = 0;
    while (I.getOperand(numDefs).isReg() &&
           I.getOperand(numDefs).isDef())
        ++numDefs;

    TCEString opName = I.getOperand(numDefs).getSymbolName();

    return Conversion::toUnsignedInt(opName.split(".").at(2));
}

unsigned
PRegionMarkerAnalyzer::pregion(const llvm::MachineInstr &I) const {
    if (pregionIDs_.find(&I) != pregionIDs_.end())
        return (*pregionIDs_.find(&I)).second;
    else
        return UINT_MAX;
}

