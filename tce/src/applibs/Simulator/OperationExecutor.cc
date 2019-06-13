/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file OperationExecutor.cc
 *
 * Definition of OperationExecutor class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2010 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "OperationExecutor.hh"
#include "Operation.hh"
#include "FUState.hh"
#include "PortState.hh"

using std::string;

/**
 * Constructor.
 *
 * @param parent Parent FUState.
 */
OperationExecutor::OperationExecutor(FUState& parent) : 
     hasPendingOperations_(false), parent_(&parent) {
}

/**
 * Destructor.
 */
OperationExecutor::~OperationExecutor() {
}

/**
 * Adds binding of operand to a port.
 *
 * @param io Operand index.
 * @param port Port to be bind.
 * @exception IllegalRegistration If parent of the port is not the same as the
 *                                parent of the executor.
 */
void
OperationExecutor::addBinding(int io, PortState& port) {
    if (io > static_cast<int>(bindings_.size()) - 1) {
        bindings_.resize(io, NULL);
    }
    
    if (&port.parent() != parent_) {
        string msg = "Parent of the port and executors differs";
        throw IllegalRegistration(__FILE__, __LINE__, __func__, msg);
    }
 
    if (bindings_[io - 1] != NULL) {
        string msg = "Operand with given index has been already bound"
            " to a port";
        throw IllegalRegistration(__FILE__, __LINE__, __func__, msg);
    }

    bindings_[io - 1] = &port;
}
