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
 * @file TransportPipeline.hh
 *
 * Declaration of TransportPipeline class.
 * 
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TRANSPORT_PIPELINE_HH
#define TTA_TRANSPORT_PIPELINE_HH

#include <string>

#include "OperationExecutor.hh"
#include "OperationContext.hh"

class FUState;
class GCUState;
class Operation;

/**
 * Models the transport pipeline of global control unit.
 */
class TransportPipeline : public OperationExecutor {
public:
    explicit TransportPipeline(GCUState& parent);
    virtual ~TransportPipeline();

    virtual int latency() const;
    virtual void startOperation(Operation& op);
    virtual void advanceClock();
    virtual OperationExecutor* copy();
    virtual void setContext(OperationContext& context);
   
private:
    /// Assignment not allowed.
    TransportPipeline& operator=(const TransportPipeline&);
    
    /// Operation to be triggered next.
    Operation* operation_;
    /// Operation context used to fetch the values for PC and RA.
    OperationContext* context_;
    /// Operation context seen by the operation.
    OperationContext tempContext_;
    /// Storage for PC value in the context.
    InstructionAddress PC_;
    /// Storage for RA value in the context.
    SimValue RA_;
    /// The owner GCUState.
    GCUState& parent_;

};

#endif
