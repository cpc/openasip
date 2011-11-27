#include "StringTools.hh"

#include "ExecutionPipelineResourceTable.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "PipelineElement.hh"

namespace TTAMachine {
    class FUPort;
}

using TTAMachine::HWOperation;
using TTAMachine::ExecutionPipeline;
using TTAMachine::PipelineElement;


ExecutionPipelineResourceTable::ExecutionPipelineResourceTable(
    const TTAMachine::FunctionUnit& fu) : 
    name_(fu.name()), 
    numberOfResources_(fu.pipelineElementCount() + fu.operationPortCount()), 
    maximalLatency_(fu.maxLatency()) {
    
    for (int j = 0; j < fu.operationCount(); j++) {
        HWOperation& hwop = *fu.operation(j);
        ExecutionPipeline* ep = hwop.pipeline();
        std::string opName = StringTools::stringToUpper(
            fu.operation(j)->name());

        for (int l = 0; l < ep->latency(); l++ ) {
            for (int k = 0; k < fu.pipelineElementCount(); k++) {
                PipelineElement *pe = fu.pipelineElement(k);
                if (ep->isResourceUsed(pe->name(),l)){
                    setResourceUse(opName, l, k);
                }
            }
            for (int k = 0; k < fu.operationPortCount(); k++) {
                TTAMachine::FUPort* fuPort = fu.operationPort(k);
                if (ep->isPortWritten(*fuPort,l)) {
                    setResourceUse(
                        opName, l,fu.pipelineElementCount()+k);
                }
                if (ep->isPortRead(*fuPort,l)) {
                    setResourceUse(
                        opName, -1, fu.pipelineElementCount()+k);
                }
            }
        }
        
        // set operation latencies
        ExecutionPipeline::OperandSet writes = ep->writtenOperands();
        for (ExecutionPipeline::OperandSet::iterator iter =
                 writes.begin(); iter != writes.end(); iter++) {
            int index = *iter;
            int latency = hwop.latency(index);
            setLatency(opName, index, latency);
        }
    }
}

/**
 * Sets usage of resource/port to true for given cycle and resource number
 * and particular pipeline.
 *
 * @param opName Name of operation to set resource for
 * @param cycle Cycle in which to set usage
 * @param index Index of resource/port in resource vector
 */
void
ExecutionPipelineResourceTable::setResourceUse(
    const std::string& opName,
    const int cycle,
    const int resIndex) {

    if (cycle > (signed)maximalLatency_) {
        throw InvalidData(__FILE__, __LINE__, __func__,
            "Trying to set resource use to cycle out of scope of "
            "FU pipeline!");
    }
    if (resIndex >= numberOfResources_ || resIndex < 0){
        throw InvalidData(__FILE__, __LINE__, __func__,
            "Trying to set resource use for resource out of scope of "
            "FU pipeline!");
    }

    if (!MapTools::containsKey(operationSupported_, opName)) {
        ResourceTable newOp(
            maximalLatency_, std::vector<bool>(numberOfResources_, false));
        operationPipelines_.push_back(newOp);
        operationSupported_[opName] = operationPipelines_.size() - 1;
    }
    int pIndex = MapTools::valueForKey<int>(operationSupported_, opName);
    if(cycle > -1) {
        operationPipelines_[pIndex][cycle][resIndex] = true;
    }
}

/**
 * Sets latency of an output of an operation.
 * The resource usage of the operation has to be set before calling this.
 * @param opName operation to set the latency
 * @param output index of the output operand(stating from
 *               numberofinputoperand, not 0/1)
 * @param latency latency of the output of the operation
 */
void
ExecutionPipelineResourceTable::setLatency(
    const std::string& opName,
    const int output,
    const int latency) {

    if (!MapTools::containsKey(operationSupported_, opName)) {
        ResourceTable newOp(maximalLatency_, std::vector<bool>(0, false));
        operationPipelines_.push_back(newOp);
        operationSupported_[opName] = operationPipelines_.size() - 1;
    }

    int pIndex = MapTools::valueForKey<int>(operationSupported_, opName);
    while (static_cast<int>(operationLatencies_.size()) <= pIndex) {
        operationLatencies_.push_back(std::map<int,int>());
    }
    operationLatencies_[pIndex][output] = latency;
}

/**
 * Gives an resource table for given FU. 
 * If no existing found, creates a new one.
 * 
 * @param fu function unit whose resource table we are asking for.
 */
const ExecutionPipelineResourceTable& 
ExecutionPipelineResourceTable::resourceTable(
    const TTAMachine::FunctionUnit& fu) {
    
    ResourceTableMap::iterator i = allResourceTables_.find(&fu);

    if (i != allResourceTables_.end()) {
        return *i->second;
    } 

    ExecutionPipelineResourceTable* newTable = 
        new ExecutionPipelineResourceTable(fu);
    allResourceTables_[&fu] = newTable;
    return *newTable;
}

/**
 * Delete all the resource tables.
 * This can be called after scheduling is done, 
 * when resource managers are longer used.
 */
void
ExecutionPipelineResourceTable::finalize() {
    MapTools::deleteAllValues(allResourceTables_);
}

ExecutionPipelineResourceTable::ResourceTableMap 
ExecutionPipelineResourceTable::allResourceTables_;
