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
 * @file ITemplateBroker.hh
 *
 * Declaration of ITemplateBroker class.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme@tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ITEMPLATE_BROKER_HH
#define TTA_ITEMPLATE_BROKER_HH

#include <vector>

#include "ResourceBroker.hh"
#include "SimpleResourceManager.hh"
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

/**
 * ITemplate broker.
 */
class ITemplateBroker : public ResourceBroker {
public:
    ITemplateBroker(std::string);
    ITemplateBroker(std::string, SimpleResourceManager*);
    virtual ~ITemplateBroker();

    virtual bool isAnyResourceAvailable(int, const MoveNode&) const;
    virtual SchedulingResourceSet allAvailableResources(
        int,
        const MoveNode&) const;
    virtual void assign(int cycle, MoveNode& node, SchedulingResource& res)
        throw (Exception);
    virtual void unassign(MoveNode& node);

    virtual int earliestCycle(int cycle, const MoveNode& node) const;
    virtual int latestCycle(int cycle, const MoveNode& node) const;
    virtual bool isAlreadyAssigned(int cycle, const MoveNode& node) const;
    virtual bool isApplicable(const MoveNode& node) const;
    virtual void buildResources(const TTAMachine::Machine& target);
    virtual void setupResourceLinks(const ResourceMapper& mapper);

    virtual bool isITemplateBroker() const;
    virtual TTAProgram::Instruction* instruction(int cycle);
    virtual void loseInstructionOwnership();
    virtual bool isTemplateAvailable(int, TTAProgram::Immediate*) const;
private:
    typedef std::vector<TTAProgram::Move*> Moves;
    typedef std::vector<TTAProgram::Immediate*> Immediates;

    SchedulingResourceSet findITemplates(int, Moves, Immediates) const;    
    void assignImmediate(int, TTAProgram::Immediate&) throw (Exception);
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
    
    bool ownsInstructions_;
    // Pointer to resource manager, needed to get information about
    // immediate from IUBroker
    SimpleResourceManager* rm_;
};

#endif
