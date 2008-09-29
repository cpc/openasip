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
 * @file OperationExecutor.cc
 *
 * Definition of OperationExecutor class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
OperationExecutor::addBinding(int io, PortState& port) 
    throw (IllegalRegistration) {

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
