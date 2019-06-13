/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file ResourceConstraintAnalyzer.hh
 *
 * Declaration of ResourceConstraintAlanyzer class.
 *
 * @author Pekka J‰‰skel‰inen 2010
 * @note rating: red
 */

#ifndef TTA_RESOURCE_CONSTRAINT_ANALYZER_HH
#define TTA_RESOURCE_CONSTRAINT_ANALYZER_HH

#include <map>
#include <set>
#include "TCEString.hh"

class DataDependenceGraph;
class SimpleResourceManager;
class MoveNode;

/**
 * A class that analyzes the most important resource constraints in the 
 * given scheduled basic block DDG.
 */
class ResourceConstraintAnalyzer {
public:
    ResourceConstraintAnalyzer(
        DataDependenceGraph& ddg,
        SimpleResourceManager& rm,
        TCEString graphName) : 
        origDDG_(ddg), rm_(rm), graphName_(graphName) {}
    virtual ~ResourceConstraintAnalyzer() {}

    bool analyze();

private:

    bool analyzeRegisterAntideps(
        DataDependenceGraph& ddg, std::ostream& s);
    bool analyzeMoveNode(const MoveNode& node);

    MoveNode* findResourceConstrainedParent(MoveNode* child);

    void optimalScheduleResourceUsage(
        DataDependenceGraph& ddg, TCEString graphName);

    typedef std::map<std::string, int> RFCountMap;
    typedef std::map<std::string, int> OperationCountMap;

    int busConstrained_;
    int operationConstrained_;
    int rfWritePortConstrained_;
    int rfReadPortConstrained_;
    int minScheduleLength_;
    int unknownConstrained_;

    RFCountMap writePortConstrainedRfs_;
    RFCountMap readPortConstrainedRfs_;

    OperationCountMap operationConstrainedMoves_;

    DataDependenceGraph& origDDG_;
    SimpleResourceManager& rm_;

    std::set<MoveNode*> foundMoves_;

    TCEString graphName_;
};

#endif
