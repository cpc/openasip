/**
 */

#ifndef TTA_CONSTANT_ALIAS_ANALYZER
#define TTA_CONSTANT_ALIAS_ANALYZER

#include "MemoryAliasAnalyzer.hh"

class ConstantAliasAnalyzer : public MemoryAliasAnalyzer {

    virtual bool addressTraceable(MoveNode& mn);
    virtual AliasingResult analyze(
        DataDependenceGraph& ddg, MoveNode& node1, MoveNode& node2);
    
    ~ConstantAliasAnalyzer();
};

#endif
