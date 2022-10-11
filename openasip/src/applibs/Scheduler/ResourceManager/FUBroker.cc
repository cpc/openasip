/*
    Copyright (c) 2002-2016 Tampere University.

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
 * @file InputFUBroker.cc
 *
 * Implementation of InputFUBroker class.
 *
 * @author Heikki Kultala 2016-2020 (heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */
#include "FUBroker.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "FunctionUnit.hh"

std::pair<bool, const TTAMachine::FunctionUnit*> FUBroker::findDstFUOfMove(
    const MoveNode& node, const TTAMachine::FunctionUnit* resFU,
    DataDependenceGraph::NodeSet& processedInputNodes) const {
    // already processed this once. do not do it twice
    if (processedInputNodes.count(const_cast<MoveNode*>(&node))) {
        return std::make_pair(true, resFU);
    }
    processedInputNodes.insert(const_cast<MoveNode*>(&node));
    const TTAProgram::Move& move = node.move();
    const TTAProgram::Terminal& dst = move.destination();
    if (!dst.isFUPort()) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Output move does not have FU source!");
    }
    const TTAMachine::FunctionUnit& fu = dst.functionUnit();
    if (hasResourceOf(fu)) {
        if (resFU == NULL) {
            resFU = &fu;
        } else {
            if (resFU != &fu) {
                return std::make_pair(false, resFU);
                // illegal/empty
            }
        }
    }
    for (unsigned int i = 0; i < node.destinationOperationCount(); i++) {
        ProgramOperation& po = node.destinationOperation(i);
        auto a = findFUOfPO(po, resFU);
        if (a.first) {
            resFU = a.second;
        } else {
            return a;
        }
    }
    return std::make_pair(true, resFU);
}


// first part is if no conflict?
std::pair<bool, const TTAMachine::FunctionUnit*> FUBroker::findFUOfPO(
    ProgramOperation& po, const TTAMachine::FunctionUnit* resFU) const {

    const TTAMachine::FunctionUnit* fu = po.scheduledFU();

    // did not find anything. use default or return not found.
    if (fu == nullptr) {
        if (resFU && !po.isLegalFU(*resFU)) {
            return std::make_pair(false, resFU);
        }
        return std::make_pair(true, resFU);
    }

    // illegal annotations?
    if (!po.isLegalFU(*fu)) {
        return std::make_pair(false, resFU);
    }

    assert(hasResourceOf(*fu));
    // did find something, but it's different than default
    if (fu != resFU) {
        if (resFU == nullptr) {
            // default was null, use then one found
            return std::make_pair(true, fu);
        } else {
            // default not null. conflict! return not found.
            return std::make_pair(false, resFU);
        }
    }
    // fu and resfu are the same? ok!
    return std::make_pair(true,fu);

}
