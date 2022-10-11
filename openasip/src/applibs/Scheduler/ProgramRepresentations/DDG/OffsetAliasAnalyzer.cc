/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file OffsetAliasAnalyzer.cc
 *
 * Implementation of OffsetAliasAnalyzer class.
 * 
 * This class does simple alias analysis between memory addresses
 * which are offsets of same register (register + index)
 * Usually this are fields of same struct.
 *
 * @author Heikki Kultala 2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "OffsetAliasAnalyzer.hh"

#include "MoveNode.hh"
#include "Move.hh"
#include "ProgramOperation.hh"
#include "DataDependenceGraph.hh"
#include "RegisterFile.hh"
#include "Terminal.hh"
#include "MoveNodeSet.hh"
#include "Operation.hh"

using namespace TTAProgram;
using namespace TTAMachine;

/**
 * Checks if the node contains an adress that is an offset.
 *
 * @param ddg DDG where to analyze from
 * @param mn the node being checked
 * @return true if is a traceable stack offset, false if not.
 */
bool 
OffsetAliasAnalyzer::isAddressTraceable(
    DataDependenceGraph& ddg, const ProgramOperation& pop) {

    std::map<int,OffsetData>::iterator i = offsetData_.find(pop.poId());
    if (i != offsetData_.end()) {
        if (i->second.baseNode == NULL) {
            return false;
        } else {
            return true;
        }
    }

    const MoveNode* addrMove = addressOperandMove(pop);
    if (addrMove == NULL) {

        TwoPartAddressOperandDetection addressParts =
            findTwoPartAddressOperands(pop);
        if (addressParts.offsetOperation == 
            TwoPartAddressOperandDetection::NOT_FOUND) {
            return false;
        }
        
        MoveNodeSet& addr1Set = pop.inputNode(addressParts.operand1);
        MoveNodeSet& addr2Set = pop.inputNode(addressParts.operand2);
        if (addr1Set.count() != 1) {
            return false;
        } 
        if (addr2Set.count() != 1) {
            return false;
        }
        MoveNode& addr1 = addr1Set.at(0);
        MoveNode& addr2 = addr2Set.at(0);

        if ((addr1.move().source().isImmediate() &&
             addr2.move().source().isGPR()) ||
            (addr2.move().source().isImmediate() &&
             addr1.move().source().isGPR())) {
            return true;
        } else {
            return false;
        }
    }
    
    // first find base and index of the first node.
    const MoveNode* rawSrc = ddg.onlyRegisterRawAncestor(*addrMove, sp_);
        
    if (!rawSrc->isSourceOperation()) {
        offsetData_.insert(
            std::pair<int,OffsetData>(
                pop.poId(),OffsetData(rawSrc, 0)));
        return true;
    }

    ProgramOperation& po = rawSrc->sourceOperation();

    if (po.operation().name() != "ADD" &&
        po.operation().name() != "SUB") {
        if (ddg.regRawSuccessorCount(*rawSrc, false) > 1) {
            offsetData_.insert(
                std::pair<int,OffsetData>(
                    pop.poId(),OffsetData(rawSrc, 0)));
            return true;
        } else {
            offsetData_.insert(
                std::pair<int,OffsetData>(
                    pop.poId(),OffsetData(NULL, INT_MAX)));
            return false;
        }
    }

    // offset calc, add or sub. check for node2
    MoveNodeSet& baseSet = po.inputNode(1);
    MoveNodeSet& offsetSet = po.inputNode(2);
    
    if (baseSet.count() != 1 || offsetSet.count() != 1) {
        offsetData_.insert(
            std::pair<int,OffsetData>(
                pop.poId(),OffsetData(NULL, INT_MAX)));
        return false;
    } 

    MoveNode& offsetNode = offsetSet.at(0);
    MoveNode& base = baseSet.at(0);

    if (!base.move().source().isGPR() || 
        !offsetNode.move().source().isImmediate()) {
        offsetData_.insert(
            std::pair<int,OffsetData>(
                pop.poId(), OffsetData(NULL, INT_MAX)));
        return false;
    }

    return true;
}

/** 
 * Analyzes aliasing of two memory adderesses.
 * 
 * Checks if they are stack offsets and compares the offsets.
 * 
 * @param ddg ddg where they belong.
 * @param node1 first node to compare
 * @param another anpther node to compare
 * @return ALIAS_TRUE if they alias, ALIAS_FALSE if they don't or
 *         ALIAS_UNKNOWN if cannot analyze.
 */
