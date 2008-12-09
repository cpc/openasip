/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file CFGStatistics.cc
 *
 * Implementation of prototype class that collects statistics about
 * control flow graph.
 *
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "CFGStatistics.hh"
/**
 * Constructor, creates statistics structure with zero content.
 */
CFGStatistics::CFGStatistics() :
    BasicBlockStatistics(),
    normalBBCount_(0),
    maxMoveCount_(0), 
    maxImmediateCount_(0), 
    maxInstructionCount_(0), 
    maxBypassCount_(0) {
}

/**
 * To remove compile warning.
 */
CFGStatistics::~CFGStatistics() {
}

/**
 * Returns maximum move count from statistics object.
 * @return count of moves of largest BB
 */
int 
CFGStatistics::maxMoveCount() const {
    return maxMoveCount_;
}
/**
 * Returns immediate count from statistics object for largest BB.
 * @return count of immediates stored in object
 */
int 
CFGStatistics::maxImmediateCount() const {
    return maxImmediateCount_;
}
/**
 * Returns instruction count from statistics object for largest BB.
 * @return count of instructions stored in object
 */
int 
CFGStatistics::maxInstructionCount() const {
    return maxInstructionCount_;
}
/**
 * Returns bypassed move count from statistics object for largest BB.
 * @return count of bypassed moves stored in object
 */
int 
CFGStatistics::maxBypassedCount() const {
    return maxBypassCount_;
}

/**
 * Returns the number of basic blocks in procedure.
 */
int
CFGStatistics::normalBBCount() const {
    return normalBBCount_;
}
/**
 * Sets the move count in statistic object.
 * 
 * @param count number of moves to store in object
 */
void 
CFGStatistics::setMaxMoveCount(int count) {
    maxMoveCount_ = count;
}
/**
 * Sets the immediate count in statistic object.
 * 
 * @param count number of immediates to store in object
 */

void 
CFGStatistics::setMaxImmediateCount(int count) {
    maxImmediateCount_ = count;
}
/**
 * Sets the instruction count in statistic object.
 * 
 * @param count number of instructions to store in object
 */

void 
CFGStatistics::setMaxInstructionCount(int count) {
    maxInstructionCount_ = count;
}
/**
 * Sets the bypassed move count in statistic object.
 * 
 * @param count number of bypassed moves to store in object
 */
void 
CFGStatistics::setMaxBypassedCount(int count) {
    maxBypassCount_ = count;
}

/**
 * Sets count of basic blocks in procedure into statistics object.
 * 
 * @param count number of basic blocks in procedure.
 */
void
CFGStatistics::setNormalBBCount(int count) {
    normalBBCount_ = count;
}
