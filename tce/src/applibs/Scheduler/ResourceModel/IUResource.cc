/**
 * @file IUResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the abstract IUResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "IUResource.hh"
#include "Application.hh"
#include "Conversion.hh"
#include "Move.hh"
#include "SequenceTools.hh"
#include "MoveNode.hh"
#include "MathTools.hh"

/**
 * Constructor defining resource name, register count and register width
 *
 * @param name Name of resource
 * @param registers Number of registers in IU
 * @param width Bit width of registers in immediate unit
 * @param latency Latency of Immediate unit, defaults to 1 in TCE
 * @param signExtension Indicates if IU is using Zero or Sign extend
 */
IUResource::IUResource(
    const std::string& name,
    const int registers,
    const int width,
    const int latency,
    const bool signExtension)
    : SchedulingResource(name) , registerCount_(registers), width_(width),
    latency_(latency) , signExtension_(signExtension){
    for (int i = 0; i < registerCount(); i++) {
        ResourceRecordVectorType vt;
        resourceRecord_.push_back(vt);
    }
}

/**
 * Empty destructor
 */
IUResource::~IUResource() {
    for (int i = 0; i < registerCount(); i++) {
        SequenceTools::deleteAllItems(resourceRecord_.at(i));
    }
}

/**
 * Test if resource IUResource is used in given cycle
 * @param cycle Cycle which to test
 * @return True if any (at least one) register of IU is already used in cycle
 */
