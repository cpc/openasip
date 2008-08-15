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
 * @file SimpleResourceManager.hh
 *
 * Declaration of SimpleResourceManager class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_RESOURCE_MANAGER_HH
#define TTA_SIMPLE_RESOURCE_MANAGER_HH

#include "ResourceManager.hh"
#include "SimpleBrokerDirector.hh"
#include "AssignmentPlan.hh"
#include "ResourceBuildDirector.hh"

/**
 * A simple resource manager.
 */
class SimpleResourceManager : public ResourceManager {
public:
    SimpleResourceManager(const TTAMachine::Machine& machine);
    virtual ~SimpleResourceManager();

    virtual bool canAssign(int cycle, MoveNode& node) const;
    virtual bool canTransportImmediate(const MoveNode& node) const;
    virtual void assign(int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(MoveNode& node)
        throw (Exception);
    virtual int earliestCycle(MoveNode& node) const
        throw (Exception);
    virtual int earliestCycle(int cycle, MoveNode& node) const
        throw (Exception);
    virtual int latestCycle(MoveNode& node) const;
    virtual int latestCycle(int cycle, MoveNode& node) const;

    virtual bool hasConnection(MoveNodeSet& nodes);
    virtual bool hasGuard(const MoveNode& node) const;
    virtual TTAProgram::Instruction* instruction(int cycle) const;
    virtual bool supportsExternalAssignments() const;
    virtual int largestCycle() const;
    virtual void loseInstructionOwnership();
    virtual TTAProgram::Terminal* immediateValue(const MoveNode&);
    virtual int immediateWriteCycle(const MoveNode&) const;
    virtual bool isTemplateAvailable(int, TTAProgram::Immediate*) const;
private:
    /// Resource manager's broker director.
    SimpleBrokerDirector* director_;
    /// Resource assignment plan.
    AssignmentPlan plan_;
    /// Resource build director.
    ResourceBuildDirector buildDirector_;
};

#endif
