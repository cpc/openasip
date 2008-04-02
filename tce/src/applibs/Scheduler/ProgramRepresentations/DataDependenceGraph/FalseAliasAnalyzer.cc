/**
 * @file FalseAliasAnalyzer.hh
 *
 * Implementation of FalseAliasAnalyzer class.
 * 
 * @author Heikki Kultala 2007 (heikki.kultala@tut.fi)
 * @note rating: ref
 */

#include "FalseAliasAnalyzer.hh"

/**
 * Checks whether can say something about a memory address. 
 *
 * @return always returns true
 */
bool 
FalseAliasAnalyzer::addressTraceable(const MoveNode&) {
    return true;
}

/**
 * checks whether mem adderesses alias. As this is fake, always returns false.
 * @return always ALIAS_FALSE
 */
MemoryAliasAnalyzer::AliasingResult
FalseAliasAnalyzer::analyze(
    DataDependenceGraph&, const MoveNode&, const MoveNode&) {
    return ALIAS_FALSE;
}

/**
 * Desctructor
 */
FalseAliasAnalyzer::~FalseAliasAnalyzer() {}
