#ifndef LOOP_ANALYZER_HH
#define LOOP_ANALYZER_HH

#include "DataDependenceGraph.hh"

class LoopAnalyzer {
public:

    // MoveNode contains the node that reads the limit from a register,
    // if limit not constant.
    // Not just a string with register name to make ddg updates easier when
    // creating the loop buffer init.
    struct LoopAnalysisResult {
        LoopAnalysisResult(long iCount = INT_MIN, MoveNode* n = NULL, int cMul = 1) :
            counterValueNode(n), iterationCount(iCount), counterMultiplier(cMul) {}
        MoveNode* counterValueNode; // NULL if static
        long iterationCount; // may be offset to counter value node
        int counterMultiplier; // if the val is a multiply of iter count.
    };
    typedef std::pair<int, MoveNode*> EndCondition;

    struct InitAndUpdate {
        MoveNode* initNode;
        int initVal;
        int update;
        int loopEdge;
        InitAndUpdate(int i, int u, bool l) : initNode(NULL), initVal(i), update(u), loopEdge(l) {}
        InitAndUpdate(MoveNode& mn, int u, bool l) : initNode(&mn), initVal(0), update(u), loopEdge(l) {}
        InitAndUpdate(MoveNode& mn, int i, int u, bool l) : initNode(&mn), initVal(i), update(u), loopEdge(l) {}
    };
    static LoopAnalysisResult* 
    analyze(BasicBlockNode& bbn, DataDependenceGraph& ddg);

private:
    static InitAndUpdate* findInitAndUpdate(DataDependenceGraph& ddg, MoveNode& cmpVal);

    static EndCondition* findEndCond(DataDependenceGraph& ddg, MoveNode& cmpVal, bool allowVariable);

    static EndCondition* tryTrackCommonAncestor(
        DataDependenceGraph& ddg, MoveNode& init, MoveNode& endCond);
};

#endif
