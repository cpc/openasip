/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
