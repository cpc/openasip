/**
 * @file FalseAliasAnalyzer.hh
 *
 * Declaration of FalseAliasAnalyzer class.
 * 
 * @author Heikki Kultala 2007 (heikki.kultala@tut.fi)
 * @note rating: ref
 */

#ifndef TTA_FALSE_ALIAS_ANALYZER_HH
#define TTA_FALSE_ALIAS_ANALYZER_HH

#include "MemoryAliasAnalyzer.hh"

class FalseAliasAnalyzer : public MemoryAliasAnalyzer {
        virtual bool addressTraceable(const MoveNode& mn);
    virtual AliasingResult analyze(
        DataDependenceGraph& ddg, const MoveNode& node1, 
        const MoveNode& node2);
    
    ~FalseAliasAnalyzer();
};

#endif
