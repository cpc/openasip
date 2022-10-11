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
 */

/**
 * @file StackAliasAnalyzer.hh
 *
 * Declaration of StackAliasAnalyzer class.
 * 
 * This class does simple alias analysis between memove addresses
 * that point to the stack.
 *
 * @author Heikki Kultala 2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: ref
 */

#ifndef TTA_OFFSET_ALIAS_ANALYZER
#define TTA_OFFSET_ALIAS_ANALYZER

#include "MemoryAliasAnalyzer.hh"
#include "TCEString.hh"
#include <map>

class OffsetAliasAnalyzer : public MemoryAliasAnalyzer {
public:
    virtual bool isAddressTraceable(
        DataDependenceGraph& ddg, const ProgramOperation& pop);
    virtual AliasingResult analyze(
        DataDependenceGraph& ddg, const ProgramOperation& pop1, 
        const ProgramOperation& pop2, MoveNodeUse::BBRelation bbRelation);

    OffsetAliasAnalyzer(const TCEString& sp);
    ~OffsetAliasAnalyzer();
private:

    bool analyzeLoopPtrIncrease(
        const DataDependenceGraph& ddg, const MoveNode& mn, long& offset);

    bool sameLoopAndPrevSources(const DataDependenceGraph& ddg, const MoveNode& anc1, const MoveNode& anc2);

    struct OffsetData {
        const MoveNode *baseNode;
        int offset;
        OffsetData(const MoveNode* node, int offs) : 
            baseNode(node), offset(offs) {}
    };
    std::map <int,OffsetData> offsetData_;
        
    TCEString sp_;
};

#endif
