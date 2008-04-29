/**
 * @file OperationPool.cc
 *
 * Definition of OperationPool class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @author Miakel Lepistö 2007 (mikael.lepisto@tut.fi)
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
