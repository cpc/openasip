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
 * @file BFUpdateMoveOnBypass.cc
 *
 * Definition of scheduler operation which updates move to make a bypass.
 *
 * @author Heikki Kultala 2020 (hkultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFUpdateMoveOnBypass.hh"
#include "ProgramOperation.hh"
#include "ProgramAnnotation.hh"
#include "Move.hh"
#include "Terminal.hh"

using TTAProgram::ProgramAnnotation;

bool BFUpdateMoveOnBypass::operator()() {

    auto& dm = dst_.move();
    auto& sm = src_.move();

    // update the move
    dm.setSource(sm.source().copy());

    if (!src_.isSourceOperation()) {
        return true;
    }

    ProgramOperationPtr srcOp = src_.sourceOperationPtr();
    srcOp->addOutputNode(dst_);
    dst_.setSourceOperationPtr(srcOp);

    // set fu annotations
    for (int j = 0; j < sm.annotationCount(); j++) {
        TTAProgram::ProgramAnnotation anno = sm.annotation(j);
        if (anno.id() == ProgramAnnotation::ANN_ALLOWED_UNIT_SRC) {
            dm.addAnnotation(
                ProgramAnnotation(
                    ProgramAnnotation::ANN_ALLOWED_UNIT_SRC,
                    anno.payload()));
        }
        if (anno.id() ==
            ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC) {
            dm.addAnnotation(
                ProgramAnnotation(
                    ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC,
                    anno.payload()));
        }
        if (anno.id() ==
            ProgramAnnotation::ANN_REJECTED_UNIT_SRC) {
            dm.addAnnotation(
                ProgramAnnotation(
                    ProgramAnnotation::ANN_REJECTED_UNIT_SRC,
                    anno.payload()));
        }
    }
    return true;
}

void BFUpdateMoveOnBypass::undoOnlyMe() {
        // unset programoperation from bypassed
    if (dst_.isSourceOperation()) {
        ProgramOperation& srcOp = dst_.sourceOperation();
        srcOp.removeOutputNode(dst_);
        dst_.unsetSourceOperation();

        // unset fu annotations
        dst_.move().removeAnnotations(
            TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC);
        dst_.move().removeAnnotations(
            TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC);
        dst_.move().removeAnnotations(
            TTAProgram::ProgramAnnotation::ANN_REJECTED_UNIT_SRC);
    }
    // do the actual unmerge by returning source to original register.
    dst_.move().setSource(src_.move().destination().copy());
}
