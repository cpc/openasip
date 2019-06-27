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
 * @file ITemplateBroker.hh
 *
 * Declaration of ITemplateBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_ITEMPLATE_BROKER_HH
#define TTA_ITEMPLATE_BROKER_HH

#include <vector>

#include "ResourceBroker.hh"
#include "ImmediateUnit.hh"

namespace TTAMachine {
    class Bus;
    class Machine;
    class MachinePart;
    class ImmediateUnit;
}

namespace TTAProgram {
    class Instruction;
    class Immediate;
    class Move;
}

class MoveNode;
class ResourceMapper;
class SchedulingResource;
class SimpleResourceManager;
class BusBroker;

/**
 * ITemplate broker.
 */
class ITemplateBroker : public ResourceBroker {
public:
    ITemplateBroker(std::string, BusBroker& busBroker, unsigned int initiationInterval = 0);
    ITemplateBroker(std::string, BusBroker& busBroker, SimpleResourceManager*, unsigned int initiationInterval = 0);
    virtual ~ITemplateBroker();

    virtual bool isAnyResourceAvailable(int, const MoveNode&) const;
    virtual SchedulingResourceSet allAvailableResources(
        int,
        const MoveNode&) const;
    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res);
    virtual void unassign(MoveNode& node);

    virtual int earliestCycle(int cycle, const MoveNode& node) const;
    virtual int latestCycle(int cycle, const MoveNode& node) const;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node) const;
    virtual bool isApplicable(const MoveNode& node) const;
    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);

    virtual bool isITemplateBroker() const;
    virtual TTAProgram::Instruction* instruction(int cycle);
    virtual void loseInstructionOwnership(int cycle);
    virtual bool isTemplateAvailable(int, TTAProgram::Immediate*) const;
    void clearOldResources();
    void clear();
private:
    typedef std::vector<TTAProgram::Move*> Moves;
    typedef std::vector<TTAProgram::Immediate*> Immediates;

    SchedulingResourceSet findITemplates(int, Moves, Immediates) const;
    void assignImmediate(int, TTAProgram::Immediate&);
    void unassignImmediate(int,const TTAMachine::ImmediateUnit&);
    bool isImmediateInTemplate(int, const TTAProgram::Immediate*) const;
    
    /// Move/immediate slots.
    std::vector<TTAMachine::Bus*> slots_;
    /// cycle/instruction
    std::map<int, TTAProgram::Instruction*> instructions_;
    /// MoveNode/ original parent instruction
    std::map<const MoveNode*, TTAProgram::Instruction*, MoveNode::Comparator>
    oldParentInstruction_;
    
    /// Record cycle for MoveNodes that needed immediate writes,
    /// IU broker restores immediates before the template broker is called
    /// so we need to explicitely find the Immediate to remove it from
    /// template
    std::map<const MoveNode*, int, GraphNode::Comparator> immediateCycles_;
    std::map<const MoveNode*, TTAProgram::Immediate*, 
             GraphNode::Comparator> immediateValues_;
    
    std::map<int, bool> instructionsNotOwned_;

    // Pointer to resource manager, needed to get information about
    // immediate from IUBroker
    SimpleResourceManager* rm_;
    BusBroker& busBroker_;
};

#endif
