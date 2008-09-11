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
