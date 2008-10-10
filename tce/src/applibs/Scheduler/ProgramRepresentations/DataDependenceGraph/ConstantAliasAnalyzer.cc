/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ConstantAliasAnalyzer.hh
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
