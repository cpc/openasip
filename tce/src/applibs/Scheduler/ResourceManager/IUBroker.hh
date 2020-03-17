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
 * @file IUBroker.hh
 *
 * Declaration of IUBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_IU_BROKER_HH
#define TTA_IU_BROKER_HH

#include "ResourceBroker.hh"
#include "IUResource.hh"

namespace TTAMachine {
    class Machine;
    class MachinePart;
}

namespace TTAProgram {
    class Terminal;
}

class MoveNode;
class ResourceMapper;
class SchedulingResource;
class SimpleResourceManager;

/**
 * Immediate unit broker.
 */
class IUBroker : public ResourceBroker {
public:
    IUBroker(std::string);
    IUBroker(std::string, SimpleResourceManager*, unsigned int initiationInterval = 0);
    virtual ~IUBroker();

    bool isAnyResourceAvailable(int useCycle, const MoveNode& node,
                                const TTAMachine::Bus* bus,
                                const TTAMachine::FunctionUnit* srcFU,
                                const TTAMachine::FunctionUnit* dstFU,
                                int immWriteCycle,
                                const TTAMachine::ImmediateUnit* immu,
                                int immRegIndex) const override;
    virtual SchedulingResource& availableResource(
        int cycle,
        const MoveNode& node,
        const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
        const TTAMachine::ImmediateUnit* immu,
        int immRegIndex) const override;

    virtual SchedulingResourceSet allAvailableResources(
        int,
        const MoveNode&,
        const TTAMachine::Bus* bus,
        const TTAMachine::FunctionUnit* srcFU,
        const TTAMachine::FunctionUnit* dstFU,
        int immWriteCycle,
        const TTAMachine::ImmediateUnit* immu,
        int immRegIndex) const override;

    virtual void assign(int, MoveNode&, SchedulingResource&,
			int immWriteCycle,
			int immRegIndex) override;

    virtual void unassign(MoveNode& node) override;

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
        const MoveNode& node, const TTAMachine::Bus* preassignedBus)
        const override;
    virtual void buildResources(const TTAMachine::Machine& target) override;
    virtual void setupResourceLinks(const ResourceMapper& mapper) override;

    virtual bool isIUBroker() const override;
    virtual std::shared_ptr<TTAProgram::TerminalImmediate>
    immediateValue(const MoveNode& node) const;
    virtual int immediateWriteCycle(const MoveNode& node) const;
    
    void clearOldResources();
private:
    /// Target machine.
    const TTAMachine::Machine* target_;
    /// Functor for sorting result of allAvailable by register width.
    struct less_width {
        bool operator()(IUResource* x, IUResource* y) {
            return x->width() < y->width();
        }
    };
    // Pointer to resource manager, needed to get information about
    // immediate from IUBroker
    SimpleResourceManager* rm_;    
};

#endif
