/**
 * @file CFGStatistics.hh
 *
 * Declaration of prototype class that collects statistics about
 * control flow graph.
 *
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TCE_CFGSTATISTICS_HH
#define TCE_CFGSTATISTICS_HH

#include "BasicBlock.hh"
/**
 * Represents statistics about single control flow graph.
 * Count of BB, sum of moves, immediates, instructions and bypasses as well as
 * largest basic block and immediates, instructions and bypassed in it.
 */

class CFGStatistics : public BasicBlockStatistics {
public:
    CFGStatistics();
    virtual ~CFGStatistics();
    virtual int normalBBCount() const;
    virtual int maxMoveCount() const;
    virtual int maxImmediateCount() const;
    virtual int maxInstructionCount() const;
    virtual int maxBypassedCount() const;
    virtual void setNormalBBCount(int);
    virtual void setMaxMoveCount(int);
    virtual void setMaxImmediateCount(int);
    virtual void setMaxInstructionCount(int);
    virtual void setMaxBypassedCount(int);

private:    
    int normalBBCount_;
    int maxMoveCount_;
    int maxImmediateCount_;
    int maxInstructionCount_;
    int maxBypassCount_;
};

#endif
