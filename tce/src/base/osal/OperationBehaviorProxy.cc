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
 * @file OperationBehaviorProxy.cc 
 *
 * Definition of OperationBehaviorProxy class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#include <string>

#include "OperationBehaviorProxy.hh"
#include "Application.hh"
#include "Operation.hh"
#include "SimValue.hh"
#include "OperationContext.hh"
#include "OperationBehaviorLoader.hh"

using std::string;

/**
 * Constructor.
 *
 * Registers the operation for which this proxy represents the behavior model.
 * Registers the operation behavior loader that is used to find the appropriate
 * behavior model of the operation.
 *
 * @param targetOperation Target operation for proxy.
 * @param loader Operation behavior loader for the proxy.
 */
OperationBehaviorProxy::OperationBehaviorProxy(
    Operation& targetOperation,
    OperationBehaviorLoader& loader) : 
    OperationBehavior(), target_(&targetOperation), loader_(&loader),
    initialized_(false) {
}

/**
 * Destructor.
 *
 * Operation behavior loader frees all loaded operation behavior models.
 */
OperationBehaviorProxy::~OperationBehaviorProxy() {
    while (!cleanUs_.empty()) {
        delete *(cleanUs_.begin());
        cleanUs_.erase(cleanUs_.begin());
    }
}

/**
 * Imports operation behavior model for the operation that owns this proxy
 * and then delegates the work to that imported model.
 *
 * After initializing the operation with imported operation behavior model
 * proxy has done its job.
 *
 * @param io The input and output operands.
 * @param context The operation context.
 * @return True if all values could be computed, false otherwise.
 * @exception IllegalOperationBehavior If trigger command lacks return 
 *                                     statement.
 */
bool
OperationBehaviorProxy::simulateTrigger(
    SimValue** io,
    OperationContext& context) const {
    
    initializeBehavior();
    return target_->simulateTrigger(io, context);
}

/**
 * Imports operation behavior model for the operation that owns this proxy
 * and then delegates the work to that imported model.
 *
 * After initializing the operation with imported operation behavior model
 * proxy has done its job.
 *
 * @param context The operation context to add the state in.
 */
void
OperationBehaviorProxy::createState(OperationContext& context) const {
    initializeBehavior();
    target_->createState(context);
}

/**
 * Imports operation behavior model for the operation that owns this proxy
 * and then delegates the work to that imported model.
 *
 * After initializing the operation with imported operation behavior model
 * proxy has done its job.
 *
 * @param context The operation context to delete the state from.
 */
void
OperationBehaviorProxy::deleteState(OperationContext& context) const {
    initializeBehavior();
    target_->deleteState(context);
}


/**
 * Imports operation behavior model for the operation that owns this proxy
 * and then delegates the work to that imported model.
 *
 * After initializing the operation with imported operation behavior model
 * proxy has done its job.
 *
 * @return True if this operation has behavior, or dag which is
 */
bool
OperationBehaviorProxy::canBeSimulated() const {    
    initializeBehavior();
    
    // if initialization was not success
    if (&target_->behavior() == this) {
        return false;
    } else {
        return target_->canBeSimulated();
    }
}

/**
 * Initializes the operation that owns this proxy with an operation behavior 
 * model.
 *
 * Proxy replaces itself with the imported operation behavior model. After that
 * proxy methods are never called.
 *
 * This method is executed only once. After that, this function does nothing.
 * This function may abort the program, if error condition occurs while
 * operation behavior model is imported.
 *
 * @exception DynamicLibraryException Leaked from importBehavior in case 
 *                                    behavior not found.
 */
void
OperationBehaviorProxy::initializeBehavior() const {

    assert(!initialized_);
    
    // maybe there is easier check... tried that one.. there was not.
    try {
        OperationBehavior& ob = loader_->importBehavior(*target_);
        target_->setBehavior(ob);

    } catch (Exception &e) {
        
        // if there is DAG to create behavior model...
        if (target_->dagCount() == 0) {
            return;
        }

        // there was not behavior plugin so let's use dag for simulation
        OperationDAGBehavior* behavior = 
            new OperationDAGBehavior(
                target_->dag(0), 
                target_->numberOfInputs() + 
                target_->numberOfOutputs());        
        target_->setBehavior(*behavior);
    
        // add for cleanup
        cleanUs_.insert(behavior);
    }
    
    initialized_ = true;
}

