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
 * @file PRegionAliasAnalyzer.cc
 *
 * Implementation of PRegionAliasAnalyzer class.
 * 
 * Too much copy-paste here.
 *
 * TODO: This should be handled by memory categories, not alias analyzers. 
 * This may cause too much ddg edges, slowing things down
 *
 * @author Faked Heikki Kultala 2007 (heikki.kultala-no.spam-tut.fi)
 * Real auther from log: 1737  pekka.j
 * @note rating: red
 */

#include "PRegionAliasAnalyzer.hh"

#include "MoveNode.hh"
#include "Move.hh"
#include "DataDependenceGraph.hh"
#include "Terminal.hh"

using namespace TTAProgram;
using namespace TTAMachine;

bool 
PRegionAliasAnalyzer::isAddressTraceable(
    DataDependenceGraph&, const ProgramOperation&) {

    return true;
}

MemoryAliasAnalyzer::AliasingResult
PRegionAliasAnalyzer::analyze(
    DataDependenceGraph&, 
    const ProgramOperation& pop1, 
    const ProgramOperation& pop2,
    MoveNodeUse::BBRelation) {

    const MoveNode *rawSrc1 = addressOperandMove(pop1);
    const MoveNode *rawSrc2 = addressOperandMove(pop2);
    if (rawSrc1 == NULL || rawSrc2 == NULL ||
        !rawSrc1->isMove() || !rawSrc2->isMove()) return ALIAS_UNKNOWN;

    const TTAProgram::Move &m1 = rawSrc1->move();
    const TTAProgram::Move &m2 = rawSrc2->move();
    if (!m1.hasAnnotations(ProgramAnnotation::ANN_PARALLEL_REGION_ID) ||
        !m2.hasAnnotations(ProgramAnnotation::ANN_PARALLEL_REGION_ID))
        return ALIAS_UNKNOWN;

    int pregion1 = m1.annotation(
        0, ProgramAnnotation::ANN_PARALLEL_REGION_ID).intValue();
    int pregion2 = m2.annotation(
        0, ProgramAnnotation::ANN_PARALLEL_REGION_ID).intValue();

    if (pregion1 != pregion2) {

#if 0
        Application::logStream()
            << "### based on PREGION info, removed a memory edge between "
            << m1.toString() << " (" << pregion1 << ") and "
            << m2.toString() << " (" << pregion2 << ")" << std::endl;
        Application::logStream()
            << "### src lines: ";
        if (m1.hasSourceLineNumber())
            Application::logStream()
                << m1.sourceLineNumber() << " ";
        if (m2.hasSourceLineNumber())
            Application::logStream()
                << m2.sourceLineNumber() << " ";
        Application::logStream() << std::endl;
#endif        
        return ALIAS_FALSE;
    }

    return ALIAS_UNKNOWN;
}

PRegionAliasAnalyzer::~PRegionAliasAnalyzer() {}
