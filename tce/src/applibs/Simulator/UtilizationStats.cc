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
 * @file UtilizationStats.hh
 *
 * Implementation of UtilizationStats class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */
#include "UtilizationStats.hh"
#include "MapTools.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "ExecutableInstruction.hh"
#include "Terminal.hh"
#include "FunctionUnit.hh"
#include "Bus.hh"
#include "Socket.hh"
#include "Operation.hh"
#include "StringTools.hh"
#include "BaseFUPort.hh"
#include "FUPort.hh"
#include "Guard.hh"
#include "MoveGuard.hh"
#include "hash_set.hh"
#include "Application.hh"
#include "TCEString.hh"

/**
 * Constructor.
 */
UtilizationStats::UtilizationStats() : highestRegister_(-1) {
}

/**
 * Destructor.
 */
UtilizationStats::~UtilizationStats() {
}

/**
 * Accumulates the utilization counts for each machine part used by the
 * instruction.
 *
 * @note This function assumes that moves in Instruction and 
 * ExecutableInstruction are in same order.
 *
 * @param instructionData The instruction.
 * @param executionCounts Execution counts of the instruction.
 */
void 
UtilizationStats::calculateForInstruction(
    const TTAProgram::Instruction& instructionData, 
    const ExecutableInstruction& executionCounts) {

    // used to make sure output socket utilizations are computed only
    // maximum once per instruction, even though the socket is read by
    // multiple buses
    hash_set<const char*> alreadyRegisteredOutputSockets;

    for (int i = 0; i < instructionData.moveCount(); ++i) {
        const TTAProgram::Move& move = instructionData.move(i);
        const ClockCycleCount execCount = 
            executionCounts.moveExecutionCount(i);

        // it depends on GCU implementation whether immediate jumps are
        // redirected through bus and sockets or not, let's assume they 
        // are, as they are in current CU implementations, according to Teemu
        buses_[move.bus().name()] += execCount;

        // socket utilizations
        if (!move.source().isImmediate() && 
            move.sourceSocket().name() != move.destinationSocket().name() &&
            alreadyRegisteredOutputSockets.find(
                move.sourceSocket().name().c_str()) ==
            alreadyRegisteredOutputSockets.end()) {

            sockets_[move.sourceSocket().name()] += execCount;
            alreadyRegisteredOutputSockets.insert(
                move.sourceSocket().name().c_str());
        }
        sockets_[move.destinationSocket().name()] += execCount;

        // operation utilizations
        if (move.destination().isFUPort() &&
            dynamic_cast<const TTAMachine::BaseFUPort&>(
                move.destination().port()).isTriggering()) {
            const std::string operationUpper =
                StringTools::stringToUpper(
                    move.destination().operation().name());
            const std::string fuName =
                move.destination().functionUnit().name();
            fus_[fuName] += execCount;
            operations_[operationUpper] += execCount;
            fuOperations_[fuName][operationUpper] += execCount;
        }

        // register reads 
        if (move.source().isGPR()) {
            const std::string rfName = move.source().registerFile().name();
            const int regIndex = move.source().index();
            if (regIndex > highestRegister_) {
                highestRegister_ = regIndex;
            }
            rfAccesses_[rfName][regIndex].first = 
                registerReads(rfName, regIndex) + execCount;
        }
        
        // guarded moves
        if (!move.isUnconditional()) {
            // RF reads
            if (dynamic_cast<const TTAMachine::RegisterGuard*>(
                    &move.guard().guard())) {
                const TTAMachine::RegisterGuard& moveGuard =
                    dynamic_cast<const TTAMachine::RegisterGuard&>(
                        move.guard().guard());
            
                const std::string rfName = moveGuard.registerFile()->name();
                const int regIndex = moveGuard.registerIndex();
                if (regIndex > highestRegister_)
                    highestRegister_ = regIndex;
            
                guardRfAccesses_[rfName][regIndex].first =
                    guardRegisterReads(rfName, regIndex) + execCount;
            } else { // FU Port reads
                if (dynamic_cast<const TTAMachine::PortGuard*>(
                        &move.guard().guard())) {
                    const TTAMachine::PortGuard& moveGuard =
                    dynamic_cast<const TTAMachine::PortGuard&>(
                        move.guard().guard());
                    
                    const TTAMachine::FUPort& port = *moveGuard.port();
                    const std::string fuName = port.parentUnit()->name();
                    guardFUAccesses_[fuName][port.name()] += execCount;
                }
            }
        }
        
        // immediate register reads
        if (move.source().isImmediateRegister()) {
            const std::string iuName = move.source().immediateUnit().name();
            const int regIndex = move.source().index();
            rfAccesses_[iuName][regIndex].first = 
                registerReads(iuName, regIndex) + execCount;
            if (regIndex > highestRegister_)
                highestRegister_ = regIndex;
        } 

        // register writes
        if (move.destination().isGPR()) {
            const std::string rfName = 
                move.destination().registerFile().name();
            const int regIndex = move.destination().index();
            if (regIndex > highestRegister_) {
                highestRegister_ = regIndex;
            }
            rfAccesses_[rfName][regIndex].second =
                registerWrites(rfName, regIndex) + execCount;
        }
    }
}

