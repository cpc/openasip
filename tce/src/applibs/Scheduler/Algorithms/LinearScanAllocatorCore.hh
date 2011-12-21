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
 * @file LinearScanAllocatorCore.hh
 *
 * Declaration of LinearScanAllocatorCore class.
 * 
 * Linear scan register allocator, actual implementation.
 *
 * @author Heikki Kultala 2007 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_LINEAR_SCAN_ALLOCATOR_CORE_HH
#define TTA_LINEAR_SCAN_ALLOCATOR_CORE_HH

#include <map>
#include <vector>
#include <set>
#include <list>

#include "StackCodeRemover.hh"

using std::map;
using std::vector;
using std::set;

namespace TTAProgram {
    class Move;
    class Procedure;
    class Program;
    class TerminalRegister;
    class Terminal;
}

namespace TTAMachine {
    class Machine;
}

class MoveNode;
class RegisterMap;
class StackFrameData;
class DataDependenceGraph;
class DataDependenceEdge;
class ControlFlowGraph;
class StackFrameCodeCreator;
class UniversalMachine;
class InterPassData;

class LinearScanAllocatorCore {
public:
    LinearScanAllocatorCore();
    virtual ~LinearScanAllocatorCore();

    void initialize(const TTAMachine::Machine& machine,
                    TTAProgram::Program& program, InterPassData& ipd);

    void allocateProcedure(TTAProgram::Procedure& procedure);
                            
private:
    class Variable;

    void collectVariables();
    void calculateLiveRanges();
    void initProcedure(TTAProgram::Procedure& proc);
    void sortLiveRanges();
    void allocateStatic();
    void allocate();
    void selectSpills(); // select what is going to be spilled
    void updateStackReferences();

    void updateMoves();

    bool updateMove(MoveNode& move);
    void updateMoveSrc(MoveNode& move);
    bool updateMoveDest(MoveNode& move);
    unsigned int reserveGPR(unsigned int var);
    unsigned int reserveFPR(unsigned int var);
    void freeReg(unsigned int var );
    bool isReservedReg(unsigned int var);
    void updateSavedRegs();
    void createCSCode(); 

    std::map<int, Variable*> variables_;
    
    std::vector<Variable*> variablesByBirth_;
    std::vector<Variable*> variablesByDeath_;

    typedef std::map<int,int> AllocationIndexMap;
    AllocationIndexMap regAllocatedIndecesByValue_;
    AllocationIndexMap regValuesByAllocatedIndex_;

    std::map<unsigned int, unsigned int> allocatedUseCount_;
    std::set<unsigned int> regNeedsContextSave_;

    unsigned int noSaveGprCount_;
    unsigned int calleeSaveGprCount_;
    unsigned int noSaveFprCount_;
    unsigned int calleeSaveFprCount_;

    const TTAProgram::TerminalRegister* intTmp1_;
    const TTAProgram::TerminalRegister* intTmp2_;

    const TTAProgram::TerminalRegister* fprTmp1_;
    const TTAProgram::TerminalRegister* gprTmp2_;

    const UniversalMachine* um_;
    TTAProgram::Procedure* proc_;
    DataDependenceGraph* ddg_;
    ControlFlowGraph* cfg_;

    RegisterMap* regMap_;
//    StackManager* stackManager_;
    StackFrameCodeCreator* stackFrameCodeCreator_;
    StackCodeRemover scRemover_;
    StackFrameData* stackFrameData_;

    unsigned int intSpillCount_;
    unsigned int floatSpillCount_;

    unsigned int calleeSaveGprStartScheduled_;
    unsigned int calleeSaveFprStartScheduled_;

    static const unsigned int CALLEE_SAVE_GPR_START_SEQUENTIAL = 256;
    static const unsigned int FPR_OFFSET = 1024;    
    static const unsigned int CALLEE_SAVE_FPR_START_SEQUENTIAL =256+FPR_OFFSET;
    
    static const unsigned int GLOBAL_GPR_COUNT = 6;
    static const unsigned int GLOBAL_FPR_COUNT = 5;

    class AllocatedVariable {
    public:
        virtual bool isSpill() const { return false; };
        virtual void convertUse(MoveNode& movenode,
                                const TTAProgram::TerminalRegister& temp) 
            const = 0;
        virtual void convertSave(MoveNode& modenode,
                                 const TTAProgram::TerminalRegister& temp1,
                                 const TTAProgram::TerminalRegister& temp2) 
            const = 0;
        virtual ~AllocatedVariable() {}
    };

    class SpilledVariable : public AllocatedVariable {
    public:
        SpilledVariable(
            StackFrameCodeCreator& sfcc, int index);

        bool isSpill() const;
        
        void convertUse(MoveNode& movenode,
                        const TTAProgram::TerminalRegister& temp) const;
        void convertSave(MoveNode& modenode,
                         const TTAProgram::TerminalRegister& temp1,
                         const TTAProgram::TerminalRegister& temp2) const;
    protected:
        int spillIndex_;
//        StackManager& stackManager_;
        StackFrameCodeCreator& sfcc_;
    };


    class RegisterVariable: public AllocatedVariable {
    public:
        RegisterVariable(
            const TTAProgram::TerminalRegister& tr);
        
        void convertUse(MoveNode& movenode,
                        const TTAProgram::TerminalRegister& temp) const;
        void convertSave(MoveNode& modenode,
                         const TTAProgram::TerminalRegister& temp1,
                         const TTAProgram::TerminalRegister& temp2) const ;
    private:
        const TTAProgram::TerminalRegister& allocatedRegister_;
    };
    
    std::map<int,AllocatedVariable*> regAllocations_;

    class Variable {
    public:

        enum VarType { VAR_INT, VAR_BOOL, VAR_FP };

        Variable(int index, VarType type, 
                 DataDependenceGraph& ddg,
                 ControlFlowGraph& cfg);

        int birthTime() const;
        int deathTime() const;
        unsigned int originalIndex() const;

        void addRead(MoveNode& readNode);
        void addWrite(MoveNode& writeNode);

        void addEdge(DataDependenceEdge& edge,MoveNode& tail, MoveNode& head);

        int edgeCount() const;

        int readCount() const;
        int writeCount() const;

        VarType type() const;

        void calculateRange(TTAProgram::Procedure& proc);

        void setRegister(int index) {
            allocatedRegIndex_ = index;
        }

        bool operator<( const Variable& other);

        bool isSameVariable(TTAProgram::Terminal& tr);

    private:
        int moveAddress(MoveNode& mn);

        MoveNode& searchFirstWrite();
        MoveNode& searchLastRead();
    
        std::vector<MoveNode*> reads_;
        std::vector<MoveNode*> writes_;
        std::vector<DataDependenceEdge*> edges_;
        std::vector<MoveNode*> tails_;
        std::vector<MoveNode*> heads_;

        /* these are calculated */
        int bIndex_;
        int dIndex_;

        int allocatedRegIndex_;
        int umRegIndex_;
        VarType type_;
        DataDependenceGraph& ddg_;
        ControlFlowGraph& cfg_;
    };

    class VariableBirthComparator {
    public:
        bool operator() (const Variable* var1, const Variable* var2);
    };

    class VariableDeathComparator {
    public:
        bool operator() (const Variable* var1, const Variable* var2);
    };
};

#endif
