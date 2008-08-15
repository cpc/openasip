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
 * @file ResourceManager.hh
 *
 * Declaration of ResourceManager class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_RESOURCE_MANAGER_HH
#define TTA_RESOURCE_MANAGER_HH

namespace TTAMachine {
    class Machine;
}

class MoveNode;
class MoveNodeSet;

/**
 * Base interface for resource managers.
 */
class ResourceManager {
public:
    ResourceManager(const TTAMachine::Machine& machine);
    virtual ~ResourceManager();

    virtual bool canAssign(int cycle, MoveNode& node) const = 0;
    virtual void assign(int cycle, MoveNode& node) = 0;
    virtual void unassign(MoveNode& node) = 0;
    virtual int earliestCycle(MoveNode& node) const = 0;
    virtual int earliestCycle(int cycle, MoveNode& node) const = 0;
    virtual int latestCycle(MoveNode& node) const = 0;
    virtual int latestCycle(int cycle, MoveNode& node) const = 0;
    virtual bool hasConnection(MoveNodeSet& nodes) = 0;
    virtual bool supportsExternalAssignments() const = 0;
    virtual int largestCycle() const = 0;
    const TTAMachine::Machine& machine() const;

private:
    /// Target machine.
    const TTAMachine::Machine* machine_;
};

#endif
