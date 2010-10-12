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
 * @file SimpleResourceManager.hh
 *
 * Declaration of SimpleResourceManager class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
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
    static SimpleResourceManager* createRM(const TTAMachine::Machine& mach);
    static void disposeRM(SimpleResourceManager* rm);
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
    virtual void loseInstructionOwnership(int cycle);
    virtual TTAProgram::Terminal* immediateValue(const MoveNode&);
    virtual int immediateWriteCycle(const MoveNode&) const;
    virtual bool isTemplateAvailable(int, TTAProgram::Immediate*) const;
    virtual void clearOldResources();
private:
    SimpleResourceManager(const TTAMachine::Machine& machine);
    virtual ~SimpleResourceManager();

    /// Clears all bookkeeping done by this RM. 
    /// The RM can then be reused for different BB.
    void clear();

    /// Resource manager's broker director.
    SimpleBrokerDirector* director_;
    /// Resource assignment plan.
    AssignmentPlan plan_;
    /// Resource build director.
    ResourceBuildDirector buildDirector_;

    static std::map<const TTAMachine::Machine*, std::list<SimpleResourceManager*> >rmPool_;

};

#endif
