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
 * @file InputPSocketBroker.hh
 *
 * Declaration of InputPSocketBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_INPUT_PSOCKET_BROKER_HH
#define TTA_INPUT_PSOCKET_BROKER_HH

#include "ResourceBroker.hh"

namespace TTAMachine {
    class Machine;
    class MachinePart;
}

class MoveNode;
class ResourceMapper;
class SchedulingResource;

/**
 * Input p-socket broker takes care of assigning input socket and input
 * port (of FU or RF) at once.
 */
class InputPSocketBroker : public ResourceBroker {
public:
    InputPSocketBroker(
        std::string name,
        ResourceBroker& fub,
        unsigned int initiationInterval = 0);
    virtual ~InputPSocketBroker();

    virtual SchedulingResourceSet allAvailableResources(
        int cycle, const MoveNode& node) const;
    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res);
    virtual void unassign(MoveNode& node);

    virtual int earliestCycle(int cycle, const MoveNode& node) const;
    virtual int latestCycle(int cycle, const MoveNode& node) const;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node) const;
    virtual bool isApplicable(const MoveNode& node) const;
    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);
    
    void setBusBroker(ResourceBroker& sb);
private:
    ResourceBroker& inputFUBroker_;
    ResourceBroker* busBroker_;
};

#endif