bool
IUResource::isInUse(const int cycle) const {
    for (int i = 0; i < registerCount(); i++) {
        for (int j = 0;
            j < static_cast<int>(resourceRecord_.at(i).size());
            j++) {
            if (cycle > resourceRecord_.at(i).at(j)->definition_ &&
                cycle < resourceRecord_.at(i).at(j)->use_) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Test if resource IUResource is available
 * @param cycle Cycle which to test
 * @return False if all registers in IU are used in cycle
 */
bool
IUResource::isAvailable(const int cycle) const {
    for (int i = 0; i < registerCount(); i++) {
        bool marker = false;
        for (int j = 0 ;
            j < static_cast<int>(resourceRecord_.at(i).size());
            j++) {
            if (cycle > resourceRecord_.at(i).at(j)->definition_ &&
                cycle < resourceRecord_.at(i).at(j)->use_) {
                marker = true;
                break;
            }
        }
        // None of the intervals for registers overlapped cycle, register
        // is available for use -> unit is available too
        if (marker == false) {
            return true;
        }
    }
    return false;
}

/**
 * Implementation of abstract method from base class.
 * IUResource requires different api (definition and use cycle)
 * This method is not to be used!
 */

void
IUResource::assign(const int, MoveNode&)
    throw (Exception) {
    std::string msg = "IUResource: called assign with \'cycle\'";
    msg += " and \'node\'. Use assign with \'defCycle\',";
    msg += " \'useCycle\', \'node\' and \'index\' reference!";
    abortWithError(msg);
}

/**
 * Assign resource to given node for given cycle
 * @param defCycle Cycle in which the value of immediate register will be
 *          written using instruction template
 * @param useCycle Cycle in which the value of immediate register will be
 *          read by MoveNode
 * @param node MoveNode that reads the immediate register
 * @param index Will be used to return index of register assigned
 */
void
IUResource::assign(
    const int defCycle, const int useCycle, MoveNode& node, int& index)
    throw (Exception) {

    if (defCycle > useCycle) {
        std::string msg =
            "Long immediate definition cycle later than use cycle: ";
        msg += Conversion::toString(defCycle);
        msg += " > ";
        msg += Conversion::toString(useCycle);
        msg += "!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    if ((useCycle - defCycle) < latency_) {
        std::string msg = "Definition and use too close(";
        msg += Conversion::toString(useCycle - defCycle) + "). ";
        msg += name() + " has latency of " + Conversion::toString(latency_);
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    int i = findAvailable(defCycle, useCycle);
    if (i != -1) {
        index = i;
        ResourceRecordType* rc = new ResourceRecordType;
        rc->definition_ = defCycle;
        rc->use_ = useCycle;
        rc->immediateValue_ =
            node.move().source().copy();
        resourceRecord_.at(i).push_back(rc);
        return;
    }
    std::string msg = "Assignment for immediate impossible!";
    throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
}

/**
 * Unassign resource from given node for given cycle
 * @param cycle Cycle in which immediate is read which we want
 *          unassign.
 * @param node MoveNode to remove assignment from
 */
void
IUResource::unassign(const int, MoveNode& node)
    throw (Exception) {
    if (!node.move().source().isImmediateRegister()) {
        std::string msg = "Trying to unassign move that is not immediate\
            register read!";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }
    // Find which register in given IU is assigned to node
    int regIndex = node.move().source().index();
    if (resourceRecord_.at(regIndex).size() == 0) {
        std::string msg = "The register is not assigned!";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }
    std::vector<ResourceRecordType*>::iterator itr =
        resourceRecord_.at(regIndex).begin();
    // From given IU register delete record about
    // assignment to node and restore node source
    while (itr != resourceRecord_.at(regIndex).end()) {
        if (node.cycle() == (*itr)->use_) {
            TTAProgram::TerminalImmediate* originalTerminal =
                dynamic_cast<TTAProgram::TerminalImmediate*>(
                    (*itr)->immediateValue_);
            TTAProgram::Terminal* toSet = originalTerminal->copy();
            node.move().setSource(toSet);
            resourceRecord_.at(regIndex).erase(itr);
            
            return;
        }
        itr++;
    }
    std::string msg = "Register is not assigned for given cycle!";
    throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
}

/**
 * Implementation of abstract method from base class.
 * IUResource requires different api (definition and use cycle)
 * This method is not to be used!
 */
bool
IUResource::canAssign(const int, const MoveNode&) const {
    std::string msg = "IUResource: called canAssign with \'cycle\'";
    msg += " and \'node\'. Use canAssign with \'defCycle\',";
    msg += " \'useCycle\' and \'node\'!";
    abortWithError(msg);
    return false;
}

/**
 * Return true if resource can be assigned for given resource in given cycle
 * @param defCycle definition cycle
 * @param useCycle use of register cycle
 * @param node MoveNode to test
 * @return true if node can be assigned to cycle
 */
bool
IUResource::canAssign(
    const int defCycle,
    const int useCycle,
    const MoveNode& node) const {

    if (defCycle > useCycle) {
        return false;
    }
    if ((useCycle - defCycle) < latency_) {
        return false;
    }

    MoveNode& mNode = const_cast<MoveNode&>(node);
    if (!mNode.move().source().isImmediate()) {
        return false;
    }

    TTAProgram::TerminalImmediate* iTerm =
        dynamic_cast<TTAProgram::TerminalImmediate*>(&mNode.move().source());
    if (findAvailable(defCycle, useCycle) != -1) {
        // FIXME: hack to check if terminal is floating point value
        if ((iTerm->value().width() > INT_WORD_SIZE) && 
            (iTerm->value().width() > width_)) {
            return false;
        }
        if ((signExtension_ == true) &&
            (MathTools::requiredBitsSigned(
                iTerm->value().intValue()) > width_)) {
            return false;
        } else if (MathTools::requiredBits(iTerm->value().unsignedValue()) > 
            width_) {
            return false;
        }
        for (int i = 0; i < relatedResourceGroupCount(); i++) {
            for (int j = 0, count = relatedResourceCount(i); j < count; j++) {
                if (relatedResource(i,j).isOutputPSocketResource() &&
                    !relatedResource(i,j).isInUse(useCycle)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * Allways return true
 * @return true
 */
bool
IUResource::isIUResource() const {
    return true;
}

/**
 * Set the number of registers available in IU
 * @param registers Number of registers in this particular IU
 */
void
IUResource::setRegisterCount(const int registers) {
    // We shell not provide API for decreasing size of existing IU
    if (registers < registerCount()) {
        throw InvalidData(__FILE__, __LINE__, __func__, 
            "Can not decrease number of registers in IMM unit!");
    }
    for (int i = registerCount_; i < registers; i++) {
        ResourceRecordVectorType vt;
        resourceRecord_.push_back(vt);
    }
    registerCount_ = registers;
}

/**
 * Get number of registers in IU
 * @return Number of registers in IU
 */
int
IUResource::registerCount() const {
    return registerCount_;
}

/**
 * Returns a constant that should be written to immediate register
 * used by MoveNode in cycle
 * @param cycle Cycle in which immediate register is read
 * @param node MoveNode which is reading the register
 * @return Long immediate constant that is expected to be in register
 */
TTAProgram::Terminal*
IUResource::immediateValue(const MoveNode& node) const
    throw (KeyNotFound) {

    MoveNode& testNode = const_cast<MoveNode&>(node);
    if (!testNode.move().source().isImmediateRegister()) {
        std::string msg = "Immediate register was not assigned!";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }
    int regIndex = testNode.move().source().index();
    for (int j = 0;
        j < static_cast<int>(resourceRecord_.at(regIndex).size());
        j++) {
        if (node.cycle() == resourceRecord_.at(regIndex).at(j)->use_) {
            // Test if cycle equal to use of value
            // for register, if it is we return original terminal
            return resourceRecord_.at(regIndex).at(j)->immediateValue_;
        }
    }
    std::string msg = "Immediate register was not recorded in resource!";
    throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
}
/**
 * Returns a cycle in which immediate register used by MoveNode is written
 * 
 * @param node MoveNode which is reading the register
 * @return cycle in which register is written
 */
int
IUResource::immediateWriteCycle(const MoveNode& node) const
    throw (KeyNotFound){

    MoveNode& testNode = const_cast<MoveNode&>(node);
    if (!testNode.move().source().isImmediateRegister()) {
        std::string msg = "Immediate register was not assigned!";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }
    int regIndex = testNode.move().source().index();
    for (int j = 0;
        j < static_cast<int>(resourceRecord_.at(regIndex).size());
        j++) {
        if (node.cycle() == resourceRecord_.at(regIndex).at(j)->use_) {
            // Test if cycle equal to use of value
            // for register, if it is we return definition cycle
            return resourceRecord_.at(regIndex).at(j)->definition_;
        }
    }
    std::string msg = "Immediate register was not recorded in resource!";
    throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
}
/**
 * Tests if all referred resources in dependent groups are of
 * proper types
 * @return true If all resources in dependent groups are
 *              Immediate Registers
 */
bool
IUResource::validateDependentGroups() {
    return true;
}

/**
 * Tests if all referred resources in related groups are of
 * proper types
 * @return true If all resources in related groups are
 *              Instruction Templates or P-Sockets
 */
bool
IUResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        for (int j = 0, count = relatedResourceCount(i); j < count; j++) {
            if (!(relatedResource(i, j).isOutputPSocketResource() ||
                relatedResource(i, j).isInputPSocketResource()   ||
                relatedResource(i, j).isITemplateResource())) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Internal helper method, find available register for given pair of
 * definition and use cycles.
 */
int
IUResource::findAvailable(const int defCycle, const int useCycle) const {
    for (int i = 0; i < registerCount(); i++) {
        bool marker = false;
        for (int j = 0;
            j < static_cast<int>(resourceRecord_.at(i).size());
            j++) {
            if ((useCycle > resourceRecord_.at(i).at(j)->definition_) &&
                (useCycle <= resourceRecord_.at(i).at(j)->use_)) {
                // Test if useCycle is already within any assigned interval
                // for register, if it is we can not use it
                // Allows write in same cycle as reading previous
                marker = true;
                break;
            }
            if ((defCycle >= resourceRecord_.at(i).at(j)->definition_) &&
                (defCycle < resourceRecord_.at(i).at(j)->use_)) {
                // Test if defCycle is already within any assigned interval
                // for register, if it is we can not use it
                marker = true;
                break;
            }
            if ((defCycle <= resourceRecord_.at(i).at(j)->definition_) &&
                (useCycle >= resourceRecord_.at(i).at(j)->use_)) {
                // Test if defCycle is before definition of register
                // and useCycle is after the use of register (means the
                // already defined value would be inside interval we want to
                // use.
                marker = true;
                break;
            }
        }
        if (marker == false) {
            return i;
        }
    }
    return -1;
}

/**
 * Destructor for internal storage for assignment records.
 * Deletes a backup copy of original terminal when the node is unassigned.
 */
IUResource::ResourceRecordType::~ResourceRecordType(){
    if (immediateValue_ != NULL)
        delete immediateValue_;
}

/**
 * Constructor for internal storage for assignment records.
 * Assigns all values to default value.
 */
IUResource::ResourceRecordType::ResourceRecordType() :
    definition_(-1), use_(-1), immediateValue_(NULL){
}

/**
 * Returns a width of registers in Immediate Unit.
 *
 * @return Width of registers in Immediate Unit.
 */
int
IUResource::width() const {
    return width_;
}
