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
 * Implementation of StackFrameData class
 *
 * Contains information of a procedure's stack frame.
 */

#include "StackFrameData.hh"

#include "Terminal.hh"
#include "TerminalRegister.hh"
#include "RegisterFile.hh"

//using namespace TTAMachine;
using namespace TTAProgram;

StackFrameData::StackFrameData() : 
    constrCodeSize_(0), deconstrCodeSize_(0), localVarSize_(0),
    outputParamsSize_(0), spilledIntegerCount_(0), spilledFloatCount_(0),
    leafProcedure_(true), crt0_(false), stackInitAddr_(0) {
}

StackFrameData::StackFrameData(int localVarSize, int stackInitAddr) :
    constrCodeSize_(0), deconstrCodeSize_(0), localVarSize_(localVarSize),
    outputParamsSize_(0), spilledIntegerCount_(0), spilledFloatCount_(0),
    leafProcedure_(true), crt0_(stackInitAddr!=0), 
    stackInitAddr_(stackInitAddr) {
}

StackFrameData::StackFrameData(bool crt0) : 
    constrCodeSize_(0), deconstrCodeSize_(0), localVarSize_(0),
    outputParamsSize_(0), spilledIntegerCount_(0), spilledFloatCount_(0),
    leafProcedure_(true), crt0_(crt0), stackInitAddr_(0) {
}

bool
StackFrameData::isLeafProcedure() const {
    return leafProcedure_;
}

/**
 * Returns the original local variable space in stack frame.
 *
 * @return local var size before register spilling
 */
int
StackFrameData::localVarSize() const {
    return localVarSize_;
}

/**
 * Returns the updated local variable space onn stack frame after spilling
 *
 * @return local var size including space for spilled variables.
 */
int
StackFrameData::totalLocalVarSize() const {
    return localVarSize_ + (spilledIntegerCount()*4) + 
        (spilledFloatCount()*8);
}

int
StackFrameData::outputParamsSize() const {
    return outputParamsSize_;
}

int
StackFrameData::fprSaveCount() const {
    return savedFPRs_.size();
}

int
StackFrameData::gprSaveCount() const {
    return savedGPRs_.size();
}

int
StackFrameData::registerSaveCount() const {
    return gprSaveCount() + fprSaveCount();
}

int StackFrameData::stackInitAddr() const {
    return stackInitAddr_;
}

bool StackFrameData::isCrt0() const {
    return crt0_;
}

int StackFrameData::constrCodeSize() const {
    return constrCodeSize_;
}

int StackFrameData::deconstrCodeSize() const {
    return deconstrCodeSize_;
}


const TTAProgram::TerminalRegister&
StackFrameData::getSavedRegister(int index) const {
    if( index < gprSaveCount()) {
        return *savedGPRs_.at(index);
    } else {
        return *savedFPRs_.at(index - gprSaveCount());
    }
}

void
StackFrameData::setLeafProcedure(bool leafProcedure) {
    leafProcedure_ = leafProcedure;
}

void
StackFrameData::setLocalVarSize(int localVarSize) {
    localVarSize_ = localVarSize;
}

int
StackFrameData::spilledFloatCount() const {
    return spilledFloatCount_;
}

int
StackFrameData::spilledIntegerCount() const {
    return spilledIntegerCount_;
}

void 
StackFrameData::setOutputParamsSize(int outputParamsSize) {
    outputParamsSize_ = outputParamsSize;
}

void
StackFrameData::setStackInitAddr(int stackInitAddr) {
    stackInitAddr_ = stackInitAddr;
    crt0_ = true;
}

void 
StackFrameData::addRegisterSave(const TTAProgram::Terminal& terminal) {
    Terminal* term = terminal.copy();
    TerminalRegister* tr = dynamic_cast<TerminalRegister*>(term);
    if( tr == NULL ) {
        delete term;
        throw IllegalProgram(
            __FILE__,__LINE__,"", "Terminal not terminalregister");
    }
    if( tr->registerFile().width() == 32 ) {
        savedGPRs_.push_back(tr);
    } else if(tr->registerFile().width() == 64 ) {
        savedFPRs_.push_back(tr);
    } else {
        delete term;
        throw IllegalProgram(
            __FILE__,__LINE__,"", "Strange register save");
    }
    
}

StackFrameData::~StackFrameData() {
    for (unsigned int i = 0; i < savedGPRs_.size(); i++) {
        delete savedGPRs_.at(i);
    }

    for (unsigned int i = 0; i < savedFPRs_.size(); i++) {
        delete savedFPRs_.at(i);
    }
}

/**
 * Sets the count of (sequential) instructions in the stack frame construction 
 * and destruction codes in the function.
 *
 * @param constrCodeSize Count of instructions in construction of the stack
 *        frame.
 * @param deconstrCodeSize Count of instructions in construction of the stack
 *        frame. This includes all stack unwind instructions before the 
 *        procedure return.
 */
void 
StackFrameData::setCodeSizes(int constrCodeSize, int deconstrCodeSize) {
    constrCodeSize_ = constrCodeSize;
    deconstrCodeSize_ = deconstrCodeSize;
}

void 
StackFrameData::setSpilledIntegerCount(int spilledIntegerCount) {
    spilledIntegerCount_ = spilledIntegerCount;
}

void 
StackFrameData::setSpilledFloatCount(int spilledFloatCount) {
    spilledFloatCount_ = spilledFloatCount;
}
