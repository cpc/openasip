/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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

using namespace TTAProgram;
using namespace TTAMachine;

bool 
ConstantAliasAnalyzer::isAddressTraceable(
    DataDependenceGraph& ddg, const ProgramOperation& po) {

    const MoveNode *rawSrc = addressOperandMove(po);

    while (rawSrc != NULL && rawSrc->isMove() && 
           rawSrc->move().source().isGPR()) {
        rawSrc = ddg.onlyRegisterRawSource(*rawSrc);
    }
    if (rawSrc == NULL) {
        return false;
    }
           
    if (rawSrc->isMove()) {
        const Move& move = rawSrc->move();
        if (!move.isCall()) {
            const Terminal& src = move.source();
            if (src.isImmediate()) {
                return true;
            }
        }
    }
    return false;
}

// TODO: does not handle unaligned 64-bit memory operations well.
MemoryAliasAnalyzer::AliasingResult
ConstantAliasAnalyzer::analyze(
    DataDependenceGraph& ddg, 
    const ProgramOperation& pop1, 
    const ProgramOperation& pop2) {

    if (isAddressTraceable(ddg, pop1) && isAddressTraceable(ddg, pop2)) {

        const MoveNode *rawSrc1 = addressOperandMove(pop1);
        while (rawSrc1 != NULL && rawSrc1->isMove() && rawSrc1->move().
               source().isGPR()) {
            rawSrc1 = ddg.onlyRegisterRawSource(*rawSrc1);
        }

        const MoveNode *rawSrc2 = addressOperandMove(pop2);
        while (rawSrc2 != NULL && rawSrc2->isMove() && rawSrc2->move().
               source().isGPR()) {
            rawSrc2 = ddg.onlyRegisterRawSource(*rawSrc2);
        }

        int addr1 = rawSrc1->move().source().value().intValue();
        int addr2 = rawSrc2->move().source().value().intValue();

        return compareIndeces(addr1, addr2, pop1, pop2);
    } else {
        return ALIAS_UNKNOWN;
    }

}

ConstantAliasAnalyzer::~ConstantAliasAnalyzer() {}
