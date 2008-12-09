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
 * @file UtilizationStats.hh
 *
 * Declaration of UtilizationStats class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_UTILIZATION_STATS_HH
#define TTA_UTILIZATION_STATS_HH

#include <map>
#include <string>

#include "SimulationStatisticsCalculator.hh"
#include "SimulatorConstants.hh"

namespace TTAMachine {
    class Guard;
}

/**
 * Calculates processor utilization data from instructions and their
 * execution counts.
 */
class UtilizationStats : public SimulationStatisticsCalculator {
public:
    /// Index for connecting component names to utilization counts.
    typedef std::map<std::string, ClockCycleCount> ComponentUtilizationIndex;
    /// Index for connecting register indices to utilization counts.
    /// The first number in the pair is the count of reads, and the second
    /// is the count of writes.
    typedef std::map<int, std::pair<ClockCycleCount, ClockCycleCount> > 
    RegisterUtilizationIndex;
    /// Index for connecting function unit and operations implemented in them
    /// to utilization counts.
    typedef std::map<std::string, ComponentUtilizationIndex>
    FUOperationUtilizationIndex;    
    /// Index for connecting register files and register utilization indices
    /// in them.
    typedef std::map<std::string, RegisterUtilizationIndex>
    RFRegisterUtilizationIndex;
    
    UtilizationStats();
    virtual ~UtilizationStats();

    virtual void calculate(
        const TTAProgram::Instruction& instructionData, 
        const ExecutableInstruction& executionCounts);

    ClockCycleCount busWrites(const std::string& busName) const;
    ClockCycleCount socketWrites(const std::string& socketName) const;
    ClockCycleCount triggerCount(const std::string& fuName) const;
    ClockCycleCount operationExecutions(
        const std::string& operationName) const;
    ClockCycleCount operationExecutions(
        const std::string& fuName, const std::string& operationName) const;
    ClockCycleCount registerReads(
        const std::string& rfName, 
        int registerIndex) const;
    ClockCycleCount guardRegisterReads(
        const std::string& rfName,
        int registerIndex) const;
    ClockCycleCount registerWrites(
        const std::string& rfName, 
        int registerIndex) const;
    ClockCycleCount FUGuardAccesses(
        const std::string& fuName,
        const std::string& fuPort) const;
    
    FUOperationUtilizationIndex FUGuardAccesses() const;
    
    int highestUsedRegisterIndex() const;

private:
    /// Socket write counts.
    ComponentUtilizationIndex sockets_;
    /// Bus write counts.
    ComponentUtilizationIndex buses_;
    /// Function unit utilizations, i.e., total operation triggerings.
    ComponentUtilizationIndex fus_;
    /// Operation utilizations (started operations).
    ComponentUtilizationIndex operations_;
    /// Index for operation utilizations for each function unit.
    FUOperationUtilizationIndex fuOperations_;
    /// Register read and write data for each register in each register file.
    RFRegisterUtilizationIndex rfAccesses_;
    
    /// Guard register accesses for each register in a RF.
    RFRegisterUtilizationIndex guardRfAccesses_;
    /// Guard FU port accesses
    FUOperationUtilizationIndex guardFUAccesses_;
    
    /// The highest register index used. This is an uglish way to fetch
    /// register access info for sequential simulation.
    int highestRegister_;
};

#endif
