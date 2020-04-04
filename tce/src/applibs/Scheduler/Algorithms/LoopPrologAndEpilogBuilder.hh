/*
    Copyright (c) 2002-2020 Tampere University.

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
 * @file LoopPrologAndEpilogBuilder.hh
 *
 * Declaration of LoopPrologAndEpilogBuilder interface.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto-no.spam-tut.fi)
 * @author Heikki Kultala 2015-2020 (heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef TTA_LOOP_PROLOG_AND_EPILOG_BUILDER_HH
#define TTA_LOOP_PROLOG_AND_EPILOG_BUILDER_HH

class DataDependenceGraph;
class SimpleResourceManager;
class BasicBlockNode;
class ControlFlowGraph;
class ControlFlowEdge;

namespace TTAProgram {
    class Move;
    class BasicBlock;
    class InstructionReferenceManager;
    class Immediate;
}

/**
 * Classes that implement this interface are able to software bypass
 * moves.
 */
class LoopPrologAndEpilogBuilder {
public:
    LoopPrologAndEpilogBuilder();
    virtual ~LoopPrologAndEpilogBuilder();
    virtual int build(DataDependenceGraph& ddg, SimpleResourceManager& rm,
                      ControlFlowGraph& cfg, BasicBlockNode& loopBBN,
                      int endCycle = -1, bool createEpilog = true);

    BasicBlockNode* addPrologFromRM(
        SimpleResourceManager& prologRM,
        SimpleResourceManager& loopRM,
        ControlFlowGraph& cfg, BasicBlockNode& loopBBN);

    BasicBlockNode* addEpilogFromRM(
        SimpleResourceManager& prologEpilogRM,
        int ii, ControlFlowGraph& cfg, BasicBlockNode& loopBBN);

private:

void moveJumpDestination(
    TTAProgram::InstructionReferenceManager& irm,
    BasicBlockNode& tail,
    BasicBlockNode& dst, ControlFlowEdge& jumpEdge);

    void addPrologIntoCfg(
        ControlFlowGraph& cfg, BasicBlockNode& prologBBN,
        BasicBlockNode& loopBBN);

    void addEpilogIntoCfg(
        ControlFlowGraph& cfg, BasicBlockNode& epilogBBN,
        BasicBlockNode& loopBBN);

    bool optimizeProlog(TTAProgram::BasicBlock& prolog);
    bool optimizeEpilog(TTAProgram::BasicBlock& epilog);
};

#endif
