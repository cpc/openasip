/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file OperationBehaviorProxy.cc 
 *
 * Definition of OperationBehaviorProxy class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepist� 2007 (mikael.lepisto-no.spam-tut.fi)
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
    OperationBehaviorLoader& loader,
    bool alwaysReloadBehavior) : 
    OperationBehavior(), target_(&targetOperation), loader_(&loader),
    initialized_(false),  alwaysReloadBehavior_(alwaysReloadBehavior) {
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
    bool retVal = target_->simulateTrigger(io, context);
    if (alwaysReloadBehavior_) uninitializeBehavior();
    return retVal;
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
    if (alwaysReloadBehavior_) uninitializeBehavior();
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
    if (alwaysReloadBehavior_) uninitializeBehavior();
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
    try {
        initializeBehavior();
    } catch (Exception&) {
        if (alwaysReloadBehavior_) uninitializeBehavior();
        return false;
    }
    // if initialization was not success
    if (&target_->behavior() == this) {
        if (alwaysReloadBehavior_) uninitializeBehavior();
        return false;
    } else {
        bool retVal = target_->canBeSimulated();
        if (alwaysReloadBehavior_) uninitializeBehavior();
        return retVal;
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
 *                                    behavior file was invalid.
 */
void
OperationBehaviorProxy::initializeBehavior() const {

    if (initialized_) {
        return;
    }
    
    try {
        OperationBehavior& ob = loader_->importBehavior(*target_);
        target_->setBehavior(ob);
        return;
    } catch (FileNotFound&) {
    } catch (SymbolNotFound& e) {
        // try loading behavior from DAG
        if (target_->dagCount() == 0) {
            throw e;
        }
    } catch (Exception& e) {
        throw e;
    }

    // no compiled behavior in .opb found

    // if there is DAG to create behavior model...
    if (target_->dagCount() == 0) {
        initialized_ = true;
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

    initialized_ = true;
}


/**
 * Uninitializes the behavior description from the operation so it will be
 * reloaded again when a behavior method is called the next time.
 */
void
OperationBehaviorProxy::uninitializeBehavior() const {
    while (!cleanUs_.empty()) {
        OperationBehavior* behavior = *cleanUs_.begin();
        delete behavior;
        cleanUs_.erase(cleanUs_.begin());
    }
    target_->setBehavior(const_cast<OperationBehaviorProxy&>(*this));
    loader_->freeBehavior();
    initialized_ = false;
}
