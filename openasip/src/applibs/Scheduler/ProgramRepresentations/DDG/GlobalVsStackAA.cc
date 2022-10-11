#include "GlobalVsStackAA.hh"
#include "ConstantAliasAnalyzer.hh"
#include "StackAliasAnalyzer.hh"


bool 
GlobalVsStackAA::isAddressTraceable(
    DataDependenceGraph& ddg, const ProgramOperation& po) {
    long tmp;
    long tmp2;
    return ConstantAliasAnalyzer::getConstantAddress(ddg, po, tmp, tmp2) ||
        StackAliasAnalyzer::getStackOffset(ddg, po, tmp, tmp2, sp_);
}

// TODO: does not handle unaligned 64-bit memory operations well.
MemoryAliasAnalyzer::AliasingResult
GlobalVsStackAA::analyze(
    DataDependenceGraph& ddg, 
    const ProgramOperation& pop1, 
    const ProgramOperation& pop2, 
    MoveNodeUse::BBRelation) {

    long tmp;
    long tmp2;
    long tmp3;
    long tmp4;
    if ((StackAliasAnalyzer::getStackOffset(ddg, pop2, tmp2, tmp4, sp_) &&
         ConstantAliasAnalyzer::getConstantAddress(ddg, pop1, tmp, tmp3)) ||
        (StackAliasAnalyzer::getStackOffset(ddg, pop1, tmp2, tmp4, sp_) &&
         ConstantAliasAnalyzer::getConstantAddress(ddg, pop2, tmp, tmp3))) {
        return ALIAS_FALSE;
    }
    return ALIAS_UNKNOWN;

}

GlobalVsStackAA::~GlobalVsStackAA() {}

GlobalVsStackAA::GlobalVsStackAA(const TCEString& sp) : sp_(sp) {
}
