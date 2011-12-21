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
 * @file ExecutionPipeline.cc
 *
 * Implementation of class ExecutionPipeline.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "ExecutionPipeline.hh"
#include "HWOperation.hh"
#include "FunctionUnit.hh"
#include "PipelineElement.hh"
#include "FUPort.hh"
#include "MachineTester.hh"
#include "MOMTextGenerator.hh"
#include "Application.hh"
#include "ContainerTools.hh"
#include "AssocTools.hh"
#include "MapTools.hh"
#include "ObjectState.hh"

using std::string;
using std::pair;
using std::set;
using boost::format;

namespace TTAMachine {

// initialization of static data members
const string ExecutionPipeline::OSNAME_PIPELINE = "pipeline";
const string ExecutionPipeline::OSNAME_RESOURCE_USAGE = "resource_usage";
const string ExecutionPipeline::OSNAME_OPERAND_READ = "op_read";
const string ExecutionPipeline::OSNAME_OPERAND_WRITE = "op_write";
const string ExecutionPipeline::OSKEY_RESOURCE_NAME = "res_name";
const string ExecutionPipeline::OSKEY_OPERAND = "operand";
const string ExecutionPipeline::OSKEY_START_CYCLE = "start_cycle";
const string ExecutionPipeline::OSKEY_CYCLES = "cycles";

/**
 * Constructor.
 *
 * @param parentOperation The parent operation which uses the pipeline.
 */
ExecutionPipeline::ExecutionPipeline(HWOperation& parentOperation) :
    SubComponent(), parent_(&parentOperation) {

    // run time sanity check
    assert(parentOperation.pipeline() == NULL);
}


/**
 * Destructor.
 */
ExecutionPipeline::~ExecutionPipeline() {
    removeAllResourceUses();
}

/**
 * Returns the parent HWOperation.
 *
 * @return The parent.
 */
const HWOperation*
ExecutionPipeline::parentOperation() const {
    return parent_;
}

/**
 * Adds use for the given pipeline element.
 *
 * If the function unit does not have a pipeline element by the given name
 * the pipeline element is added to the function unit. Pipeline is valid only
 * if it starts at cycle 0 or 1.
 *
 * @param name Name of the pipeline element.
 * @param start First cycle in which the element is used relative to
 *              operation start cycle.
 * @param duration Number of cycles the element is used.
 * @exception OutOfRange If given start cycle or duration is out of range.
 * @exception InvalidName If the given name is not a valid component name.
 * @exception StartTooLate If the pipeline does not start by resource usage
 *                         or operand read at cycle 0 or 1.
 * @exception NotAvailable If the given resource is already in use at a
 *                         cycle in the given time interval.
 */
void
ExecutionPipeline::addResourceUse(
    const std::string& name,
    int start,
    int duration)
    throw (OutOfRange, InvalidName, StartTooLate, NotAvailable) {

    const string procName = "ExecutionPipeline::addResourceUse";

    checkStartCycle(start);
    checkDuration(duration);
    checkResourceName(name);
    checkResourceAvailability(name, start, duration);

    internalAddResourceUse(name, start, duration);
}


/**
 * Adds read usage of an operand.
 *
 * @param operand The operand to be read.
 * @param start Start cycle of the usage.
 * @param duration Duration of the usage.
 * @exception OutOfRange If the given cycle is less than 0 or if duration or
 *                       operand is less than 1.
 * @exception WrongOperandType If the given operand is not an input operand,
 *                             that is, if a port write is already added with
 *                             the given operand.
 * @exception StartTooLate If the pipeline does not start by resource usage
 *                         or operand read at cycle 0 or 1.
 * @exception NotAvailable If the given operand is already read at a cycle in
 *                         the given time interval.
 */
void
ExecutionPipeline::addPortRead(
    int operand,
    int start,
    int duration)
    throw (OutOfRange, WrongOperandType, StartTooLate, NotAvailable) {

    const string procName = "ExecutionPipeline::addPortRead";

    checkStartCycle(start);
    checkDuration(duration);
    checkInputOperand(operand);
    checkOperandAvailability(operand, start, duration);

    internalAddPortUse(operand, start, duration, opReads_);
}


/**
 * Adds write usage of an operand.
 *
 * @param operand The operand to be written.
 * @param start Start cycle of the usage.
 * @param duration Duration of the usage.
 * @exception OutOfRange If the given cycle is less than 0 or if duration or
 *                       operand is less than 1.
 * @exception WrongOperandType If the given operand is not an output operand,
 *                             that is, if a port read is already added with
 *                             the given operand.
 * @exception NotAvailable If the given operand is already written at a cycle
 *                         in the given time interval.
 */
void
ExecutionPipeline::addPortWrite(
    int operand,
    int start,
    int duration)
    throw (OutOfRange, WrongOperandType, NotAvailable) {

    const string procName = "ExecutionPipeline::addPortWrite";

    if (start < 0) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    checkDuration(duration);
    checkOutputOperand(operand);
    checkOperandAvailability(operand, start, duration);

    internalAddPortUse(operand, start, duration, opWrites_);
}


/**
 * Removes any usage of given pipeline element in the pipeline.
 *
 * @param name Name of the pipeline element.
 * @exception StartTooLate If the pipeline does not start by resource usage
 *                         or operand read at cycle 0 or 1.
 */
void
ExecutionPipeline::removeResourceUse(const std::string& name)
    throw (StartTooLate) {

    const string procName = "ExecutionPipeline::removeResourceUse";

    FunctionUnit* fu = parent_->parentUnit();
    if (!fu->hasPipelineElement(name)) {
        return;
    }

    if (firstCycleWithoutResource(name) > 1) {
        throw StartTooLate(__FILE__, __LINE__, procName);
    }

    internalRemoveResourceUse(name, 0, latency());
}


/**
 * Removes any usage of given pipeline element in given cycle from the
 * pipeline.
 *
 * @param name Name of the pipeline element.
 * @param cycle Cycle in which element use is to be removed.
 * @exception OutOfRange If the given cycle is negative.
 * @exception StartTooLate If the pipeline does not start by resource usage
 *                         or operand read at cycle 0 or 1.
 */
void
ExecutionPipeline::removeResourceUse(const std::string& name, int cycle)
    throw (OutOfRange, StartTooLate) {

    const string procName = "ExecutionPipeline::removeResourceUse";

    if (pipelineElement(name) == NULL) {
        return;
    }

    if (cycle < 0) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (latency() <= cycle) {
        return;
    }

    if (cycle == 0 || cycle == 1) {
        if (firstCycleWithoutResource(name, cycle) > 1) {
            throw StartTooLate(__FILE__, __LINE__, procName);
        }
    }

    internalRemoveResourceUse(name, cycle, 1);
}


/**
 * Removes the all the usages of resources.
 */
void
ExecutionPipeline::removeAllResourceUses() {

    ResourceSet usedResources = this->usedResources();

    resourceUsage_.clear();
    opWrites_.clear();
    opReads_.clear();

    FunctionUnit* parent = parent_->parentUnit();

    // clean up pipeline elements
    for (ResourceSet::const_iterator iter = usedResources.begin();
         iter != usedResources.end(); iter++) {
        parent->cleanup((*iter)->name());
    }
}


/**
 * Removes reads and writes of the given operand at the given cycle.
 *
 * @param operand The operand.
 * @param cycle The cycle.
 * @exception OutOfRange If the given cycle is less than 0 or the given
 *                       operand is smaller than 1.
 * @exception StartTooLate If the pipeline does not start by resource usage
 *                         or operand read at cycle 0 or 1.
 */
void
ExecutionPipeline::removeOperandUse(int operand, int cycle)
    throw (OutOfRange, StartTooLate) {

    const string procName = "ExecutionPipeline::removeOperandUse";

    if (cycle < 0 || operand < 1) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (latency() <= cycle) {
        return;
    }

    if (firstCycleWithoutOperandUse(operand, cycle) > 1) {
        throw StartTooLate(__FILE__, __LINE__, procName);
    }

    internalRemoveOperandUse(operand, cycle, 1);
}


/**
 * Tells whether a given pipeline element is used in the given cycle by this
 * pipeline.
 *
 * @param name Name of the pipeline element.
 * @param cycle Cycle in which element use is to be tested.
 * @return True if the element is in use in the given cycle.
 * @exception OutOfRange If the given cycle is negative.
 */
bool
ExecutionPipeline::isResourceUsed(const std::string& name, int cycle) const
    throw (OutOfRange) {

    if (cycle < 0) {
        const string procName = "ExecutionPipeline::isResourceUsed";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (latency() <= cycle) {
        return false;
    }

    PipelineElement* resource = pipelineElement(name);
    if (resource == NULL) {
        return false;
    }

    ResourceSet usedResources = resourceUsage_[cycle];
    return (AssocTools::containsKey(usedResources, resource));
}

/**
 * Returns the resource usages in the given cycle.
 *
 * @param cycle Cycle of which resource usages to return.
 * @return True if the element is in use in the given cycle.
 * @exception OutOfRange If the given cycle is negative.
 */
ExecutionPipeline::ResourceSet
ExecutionPipeline::resourceUsages(int cycle) const
    throw (OutOfRange) {

    if (cycle < 0) {
        const string procName = "ExecutionPipeline::isResourceUsed";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (latency() <= cycle) {
        return ResourceSet();
    }

    return resourceUsage_[cycle];
}

/**
 * Checks whether the given port is used at the given cycle by the pipeline.
 *
 * @param port The port.
 * @param cycle The cycle.
 * @return True if the port is used, otherwise false.
 * @exception OutOfRange If the given cycle is negative.
 */
bool
ExecutionPipeline::isPortUsed(const FUPort& port, int cycle) const
    throw (OutOfRange) {

    return isPortRead(port, cycle) || isPortWritten(port, cycle);
}


/**
 * Checks whether the given port is read at the given cycle by the pipeline.
 *
 * @param port The port.
 * @param cycle The cycle.
 * @return True if the port is read, otherwise false.
 * @exception OutOfRange If the given cycle is negative.
 */
bool
ExecutionPipeline::isPortRead(const FUPort& port, int cycle) const
    throw (OutOfRange) {

    if (cycle < 0) {
        const string procName = "ExecutionPipeline::isPortRead";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (cycle >= latency()) {
        return false;
    }

    OperandSet readOperands = opReads_[cycle];
    return (isOperandBound(port, readOperands));
}


/**
 * Checks whether the given port is written at the given cycle by the
 * pipeline.
 *
 * @param port The port.
 * @param cycle The cycle.
 * @return True if the port is written, otherwise false.
 * @exception OutOfRange If the given cycle is negative.
 */
bool
ExecutionPipeline::isPortWritten(const FUPort& port, int cycle) const
    throw (OutOfRange) {

    if (cycle < 0) {
        const string procName = "ExecutionPipeline::isPortRead";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (cycle >= latency()) {
        return false;
    }

    OperandSet writtenOperands = opWrites_[cycle];
    return (isOperandBound(port, writtenOperands));
}


/**
 * Returns the operands that are read at the given cycle.
 *
 * @param cycle The cycle.
 * @return Set of operand indexes.
 * @exception OutOfRange If the cycle is smaller than 0.
 */
ExecutionPipeline::OperandSet
ExecutionPipeline::readOperands(int cycle) const
    throw (OutOfRange) {

    if (cycle < 0) {
        const string procName = "ExecutionPipeline::readOperands";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (cycle >= latency()) {
        return OperandSet();
    }

    return opReads_[cycle];
}


/**
 * Returns the operands that are written at the given cycle.
 *
 * @param cycle The cycle.
 * @return Set of operand indexes.
 * @exception OutOfRange If the given cycle is smaller than 0.
 */
ExecutionPipeline::OperandSet
ExecutionPipeline::writtenOperands(int cycle) const
    throw (OutOfRange) {

    if (cycle < 0) {
        const string procName = "ExecutionPipeline::readOperands";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (cycle >= latency()) {
        return OperandSet();
    }

    return opWrites_[cycle];
}


/**
 * Returns a set of operands that are read by this pipeline.
 *
 * @return The operand set.
 */
ExecutionPipeline::OperandSet
ExecutionPipeline::readOperands() const {
    OperandSet operands;
    for (int i = 0; i < latency(); i++) {
        OperandSet cycleOperands = readOperands(i);
        operands.insert(cycleOperands.begin(), cycleOperands.end());
    }
    return operands;
}


/**
 * Returns a set of operands that are written by this pipeline.
 *
 * @return The operand set.
 */
ExecutionPipeline::OperandSet
ExecutionPipeline::writtenOperands() const {
    OperandSet operands;
    for (int i = 0; i < latency(); i++) {
        OperandSet cycleOperands = writtenOperands(i);
        operands.insert(cycleOperands.begin(), cycleOperands.end());
    }
    return operands;
}


/**
 * Returns the latency of the pipeline.
 *
 * If the pipeline is totally empty, returns 0.
 *
 * @return The latency of the pipeline.
 */
int
ExecutionPipeline::latency() const {
    assert(resourceUsage_.size() == opReads_.size());
    assert(resourceUsage_.size() == opWrites_.size());
    return resourceUsage_.size();
}


/**
 * Returns the latency for the given output.
 *
 * @param output The number of the output.
 * @return The latency for the given output.
 * @exception IllegalParameters If the given output is not written in the
 *                              pipeline.
 */
int
ExecutionPipeline::latency(int output) const
    throw (IllegalParameters) {

    int cycle = latency();
    for (IOUsage::const_reverse_iterator iter = opWrites_.rbegin(); 
         iter != opWrites_.rend(); iter++) {
        if (AssocTools::containsKey(*iter, output)) {
            return cycle;
        }
        cycle--;
    }

    const string msg =
        (boost::format("Latency for operand index %d was not found for "
            "operation '%s' on unit '%s'!") % output % parent_->name()
            % parent_->parentUnit()->name()).str();
    throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
}


/**
 * Returns the slack of the given input.
 *
 * The slack tells how many cycles AFTER the trigger, opcode-setting move is
 * scheduled, can the operand be scheduled (and still affect correctly the
 * result of the operation).
 *
 * @param input The number of the input.
 * @return The slack of the given input.
 * @exception IllegalParameters If the given input is not read in the 
 *                              pipeline.
 */
int
ExecutionPipeline::slack(int input) const
    throw (IllegalParameters) {

    int slack(0);
    for (IOUsage::const_iterator iter = opReads_.begin();
         iter != opReads_.end(); iter++) {
        if (AssocTools::containsKey(*iter, input)) {
            return slack;
        }
        slack++;
    }

    const string procName = "ExecutionPipeline::slack";
    throw IllegalParameters(__FILE__, __LINE__, procName);
}
   

/**
 * Saves the pipeline to ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
ExecutionPipeline::saveState() const {

    ObjectState* pipelineState = new ObjectState(OSNAME_PIPELINE);

    // save resources usages in different ObjectState instances
    int cycle = 0;
    for (ResourceUsage::const_iterator resUsageIter = resourceUsage_.begin();
         resUsageIter != resourceUsage_.end(); resUsageIter++) {

        ResourceSet cycleUsage = *resUsageIter;
        for (ResourceSet::const_iterator cycleUsageIter = cycleUsage.begin();
             cycleUsageIter != cycleUsage.end(); cycleUsageIter++) {
            PipelineElement* element = *cycleUsageIter;
            saveResourceUse(element, cycle, pipelineState);
        }
        cycle++;
    }

    // save operand reads in different ObjectState instances
    cycle = 0;
    for (IOUsage::const_iterator readsIter = opReads_.begin();
         readsIter != opReads_.end(); readsIter++) {

        OperandSet operands = *readsIter;
        for (OperandSet::const_iterator opIter = operands.begin();
             opIter != operands.end(); opIter++) {
            int operand = *opIter;
            saveOperandUse(
                operand, cycle, pipelineState, OSNAME_OPERAND_READ);
        }
        cycle++;
    }

    // save operand writes in different ObjectState instances
    cycle = 0;
    for (IOUsage::const_iterator writesIter = opWrites_.begin();
         writesIter != opWrites_.end(); writesIter++) {

        OperandSet operands = *writesIter;
        for (OperandSet::const_iterator opIter = operands.begin();
             opIter != operands.end(); opIter++) {
            int operand = *opIter;
            saveOperandUse(
                operand, cycle, pipelineState, OSNAME_OPERAND_WRITE);
        }
        cycle++;
    }

    return pipelineState;
}


/**
 * Loads the state of pipeline from the given ObjectState tree.
 *
 * @param pipelineState An ObjectState tree representing state of a pipeline.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        state.
 */
void
ExecutionPipeline::loadState(const ObjectState* pipelineState)
    throw (ObjectStateLoadingException) {

    const string procName = "ExecutionPipeline::loadState";

    if (pipelineState->name() != OSNAME_PIPELINE) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    removeAllResourceUses();
    ObjectStateTable childTable = sortResourceUsages(pipelineState);
    MOMTextGenerator textGen;

    try {
        for (ObjectStateTable::const_iterator iter = childTable.begin();
             iter != childTable.end(); iter++) {
            const ObjectState* usage = *iter;
            int startCycle = usage->intAttribute(OSKEY_START_CYCLE);
            int cycles = usage->intAttribute(OSKEY_CYCLES);
            int operand = 0;
            string resourceName;

            try {

                if (usage->name() == OSNAME_RESOURCE_USAGE) {
                    resourceName = usage->stringAttribute(
                        OSKEY_RESOURCE_NAME);
                    addResourceUse(resourceName, startCycle, cycles);
                } else if (usage->name() == OSNAME_OPERAND_READ) {
                    operand = usage->intAttribute(OSKEY_OPERAND);
                    addPortRead(operand, startCycle, cycles);
                } else if (usage->name() == OSNAME_OPERAND_WRITE) {
                    operand = usage->intAttribute(OSKEY_OPERAND);
                    addPortWrite(operand, startCycle, cycles);
                } else {
                    throw ObjectStateLoadingException(
                        __FILE__, __LINE__, procName);
                }

            } catch (const WrongOperandType&) {
                format errorMsg = textGen.text(
                    MOMTextGenerator::TXT_PORT_READ_AND_WRITTEN_BY_PIPELINE);
                errorMsg % operand % parent_->name() %
                    parent_->parentUnit()->name();
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName, errorMsg.str());

            } catch (const InvalidName&) {
                format errorMsg = textGen.text(
                    MOMTextGenerator::TXT_INVALID_NAME);
                errorMsg % resourceName;
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName, errorMsg.str());

            } catch (const StartTooLate&) {
                format errorMsg = textGen.text(
                    MOMTextGenerator::TXT_PIPELINE_START_TOO_LATE);
                errorMsg % parent_->name() % parent_->parentUnit()->name() %
                    startCycle;
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName, errorMsg.str());

            } catch (const NotAvailable&) {
                format errorMsg = textGen.text(
                    MOMTextGenerator::TXT_PIPELINE_NOT_CANONICAL);
                errorMsg % parent_->name() % parent_->parentUnit()->name();
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, errorMsg.str());
            }
        }

    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * Compares two ExecutionPipeline architectures.
 *
 * @param pipeline ExecutionPipeline to compare with.
 * @return True if the two ExecutionPipeline architectures are equal.
 */
bool
ExecutionPipeline::isArchitectureEqual(
    const ExecutionPipeline* pipeline) const {
    
    for (unsigned int i = 0; i < resourceUsage_.size(); i++) {
        ResourceSet::const_iterator iter = resourceUsage_[i].begin();
        for (; iter != resourceUsage_[i].end(); iter++) {
            if (!pipeline->isResourceUsed((*iter)->name(), i)) {
                return false;
            }
        }
    }
    if (readOperands() != pipeline->readOperands()) {
        return false;
    }
    if (writtenOperands() != pipeline->writtenOperands()) {
        return false;
    }
    return true;
}


/**
 * Checks whether the pipeline would start too late if a new resource usage
 * or operand read was added starting at given cycle.
 *
 * @param startCycle The start cycle.
 * @exception OutOfRange If the given cycle is negative.
 * @exception StartTooLate If the pipeline would start too late.
 */
void
ExecutionPipeline::checkStartCycle(int startCycle) const
    throw (OutOfRange, StartTooLate) {

    const string procName = "ExecutionPipeline::checkStartCycle";

    if (startCycle < 0) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (startCycle > 1) {
        if (latency() == 0) {
            throw StartTooLate(__FILE__, __LINE__, procName);
        } else if (latency() == 1 && resourceUsage_[0].empty() &&
                   opReads_[0].empty()) {
            throw StartTooLate(__FILE__, __LINE__, procName);
        } else if (latency() >= 2 && resourceUsage_[0].empty() && 
                   resourceUsage_[1].empty() && opReads_[0].empty() &&
                   opReads_[1].empty()) {
            throw StartTooLate(__FILE__, __LINE__, procName);
        }
    }
}


/**
 * Checks whether the given value is valid for duration of resource usage.
 *
 * @param duration The duration value.
 * @exception OutOfRange If the value is not in a valid range.
 */
void
ExecutionPipeline::checkDuration(int duration)
    throw (OutOfRange) {

    if (duration < 1) {
        const string procName = "ExecutionPipeline::checkDuration";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
}


/**
 * Checks whether the given name is valid for pipeline resource.
 *
 * @param name The name.
 * @exception InvalidName If the name is not valid for pipeline resource.
 */
void
ExecutionPipeline::checkResourceName(const std::string& name)
    throw (InvalidName) {

    if (!MachineTester::isValidComponentName(name)) {
        const string procName = "ExecutionPipeline::checkResourceName";
        throw InvalidName(__FILE__, __LINE__, procName);
    }
}


/**
 * Checks whether the given operand is an input operand.
 *
 * The operand is an input operand if it is read by the pipeline already.
 *
 * @param operand The operand.
 * @exception OutOfRange If the given operand is smaller than 1.
 * @exception WrongOperandType If the operand is written by the pipeline.
 */
void
ExecutionPipeline::checkInputOperand(int operand) const
    throw (OutOfRange, WrongOperandType) {

    const string procName = "ExecutionPipeline::checkInputOperand";

    if (operand < 1) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (isOperandWritten(operand)) {
        throw WrongOperandType(__FILE__, __LINE__, procName);
    }
}


/**
 * Checks whether the given operand is an output operand.
 *
 * The operand is an output operand if it is written by the pipeline already.
 *
 * @param operand The operand.
 * @exception OutOfRange If the given operand is smaller than 1.
 * @exception WrongOperandType If the operand is read by the pipeline.
 */
void
ExecutionPipeline::checkOutputOperand(int operand) const
    throw (OutOfRange, WrongOperandType) {

    const string procName = "ExecutionPipeline::checkOutputOperand";

    if (operand < 1) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (isOperandRead(operand)) {
        throw WrongOperandType(__FILE__, __LINE__, procName);
    }
}


/**
 * Checks whether the given resource is available all the time of the given
 * time interval.
 *
 * @param resource Name of the pipeline resource.
 * @param start The start cycle.
 * @param duration Duration of the usage.
 * @exception NotAvailable If the given resource is not available.
 */
void
ExecutionPipeline::checkResourceAvailability(
    const std::string& resource,
    int start,
    int duration) const
    throw (NotAvailable) {

    PipelineElement* element = pipelineElement(resource);
    if (element == NULL) {
        return;
    }

    int end = start + duration - 1;
    if (end >= latency()) {
        end = latency() - 1;
    }

    for (int cycle = start; cycle <= end; cycle++) {
        if (AssocTools::containsKey(resourceUsage_[cycle], element)) {
            const string procName =
                "ExecutionPipeline::checkResourceAvailability";
            throw NotAvailable(__FILE__, __LINE__, procName);
        }
    }
}


/**
 * Checks whether the given operand is not read or written at the given time
 * interval.
 *
 * @param operand The operand.
 * @param start The start cycle.
 * @param duration Duration of the usage.
 * @exception NotAvailable If the given operand is used at the given time
 *                         interval.
 */
void
ExecutionPipeline::checkOperandAvailability(
    int operand,
    int start,
    int duration) const
    throw (NotAvailable) {

    int end = start + duration - 1;
    if (end >= latency()) {
        end = latency() - 1;
    }

    for (int cycle = start; cycle <= end; cycle++) {
        if (AssocTools::containsKey(opReads_[cycle], operand) ||
            AssocTools::containsKey(opWrites_[cycle], operand)) {
            const string procName =
                "ExecutionPipeline::checkOperandAvailability";
            throw NotAvailable(__FILE__, __LINE__, procName);
        }
    }
}


/**
 * Internally adds the resource usage of the given resource at the given
 * time interval.
 *
 * @param name Name of the resource.
 * @param start The start cycle.
 * @param duration The duration of the usage.
 */
void
ExecutionPipeline::internalAddResourceUse(
    const std::string& name,
    int start,
    int duration) {

    PipelineElement* used = pipelineElement(name);
    if (used == NULL) {
        used = addPipelineElement(name);
    }

    adjustLatency(start + duration);

    for (int cycle = start; cycle < start + duration; cycle++) {
        resourceUsage_[cycle].insert(used);
    }
}


/**
 * Internally adds use of the given operand.
 *
 * @param operand The operand to add the usage for.
 * @param start The start cycle of the operand usage.
 * @param duration Duration of the operand usage.
 * @param toModify The IOUsage to modify.
 */
void
ExecutionPipeline::internalAddPortUse(
    int operand,
    int start,
    int duration,
    IOUsage& toModify) {

    if (start + duration > latency()) {
        adjustLatency(start + duration);
    }

    for (int cycle = start; cycle < start + duration; cycle++) {
        toModify[cycle].insert(operand);
    }
}


/**
 * Internally removes the resource usage of the given resource at the given
 * time interval.
 *
 * @param name Name of the resource.
 * @param start The start cycle.
 * @param duration The duration of the usage to be removed.
 */
void
ExecutionPipeline::internalRemoveResourceUse(
    const std::string& name,
    int start,
    int duration) {

    PipelineElement* toRemove = pipelineElement(name);
    if (toRemove == NULL) {
        return;
    }

    int end = start + duration - 1;
    if (end >= latency()) {
        end = latency() - 1;
    }

    for (int cycle = start; cycle <= end; cycle++) {
        ContainerTools::removeValueIfExists(resourceUsage_[cycle], toRemove);
    }

    adjustLatency(0);

    FunctionUnit* fu = parent_->parentUnit();
    fu->cleanup(name);
}


/**
 * Internally removes the operand usage of the given operand at the given
 * time interval.
 *
 * @param operand The operand.
 * @param start The start cycle.
 * @param duration The duration of the usage to be removed.
 */
void
ExecutionPipeline::internalRemoveOperandUse(
    int operand,
    int start,
    int duration) {

    int end = start + duration - 1;
    if (end >= latency()) {
        end = latency() - 1;
    }

    for (int cycle = start; cycle <= end; cycle++) {
        ContainerTools::removeValueIfExists(opReads_[cycle], operand);
        ContainerTools::removeValueIfExists(opWrites_[cycle], operand);
    }

    adjustLatency(0);
}


/**
 * Adjusts the size of usage vectors for the given latency.
 *
 * If the given latency is greater than the current latency, sizes of
 * the usage vectors are increased. If the given latency is smaller
 * than the current latency (vector size), sizes of the vectors are
 * tried to decrease to match with the given latency. However, if
 * there are non-empty elements in the usage vectors, they are not
 * removed.
 *
 * @param newLatency The new latency.
 */
void
ExecutionPipeline::adjustLatency(int newLatency) {

    if (newLatency > latency()) {
        resourceUsage_.resize(newLatency);
        opReads_.resize(newLatency);
        opWrites_.resize(newLatency);

    } else if (newLatency < latency()) {
        for (int cycle = latency() - 1; cycle >= newLatency; cycle--) {
            if (resourceUsage_[cycle].empty() && opReads_[cycle].empty() &&
                opWrites_[cycle].empty()) {
                resourceUsage_.resize(cycle);
                opReads_.resize(cycle);
                opWrites_.resize(cycle);
            } else {
                break;
            }
        }
    }
}


/**
 * Checks whether the given port is bound to one of the operands in the given
 * operand set.
 *
 * @param port The port.
 * @param operands The operands.
 * @return True if at least one of the operands is bound to the given port,
 *         otherwise false.
 */
bool
ExecutionPipeline::isOperandBound(
    const FUPort& port,
    const OperandSet& operands) const {

    for (OperandSet::const_iterator iter = operands.begin();
         iter != operands.end(); iter++) {
        if (parent_->port(*iter) == &port) {
            return true;
        }
    }

    return false;
}


/**
 * Checks whether the given operand is written by the pipeline.
 *
 * @param operand The operand.
 * @return True if the operand is written, otherwise false.
 */
bool
ExecutionPipeline::isOperandWritten(int operand) const {
    int latency = this->latency();
    for (int cycle = 0; cycle < latency; cycle++) {
        OperandSet cycleWrites = opWrites_[cycle];
        if (AssocTools::containsKey(cycleWrites, operand)) {
            return true;
        }
    }
    return false;
}


/**
 * Checks whether the given operand is read by the pipeline.
 *
 * @param operand The operand.
 * @return True if the operand is read, otherwise false.
 */
bool
ExecutionPipeline::isOperandRead(int operand) const {
    int latency = this->latency();
    for (int cycle = 0; cycle < latency; cycle++) {
        OperandSet cycleReads = opReads_[cycle];
        if (AssocTools::containsKey(cycleReads, operand)) {
            return true;
        }
    }
    return false;
}


/**
 * Returns the first cycle when a resource is used or a port read by this
 * pipeline.
 *
 * Returns -1 if no resource is used or port read by this pipeline.
 *
 * @return The first cycle or -1.
 */
int
ExecutionPipeline::firstCycle() const {

    int latency = this->latency();

    for (int cycle = 0; cycle < latency; cycle++) {
        if (!resourceUsage_[cycle].empty() || !opReads_[cycle].empty()) {
            return cycle;
        }
    }

    return -1;
}


/**
 * Returns the first cycle when a resource is used or port read by this
 * pipeline if the given pipeline resource is not used.
 *
 * Returns -1 if no resource is used or port read.
 *
 * @param resource Name of the pipeline resource.
 * @return The first cycle or -1.
 */
int
ExecutionPipeline::firstCycleWithoutResource(
    const std::string& resource) const {

    PipelineElement* element = pipelineElement(resource);
    if (element == NULL) {
        return firstCycle();
    }

    int latency = this->latency();
    for (int i = 0; i < latency; i++) {
        if (!opReads_[i].empty()) {
            return i;
        }
        if (resourceUsage_[i].size() > 1 ||
            (resourceUsage_[i].size() == 1 &&
             !AssocTools::containsKey(resourceUsage_[i], element))) {
            return i;
        }
    }

    return -1;
}


/**
 * Returns the first cycle when a resource is used or port read if usage of
 * the given resource is removed from the given cycle.
 *
 * Returns -1 if no resource is used or port read.
 *
 * @param resource Name of the pipeline resource.
 * @param cycle The cycle when the pipeline resource is not used.
 * @return The first cycle or -1.
 */
int
ExecutionPipeline::firstCycleWithoutResource(
    const std::string& resource,
    int cycle) const {

    int currentFirstCycle = firstCycle();
    if (currentFirstCycle != cycle) {
        return currentFirstCycle;
    }

    PipelineElement* element = pipelineElement(resource);
    if (element == NULL) {
        return currentFirstCycle;
    }

    if (!opReads_[cycle].empty()) {
        return cycle;
    }
    if (resourceUsage_[cycle].size() > 1 ||
        (resourceUsage_[cycle].size() == 1 &&
         !AssocTools::containsKey(resourceUsage_[cycle], element))) {
        return cycle;
    }

    int latency = this->latency();
    for (int i = cycle + 1; i < latency; i++) {
        if (!opReads_[i].empty() || !resourceUsage_[i].empty()) {
            return i;
        }
    }

    return -1;
}


/**
 * Returns the first cycle when a resource is used or port read if usage of
 * the given operand is removed from the given cycle.
 *
 * Returns -1 if no resource is used.
 *
 * @param operand The operand.
 * @param cycle The cycle.
 * @return The first cycle or -1.
 */
int
ExecutionPipeline::firstCycleWithoutOperandUse(
    int operand,
    int cycle) const {

    int currentFirstCycle = firstCycle();
    if (currentFirstCycle != cycle) {
        return currentFirstCycle;
    }


    if (opReads_[cycle].size() > 1 ||
        (opReads_[cycle].size() == 1 &&
         !AssocTools::containsKey(opReads_[cycle], operand))) {
        return cycle;
    }

    if (!resourceUsage_[cycle].empty()) {
        return cycle;
    }

    int latency = this->latency();
    for (int i = cycle + 1; i < latency; i++) {
        if (!resourceUsage_[i].empty() || !opReads_[i].empty()) {
            return i;
        }
    }

    return -1;
}


/**
 * Returns the pipeline resources used by this pipeline.
 *
 * @return The pipeline resources used by this pipeline.
 */
ExecutionPipeline::ResourceSet
ExecutionPipeline::usedResources() const {

    ResourceSet resources;
    int latency = this->latency();

    for (int cycle = 0; cycle < latency; cycle++) {
        ResourceSet cycleResources = resourceUsage_[cycle];
        resources.insert(cycleResources.begin(), cycleResources.end());
    }

    return resources;
}


/**
 * Returns the pipeline element by the given name.
 *
 * @param name The name.
 * @return The pipeline element or NULL if there is no such pipeline element.
 */
PipelineElement*
ExecutionPipeline::pipelineElement(const std::string& name) const {
    FunctionUnit* fu = parent_->parentUnit();
    return fu->pipelineElement(name);
}


/**
 * Creates a pipeline element by the given name.
 *
 * @param name The name of the pipeline element.
 * @return The pipeline element that was created.
 */
PipelineElement*
ExecutionPipeline::addPipelineElement(const std::string& name) const {
    FunctionUnit* fu = parent_->parentUnit();
    if (!fu->hasPipelineElement(name)) {
        new PipelineElement(name, *fu);
    }
    return fu->pipelineElement(name);
}


/**
 * Saves usage of a pipeline element to ObjectState tree.
 *
 * @param element The pipelineElement used.
 * @param cycleToSave The cycle to save.
 * @param pipelineState ObjectState instance representing the state of the
 *                      pipeline.
 */
void
ExecutionPipeline::saveResourceUse(
    const PipelineElement* element,
    int cycleToSave,
    ObjectState* pipelineState) {

    string elementName = element->name();
    bool resourceFound = false;

    for (int i = 0; i < pipelineState->childCount(); i++) {
        ObjectState* child = pipelineState->child(i);
        if (child->name() == OSNAME_RESOURCE_USAGE &&
            child->stringAttribute(OSKEY_RESOURCE_NAME) == elementName) {

            int cycles = child->intAttribute(OSKEY_CYCLES);
            if (child->intAttribute(OSKEY_START_CYCLE) + cycles ==
                cycleToSave) {
                child->setAttribute(OSKEY_CYCLES, cycles + 1);
                resourceFound = true;
                break;
            }
        }
    }

    if (!resourceFound) {
        ObjectState* resource = new ObjectState(OSNAME_RESOURCE_USAGE);
        pipelineState->addChild(resource);
        resource->setAttribute(OSKEY_RESOURCE_NAME, elementName);
        resource->setAttribute(OSKEY_START_CYCLE, cycleToSave);
        resource->setAttribute(OSKEY_CYCLES, 1);
    }
}


/**
 * Saves use of an operand to ObjectState tree.
 *
 * @param operand The operand.
 * @param cycleToSave The cycle to save.
 * @param pipelineState ObjectState instance representing the state of the
 *                      pipeline.
 * @param osName Name of the ObjectState instance in which to save the
 *               operand use.
 */
void
ExecutionPipeline::saveOperandUse(
    int operand,
    int cycleToSave,
    ObjectState* pipelineState,
    const std::string& osName) {

    bool found = false;

    for (int i = 0; i < pipelineState->childCount(); i++) {
        ObjectState* child = pipelineState->child(i);
        if (child->name() == osName &&
            child->intAttribute(OSKEY_OPERAND) == operand) {

            int cycles = child->intAttribute(OSKEY_CYCLES);
            if (child->intAttribute(OSKEY_START_CYCLE) + cycles ==
                cycleToSave) {
                child->setAttribute(OSKEY_CYCLES, cycles + 1);
                found = true;
                break;
            }
        }
    }

    if (!found) {
        ObjectState* operandUsage = new ObjectState(osName);
        pipelineState->addChild(operandUsage);
        operandUsage->setAttribute(OSKEY_OPERAND, operand);
        operandUsage->setAttribute(OSKEY_START_CYCLE, cycleToSave);
        operandUsage->setAttribute(OSKEY_CYCLES, 1);
    }
}


/**
 * Sorts the resource and operand usages by the start cycle of the usage.
 *
 * @param pipelineState An ObjectState instance representing an execution
 *                      pipeline.
 * @return A vector of ObjectState instances each of them representing a
 *         resource or operand usage.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
ExecutionPipeline::ObjectStateTable
ExecutionPipeline::sortResourceUsages(
    const ObjectState* pipelineState) const
    throw (ObjectStateLoadingException) {

    ObjectStateTable usages;
    for (int i = 0; i < pipelineState->childCount(); i++) {
        ObjectState* usageState = pipelineState->child(i);
        addResourceUsage(usages, usageState);
    }

    return usages;
}


/**
 * Adds the given ObjectState instance representing a resource usage to the
 * given set of usages to correct position.
 *
 * The correct position is determined by the start cycle of the resource
 * usage.
 *
 * @param usages The set of resource usages where to add the given usage.
 * @param usageState The usageState to add.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
ExecutionPipeline::addResourceUsage(
    ObjectStateTable& usages,
    const ObjectState* usageState) const
    throw (ObjectStateLoadingException) {

    try {
        int startCycle = usageState->intAttribute(OSKEY_START_CYCLE);
        for (ObjectStateTable::iterator iter = usages.begin();
             iter != usages.end(); iter++) {
            const ObjectState* usage = *iter;
            int usageStart = usage->intAttribute(OSKEY_START_CYCLE);
            if (startCycle < usageStart) {
                usages.insert(iter, usageState);
                return;
            }
        }
    } catch (const Exception& exception) {
        string procName = "ExecutionPipeline::addResourceUsage";
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }

    usages.push_back(usageState);
}

}
