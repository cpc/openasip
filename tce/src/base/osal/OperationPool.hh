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
 * @file OperationPool.hh
 *
 * Declaration of OperationPool class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#ifndef TTA_OPERATION_POOL_HH
#define TTA_OPERATION_POOL_HH

class OperationBehaviorLoader;
class OperationBehaviorProxy;
class OperationIndex;
class Operation;
class OperationPoolPimpl;
namespace llvm {
    class TargetInstrInfo;
}

/**
 * OperationPool provides interface for obtaining operations of the target
 * architecture template.
 */
class OperationPool {
public:
    OperationPool();
    virtual ~OperationPool();

    Operation& operation(const char* name);
    OperationIndex& index();

    static void cleanupCache();

    static void setLLVMTargetInstrInfo(const llvm::TargetInstrInfo* tid);
  
private:
    /// Copying not allowed.
    OperationPool(const OperationPool&);
    /// Assignment not allowed.
    OperationPool& operator=(const OperationPool&);
    
    /// Private implementation in a separate source file.
    OperationPoolPimpl* pimpl_;
};

#endif
