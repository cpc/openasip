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
 * @file StackAliasAnalyzer.cc
 *
 * Implementation of StackAliasAnalyzer class.
 * 
 * This class does simple alias analysis between memove addresses
 * that point to the stack.
 *
 * @author Heikki Kultala 2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "StackAliasAnalyzer.hh"

#include "MoveNode.hh"
#include "MoveNodeSet.hh"
#include "Move.hh"
#include "ProgramOperation.hh"
#include "DataDependenceGraph.hh"
#include "RegisterFile.hh"
#include "OperationDAG.hh"
#include "OperationNode.hh"
#include "TerminalNode.hh"
#include "Operand.hh"
#include "Terminal.hh"
#include "Operation.hh"

using namespace TTAProgram;
using namespace TTAMachine;

/**
 * Checks if the node contains an adress that is an stack offset.
 *
 * @param ddg DDG where to analyze from
 * @param mn the node being checked
 * @return true if is a traceable stack offset, false if not.
 */
bool 
StackAliasAnalyzer::isAddressTraceable(
    DataDependenceGraph& ddg, const ProgramOperation& pop) {
    auto i = offsetData_.find(pop.poId());
    if (i != offsetData_.end()) {
        return i->second.first != INT_MAX;
    } else {
        long tmp, tmp2;
        bool analyzable = getStackOffset(ddg, pop, tmp, tmp2, sp_);
        if (analyzable) {
            offsetData_[pop.poId()] = std::make_pair(tmp,tmp2);
            return true;
        } else {
            offsetData_[pop.poId()] = std::make_pair(INT_MAX,tmp2);
            return false;
        }
    }
    return false;
}

/**
 * Gets stack offset of a move which is transporting an address.
 * 
 * @param ddg DDG where to track the address from.
 * @param node Node which transfers the address.
 * @param stackOffset place where to return the stack offset.
 * @return true if can calculate stack offset, false if not stack offset.
 */
bool
StackAliasAnalyzer::getStackOffset(
    DataDependenceGraph& ddg, const ProgramOperation& pop,
    long& stackOffset, long& loopIncrement, const TCEString& sp) {

    stackOffset = 0;
    loopIncrement = 0;

    // TODO: support for base+offset ops here.

    const MoveNode* mn = addressOperandMove(pop);
    if (mn == NULL) {

        int offsetMul = 0;
        TwoPartAddressOperandDetection addressParts =
            findTwoPartAddressOperands(pop);
        switch(addressParts.offsetOperation) {
        case TwoPartAddressOperandDetection::ADD:
            offsetMul = 1;
            break;
        case TwoPartAddressOperandDetection::SUB:
            offsetMul = -1;
            break;
        case TwoPartAddressOperandDetection::NOT_FOUND:
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

        if (addr1.isSourceConstant()) {
            int offsetVal = addr1.move().source().value().intValue();
            stackOffset += (offsetMul * offsetVal);
            mn = &addr2;
        }
        
        if (addr2.isSourceConstant()) {
            int offsetVal = addr2.move().source().value().intValue();
            stackOffset += (offsetMul * offsetVal);
            mn = &addr1;
        }
    }

    while(mn != NULL && mn->isMove()) {
        if (mn->isSourceVariable()) {
            if (mn->isSourceReg(sp)) {
                return true;
            }

            MoveNode* prevSrc = ddg.onlyRegisterRawSource(*mn,2,2);
            MoveNode* loopSrc = ddg.onlyRegisterRawSource(*mn,2,1);
            // borken ddg??
            if (prevSrc == NULL) {
                break;
            }
            if (loopSrc) {
                if (!findIncrement(*loopSrc, loopIncrement)) {
                    return false;
                }
            } 
            mn = prevSrc;
        } else {
            if (mn->isSourceOperation()) {
                const MoveNode* incrementInput = findIncrement(*mn, stackOffset);
                mn = incrementInput;
            } else {
                return false;
            }
        }
    }
    return false;
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
StackAliasAnalyzer::analyze(
    DataDependenceGraph& ddg, const ProgramOperation& pop1, 
    const ProgramOperation& pop2, MoveNodeUse::BBRelation bbRel) {

    long addr1, addr2;
    long incr1, incr2;
    auto i = offsetData_.find(pop1.poId());
    if (i != offsetData_.end()) {
        if (i->second.first != INT_MAX) {
            addr1 = i->second.first;
            incr1 = i->second.second;
        } else {
            return ALIAS_UNKNOWN;
        }
    } else {
        if (!(getStackOffset(ddg, pop1, addr1, incr1, sp_))) {
            offsetData_[pop1.poId()] = std::make_pair(INT_MAX, incr1);
            return ALIAS_UNKNOWN;
        } else {
            offsetData_[pop1.poId()] = std::make_pair(addr1, incr1);
        }
    }

    i = offsetData_.find(pop2.poId());
    if (i != offsetData_.end()) {
        if (i->second.first != INT_MAX) {
            addr2 = i->second.first;
            incr2 = i->second.second;
        } else {
            return ALIAS_UNKNOWN;
        }
    } else {
        if (!(getStackOffset(ddg, pop2, addr2, incr2, sp_))) {
            offsetData_[pop2.poId()] = std::make_pair(INT_MAX, incr2);
            return ALIAS_UNKNOWN;
        } else {
            offsetData_[pop2.poId()] = std::make_pair(addr2, incr2);
        }

    }

    if (incr1 != incr2) {
        return ALIAS_UNKNOWN;
    }

    if (bbRel != MoveNodeUse::LOOP) {
        return compareIndeces(addr1, addr2, pop1, pop2);
    } else {
        return compareIndeces(addr1 + incr1, addr2, pop1, pop2);
    }
}

StackAliasAnalyzer::~StackAliasAnalyzer() {}

StackAliasAnalyzer::StackAliasAnalyzer(const TCEString& sp) : sp_(sp) {
}
