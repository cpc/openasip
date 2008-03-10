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

