/**
 */

#ifndef TTA_CONSTANT_ALIAS_ANALYZER
#define TTA_CONSTANT_ALIAS_ANALYZER

#include "MemoryAliasAnalyzer.hh"

class ConstantAliasAnalyzer : public MemoryAliasAnalyzer {

    virtual bool addressTraceable(const MoveNode& mn);
    virtual AliasingResult analyze(
        DataDependenceGraph& ddg, const MoveNode& node1, 
        const MoveNode& node2);
    
    ~ConstantAliasAnalyzer();
};

#endif
