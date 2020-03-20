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
 * @file PendingAssignment.hh
 *
 * Declaration of PendingAssignment class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PENDING_ASSIGNMENT_HH
#define TTA_PENDING_ASSIGNMENT_HH

#include "SchedulingResource.hh"

class ResourceBroker;
class MoveNode;

namespace TTAMachine {
    class Bus;
    class FunctionUnit;
}

/**
 */
class PendingAssignment {
public:
    PendingAssignment(ResourceBroker& broker);
    virtual ~PendingAssignment();

    ResourceBroker& broker();
    void setRequest(int cycle, MoveNode& node,
                    const TTAMachine::Bus* bus,
                    const TTAMachine::FunctionUnit* srcFU,
                    const TTAMachine::FunctionUnit* dstFU,
                    int immWriteCycle,
                    const TTAMachine::ImmediateUnit* immu,
                    int immRegIndex);
    bool isAssignmentPossible();
    void tryNext();
    void undoAssignment();
    void forget();
    void clear();
    SchedulingResource& resource(int index) {
        return availableResources_.resource(index); }
    int lastTriedAssignment() const { return lastTriedAssignment_; }
private:
    /// Corresponding resource broker.
    ResourceBroker& broker_;
    int cycle_;
    MoveNode* node_;
    const TTAMachine::Bus* bus_;
    const TTAMachine::FunctionUnit* srcFU_;
    const TTAMachine::FunctionUnit* dstFU_;
    int immWriteCycle_;
    bool assignmentTried_;
    SchedulingResourceSet availableResources_;
    int lastTriedAssignment_;
    const TTAMachine::ImmediateUnit* immu_;
    int immRegIndex_;
};

#endif
