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
 * @file BasicBlock.hh
 *
 * Declaration of the BasicBlock class.
 *
 * @author Pekka J��skel�inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BASIC_BLOCK_HH
#define TTA_BASIC_BLOCK_HH

#include <vector>
#include "Exception.hh"
#include "CodeSnippet.hh"

namespace TTAProgram {
    class Instruction;
    class CodeSnippet;
}

/**
 * Represents statistics about single basic block.
 */
class BasicBlockStatistics {
public:
    BasicBlockStatistics();
    virtual ~BasicBlockStatistics();
    virtual int moveCount() const;
    virtual int immediateCount() const;
    virtual int instructionCount() const;
    virtual int bypassedCount() const;
    virtual void setMoveCount(int);
    virtual void setImmediateCount(int);
    virtual void setInstructionCount(int);
    virtual void setBypassedCount(int);
private:
    int moveCount_;
    int immediateCount_;
    int instructionCount_;
    int bypassedCount_; 
};

/**
 * Represents contents and information of one basic block.
 *
 * A basic block is defined as an ordered sequence of adjacent instructions
 * in which only the first can be target of jump and only the last one (save
 * jump delay slots) can contain jumps.
 *
 * @note Instructions that contain function calls are control flow barriers
 * also. For now they split basic blocks into two, with special type of edge
 * connecting them.
 */
class BasicBlock : public TTAProgram::CodeSnippet {
public:

    BasicBlock();
    virtual ~BasicBlock();

    BasicBlock* copy() const;
    virtual void clear();
    
    std::string disassemble() const;

    int skippedFirstInstructions() const;
    void skipFirstInstructions(int count);
    const BasicBlockStatistics& statistics();
private:
    int skippedFirstInstructions_;
    bool statisticsValid_;
    BasicBlockStatistics statistics_;    
};

#endif

