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
 * @file CFGStatistics.hh
 *
 * Declaration of prototype class that collects statistics about
 * control flow graph.
 *
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TCE_CFGSTATISTICS_HH
#define TCE_CFGSTATISTICS_HH

#include "BasicBlock.hh"
/**
 * Represents statistics about single control flow graph.
 * Count of BB, sum of moves, immediates, instructions and bypasses as well as
 * largest basic block and immediates, instructions and bypassed in it.
 */

class CFGStatistics : public BasicBlockStatistics {
public:
    CFGStatistics();
    virtual ~CFGStatistics();
    virtual int normalBBCount() const;
    virtual int maxMoveCount() const;
    virtual int maxImmediateCount() const;
    virtual int maxInstructionCount() const;
    virtual int maxBypassedCount() const;
    virtual void setNormalBBCount(int);
    virtual void setMaxMoveCount(int);
    virtual void setMaxImmediateCount(int);
    virtual void setMaxInstructionCount(int);
    virtual void setMaxBypassedCount(int);

private:    
    int normalBBCount_;
    int maxMoveCount_;
    int maxImmediateCount_;
    int maxInstructionCount_;
    int maxBypassCount_;
};

#endif
