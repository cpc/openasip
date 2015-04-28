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
 * @file OperationPool.cc
 *
 * Definition of OperationPool class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @author Miakel Lepistö 2007 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#include <string>
#include <vector>

#include "OperationPool.hh"
#include "OperationModule.hh"
#include "Operation.hh"
#include "OperationBehaviorProxy.hh"
#include "OperationDAGBehavior.hh"
#include "OperationBehaviorLoader.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "Application.hh"
#include "AssocTools.hh"
#include "SequenceTools.hh"
#include "StringTools.hh"
#include "Application.hh"
#include "OperationIndex.hh"
#include "OperationPoolPimpl.hh"

using std::string;
using std::vector;


/**
 * Constructor.
 *
 * Records the search paths of the operation modules.
 */
OperationPool::OperationPool() : pimpl_(new OperationPoolPimpl()) {
}

/**
 * Destructor.
 *
 * Cleans proxies and operations.
 */
OperationPool::~OperationPool() {
    delete pimpl_;
    pimpl_ = NULL;
}

/**
 * Cleans up the static Operation cache.
 *
 * Deletes also the Operation instances, so be sure you are not using
 * them after calling this!
 */
void 
OperationPool::cleanupCache() {
    OperationPoolPimpl::cleanupCache();
}


/**
 * Looks up an operation identified by its name and returns a reference to it.
 *
 * The first found operation is returned. If operation is not found, a null 
 * operation is returned.
 * 
 * @param name The name of the operation.
 * @return The wanted operation.
 */
Operation&
OperationPool::operation(const char* name) {
    return pimpl_->operation(name);
}

/**
 * Returns the operation index of operation pool.
 *
 * @return The operation index.
 */
OperationIndex&
OperationPool::index() {
    return pimpl_->index();
}

/**
 * Checks globally from all the loaded operations whether
 * the given operation shares state with any of them.
 *
 * In case another operation affects this operation or
 * vice-versa, they are assumed to share state. The
 * shares-property has to be in only one of the operation's
 * description.
 */
bool
OperationPool::sharesState(const Operation& op) {
    return pimpl_->sharesState(op);
}

/**
 * Used to force loading of OSAL data straight from LLVM backend data 
 * instead of the .opp XML files.
 */
void
OperationPool::setLLVMTargetInstrInfo(const llvm::MCInstrInfo* tid) {
    OperationPoolPimpl::setLLVMTargetInstrInfo(tid);
}
