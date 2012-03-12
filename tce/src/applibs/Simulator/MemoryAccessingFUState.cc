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
 * @file MemoryAccessingFUState.cc
 *
 * Definition of MemoryAccessingFUState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "MemoryAccessingFUState.hh"
#include "Memory.hh"
#include "OperationContext.hh"
#include "Memory.hh"

/**
 * Constructor (no explicitly given FU name).
 *
 * @param memory Memory instance.
 * @param MAUSize Width of the minimum addressable unit.
 * @param lock Global lock signal.
 * @todo Do not hard code the MAU or NW sizes!
 */
MemoryAccessingFUState::MemoryAccessingFUState(
    Memory& memory) : 
    FUState(), memory_(memory), operationContext_(&OperationContext::DEFAULT_FU_NAME) {
       
    operationContext_.setMemory(&memory_);
}

/**
 * Constructor (with explicitly given FU name).
 *
 * @param memory Memory instance.
 * @param MAUSize Width of the minimum addressable unit.
 * @param lock Global lock signal.
 * @todo Do not hard code the MAU or NW sizes!
 */
MemoryAccessingFUState::MemoryAccessingFUState(
	std::string *name,
    Memory& memory) : 
    FUState(), memory_(memory), operationContext_(name) {
       
    operationContext_.setMemory(&memory_);
}

/**
 * Destructor.
 */
MemoryAccessingFUState::~MemoryAccessingFUState() {
}

/**
 * Returns the operation context.
 *
 * This is basically a "template method" to allow differently initialized
 * OperationContext-classes in FUState subclasses.
 *
 * @return The operation context for the FU.
 */
OperationContext&
MemoryAccessingFUState::context() {
    return operationContext_;
}


