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
 * @file BusBroker.hh
 *
 * Declaration of BusBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUS_BROKER_HH
#define TTA_BUS_BROKER_HH

#include "ResourceBroker.hh"

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
};

#endif
