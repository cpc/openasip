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
 * @file IUBroker.hh
 *
 * Declaration of IUBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IU_BROKER_HH
#define TTA_IU_BROKER_HH

#include "ResourceBroker.hh"
#include "SimpleResourceManager.hh"
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

/**
 * Immediate unit broker.
 */
class IUBroker : public ResourceBroker {
public:
    IUBroker(std::string);
    IUBroker(std::string, SimpleResourceManager*);
    virtual ~IUBroker();

    bool isAnyResourceAvailable(int useCycle, const MoveNode& node) const;
    virtual SchedulingResource& availableResource(
        int cycle,
        const MoveNode& node) const
        throw (InstanceNotFound);
    virtual SchedulingResourceSet allAvailableResources(
        int,
        const MoveNode&) const;
    virtual void assign(int, MoveNode&, SchedulingResource&)
        throw (Exception);
    virtual void unassign(MoveNode& node);

    virtual int earliestCycle(int cycle, const MoveNode& node) const;
    virtual int latestCycle(int cycle, const MoveNode& node) const;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node) const;
    virtual bool isApplicable(const MoveNode& node) const;
    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);

    virtual bool isIUBroker() const;
    virtual TTAProgram::Terminal* immediateValue(const MoveNode& node) const;
    virtual int immediateWriteCycle(const MoveNode& node) const;
    

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
