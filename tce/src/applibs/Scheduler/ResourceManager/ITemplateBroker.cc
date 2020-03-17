/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file ITemplateBroker.cc
 *
 * Implementation of ITemplateBroker class.
 *
 * @author Ari Metsähalme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wunused-local-typedef")
#include <boost/lexical_cast.hpp>
POP_CLANG_DIAGS

#include "ITemplateBroker.hh"
#include "ITemplateResource.hh"
#include "Machine.hh"
#include "MachineInfo.hh"
#include "InstructionTemplate.hh"
#include "NullInstructionTemplate.hh"
#include "ResourceMapper.hh"
#include "Move.hh"
#include "Instruction.hh"
#include "MapTools.hh"
#include "NullInstruction.hh"
#include "POMDisassembler.hh"
#include "MoveNode.hh"
#include "TerminalInstructionAddress.hh"
#include "MathTools.hh"
#include "TerminalRegister.hh"
#include "Immediate.hh"
#include "SimpleResourceManager.hh"
#include "BusBroker.hh"
#include "ControlUnit.hh"

using std::string;
using namespace TTAMachine;
using namespace TTAProgram;

/**
 * Constructor.
 */
ITemplateBroker::ITemplateBroker(
    std::string name, BusBroker& busBroker, unsigned int initiationInterval) :
    ResourceBroker(name, initiationInterval),
    rm_(NULL), busBroker_(busBroker) {
}

/**
 * Constructor.
 */
ITemplateBroker::ITemplateBroker(
    std::string name,
    BusBroker& busBroker,
    SimpleResourceManager* rm,
    unsigned int initiationInterval) :
    ResourceBroker(name, initiationInterval),
    rm_(rm), busBroker_(busBroker) {
}

/**
 * Destructor.
 */
ITemplateBroker::~ITemplateBroker(){
    for (std::map<int, TTAProgram::Instruction*>::iterator i =
             instructions_.begin(); i != instructions_.end(); i++) {
        std::map<int, bool>::iterator j = instructionsNotOwned_.find(i->first);
        // not found = >owns, deletes. 
        if (j == instructionsNotOwned_.end() || j->second == false) {
            delete i->second;
        }
    }

}

/**
 * Return true if there's an instruction template available to be used
 * on the given cycle for the given move.
 *
 * If the given node contains a long immediate register read, the
 * instruction template must be able to encode required bits for it.
 *
 * @param cycle Cycle where instruction template is used.
 * @param node Node which contains long immediate register read.
 * @return True if there's an instruction template available to be used
 * on the given cycle, and the template is able to encode bits for long
 * immediate that is read in given node.
 */
bool
ITemplateBroker::isAnyResourceAvailable(
    int cycle,
    const MoveNode& node,
    const TTAMachine::Bus* bus,
    const TTAMachine::FunctionUnit* srcFU,
    const TTAMachine::FunctionUnit* dstFU,
    int immWriteCycle,
    const TTAMachine::ImmediateUnit* immu,
    int immRegIndex) const {
    cycle = instructionIndex(cycle);
    int resultCount = allAvailableResources(
	cycle, node, bus, srcFU, dstFU, immWriteCycle, immu, immRegIndex).
	count();
    return resultCount > 0;
}


/**
 * Return all resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return All resources managed by this broker that can be assigned to
 * the given node in the given cycle.
 * @note This method is called twice, first it check available resources
 * for immediate write, second call checks resources for MoveNode
 */
SchedulingResourceSet
ITemplateBroker::allAvailableResources(
    int cycle,
    const MoveNode& node,
    const TTAMachine::Bus*,
    const TTAMachine::FunctionUnit*,
    const TTAMachine::FunctionUnit*, int,
    const TTAMachine::ImmediateUnit*,
    int) const {
    cycle = instructionIndex(cycle);
    Moves moves;
    Immediates immediates;
    MoveNode& testedNode = const_cast<MoveNode&>(node);
    if (node.isMove()) {
        moves.push_back(testedNode.movePtr());
    }
    return findITemplates(cycle, moves, immediates);
}


