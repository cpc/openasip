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
 * @file BusBroker.hh
 *
 * Declaration of BusBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUS_BROKER_HH
#define TTA_BUS_BROKER_HH

#include "ResourceBroker.hh"

#include <list>

namespace TTAMachine {
    class Machine;
    class MachinePart;
}

namespace TTAProgram {
    class TerminalImmediate;
}

class MoveNode;
class ResourceMapper;
class SchedulingResource;
class BusResource;
class ShortImmPSocketResource;

/**
 * Bus broker.
 */
class BusBroker : public ResourceBroker {
public:
    BusBroker(std::string name);
    virtual ~BusBroker();

    virtual bool isAnyResourceAvailable(int cycle, const MoveNode& node)
        const;
    virtual SchedulingResource& availableResource(
        int cycle,
        const MoveNode& node) const throw (InstanceNotFound);
    virtual SchedulingResourceSet allAvailableResources(
        int cycle, const MoveNode& node) const;
    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res)
        throw (Exception);
    virtual void unassign(MoveNode& node);

    virtual int earliestCycle(int cycle, const MoveNode& node) const;
    virtual int latestCycle(int cycle, const MoveNode& node) const;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node) const;
    virtual bool isApplicable(const MoveNode& node) const;
    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);

    virtual bool isBusBroker() const;

    virtual bool canTransportImmediate(const MoveNode& node) const;
    virtual bool isInUse(int cycle, const MoveNode& node) const;
    virtual bool hasGuard(const MoveNode& node) const;

private:
    virtual bool canTransportImmediate(
        const MoveNode& node,
        ShortImmPSocketResource& immRes) const;
    virtual ShortImmPSocketResource& findImmResource(
        BusResource& busRes) const;
    std::list<SchedulingResource*> shortImmPSocketResources_;
    bool hasLimm_;
};

#endif
