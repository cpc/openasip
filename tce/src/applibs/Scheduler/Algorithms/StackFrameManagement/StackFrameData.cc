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