/**
 * Assigns instruction template resource to the parent instruction of
 * the given node.
 *
 * @param cycle Cycle of node.
 * @param node Node.
 * @param res Instruction template resource to assign.
 * @exception WrongSubclass If this broker does not recognise the given
 * type of resource.
 * @exception InvalidData If he given resource cannot be assigned to
 * given node or no corresponding machine part is found.
 * @note Ownership of a Move in MoveNode will be passed to Instruction
 * inside the broker.
 */
void
ITemplateBroker::assign(
    int cycle, MoveNode& node, SchedulingResource& res,int, int) {

    cycle = instructionIndex(cycle);

    ITemplateResource& templateRes =
        static_cast<ITemplateResource&>(res);
    const InstructionTemplate& iTemplate =
        static_cast<const InstructionTemplate&>(machinePartOf(res));
    TTAProgram::Instruction* ins = NULL;
    if (MapTools::containsKey(instructions_, cycle)) {
        ins = MapTools::valueForKey<TTAProgram::Instruction*>(
            instructions_, cycle);

        // In case template was already assigned and it changes
        if (ins->instructionTemplate().name() != iTemplate.name()) {
            SchedulingResource& oldRes =
                *resourceOf(ins->instructionTemplate());
            ITemplateResource& oldTemplateRes =
                dynamic_cast<ITemplateResource&>(oldRes);
            oldTemplateRes.unassign(cycle);

            // In case the amount of transportable immediate bits decreases
            // due to ITemplate change, update each immediate bit width to
            // the new width. If this is not done, negative immediates ends up
            // to take more bits than they really need in TPEF and later
            // loading the TPEF fails in TPEFProgramFactory.
            for (int i = 0; i < ins->immediateCount(); i++) {
                // Can not change bit width of these.
                if (ins->immediate(i).value().isInstructionAddress() ||
                    ins->immediate(i).value().isBasicBlockReference() ||
                    ins->immediate(i).value().isCodeSymbolReference()) {
                    continue;
                }
                int currentWidth = ins->immediate(i).value().value().width();
                const ImmediateUnit& destIU =
                    ins->immediate(i).destination().immediateUnit();
                int newSupportedWidth = iTemplate.supportedWidth(destIU);
                if (currentWidth > newSupportedWidth) {
                    SimValue sim(
                        (destIU.signExtends() ?
                            ins->immediate(i).value().value().intValue() :
                            ins->immediate(i).value().value().unsignedValue()),
                        newSupportedWidth);
                    TerminalImmediate* ti = new TerminalImmediate(sim);
                    ins->immediate(i).setValue(ti);
                }
            }

            ins->setInstructionTemplate(iTemplate);
            templateRes.assign(cycle);
        }
    } else {
        ins = new TTAProgram::Instruction(iTemplate);
        instructions_[cycle] = ins;
        templateRes.assign(cycle);
    }

    Instruction* oldIn = NULL;
    if (node.move().isInInstruction()) {
        oldIn = &node.move().parent();
        oldIn->removeMove(node.move());
    }
    ins->addMove(node.movePtr());
    oldParentInstruction_.insert(
        std::pair<const MoveNode*, TTAProgram::Instruction*>
        (&node, oldIn));

    // TODO: refactor this away.
    if (node.isSourceImmediateRegister()) {
        // Gets data from Immediate Unit broker, indirectly

        auto immValue = rm_->immediateValue(node);
        if (immValue) {
            TerminalImmediate* tempImm =
                dynamic_cast<TerminalImmediate*>(
                    immValue->copy());
            int defCycle = instructionIndex(rm_->immediateWriteCycle(node));
            if (defCycle >= 0) {
                TerminalRegister* tmpReg =
                    dynamic_cast<TerminalRegister*>(
                        node.move().source().copy());
                auto imm = std::make_shared<Immediate>(tempImm, tmpReg);
                if (!isImmediateInTemplate(defCycle,imm)) {
                    assignImmediate(defCycle, imm);
                    immediateCycles_.insert(
                        std::pair<const MoveNode*, int>(&node,defCycle));
                    immediateValues_.insert(
                        std::pair<const MoveNode*,
                        std::shared_ptr<TTAProgram::Immediate> >(
                            &node,imm));
                } else {
                    abortWithError("Failed to assign immediate write");
                }
            }
        }
    }
}

