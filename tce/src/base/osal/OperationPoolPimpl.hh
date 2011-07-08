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
 * @file OperationPoolPimpl.hh
 *
 * Declaration of OperationPoolPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef OPERATIONPOOL_PIMPL_HH
#define OPERATIONPOOL_PIMPL_HH

#include <string>
#include <map>
#include "tce_config.h"

class OperationPool;
class OperationBehaviorLoader;
class OperationBehaviorProxy;
class OperationIndex;
class Operation;
namespace llvm {
#ifdef LLVM_2_9
    class TargetInstrDesc;
    class TargetInstrInfo;
#else
    class MCInstrDesc;
    class MCInstrInfo;
#endif
}

/**
 * A private implementation class for OperationPool
 */
class OperationPoolPimpl {
public:
    friend class OperationPool;
    ~OperationPoolPimpl();
    
    Operation& operation(const char* name);
    OperationIndex& index();

    static void cleanupCache();

#ifdef LLVM_2_9
    static void setLLVMTargetInstrInfo(const llvm::TargetInstrInfo* tid) {
        llvmTargetInstrInfo_ = tid;
    }
#else
    static void setLLVMTargetInstrInfo(const llvm::MCInstrInfo* tid) {
        llvmTargetInstrInfo_ = tid;
    }
#endif
private:
    OperationPoolPimpl();
    
    /// Container for operations indexed by their names.
    typedef std::map<std::string, Operation*> OperationTable;

    /// Copying not allowed.
    OperationPoolPimpl(const OperationPoolPimpl&);
    /// Assignment not allowed.
    OperationPoolPimpl& operator=(const OperationPoolPimpl&);
#ifdef LLVM_2_9
    Operation* loadFromLLVM(const llvm::TargetInstrDesc& tid);
#else
    Operation* loadFromLLVM(const llvm::MCInstrDesc& tid);
#endif
    /// Operation pool uses this to load behavior models of the operations.
    static OperationBehaviorLoader* loader_;
    /// Indexed table used to find out which operation module contains the
    /// given operation.
    static OperationIndex* index_;
    
    OperationSerializer serializer_;
    
    /// Contains all operations that have been already requested by the client.
    static OperationTable operationCache_;
    /// Contains all operation behavior proxies.
    static std::vector<OperationBehaviorProxy*> proxies_; 
    /// If this is set, OSAL data is loaded from the TargetInstrInfo
    /// instead of .opp XML files. Used when calling the TCE scheduler from
    /// non-TTA LLVM targets.
#ifdef LLVM_2_9
    static const llvm::TargetInstrInfo* llvmTargetInstrInfo_;
#else
    static const llvm::MCInstrInfo* llvmTargetInstrInfo_;
#endif
};

#endif

