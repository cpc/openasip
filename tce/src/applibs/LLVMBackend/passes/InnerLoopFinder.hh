/**
 * @file InnerLoopFinder.hh
 *
 * LLVM pass for finding loops that can be software pipelined by the TCE
 * loop scheduler.
 *
 * @author Pekka J‰‰skel‰inen
 */
#ifndef HH_INNER_LOOP_FINDER
#define HH_INNER_LOOP_FINDER

#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Support/CommandLine.h>

#include "tce_config.h"
#include <llvm/IR/Module.h>

namespace llvm {
    void initializeInnerLoopFinderPass(llvm::PassRegistry&);
}

struct InnerLoopFinder : public llvm::LoopPass {
    static char ID;
    typedef std::map<std::string, std::ostream*> DumpFileIndex;

    class InnerLoopInfo {
    public:
        InnerLoopInfo() : tripCount_(0) {}
        InnerLoopInfo(int tripCount) { tripCount_ = tripCount; }
        int tripCount() const { return tripCount_; }
        bool isTripCountKnown() const { return tripCount_ > 0; }
    private:
        int tripCount_;
    };

    typedef std::map<const llvm::BasicBlock*, InnerLoopInfo> InnerLoopInfoIndex;

    DumpFileIndex dumpFiles;
    bool dump;

    InnerLoopFinder();
    ~InnerLoopFinder();

    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const;

    std::ostream& out(llvm::Loop* l);

    virtual std::string loopDescription(llvm::Loop* l);
    virtual bool runOnLoop(llvm::Loop* l, llvm::LPPassManager &LPM);
    unsigned getSmallConstantTripCount(llvm::Loop* loop);

    InnerLoopInfoIndex innerLoopInfo() { return loopInfos_; }
private:
    InnerLoopInfoIndex loopInfos_;
};

#endif
