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
 * @file FUArchitecture.cc
 *
 * Implementation of FUArchitecture class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "FUArchitecture.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "AssocTools.hh"
#include "MapTools.hh"
#include "FUPort.hh"
#include "PipelineElement.hh"

using namespace TTAMachine;
using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * @param fu The function unit of which architecture is represented. Becomes
 * property of the FUArchitecture object.
 */
FUArchitecture::FUArchitecture(TTAMachine::FunctionUnit* fu) : fu_(fu) {
}

/**
 * Copy constructor.
 *
 * @param original FUArchitecture to copy.
 */
FUArchitecture::FUArchitecture(const FUArchitecture& original) :
    HWBlockArchitecture(original) {

    fu_ = original.fu_->copy();
    parameterizedPorts_ = original.parameterizedPorts_;
    guardedPorts_ = original.guardedPorts_;
}

/**
 * The destructor.
 */
FUArchitecture::~FUArchitecture() {
    delete fu_;
}


/**
 * Tells whether the given port has parameterized width.
 *
 * @param port Name of the port.
 * @return True if the port is parameterized, otherwise false.
 */
bool
FUArchitecture::hasParameterizedWidth(const std::string& port) const {
    return AssocTools::containsKey(parameterizedPorts_, port);
}


/**
 * Sets parameterized width for the given port.
 *
 * @param port Name of the port.
 */
void
FUArchitecture::setParameterizedWidth(const std::string& port) {
    parameterizedPorts_.insert(port);
}


/**
 * Tells whether the given port is guarded.
 *
 * @param port Name of the port.
 * @return True if the port is guarded, otherwise false.
 */
bool
FUArchitecture::hasGuardSupport(const std::string& port) const {
    return AssocTools::containsKey(guardedPorts_, port);
}


/**
 * Sets guard support for the given port.
 *
 * @param port Name of the port.
 */
void
FUArchitecture::setGuardSupport(const std::string& port) {
    guardedPorts_.insert(port);
}


/**
 * Returns the FunctionUnit instance that represents the architecture.
 *
 * @return The FunctionUnit instance.
 */
TTAMachine::FunctionUnit&
FUArchitecture::architecture() const {
    return *fu_;
}


/**
 * Tells the direction of the given port.
 *
 * @param portName Name of the port in the FU architecture.
 * @exception InstanceNotFound If the FU architecture does not have the given
 *                             port.
 * @exception InvalidData If the given port is not used by any pipeline.
 */
HDB::Direction
FUArchitecture::portDirection(const std::string& portName) const
    throw (InstanceNotFound, InvalidData) {

    FunctionUnit& fu = architecture();
    if (!fu.hasOperationPort(portName)) {
        throw InstanceNotFound(__FILE__, __LINE__, __func__);
    }

    FUPort* port = fu.operationPort(portName);
    bool read = false;
    bool written = false;
    
    int operationCount = fu.operationCount();
    for (int i = 0; i < operationCount; i++) {
        HWOperation* operation = fu.operation(i);
        ExecutionPipeline* pLine = operation->pipeline();
        int latency = operation->latency();
        for (int cycle = 0; cycle < latency; cycle++) {
            if (!read) {
                if (pLine->isPortRead(*port, cycle)) {
                    read = true;
                }
            }
            if (!written) {
                if (pLine->isPortWritten(*port, cycle)) {
                    written = true;
                }
            }
        }
        if (read && written) {
            break;
        }
    }

    if (read && written) {
        return HDB::BIDIR;
    } else if (read) {
        return HDB::IN;
    } else if (written) {
        return HDB::OUT;
    } else {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }           
}


/**
 * Checks whether the given FU has a mathing architecture with the given FU 
 * architecture instance.
 *
 * @param rightHand Right hand operand.
 * @return True if the architectures match, otherwise false.
 */
