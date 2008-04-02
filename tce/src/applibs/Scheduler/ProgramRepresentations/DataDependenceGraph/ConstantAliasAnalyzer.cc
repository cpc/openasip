/**
 * @file ConstantAliasAnalyzer.hh
 *
 * Implementation of ConstantAliasAnalyzer class.
 * 
 * This class does simple alias analysis between memove addresses
 * that come directly from immediates.
 *
 * @author Heikki Kultala 2007 (heikki.kultala@tut.fi)
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
