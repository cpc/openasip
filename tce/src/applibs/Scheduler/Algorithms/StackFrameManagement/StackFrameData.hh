/**
 * @file StackFrameData.hh
 * 
 * Declaration of StackFrameData class
 *
 * Contains information of a procedure's stack frame.
 *
 */

#include <vector>

namespace TTAProgram {
    class TerminalRegister;
    class Terminal;
}

#ifndef STACK_FRAME_DATA_HH
#define STACK_FRAME_DATA_HH

class StackFrameData {
public: 
    StackFrameData();
    ~StackFrameData();

    StackFrameData(int localVarSize, int stackInitAddr) ;
    StackFrameData(bool crt0);

    bool isLeafProcedure() const;
    int localVarSize() const;
    int totalLocalVarSize() const;
    int outputParamsSize() const;
    int fprSaveCount() const;
    int gprSaveCount() const;
    int registerSaveCount() const; 

    int constrCodeSize() const;
    int deconstrCodeSize() const;

    int stackInitAddr() const;
    bool isCrt0() const;
    int spilledIntegerCount() const;
    int spilledFloatCount() const;

    void clearRegisterSaves();
    void setLeafProcedure(bool leafProcedure);
    void setCodeSizes(int consCodeSize, int deconstrCodeSize);
    void setOutputParamsSize(int outputParamsSize);
    void setLocalVarSize(int size);
    void setSpilledIntegerCount(int count);
    void setSpilledFloatCount(int count);

    void setStackInitAddr(int stackInitializer);
    
    void addRegisterSave(const TTAProgram::Terminal& terminal);

    const TTAProgram::TerminalRegister& getSavedGPR(int index) const;
    const TTAProgram::TerminalRegister& getSavedFPR(int index) const;
    const TTAProgram::TerminalRegister& getSavedRegister(int index) const;

private:
    int constrCodeSize_;
    int deconstrCodeSize_;
    int localVarSize_;
    int outputParamsSize_;
    int spilledIntegerCount_;
    int spilledFloatCount_;

//    int gprSaveCount_;
//    int fprSaveCount_;
    bool leafProcedure_;
    bool crt0_;
    int stackInitAddr_;

    std::vector<TTAProgram::TerminalRegister*> savedGPRs_;
    std::vector<TTAProgram::TerminalRegister*> savedFPRs_;
};

#endif
