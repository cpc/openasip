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