MemoryAliasAnalyzer::AliasingResult
OffsetAliasAnalyzer::analyze(
    DataDependenceGraph& ddg, const ProgramOperation& pop1, 
    const ProgramOperation& pop2, MoveNodeUse::BBRelation bbRel) {
    const MoveNode* anc1 = NULL;
    const MoveNode* anc2 = NULL;
    long offsetVal1 = 0;
    long offsetVal2 = 0;

    std::map<int,OffsetData>::const_iterator i = 
        offsetData_.find(pop1.poId());
    if (i != offsetData_.end()) {
        if (i->second.baseNode == NULL) {
            return ALIAS_UNKNOWN;
        } else {
            anc1 = i->second.baseNode;
            offsetVal1 = i->second.offset;
        }
    } else {

        const MoveNode* addrMove1 = addressOperandMove(pop1);
        if (addrMove1 == NULL) {

            int offsetMul = 0;
            TwoPartAddressOperandDetection addressParts =
                findTwoPartAddressOperands(pop1);
            switch(addressParts.offsetOperation) {
            case TwoPartAddressOperandDetection::ADD:
                offsetMul = 1;
                break;
            case TwoPartAddressOperandDetection::SUB:
                offsetMul = -1;
                break;
            case TwoPartAddressOperandDetection::NOT_FOUND:
                offsetData_.insert(
                    std::pair<int,OffsetData>(
                        pop1.poId(),OffsetData(NULL, INT_MAX)));
                return ALIAS_UNKNOWN;
            }
            
            MoveNodeSet& addr1Set = pop1.inputNode(addressParts.operand1);
            MoveNodeSet& addr2Set = pop1.inputNode(addressParts.operand2);
            if (addr1Set.count() != 1) {
                offsetData_.insert(
                    std::pair<int,OffsetData>(
                        pop1.poId(),OffsetData(NULL, INT_MAX)));
                return ALIAS_UNKNOWN;
            } 
            if (addr2Set.count() != 1) {
                offsetData_.insert(
                    std::pair<int,OffsetData>(
                        pop1.poId(),OffsetData(NULL, INT_MAX)));
                return ALIAS_UNKNOWN;
            }
            MoveNode& addr1 = addr1Set.at(0);
            MoveNode& addr2 = addr2Set.at(0);
            
            if (addr1.isSourceConstant() &&
                addr2.move().source().isGPR()) {
                anc1 = ddg.onlyRegisterRawAncestor(addr2, sp_);
                offsetVal1 = addr1.move().source().value().intValue() *
                    offsetMul;
            } else {
                if (addr2.isSourceConstant() &&
                    addr1.move().source().isGPR()) {
                    anc1 = ddg.onlyRegisterRawAncestor(addr1, sp_);
                    offsetVal1 = addr2.move().source().value().intValue() *
                        offsetMul;
                } else {
                    offsetData_.insert(
                        std::pair<int,OffsetData>(
                        pop1.poId(),OffsetData(NULL, INT_MAX)));
                    return ALIAS_UNKNOWN;
                }
            }

            offsetData_.insert(
                std::pair<int,OffsetData>(
                    pop1.poId(),OffsetData(anc1, offsetVal1)));

        } else {
            // direct memory op. have to search for the add.

            // first find base and index of the first node.
            const MoveNode* rawSrc1 = 
                ddg.onlyRegisterRawAncestor(*addrMove1, sp_);
            
            if (!rawSrc1->isSourceOperation()) {
                offsetData_.insert(
                    std::pair<int,OffsetData>(
                        pop1.poId(),OffsetData(rawSrc1, 0)));
            } else {
            
            ProgramOperation& po1 = rawSrc1->sourceOperation();
            
            if (po1.operation().name() == "ADD" ||
                po1.operation().name() == "SUB") {
                
                
                // offset calc, add or sub. check for node2
                MoveNodeSet& baseSet1 = po1.inputNode(1);
                MoveNodeSet& offsetSet1 = po1.inputNode(2);
                
                if (baseSet1.count() != 1 || offsetSet1.count() != 1) {
                    offsetData_.insert(
                        std::pair<int,OffsetData>(
                            pop1.poId(),OffsetData(NULL, INT_MAX)));
                    return ALIAS_UNKNOWN;
                }
                
                MoveNode& offsetNode1 = offsetSet1.at(0);
                MoveNode& base1 = baseSet1.at(0);
                
                if (!base1.move().source().isGPR() ||
                    !offsetNode1.move().source().isImmediate()) {
                    offsetData_.insert(
                        std::pair<int,OffsetData>(
                            pop1.poId(),OffsetData(NULL, INT_MAX)));
                    return ALIAS_UNKNOWN;
                }
                
                offsetVal1 = offsetNode1.move().source().value().intValue();
                if (po1.operation().name() == "SUB") {
                    offsetVal1 = -offsetVal1;
                }
                
                anc1 = ddg.onlyRegisterRawAncestor(base1, sp_);
                // update cache.
                offsetData_.insert(
                    std::pair<int,OffsetData>(
                        pop1.poId(),OffsetData(anc1, offsetVal1)));
            } else {
                // comes directly from some untraceable op.
                if (ddg.regRawSuccessorCount(*rawSrc1, false) > 1) {
                    offsetData_.insert(
                        std::pair<int,OffsetData>(
                            pop1.poId(),OffsetData(rawSrc1, 0)));
                    anc1 = rawSrc1;
                    offsetVal1 = 0;
                } else {
                    offsetData_.insert(
                        std::pair<int,OffsetData>(
                            pop1.poId(),OffsetData(NULL, INT_MAX)));
                    return ALIAS_UNKNOWN;
                }
            }
            }
        }
    }
    // then find base and index for second node.

    //first search cache.
    i = offsetData_.find(pop2.poId());
    if (i != offsetData_.end()) {
        if (i->second.baseNode == NULL) {
            return ALIAS_UNKNOWN;
        } else {
            anc2 = i->second.baseNode;
            offsetVal2 = i->second.offset;
        }
    } else {
        const MoveNode* addrMove2 = addressOperandMove(pop2);
        if (addrMove2 == NULL) {

            int offsetMul = 0;
            TwoPartAddressOperandDetection addressParts =
                findTwoPartAddressOperands(pop2);
            switch(addressParts.offsetOperation) {
            case TwoPartAddressOperandDetection::ADD:
                offsetMul = 1;
                break;
            case TwoPartAddressOperandDetection::SUB:
                offsetMul = -1;
                break;
            case TwoPartAddressOperandDetection::NOT_FOUND:
                offsetData_.insert(
                    std::pair<int,OffsetData>(
                        pop2.poId(),OffsetData(NULL, INT_MAX)));
                return ALIAS_UNKNOWN;
            }
            
            MoveNodeSet& addr1Set = pop2.inputNode(addressParts.operand1);
            MoveNodeSet& addr2Set = pop2.inputNode(addressParts.operand2);
            if (addr1Set.count() != 1) {
                offsetData_.insert(
                    std::pair<int,OffsetData>(
                        pop2.poId(),OffsetData(NULL, INT_MAX)));
                return ALIAS_UNKNOWN;
            } 
            if (addr2Set.count() != 1) {
                offsetData_.insert(
                    std::pair<int,OffsetData>(
                        pop2.poId(),OffsetData(NULL, INT_MAX)));
                return ALIAS_UNKNOWN;
            }
            MoveNode& addr1 = addr1Set.at(0);
            MoveNode& addr2 = addr2Set.at(0);
            
            if (addr1.isSourceConstant() &&
                addr2.move().source().isGPR()) {
                anc2 = ddg.onlyRegisterRawAncestor(addr2, sp_);
                offsetVal2 = addr1.move().source().value().intValue() *
                    offsetMul;
            } else {
                if (addr2.isSourceConstant() &&
                    addr1.move().source().isGPR()) {
                    anc2 = ddg.onlyRegisterRawAncestor(addr1, sp_);
                    offsetVal2 = addr2.move().source().value().intValue() *
                        offsetMul;
                } else {
                    offsetData_.insert(
                        std::pair<int,OffsetData>(
                        pop2.poId(),OffsetData(NULL, INT_MAX)));
                    return ALIAS_UNKNOWN;
                }
            }

            offsetData_.insert(
                std::pair<int,OffsetData>(
                    pop2.poId(),OffsetData(anc2, offsetVal2)));
        } else {
            // direct memory op. have to search for the add.

            // first find base and index of the first node.
            const MoveNode* rawSrc2 = 
                ddg.onlyRegisterRawAncestor(*addrMove2, sp_);
            
            if (!rawSrc2->isSourceOperation()) {
                offsetData_.insert(
                    std::pair<int,OffsetData>(
                        pop2.poId(),OffsetData(rawSrc2, 0)));

            } else {
            
            ProgramOperation& po2 = rawSrc2->sourceOperation();
            
            if (po2.operation().name() == "ADD" ||
                po2.operation().name() == "SUB") {
                
                // offset calc, add or sub. check for node2
                MoveNodeSet& baseSet2 = po2.inputNode(1);
                MoveNodeSet& offsetSet2 = po2.inputNode(2);
                
                if (baseSet2.count() != 1 || offsetSet2.count() != 1) {
                    offsetData_.insert(
                        std::pair<int,OffsetData>(
                            pop2.poId(),OffsetData(NULL, INT_MAX)));
                    return ALIAS_UNKNOWN;
                } 
                
                MoveNode& offsetNode2 = offsetSet2.at(0);
                MoveNode& base2 = baseSet2.at(0);
                
                // check that base is register coming from same place.
                
                if (!base2.move().source().isGPR() ||
                    !offsetNode2.move().source().isImmediate()) {
                    offsetData_.insert(
                        std::pair<int,OffsetData>(
                            pop2.poId(),OffsetData(NULL, INT_MAX)));
                    return ALIAS_UNKNOWN;
                }
                
                offsetVal2 = offsetNode2.move().source().value().intValue();
                if (po2.operation().name() == "SUB") {
                    offsetVal2 = -offsetVal2;
                }
                
                anc2 = ddg.onlyRegisterRawAncestor(base2, sp_);
                // update cache.
                offsetData_.insert(
                    std::pair<int,OffsetData>(
                        pop2.poId(),OffsetData(anc2, offsetVal2)));
            } else {
                // comes directly from some untraceable op.
                if (ddg.regRawSuccessorCount(*rawSrc2, false) > 1) {
                    offsetData_.insert(
                        std::pair<int,OffsetData>(
                            pop2.poId(),OffsetData(rawSrc2, 0)));
                    anc2 = rawSrc2;
                    offsetVal2 = 0;
                } else {
                    offsetData_.insert(
                        std::pair<int,OffsetData>(
                            pop2.poId(),OffsetData(NULL, INT_MAX)));
                    return ALIAS_UNKNOWN;
                }
            }
            }
        }
    }
    // now we have the defining move for base and offset. compare them

    if (anc1 == NULL || anc2 == NULL) {
        return ALIAS_UNKNOWN;
    }

    if (anc1 == anc2 || sameLoopAndPrevSources(ddg, *anc1, *anc2)) {

        if (bbRel == MoveNodeUse::LOOP) {
            if (!analyzeLoopPtrIncrease(ddg, *anc2, offsetVal2)) {
                return ALIAS_UNKNOWN;
            }
        }
        
        AliasingResult res = compareIndeces(
            offsetVal1, offsetVal2, pop1, pop2);
        
        return res;
    }
    return ALIAS_UNKNOWN;
}

