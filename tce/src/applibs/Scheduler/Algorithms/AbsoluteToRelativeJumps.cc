/*
    Copyright (c) 2002-2019 Tampere University.

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
 * @file AbsoloteToRelativeJumps.cc
 *
 * Definition of AbsoloteToRelativeJumps class.
 *
 * @author Heikki Kultala 2019 (heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "AbsoluteToRelativeJumps.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "Machine.hh"
#include "ControlUnit.hh"
#include "Operation.hh"
#include "TerminalImmediate.hh"
#include "TerminalFUPort.hh"
#include "Immediate.hh"
#include "InstructionReferenceManager.hh"
#include "Program.hh"

namespace TTAMachine {
    class HWOperation;
}


void AbsoluteToRelativeJumps::handleProcedure(
    TTAProgram::Procedure& procedure,
    const TTAMachine::Machine& targetMachine) {

    std::map<TTAProgram::Move*, int> jumpOffsetMoves;
    std::map<std::shared_ptr<TTAProgram::Immediate>, int> jumpOffsetLimms;

    auto cu = targetMachine.controlUnit();
    if (!cu->hasOperation("reljump")) {
        return;
    } 

    auto& irm = procedure.parent().instructionReferenceManager();
    auto hwop = cu->operation("reljump");
    
    for (int i = 0; i < procedure.instructionCount(); i++) {
        auto& ins = procedure[i];
        for (int j = 0; j < ins.moveCount(); j++) {
            auto& m = ins.move(j);
            auto& src = m.source();
            auto& dst = m.destination();
            if (!dst.isFUPort()) {
                continue;
            }
            auto& dfu = dst.functionUnit();
            if (&dfu != targetMachine.controlUnit()) {
                continue;
            }
            if (!dst.isOpcodeSetting()) {
                continue;
            }
            if (dst.operation().name() != "JUMP") {
                continue;
            }

            int jumpAddr = ins.address().location();

            if (src.isInstructionAddress()) {
                int dstAddr = src.value().intValue();
                int diff = dstAddr - jumpAddr;

                auto& bus = m.bus();
                if (!bus.signExtends() && diff < 0 && bus.immediateWidth() < 32) {
                    continue;
                }

                jumpOffsetMoves[&m] = diff;
                m.setDestination(new TTAProgram::TerminalFUPort(*hwop, 1));
                continue;
            }

            if (src.isImmediateRegister()) {
                auto& immu = src.immediateUnit();
                int index = static_cast<int>(src.index());
                bool found = false;
                for (int k = i - immu.latency(); k >= 0 && !found; k--) {
                    auto &immIns = procedure.instructionAtIndex(k);
                    
                    for (int l = 0; l < immIns.immediateCount(); l++) {
                        auto imm = immIns.immediatePtr(l);
                        if (imm->destination().index() == index &&
                            &imm->destination().immediateUnit() == &immu) {
                            int dstAddr = imm->value().value().intValue();
                            int diff = dstAddr - jumpAddr;

                            if (!immu.signExtends() && diff < 0 && immu.width() < 32) {
                                found = true;
                                break;
                            }

                            jumpOffsetLimms[imm] = diff;
                            m.setDestination(new TTAProgram::TerminalFUPort(*hwop, 1));
                            found = true;
                            break;
                        }
                    }
                    if (irm.hasReference(immIns)) {
                        break;
                    }
                }
            }
        }
    }

    // Only update the indeces at the end to keep instr references valid.
    for (auto i : jumpOffsetMoves) {
        i.first->setSource(new TTAProgram::TerminalImmediate(SimValue(i.second, 32)));
    }

    for (auto i : jumpOffsetLimms) {
        i.first->setValue(new TTAProgram::TerminalImmediate(SimValue(i.second, 32)));
    }
}
