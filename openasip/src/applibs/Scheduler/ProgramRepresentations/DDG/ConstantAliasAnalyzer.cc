/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ConstantAliasAnalyzer.cc
 *
 * Implementation of ConstantAliasAnalyzer class.
 * 
 * This class does simple alias analysis between memove addresses
 * that come directly from immediates.
 *
 * @author Heikki Kultala 2007 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "ConstantAliasAnalyzer.hh"

#include "MoveNode.hh"
#include "Move.hh"
#include "DataDependenceGraph.hh"
#include "Terminal.hh"
#include "Operation.hh"
#include "MoveNodeSet.hh"

using namespace TTAProgram;
using namespace TTAMachine;



bool ConstantAliasAnalyzer::getConstantAddress(
    DataDependenceGraph& ddg, const ProgramOperation& po,
    long& addr, long& loopIncrement) {
    const MoveNode* mn = addressOperandMove(po);
    addr = 0;
    while (mn != NULL && mn->isMove()) {
        if (mn->isSourceVariable()) {
            MoveNode* prevSrc = ddg.onlyRegisterRawSource(*mn,2,2);
            MoveNode* loopSrc = ddg.onlyRegisterRawSource(*mn,2,1);
            if (prevSrc == NULL) {
                break;
            }
            if (loopSrc) {
                if (!findIncrement(*loopSrc, loopIncrement)) {
                    return false;
                }
            }
            mn = prevSrc;
        } else {
            if (mn->isSourceOperation()) {
                const MoveNode* incrementInput = findIncrement(*mn, addr);
                if (incrementInput != NULL) {
                    mn = incrementInput;
                } else {
                    mn = searchLoopIndexBasedIncrement(ddg, *mn, loopIncrement);
                }
            } else {
                if (mn->isMove()) {
                    const Move& move = mn->move();
                    if (!move.isFunctionCall()) {
                        const Terminal& src = move.source();
                        if (src.isImmediate()) {
                            addr += src.value().unsignedValue();
                            return true;
                        }
                    }
                }
                return false;
            }
        }
    }
    return false;
}

bool
ConstantAliasAnalyzer::isAddressTraceable(
    DataDependenceGraph& ddg, const ProgramOperation& po) {
    long tmp;
    long tmp2 = 0;
    return getConstantAddress(ddg, po, tmp, tmp2);
}

// TODO: does not handle unaligned 64-bit memory operations well.
MemoryAliasAnalyzer::AliasingResult
ConstantAliasAnalyzer::analyze(
    DataDependenceGraph& ddg, 
    const ProgramOperation& pop1, 
    const ProgramOperation& pop2, 
    MoveNodeUse::BBRelation bbRel) {

    long addr1, addr2;
    long inc1 = 0, inc2 = 0;

    if (!getConstantAddress(ddg, pop1, addr1, inc1) ||
        !getConstantAddress(ddg, pop2, addr2, inc2)) {
        return ALIAS_UNKNOWN;
    }

    // if updated different amount, may overlap?
    if (inc1 != inc2) {
        return ALIAS_UNKNOWN;
    }

    if (bbRel != MoveNodeUse::LOOP) {
        return compareIndeces(addr1, addr2, pop1, pop2);
    } else {
        return compareIndeces(addr1, addr2+inc2, pop1, pop2);
    }
}

ConstantAliasAnalyzer::~ConstantAliasAnalyzer() {}