bool OffsetAliasAnalyzer::sameLoopAndPrevSources(
    const DataDependenceGraph& ddg, const MoveNode& anc1, const MoveNode& anc2) {

    MoveNode* prevSrc1 = ddg.onlyRegisterRawSource(anc1,2,2);
    MoveNode* loopSrc1 = ddg.onlyRegisterRawSource(anc1,2,1);
    MoveNode* prevSrc2 = ddg.onlyRegisterRawSource(anc2,2,2);
    MoveNode* loopSrc2 = ddg.onlyRegisterRawSource(anc2,2,1);

    return (prevSrc1 == prevSrc2 && loopSrc1 == loopSrc2 &&
            prevSrc1 != NULL);
}

OffsetAliasAnalyzer::~OffsetAliasAnalyzer() {}

OffsetAliasAnalyzer::OffsetAliasAnalyzer(const TCEString& sp) : sp_(sp) {
}

bool OffsetAliasAnalyzer::analyzeLoopPtrIncrease(
    const DataDependenceGraph& ddg, const MoveNode& mn, long& offset) {

    MoveNode* prevSrc = ddg.onlyRegisterRawSource(mn,2,2);
    MoveNode* loopSrc = ddg.onlyRegisterRawSource(mn,2,1);
    // does not change in loop?
    if (loopSrc == NULL) {
        return true;
    }

    if (prevSrc == NULL) {
        return false;
    }
    return findIncrement(*loopSrc, offset);
}
