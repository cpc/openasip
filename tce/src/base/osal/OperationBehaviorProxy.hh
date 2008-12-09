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
 * @file OperationBehaviorProxy.hh
 *
 * Declaration of OperationBehaviorProxy class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#ifndef TTA_OPERATION_BEHAVIOR_PROXY_HH
#define TTA_OPERATION_BEHAVIOR_PROXY_HH

#include <vector>
#include <set>

#include "OperationBehavior.hh"
#include "OperationDAGBehavior.hh"

class Operation;
class SimValue;
class OperationContext;
class OperationBehaviorLoader;

/**
 * This class is used to create a behavior model of an operation.
 *
 * A first time operation calls simulateTrigger()
 * proxy creates the appropriate operation behavior model for the operation.
 * Proxy replaces itself in the operation with the newly created behavior 
 * model. That new model then executes all simulation functions.
 */
class OperationBehaviorProxy : public OperationBehavior {
public:
    OperationBehaviorProxy(
        Operation& targetOperation, 
        OperationBehaviorLoader& loader);
    
    virtual ~OperationBehaviorProxy();

    virtual bool simulateTrigger(
        SimValue** io,
        OperationContext& context) const;

    virtual bool canBeSimulated() const;

    virtual void createState(OperationContext& context) const;
    virtual void deleteState(OperationContext& context) const;

private:
    /// Copying not allowed.
    OperationBehaviorProxy(const OperationBehaviorProxy&);
    /// Assignment not allowed.
    OperationBehaviorProxy& operator=(const OperationBehaviorProxy&);

    void initializeBehavior() const;
   
    /// Operation that owns this proxy;
    Operation* target_;
    /// Used to load behavior model for operation.
    OperationBehaviorLoader* loader_;
    /// Flag indicating whether proxy is initialized or not.
    mutable bool initialized_;

    /// Clean up list for created OperationDAGBehaviors
    mutable std::set<OperationDAGBehavior*> cleanUs_;
};

#endif
