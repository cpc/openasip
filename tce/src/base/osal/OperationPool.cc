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
