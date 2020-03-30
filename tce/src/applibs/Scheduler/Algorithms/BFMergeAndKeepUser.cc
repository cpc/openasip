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
 * @file BFMergeAndKeepUser.cc
 *
 * Definition of scheduler operation which performs a bypass, including the
 * ddg updates.
 *
 * @author Heikki Kultala 2020 (hkultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFMergeAndKeepUser.hh"
#include "BFConnectNodes.hh"
#include "BFRemoveEdge.hh"
#include "ProgramOperation.hh"
#include "DataDependenceEdge.hh"
#include "DataDependenceGraph.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "MoveNodeDuplicator.hh"

#include "BFUpdateMoveOnBypass.hh"

bool
BFMergeAndKeepUser::updateEdges(
    MoveNode* src, MoveNode* dst, bool loopBypass, bool pre) {

    bool sourceIsRegToItselfCopy = false;

    if (src != nullptr && dst != nullptr &&
        ddg().rootGraph()->hasNode(*dst) &
        ddg().rootGraph()->hasNode(*src)) {
        bool removedRawEdge = false;
        auto edges = ddg().rootGraph()->connectingEdges(*src, *dst);
        for (auto e: edges) {
            if (e->isRAW()) {
                removedRawEdge = true;
                runChild(new BFRemoveEdge(sched_, *src, *dst, *e), pre);
                reg_ = e->data();
            }
        }
        if (!removedRawEdge) {
            undo();
            return false;
        }

        TTAProgram::Move& sm = src->move();
        sourceIsRegToItselfCopy = sm.source().equals(sm.destination());

        // if we are bypassing from a register-to-register copy, we'll have to
        // copy incoming raw edges also in rootgraph level to preserve inter-bb
        // -dependencies.
        for (int i = 0; i < ddg().rootGraphInDegree(*src); i++) {
            DataDependenceEdge& edge = ddg().rootGraphInEdge(*src,i);

            // skip antidependencies due bypassed register.. these are no more
            if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
                edge.data() == reg_) {
                if (edge.dependenceType() == DataDependenceEdge::DEP_WAW ||
                    edge.dependenceType() == DataDependenceEdge::DEP_WAR) {
                    continue;
                }
            }

            // do not copy guard use edges - the user already ahs them.
            // copying them leads to exponential increase in guard ege counts.
            if (edge.guardUse()) {
                continue;
            }

            // copy other edges.
            MoveNode& source = ddg().rootGraph()->tailNode(edge);
            DataDependenceEdge* newEdge =
                new DataDependenceEdge(edge, loopBypass);
            runChild(new BFConnectNodes(sched_, source, *dst, newEdge), pre);
        }

        if (src->isSourceVariable() || src->isSourceRA()) {
            // if bypassing reg-to-reg this copy anti edges resulting from the
            // read of the other register.
            for (int i = 0; i < ddg().rootGraphOutDegree(*src); i++) {
                DataDependenceEdge& edge = ddg().rootGraphOutEdge(*src,i);
                if ((edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER||
                     edge.edgeReason() == DataDependenceEdge::EDGE_RA) &&
                    edge.dependenceType() == DataDependenceEdge::DEP_WAR &&
                    !edge.tailPseudo()) {

                    MoveNode& target = ddg().rootGraph()->headNode(edge);

                    if (!(static_cast<DataDependenceGraph*>(ddg().rootGraph()))
                        ->exclusingGuards(*dst, target) &&
                        dst != &target) {

                        DataDependenceEdge* newEdge =
                            new DataDependenceEdge(edge, loopBypass);
                        // TODO: loop here!
                        runChild(new BFConnectNodes(
                                     sched_, *dst, target, newEdge), pre);
                    }
                }
            }
        }
    }


    if (dst != nullptr && ddg().rootGraph()->hasNode(*dst)) {
        // fix WAR antidependencies to WaW
        for (int i = 0; i < ddg().rootGraphOutDegree(*dst); i++) {
            DataDependenceEdge& edge = ddg().rootGraphOutEdge(*dst,i);
            if (edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
                edge.dependenceType() == DataDependenceEdge::DEP_WAR &&
                edge.data() == reg_) {
                // if stupid reg to itself copy, keep to in edges..
                if (!sourceIsRegToItselfCopy) {
                    runChild(
                        new BFRemoveEdge(
                            sched_, *dst,
                            ddg().rootGraph()->headNode(edge), edge), pre);
                   i--; // don't skip one edge here!
                }
            }
        }
    }
    return true;
}

bool
BFMergeAndKeepUser::operator()() {


    if (!ddg().mergeAndKeepAllowed(src_, dst_)) {
        if (!force_) {
            return false;
        }
    }

    bool loopBypass = ddg().isLoopBypass(src_, dst_);

    // Cannot bypass over multiple loop iterations. even with force flag.
    if (loopBypass) {
        for (int i = 0; i < ddg().inDegree(src_); i++) {
            DataDependenceEdge& edge = ddg().inEdge(src_,i);
            if (!edge.isFalseDep() && edge.isBackEdge()) {
                return false;
            }
        }
    }
    runPreChild(new BFUpdateMoveOnBypass(sched_, src_, dst_));

    if (!updateEdges(&src_, &dst_, loopBypass, true)) {
        return false;
    }


    if (ii() && updatePrologMove_) {
        MoveNode* prologMove = duplicator().getMoveNode(dst_);
        if (prologMove != nullptr) {
            auto prologSrcMove =
                duplicator().duplicateMoveNode(src_,true, false);
            duplicatedSrc_ = prologSrcMove.second;

            runPostChild(
                new BFUpdateMoveOnBypass(
                    sched_, *prologSrcMove.first, *prologMove));

            updateEdges(
                prologSrcMove.first, prologMove, loopBypass, false);

        }
    }
    return true;

}

void BFMergeAndKeepUser::undoOnlyMe() {
    if (duplicatedSrc_) {
        duplicator().disposeMoveNode(duplicator().getMoveNode(src_));
    }
}
