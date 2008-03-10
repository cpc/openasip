#ifndef TTA_STACK_FRAME_HH
#define TTA_STACK_FRAME_HH

#include <vector>

#include "Exception.hh"
#include "CodeSnippet.hh"

#include "StackFrameData.hh"

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Terminal;
    class TerminalRegister;
    class CodeSnippet;
    class ProgramAnnotation;
}

class UniversalMachine;
class MoveNode;

class StackFrameCodeCreator {
public:
    StackFrameCodeCreator(const TTAMachine::Machine& machine,
                          const UniversalMachine& uMach,
                          const TTAProgram::Terminal& stackPointer);

    ~StackFrameCodeCreator();

    void clearParameters();
    void setParameters(StackFrameData& sfd);
    void setOriginalLocalVariablesSize(int size);
    void setSpilledIntegerCount(int count);
    void setSpilledFPRCount(int count);
    void setOutputParamsSize(int size);
    void addContextSave(TTAProgram::TerminalRegister& tr);
    void setLeafFunction();
    void setSP(TTAProgram::TerminalRegister& sp);
    
    void createAndUpdate(TTAProgram::Procedure& proc)
        throw (IllegalMachine, IllegalProgram);
    void createStackFrameCode(TTAProgram::Procedure& proc)
        throw (IllegalMachine, IllegalProgram);
    void updateStackReferences(TTAProgram::Procedure& proc);

    void
    createSpillLoad(
        MoveNode& moveNode,
        const TTAProgram::TerminalRegister& temp, int spillIndex);

    void
    createSpillStore(
        TTAProgram::Move& move,
        const TTAProgram::TerminalRegister& temp1,
        const TTAProgram::TerminalRegister& temp2, int spillIndex);

private:

    TTAProgram::CodeSnippet*
    createSpillLoadCode(
        const TTAProgram::Terminal& dest, 
        const TTAProgram::TerminalRegister& temp, int spillIndex);

    TTAProgram::CodeSnippet*
    createSpillStoreCode(
        const TTAProgram::Terminal& dest, 
        const TTAProgram::TerminalRegister& temp1,
        const TTAProgram::TerminalRegister& temp2, int spillIndex);

    int createOutParamOffset(int offset) throw (IllegalProgram);
    int createLocalVarOffset(int offset);
    int createRAOffset() throw (IllegalProgram);
    int createInParamOffset(int offset);

    int createStackOffset(    
        TTAProgram::ProgramAnnotation& anno) throw (IllegalProgram);

    void updateStackReference(TTAProgram::Move& move);

    TTAProgram::CodeSnippet* createCRT0Code() throw (IllegalMachine);

    TTAProgram::CodeSnippet* createOffsetCalculation(
        const TTAProgram::Terminal& destination, int offset);
    
    TTAProgram::CodeSnippet* createStore (
        const TTAProgram::Terminal& source, 
        const TTAProgram::Terminal& address );


//        TTAProgram::ProgramAnnotation::Id firstAnnotationID,
//        TTAProgram::ProgramAnnotation::Id secondAnnotationID);

    TTAProgram::CodeSnippet* createLoad (
        const TTAProgram::Terminal& destination, 
        const TTAProgram::Terminal& address);

    TTAProgram::CodeSnippet* createLoad (
        const TTAProgram::Terminal& destination, 
        const TTAProgram::Terminal& address,
        TTAProgram::ProgramAnnotation::Id firstAnnotationID,
        TTAProgram::ProgramAnnotation::Id secondAnnotationID);

    TTAProgram::CodeSnippet* createSPUpdate (
        int offset, TTAProgram::ProgramAnnotation::Id annotationID);

    TTAProgram::CodeSnippet* createSPUpdate (
        int offset, TTAProgram::ProgramAnnotation::Id firstAnnotationID,
        TTAProgram::ProgramAnnotation::Id annotationID);

    TTAProgram::CodeSnippet* createStackStore (
        const TTAProgram::Terminal& source);

    TTAProgram::CodeSnippet* createStackStore (
        const TTAProgram::Terminal& source,
        TTAProgram::ProgramAnnotation::Id annotationID);

    TTAProgram::CodeSnippet* createStackStore (
        const TTAProgram::Terminal& source,
        TTAProgram::ProgramAnnotation::Id firstAnnotationID,
        TTAProgram::ProgramAnnotation::Id secondAnnotationID);

    TTAProgram::CodeSnippet* createStackLoad (
        const TTAProgram::Terminal& destination,
        TTAProgram::ProgramAnnotation::Id firstAnnotationID,
        TTAProgram::ProgramAnnotation::Id secondAnnotationID);

    TTAProgram::CodeSnippet* createStackLoad (
        const TTAProgram::Terminal& destination,
        TTAProgram::ProgramAnnotation::Id annotationID);

    TTAProgram::CodeSnippet* createPush(
        const TTAProgram::Terminal& terminal);

    TTAProgram::CodeSnippet* createPop(
        const TTAProgram::Terminal& terminal);

    TTAProgram::CodeSnippet* createConstructionCode();
    TTAProgram::CodeSnippet* createDeconstructionCode();

    TTAMachine::HWOperation* addHWOp_;
    TTAMachine::HWOperation* loadHWOp_;
    TTAMachine::HWOperation* storeHWOp_;
    TTAMachine::HWOperation* jumpHWOp_;
    TTAMachine::Bus* uvmBus_;
    TTAProgram::Terminal* RATerminal_;

    StackFrameData* sfd_;
    TTAProgram::Terminal* stackPointer_;
    const TTAMachine::Machine& machine_;
};

#endif


/* Original stack frame layout of old frontend
  Input parameters ( belongs to parent stack frame )
  Local variables
  RA storage
  Saved registers
  Output parameters
*/



/* Final stack frame layout(1st version)
  Input parameters ( belongs to parent stack frame )
  Spilled variables(fp)
  Spilled variables(int)
  Local variables
  RA storage
  Saved registers(int)
  Saved registers(fp)
  Output parameters
*/

/* Final stack frame layout(optimized version)
  Input parameters ( belongs to parent stack frame )
  Saved registers(int)
  Saved registers(fp)
  Spilled variables(fp)
  Spilled variables(int)
  Local variables
  RA storage
  Output parameters

  Benefits compared to 1st version:

  1: All storage allocation can be done at once.
  2: RA is known earlier; more stuff to put into function return delay slot.

  Pitfalls compared to 1st version:
  
  1: RA storage needs offset

*/



  
