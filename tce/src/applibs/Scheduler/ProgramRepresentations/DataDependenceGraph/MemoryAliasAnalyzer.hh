/*
 * @file MemoryAliasAnalyzer.hh
 *
 * Declaration of Memory Alias Analyzer interface
 *
 * @author Heikki Kultala 2006-2007 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_ALIAS_ANALYZER_HH
#define TTA_MEMORY_ALIAS_ANALYZER_HH

namespace TTAProgram {
    class Procedure;
}

class MoveNode;
class DataDependenceGraph;

class MemoryAliasAnalyzer {
public:

    enum AliasingResult { ALIAS_FALSE = 0,
                          ALIAS_TRUE  = 1,
                          ALIAS_UNKNOWN = 2 };

    virtual void initProcedure(TTAProgram::Procedure&) {}

    virtual AliasingResult analyze(
        DataDependenceGraph& ddg, MoveNode& node1, MoveNode& node2) = 0;

    /**
     * Checks whether the analyzer knows anything about the address.
     * 
     * ie. if it can return true or false to some query 
     * concerning this address.
     * 
     * @return true if analyzer can know something about the address.
     */
    virtual bool addressTraceable(
        MoveNode& mn) = 0;

    virtual ~MemoryAliasAnalyzer() {}
};

#endif