bool
FUArchitecture::operator==(const FUArchitecture& rightHand) const {

    if (rightHand.architecture().operationCount() != 
        this->architecture().operationCount()) {
        return false;
    }
    
    std::map<const FUPort*, const FUPort*> portMap;
    for (int i = 0; i < rightHand.architecture().operationPortCount(); i++) {
        portMap.insert(
            std::pair<const FUPort*, const FUPort*>(
                rightHand.architecture().operationPort(i), NULL));
    }
    
    PipelineElementUsageTable plineElementUsages;

    for (int i = 0; i < rightHand.architecture().operationCount(); i++) {
        HWOperation* rightHandOp = rightHand.architecture().operation(i);
        if (!architecture().hasOperation(rightHandOp->name())) {
            return false;
        }
        HWOperation* thisOp = architecture().operation(
            rightHandOp->name());
        if (rightHandOp->latency() != thisOp->latency()) {
            return false;
        }
        
        // check operand bindings
        for (int i = 0;
             i < rightHand.architecture().operationPortCount();
             i++) {
            
            FUPort* port = rightHand.architecture().operationPort(i);
            if (rightHandOp->isBound(*port)) {
                int io = rightHandOp->io(*port);
                FUPort* samePort = thisOp->port(io);
                if (samePort == NULL) {
                    return false;
                }
                const FUPort* existingSamePort =
                    MapTools::valueForKey<const FUPort*>(portMap, port);
                if (existingSamePort != NULL &&
                    existingSamePort != samePort) {
                    return false;
                }
                
                // check the width of the ports
                // widths must equal
                if (hasParameterizedWidth(samePort->name()) !=
                    rightHand.hasParameterizedWidth(port->name())) {
                    return false;
                }

                // if the FUArchitectures have parameterized port width
                // those ports widths are not needed to check
                if (!hasParameterizedWidth(samePort->name()) &&
                    samePort->width() != port->width()) {
                    return false;
                }
                
                if (port->isOpcodeSetting() != samePort->isOpcodeSetting() ||
                    port->isTriggering() != samePort->isTriggering()) {
                    return false;
                }
                portMap.erase(port);
                portMap.insert(
                    std::pair<const FUPort*, const FUPort*>(port, samePort));
            }
        }
        
        // check operation pipeline
        ExecutionPipeline* opPipeline =  rightHandOp->pipeline();
        ExecutionPipeline* thisOpPipeline = thisOp->pipeline();
        for (int cycle = 0; cycle < rightHandOp->latency(); cycle++) {
            ExecutionPipeline::OperandSet written1 = 
                opPipeline->writtenOperands(cycle);
            ExecutionPipeline::OperandSet written2 = 
                thisOpPipeline->writtenOperands(cycle);
            if (written1 != written2) {
                return false;
            }
            ExecutionPipeline::OperandSet read1 = 
                opPipeline->readOperands(cycle);
            ExecutionPipeline::OperandSet read2 = 
                thisOpPipeline->readOperands(cycle);
            if (read1 != read2) {
                return false;
            }
            
            PipelineElementUsage usage;
            for (int i = 0;
                 i < rightHand.architecture().pipelineElementCount();
                 i++) {
                
                const PipelineElement* elem =
                    rightHand.architecture().pipelineElement(i);
                if (opPipeline->isResourceUsed(elem->name(),cycle)) {
                    usage.usage1.insert(elem);
                }
            }
            
            for (int i = 0; i < architecture().pipelineElementCount(); i++) {
                const PipelineElement* elem = 
                    architecture().pipelineElement(i);
                if (thisOpPipeline->isResourceUsed(elem->name(), cycle)) {
                    usage.usage2.insert(elem);
                }
            }
            plineElementUsages.push_back(usage);
        }
    }

    std::set<const TTAMachine::PipelineElement*> difference;
    for (size_t i = 0; i < plineElementUsages.size(); i++) {
        AssocTools::difference(plineElementUsages[i].usage1,
                               plineElementUsages[i].usage2,
                               difference);
    }
    if (!difference.empty()) {
        return false;
    }
    return true;
}
}
