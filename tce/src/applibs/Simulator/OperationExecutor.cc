/**
 * @file OperationExecutor.cc
 *
 * Definition of OperationExecutor class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
