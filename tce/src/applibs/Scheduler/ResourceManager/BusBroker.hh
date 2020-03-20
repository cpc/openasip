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
    BusBroker(
        std::string name,
        ResourceBroker& ipBroker,
        ResourceBroker& opBroker,
        const TTAMachine::Machine& mach,
        unsigned int initiationInterval = 0);
    virtual ~BusBroker();

    virtual bool isAnyResourceAvailable(int cycle, const MoveNode& node,
                                        const TTAMachine::Bus* bus,
                                        const TTAMachine::FunctionUnit* srcFU,
                                        const TTAMachine::FunctionUnit* dstFU,
                                        int immWriteCycle,
                                        const TTAMachine::ImmediateUnit* immu,
                                        int immRegIndex)
        const override;
    virtual SchedulingResource& availableResource(
        int cycle,
        const MoveNode& node, const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
        const TTAMachine::ImmediateUnit* immu,
        int immRegIndex) const override;
    virtual SchedulingResourceSet allAvailableResources(
        int cycle, const MoveNode& node, const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
        const TTAMachine::ImmediateUnit* immu,
        int immRegIndex) const override;
    virtual bool isAvailable(
        SchedulingResource& des, const MoveNode& node, int cycle,
        const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
	const TTAMachine::ImmediateUnit* immu,
	int immRegIndex) const override;
    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res,
                        int immWriteCycle,
                        int immRegIndex) override;
    virtual void unassign(MoveNode& node);

    virtual int earliestCycle(int cycle, const MoveNode& node,
                              const TTAMachine::Bus* bus,
                              const TTAMachine::FunctionUnit* srcFU,
                              const TTAMachine::FunctionUnit* dstFU,
                              int immWriteCycle,
                              const TTAMachine::ImmediateUnit* immu,
                              int immRegIndex) const override;
    virtual int latestCycle(int cycle, const MoveNode& node,
                            const TTAMachine::Bus* bus,
                            const TTAMachine::FunctionUnit* srcFU,
                            const TTAMachine::FunctionUnit* dstFU,
                            int immWriteCycle,
                            const TTAMachine::ImmediateUnit* immu,
                            int immRegIndex) const override;
    virtual bool isAlreadyAssigned(
        int cycle, const MoveNode& node,
        const TTAMachine::Bus* preassignedBus) const override;
    virtual bool isApplicable(
        const MoveNode& node, const TTAMachine::Bus*) const override;
    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);

    virtual bool isBusBroker() const;

    virtual bool canTransportImmediate(
        const MoveNode& node, const TTAMachine::Bus* preAssigndBus) const;
    virtual bool isInUse(int cycle, const MoveNode& node) const;
    virtual bool hasGuard(const MoveNode& node) const;
    void clear();
private:
    virtual bool canTransportImmediate(
        const MoveNode& node,
        ShortImmPSocketResource& immRes) const;
    virtual ShortImmPSocketResource& findImmResource(
        BusResource& busRes) const;
    std::list<SchedulingResource*> shortImmPSocketResources_;
    std::map<const MoveNode*, bool> busPreassigned_;
    ResourceBroker& inputPSocketBroker_;
    ResourceBroker& outputPSocketBroker_;
    bool hasLimm_;
    const TTAMachine::Machine* mach_;
};

#endif
