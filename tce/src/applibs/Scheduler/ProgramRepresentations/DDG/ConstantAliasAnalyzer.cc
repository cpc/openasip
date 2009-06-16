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
 * @note rating: ref
 */

#include "ConstantAliasAnalyzer.hh"

#include "MoveNode.hh"
#include "Move.hh"

using namespace TTAProgram;
using namespace TTAMachine;

bool 
ConstantAliasAnalyzer::addressTraceable(const MoveNode& mn) {
    if (mn.isMove()) {
        const Move& move = mn.move();
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
    DataDependenceGraph&, const MoveNode& node1, const MoveNode& node2) {

    if (addressTraceable(node1) && addressTraceable(node2)) {
        int addr1 = node1.move().source().value().intValue();
        int addr2 = node2.move().source().value().intValue();
        if (addr1 == addr2) {
            return ALIAS_TRUE;
        }
        if ( addr1 - addr2 > 3 || addr2 - addr1 >3 ) {
            return ALIAS_FALSE;
        }
        // off-by-few addresses.
        return ALIAS_UNKNOWN;
    } else {
        return ALIAS_UNKNOWN;
    }

}

ConstantAliasAnalyzer::~ConstantAliasAnalyzer() {}