/**
 * Assigns instruction template resource to the given instruction located
 * in given cycle.
 *
 * @param cycle Cycle where to store immediate write.
 * @param immediate Immediate to be written in cycle
 * @exception WrongSubclass If this broker does not recognise the given
 * type of resource.
 * @exception InvalidData If he given resource cannot be assigned to
 * given node or no corresponding machine part is found.
 */
void
ITemplateBroker::assignImmediate(
    int cycle, std::shared_ptr<TTAProgram::Immediate> immediate) {
    cycle = instructionIndex(cycle);

    try {
        // Find a template for definition cycle
        Moves moves;
        Immediates immediates;
        immediates.push_back(immediate);

        ITemplateResource& templateRes = static_cast<ITemplateResource&>(
            findITemplates(cycle, moves, immediates).resource(0));
         const InstructionTemplate& iTemplate =
            static_cast<const InstructionTemplate&>(
                machinePartOf(templateRes));

        // Find instruction, if there is none, create new.
        TTAProgram::Instruction* ins = NULL;
        if (MapTools::containsKey(instructions_, cycle)) {
            ins = MapTools::valueForKey<TTAProgram::Instruction*>(
                    instructions_, cycle);
            // Remove old template assignment
            SchedulingResource& oldRes =
                *resourceOf(ins->instructionTemplate());
            ITemplateResource& oldTemplateRes =
                static_cast<ITemplateResource&>(oldRes);
            oldTemplateRes.unassign(cycle);
        } else {
            ins = new TTAProgram::Instruction(iTemplate);
            instructions_[cycle] = ins;
        }

        // Define new immediate, depending on supported bit width of
        // template for given immediate unit
        const ImmediateUnit&  iu = immediate->destination().immediateUnit();
        int neededBitWidth = iTemplate.supportedWidth(iu);
        if (neededBitWidth <= INT_WORD_SIZE &&
            (!immediate->value().isInstructionAddress() &&
             !immediate->value().isBasicBlockReference() &&
             !immediate->value().isCodeSymbolReference())) {
            // If it is not floating point or instruction address
            // we recreate SimValue with proper bit width based on destination
            if (iu.extensionMode() == Machine::ZERO) {
                SimValue sim(
                    immediate->value().value().unsignedValue(), neededBitWidth);
                TerminalImmediate* ti = new TerminalImmediate(sim);
                immediate->setValue(ti);
            }
            if (iu.extensionMode() == Machine::SIGN) {
                SimValue sim(
                    immediate->value().value().intValue(), neededBitWidth);
                TerminalImmediate* ti = new TerminalImmediate(sim);
                immediate->setValue(ti);
            }
        }
        templateRes.assign(cycle);
        ins->setInstructionTemplate(iTemplate);        
        ins->addImmediate(immediate);

    } catch (const std::bad_cast& e) {
        string msg = "Resource is not of an instruction template resource.";
        throw WrongSubclass(__FILE__, __LINE__, __func__, msg);
    } catch (const KeyNotFound& e) {
        string msg = "Broker does not contain given resource.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Free the instruction template resource used in the parent instruction
 * of the given node.
 *
 * @param node Node.
 * @note Returns ownership of Move in MoveNode to the original parent
 * instruction
 */
void
ITemplateBroker::unassign(MoveNode& node) {
    // Template not to be unassgined, it is just that there will be space
    // left in this template for move

    TTAProgram::Instruction& ins = node.move().parent();
    if (MapTools::containsKey(oldParentInstruction_, &node)) {
        ins.removeMove(node.move());
        TTAProgram::Instruction* oldParent =
            MapTools::valueForKey<TTAProgram::Instruction*>(
                oldParentInstruction_, &node);
        if (oldParent != NULL) {
            oldParent->addMove(node.movePtr());
        }
        oldParentInstruction_.erase(&node);
    }
    if (node.isSourceConstant() &&
        MapTools::containsKey(immediateCycles_, &node)) {
        int defCycle = MapTools::valueForKey<int>(immediateCycles_, &node);
        auto imm =
            MapTools::valueForKey<std::shared_ptr<TTAProgram::Immediate> >(
                immediateValues_, &node);

        if (isImmediateInTemplate(defCycle, imm)) {
            unassignImmediate(defCycle, imm->destination().immediateUnit());
        } else {
            abortWithError("Failed to remove Immediate from instruction");
        }
        immediateCycles_.erase(&node);
        immediateValues_.erase(&node);
    }
}

/**
 * Free the Instruction template used at this cycle. Sets new instruction
 * template that does not use freed immediate slot.
 *
 * If this broker is not applicable to the given node, or the node is
 * not assigned a resource of the managed type, this method does
 * nothing.
 *
 * @param cycle Cycle from where to unassign immediate
 * @param immediate Immediate to remove from cycle
 */
void
ITemplateBroker::unassignImmediate(
    int cycle,
    const ImmediateUnit& immediateUnit) {
    cycle = instructionIndex(cycle);

    if (!MapTools::containsKey(instructions_, cycle)) {
        return;
    }
    Instruction* ins =
        MapTools::valueForKey<Instruction*>(instructions_, cycle);
    const InstructionTemplate& iTemplate = ins->instructionTemplate();
    SchedulingResource& res = *resourceOf(iTemplate);
    ITemplateResource& templateRes = dynamic_cast<ITemplateResource&>(res);

    templateRes.unassign(cycle);
    for (int i = 0; i < ins->immediateCount(); i++) {
        if (ins->immediate(i).destination().immediateUnit().name() ==
            immediateUnit.name()) {
            ins->removeImmediate(ins->immediate(i));
            break;
        }
    }
    // set new template that does not use immediate slot just unassigned
    // moves and immediate will be filled from instruction in given cycle
    // inside findITTemplates method
    Moves moves;
    Immediates immediates;
    ITemplateResource& newTemplateRes = dynamic_cast<ITemplateResource&>(
        findITemplates(cycle, moves, immediates).resource(0));
     const InstructionTemplate& newTemplate =
        dynamic_cast<const InstructionTemplate&>(
            machinePartOf(newTemplateRes));
    ins->setInstructionTemplate(newTemplate);

    newTemplateRes.assign(cycle);
}

/**
 * Return the earliest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return The earliest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 */
int
ITemplateBroker::earliestCycle(int, const MoveNode&,
                               const TTAMachine::Bus*,
                               const TTAMachine::FunctionUnit*,
                               const TTAMachine::FunctionUnit*, int,
                               const TTAMachine::ImmediateUnit*, int) const {
    abortWithError("Not implemented.");
    return -1;
}

/**
 * Return the latest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return The latest cycle, starting from given cycle, where a
 * resource of the type managed by this broker can be assigned to the
 * given node.
 */
int
ITemplateBroker::latestCycle(int, const MoveNode&,
                             const TTAMachine::Bus*,
                             const TTAMachine::FunctionUnit*,
                             const TTAMachine::FunctionUnit*, int,
                             const TTAMachine::ImmediateUnit*, int) const {
    abortWithError("Not implemented.");
    return -1;
}

/**
 * Return true if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 *
 * @param cycle Cycle.
 * @param node Node.
 * @return True if the given node is already assigned a resource of the
 * type managed by this broker, and the assignment appears valid (that
 * is, the broker has marked that resource as in use in the given
 * cycle).
 */
bool
ITemplateBroker::isAlreadyAssigned(
    int cycle, const MoveNode& node, const TTAMachine::Bus*) const {
    Move& move = const_cast<MoveNode&>(node).move();

    cycle = instructionIndex(cycle);
    if (!move.isInInstruction()) {
        return false;
    }
    const InstructionTemplate& iTemplate =
        move.parent().instructionTemplate();

    // Cannot be already assigned if template is null.
    if (&iTemplate != &NullInstructionTemplate::instance()) {
        // When node is assigned, it's old parent is stored, thus if the
        // old parent is stored, node was assigned
        if (MapTools::containsKey(oldParentInstruction_, &node)) {
            SchedulingResource* res = resourceOf(iTemplate);
            if ( res != NULL && res->isInUse(cycle)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Return true if the given node needs a resource of the type managed
 * by this broker, false otherwise.
 *
 * @param node Node.
 * @return True if the given node needs a resource of the type managed
 * by this broker, false otherwise.
 * @todo reconsider, should be applicable for all the MoveNodes
 */
bool
ITemplateBroker::isApplicable(const MoveNode&, const TTAMachine::Bus*) const {
    return true;
}

/**
 * Build all resource objects of the controlled type required to model
 * scheduling resources of the given target processor.
 *
 * This method cannot set up the resource links (dependent and related
 * resources) of the constructed resource objects.
 *
 * @param target Target machine.
 */
void
ITemplateBroker::buildResources(const TTAMachine::Machine& target) {

    Machine::InstructionTemplateNavigator templateNavi =
        target.instructionTemplateNavigator();

    for (int i = 0; i < templateNavi.count(); i++) {
        InstructionTemplate* itemplate = templateNavi.item(i);
        ITemplateResource* itemplateResource =
            new ITemplateResource(itemplate->name(), initiationInterval_);
        ResourceBroker::addResource(*itemplate, itemplateResource);
    }

    Machine::BusNavigator busNavi = target.busNavigator();

    for (int i = 0; i < busNavi.count(); i++) {
        slots_.push_back(busNavi.item(i));
    }
}

/**
 * Complete resource initialisation by creating the references to
 * other resources due to a dependency or a relation.
 *
 * Use the given resource mapper to lookup dependent and related resources
 * using machine parts as keys.
 *
 * @param mapper Resource mapper.
 */
void
ITemplateBroker::setupResourceLinks(const ResourceMapper& mapper) {

    setResourceMapper(mapper);

    for (ResourceMap::iterator resIter = resMap_.begin();
         resIter != resMap_.end(); resIter++) {

        const InstructionTemplate* itemplate =
            dynamic_cast<const InstructionTemplate*>((*resIter).first);
        if (itemplate == NULL){
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "Broker does not have necessary Template registered!");
        }

        SchedulingResource* templateResource = (*resIter).second;

        for (unsigned int i = 0; i < slots_.size(); i++) {
            if (itemplate->usesSlot(slots_[i]->name())) {
                try {
                    SchedulingResource& depSlot =
                        mapper.resourceOf(*slots_[i]);
                    templateResource->addToDependentGroup(0, depSlot);
                } catch (const KeyNotFound& e) {
                    std::string msg = "ITemplateBroker: finding ";
                    msg += " resource for Slot ";
                    msg += " failed with error: ";
                    msg += e.errorMessageStack();
                    throw KeyNotFound(
                        __FILE__, __LINE__, __func__, msg);
                }

                ImmediateUnit* iu =
                    itemplate->destinationOfSlot(slots_[i]->name());
                try {
                    SchedulingResource& depIU = mapper.resourceOf(*iu);
                    templateResource->addToDependentGroup(1, depIU);
                } catch (const KeyNotFound& e) {
                    std::string msg = "ITemplateBroker: finding ";
                    msg += " resource for IU ";
                    msg += " failed with error: ";
                    msg += e.errorMessageStack();
                    throw KeyNotFound(
                        __FILE__, __LINE__, __func__, msg);
                }
            }
        }
    }
}

/**
 * Return true always.
 *
 * @return True always.
 */
bool
ITemplateBroker::isITemplateBroker() const {
    return true;
}

/**
 * Returns an instruction at a given cycle.
 *
 * @param cycle Cycle for which to return instruction
 * @return an instruction object for given cycle,
 * empty instruction if there is no record.
 */
TTAProgram::Instruction*
ITemplateBroker::instruction(int cycle) {
    cycle = instructionIndex(cycle);
    if (!MapTools::containsKey(instructions_, cycle)) {
        Moves moves;
        Immediates immediates;
        SchedulingResourceSet defaultTemplates = findITemplates(
            cycle, moves, immediates);
        if (defaultTemplates.count() == 0) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "No Instruction Template available!");
        }
        SchedulingResource& iTemp = defaultTemplates.resource(0);
        const MachinePart& mp = machinePartOf(iTemp);
        const InstructionTemplate& iTemplate =
                dynamic_cast<const InstructionTemplate&>(mp);
        ITemplateResource& templateRes =
                dynamic_cast<ITemplateResource&>(iTemp);
        templateRes.assign(cycle);
        TTAProgram::Instruction* ins = NULL;
        ins = new TTAProgram::Instruction(iTemplate);
        instructions_[cycle] = ins;
        ins->setInstructionTemplate(iTemplate);
        return ins;
    }
    Instruction* ins =
        MapTools::valueForKey<TTAProgram::Instruction*>(
            instructions_, cycle);
    return ins;
}

/**
 * Helper function, finds all templates that are applicable
 * for current status of instruction and new node that needs
 * to be added
 *
 * @param moves A set of moves which has to fit into template
 * @param immediates A set of immediates which has to fit into template
 * @return A set of instruction templates that supports current instruction
 * plus new instruction
 * @note The test for Moves is done by checking if a bus Move uses
 * is not allocated by template for immediate move. Immediate slots
 * are not relevant here.
 */
SchedulingResourceSet
ITemplateBroker::findITemplates(
    int cycle,
    Moves& moves,
    Immediates& immediates) const {

    cycle = instructionIndex(cycle);
    SchedulingResourceSet result;
    Instruction* ins = NULL;
    // Read in content of instruction if there is something already
    // stored in given cycle
    if (MapTools::containsKey(instructions_, cycle)) {
        ins = MapTools::valueForKey<TTAProgram::Instruction*>(
            instructions_, cycle);
        for (int i = 0; i < ins->moveCount(); i++) {
            moves.push_back(ins->movePtr(i));
        }
        for (int i = 0; i < ins->immediateCount(); i++) {
            immediates.push_back(ins->immediatePtr(i));
        }
    }
    for (ResourceMap::const_iterator resIter = resMap_.begin();
        resIter != resMap_.end(); resIter++) {
        bool addResult = true;
        // bus resources (slots) are in dependent group 0
        // immediate resources are in group 1
        // trying to find template that has exactly same number or
        // higher number of immediate writes as instruction
        if ((*resIter).second->dependentResourceGroupCount() > 1 &&
            immediates.size() > static_cast<unsigned int>(
                (*resIter).second->dependentResourceCount(1))) {
            continue;
        }
        // For each of already assigned immediate writes, template
        // has to have destination unit
        std::vector<const TTAMachine::ImmediateUnit*> unitWriten;
        for (unsigned int i = 0; i < immediates.size(); i++) {
            const ImmediateUnit& unit =
                immediates[i]->destination().immediateUnit();
            if (ContainerTools::containsValue(unitWriten, &unit)) {
                addResult = false;
                break;
            }
            const InstructionTemplate& iTemplate =
                dynamic_cast<const InstructionTemplate&>(*(*resIter).first);
            // immediate and immediate units has correct width, this checks
            // if template can transport such a number of bits into the unit
            // it is possible to have template which transport less bits
            if (!iTemplate.isOneOfDestinations(unit)) {
                addResult = false;
                break;
            }
            // FIXME: hack to check if terminal is floating point value
            if (immediates[i]->value().value().width() > INT_WORD_SIZE &&
                (immediates[i]->value().value().width() >
                iTemplate.supportedWidth(unit))) {
                addResult = false;
                break;
            }
            // FIXME: instruction addresses hold reference to instruction
            // reference which computes the SimValue depending on target
            // instruction's address. So it is not possible to modify the
            // bit width of such SimValue
            if (immediates[i]->value().isInstructionAddress() &&
                !immediates[i]->value().isBasicBlockReference() &&
                !immediates[i]->value().isCodeSymbolReference() &&
                (immediates[i]->value().value().width() >
                 iTemplate.supportedWidth(unit))) {

                addResult = false;
                break;
            }
            // Ignore instruction addresses, based on destination mode
            // tests required bits of correct representation of SimValue
            if (!immediates[i]->value().isInstructionAddress() &&
                !immediates[i]->value().isBasicBlockReference() &&
                !immediates[i]->value().isCodeSymbolReference() &&
                unit.extensionMode() == Machine::ZERO &&
                MathTools::requiredBits(
                    immediates[i]->value().value().unsignedValue()) >
                        iTemplate.supportedWidth(unit)) {
                addResult = false;
                break;
            } 
            if (!immediates[i]->value().isInstructionAddress() &&
                !immediates[i]->value().isBasicBlockReference() &&
                !immediates[i]->value().isCodeSymbolReference() &&
                unit.extensionMode() == Machine::SIGN &&
                MathTools::requiredBitsSigned(
                    immediates[i]->value().value().intValue()) >
                        iTemplate.supportedWidth(unit)) {
                addResult = false;
                break;
            }
            
            if (immediates[i]->value().isCodeSymbolReference() &&
                immediates[i]->value().toString() == "_end") {
                
                AddressSpace* dataAS;
                try {
                    dataAS =
                        MachineInfo::defaultDataAddressSpace(rm_->machine());
                } catch (Exception&) {
                    assert(false && "No default data address space");
                }
                
                int requiredBitWidth = unit.extensionMode() == Machine::SIGN ?
                        MathTools::requiredBitsSigned(dataAS->end()):
                        MathTools::requiredBits(dataAS->end());

                if (requiredBitWidth > iTemplate.supportedWidth(unit)) {
                    addResult = false;
                    break;
        	    }
                
            } else if (immediates[i]->value().isBasicBlockReference() ||
                immediates[i]->value().isCodeSymbolReference()) {

                const AddressSpace& as
                        = *rm_->machine().controlUnit()->addressSpace();
                int requiredBitWidth = unit.extensionMode() == Machine::SIGN ? 
                        MathTools::requiredBitsSigned(as.end()) : 
                        MathTools::requiredBits(as.end());
        	    
        	    if (requiredBitWidth > iTemplate.supportedWidth(unit)) {
        	        addResult = false;
        	        break;
                }

            }

            unitWriten.push_back(&unit);
        }
        // For each of moves already assigned template
        // should not use the bus for immediate transport
        for (unsigned int i = 0; i < moves.size(); i++) {
            const SchedulingResource* bus =
                busBroker_.resourceOf(moves[i]->bus());
            if ((*resIter).second->hasDependentResource(*bus)) {
                addResult = false;
                break;
            }
        }
        if (addResult) {
            result.insert(*(*resIter).second);
        }
    }
    return result;
}

/**
 * Transfer the instruction ownership away from this object,
 *
 * If this method is called, resource manager does not delete it's
 * instructions when it it destroyed.
 */
void
ITemplateBroker::loseInstructionOwnership(int cycle) {
    instructionsNotOwned_[cycle] = true;
}

/*
 * Tests if an immediate is already written into instruction template
 *
 * @param defCycle cycle which to test
 * @param imm Immediate to test in defCycle
 * @return True if immediate is already written in instruction template
 */
bool
ITemplateBroker::isImmediateInTemplate(
    int defCycle,
    std::shared_ptr<const Immediate> imm) const {

    if (!MapTools::containsKey(instructions_, defCycle)) {
        return false;
    }
    Instruction* ins =  MapTools::valueForKey<TTAProgram::Instruction*>(
        instructions_, defCycle);
    for (int i = 0; i < ins->immediateCount(); i++){
        if (ins->immediate(i).value().value() == imm->value().value() &&
            ins->immediate(i).destination().equals(imm->destination())){
            return true;
        }
    }
    return false;
}

/**
 * Tests if there is any instruction template available for immediate
 * in given cycle
 *
 * @param defCycle cycle to test
 * @param immediate Immediate to test
 * @return True if there is instruction template for given cycle which can
 * write given immediate
 */
bool
ITemplateBroker::isTemplateAvailable(
    int defCycle,
    std::shared_ptr<Immediate> immediate) const {

    Immediates immediates;
    Moves moves;
    immediates.push_back(immediate);
    int availableCount =
        findITemplates(defCycle, moves, immediates).count();
    return availableCount > 0;
}

/**
 * Clears bookkeeping which is needed for unassigning previously assigned
 * moves. After this call these cannot be unassigned, but new moves which
 * are assigned after this call can still be unassigned.
 */
void 
ITemplateBroker::clearOldResources() {
    oldParentInstruction_.clear();
}

/**
 * Clears all bookkeeping for the broker.
 *
 * the RM can then be reused for another basic block.
 */
void
ITemplateBroker::clear() {
    ResourceBroker::clear();
    clearOldResources();

    for (std::map<int, TTAProgram::Instruction*>::iterator i =
             instructions_.begin(); i != instructions_.end(); i++) {
        std::map<int, bool>::iterator j = instructionsNotOwned_.find(i->first);
        // does not find => owns, delete.
        if (j == instructionsNotOwned_.end() || j->second == false) {
            delete i->second;
        }
    }
    instructions_.clear();
    instructionsNotOwned_.clear();
    immediateCycles_.clear();
    immediateValues_.clear();
}