/**
 * Returns the count of writes to the given bus.
 *
 * @param busName The name of the bus.
 * @return The count of writes.
 */
ClockCycleCount 
UtilizationStats::busWrites(const std::string& busName) const {
    if (MapTools::containsKey(buses_, busName)) 
        return MapTools::valueForKey<ClockCycleCount>(buses_, busName);
    else 
        return 0;
}

/**
 * Returns the count of writes to the given socket.
 *
 * @param socketName The name of the socket.
 * @return The count of writes.
 */
ClockCycleCount 
UtilizationStats::socketWrites(const std::string& socketName) const {
    if (MapTools::containsKey(sockets_, socketName)) 
        return MapTools::valueForKey<ClockCycleCount>(sockets_, socketName);
    else 
        return 0;
}

/**
 * Returns the count of operation triggers in given FU.
 *
 * @param fuName The name of the FU.
 * @return The count of triggers.
 */
ClockCycleCount 
UtilizationStats::triggerCount(const std::string& fuName) const {
    if (MapTools::containsKey(fus_, fuName)) 
        return MapTools::valueForKey<ClockCycleCount>(fus_, fuName);
    else 
        return 0;
}

/**
 * Returns the total count of operation executions.
 *
 * @param operationName The name of the operation.
 * @return The count of executions.
 */
ClockCycleCount 
UtilizationStats::operationExecutions(
    const std::string& operationName) const {    
    if (MapTools::containsKey(operations_, operationName)) 
        return MapTools::valueForKey<ClockCycleCount>(
            operations_, operationName);
    else 
        return 0;
}

/**
 * Returns the total count of given operation executions in given FU.
 *
 * @param fuName The name of the function unit.
 * @param operationName The name of the operation.
 * @return The count of executions.
 */
ClockCycleCount 
UtilizationStats::operationExecutions(
    const std::string& fuName, const std::string& operationName) const {
    try {
        return MapTools::valueForKey<ClockCycleCount>(
            MapTools::valueForKey<ComponentUtilizationIndex>(
                fuOperations_, fuName), operationName);
    } catch (const KeyNotFound&) {
        return 0;
    }
}

/**
 * Returns the count of times the given register was read during simulation.
 *
 * @param rfName The name of the register file.
 * @param registerIndex The index of the register.
 * @return The count of reads.
 */
ClockCycleCount 
UtilizationStats::registerReads(
    const std::string& rfName, 
    int registerIndex) const {
    try {
        return 
            MapTools::valueForKey<
            std::pair<ClockCycleCount, ClockCycleCount> >(
                MapTools::valueForKey<RegisterUtilizationIndex>(
                    rfAccesses_, rfName), registerIndex).first;
    } catch (const KeyNotFound&) {
        return 0;
    }
}

/**
 * Returns the count of times the guarded register was read during simulation.
 *
 * @param rfName The name of the register file.
 * @param registerIndex The index of the register.
 * @return The count of guarded reads.
 */
ClockCycleCount 
UtilizationStats::guardRegisterReads(
    const std::string& rfName, 
    int registerIndex) const {
    try {
        return 
            MapTools::valueForKey<
            std::pair<ClockCycleCount, ClockCycleCount> >(
                MapTools::valueForKey<RegisterUtilizationIndex>(
                    guardRfAccesses_, rfName), registerIndex).first;
    } catch (const KeyNotFound&) {
        return 0;
    }
}

/**
 * Returns the count of times the given register was written during simulation.
 *
 * @param rfName The name of the register file.
 * @param registerIndex The index of the register.
 * @return The count of writes.
 */
ClockCycleCount 
UtilizationStats::registerWrites(
    const std::string& rfName, 
    int registerIndex) const {
    try {
        return 
            MapTools::valueForKey<
            std::pair<ClockCycleCount, ClockCycleCount> >(
                MapTools::valueForKey<RegisterUtilizationIndex>(
                    rfAccesses_, rfName), registerIndex).second;
    } catch (const KeyNotFound&) {
        return 0;
    }
}

/**
 * Returns number of reads for a single FU port guard
 * 
 * @param fuName Name of the FU
 * @param fuPort Name of the FU port
 * @return Number of reads for a single FU port guard
 */
ClockCycleCount 
UtilizationStats::FUGuardAccesses(
    const std::string& fuName,
    const std::string& fuPort) const {
        
    try {
        return MapTools::valueForKey<ClockCycleCount>(
            MapTools::valueForKey<ComponentUtilizationIndex>(
                guardFUAccesses_, fuName), fuPort);
    } catch (const KeyNotFound&) {
        return 0;
    }
}

/**
 * Returns a map containing the FU port guard accesses
 * 
 * @return A map containing the FU port guard accesses
 */
UtilizationStats::FUOperationUtilizationIndex
UtilizationStats::FUGuardAccesses() const {
    return guardFUAccesses_;    
}

/**
 * Returns the highest used register index.
 *
 * This is useful when fetching register access data for sequential simulation.
 *
 * @return The index of the highest index register.
 */
int 
UtilizationStats::highestUsedRegisterIndex() const {
    return highestRegister_;
}
