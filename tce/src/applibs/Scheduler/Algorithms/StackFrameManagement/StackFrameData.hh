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
