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
 * @file ExecutionTrace.hh
 *
 * Declaration of ExecutionTrace class.
 *
 * @author Pekka J‰‰skel‰inen 2004,2009 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_EXECUTION_TRACE_HH
#define TTA_EXECUTION_TRACE_HH
 
#include <string>
#include <vector>
#include <list>
#include <fstream>

#include "boost/tuple/tuple.hpp"

#include "SimValue.hh"
#include "FileSystem.hh"
#include "RelationalDB.hh"
#include "SimulatorConstants.hh"


class InstructionExecution;

/**
 * The main class of the Execution Trace Database (TraceDB).
 *
 * Access to the execution trace database happens through the interface of 
 * this class.
 */
class ExecutionTrace {
public:

    /// a type for storing address space identifiers
    typedef std::string AddressSpaceID;

    /// a type for storing bus identifiers
    typedef std::string BusID;

    /// a type for storing socket identifiers
    typedef std::string SocketID;

    /// a type for storing bus segment identifiers
    typedef std::string SegmentID;

    /// a type for storing function unit identifiers
    typedef std::string FunctionUnitID;

    /// a type for storing operation identifiers
    typedef std::string OperationID;

    /// a type for storing register file identifiers
    typedef std::string RegisterFileID;

    /// a type for storing register ids
    typedef int RegisterID;

    /// a type for storing memory addresses
    typedef unsigned int MemoryAddress;

    /// a type for storing data of size of the minimum addressable unit
    typedef SimValue MAU;

    /// a type for register access counts 
    typedef std::size_t RegisterAccessCount;

    /// a type for operation trigger counts 
    typedef ClockCycleCount OperationTriggerCount;

    /// a type for storing procedure entry type (entry/exit)
    enum ProcedureEntryType {
        PT_ENTRY = 0, ///< procedure entry
        PT_EXIT = 1  ///< procedure exit
    };

    /// type to be used as a key for storing concurrent RF access info
    typedef boost::tuple<
        RegisterAccessCount, /* concurrent reads */
        RegisterAccessCount, /* concurrent writes */ 
        ClockCycleCount> ConcurrentRFAccessCount;

    /// type to be used for a list of concurrent RF accesses
    typedef std::list<ConcurrentRFAccessCount> ConcurrentRFAccessCountList;

    /// type to be used as a key for storing function unit operation execution
    /// counts
    typedef boost::tuple<OperationID, OperationTriggerCount> 
    FUOperationTriggerCount;

    /// type to be used for lists of function operation execution counts
    typedef std::list<FUOperationTriggerCount> FUOperationTriggerCountList;

    void addInstructionExecution(
        ClockCycleCount cycle, 
        InstructionAddress address) 
        throw (IOException);

    void addInstructionExecutionCount(
        InstructionAddress address,
        ClockCycleCount count)
        throw (IOException);

    void addProcedureAddressRange(
        InstructionAddress firstAddress,
        InstructionAddress lastAddress,
        const std::string& procedureName)
        throw (IOException);

    void addLockedCycle(ClockCycleCount cycle);

    void addBasicBlockStart(ClockCycleCount cycle, InstructionAddress address);

    void addMemoryAccess(
        ClockCycleCount cycle, 
        const AddressSpaceID& addressSpace,
        MemoryAddress address,
        bool write,
        std::size_t size,
        const std::vector<MAU>* data = NULL);

    void addProcedureEntry(ClockCycleCount cycle, InstructionAddress address);
    
    void addProcedureExit(ClockCycleCount cycle, InstructionAddress address);
    
    void addBusActivity(
        ClockCycleCount cycle, 
        const BusID& busId,
        const SegmentID& segmentId,
        bool squash,
        const SimValue& data = NullSimValue::instance())
        throw (IOException);

    void addConcurrentRegisterFileAccessCount(
        RegisterFileID registerFile,
        RegisterAccessCount reads,
        RegisterAccessCount writes,
        ClockCycleCount count)
        throw (IOException);

    void addRegisterAccessCount(
        RegisterFileID registerFile,
        RegisterID registerIndex,
        ClockCycleCount reads,
        ClockCycleCount writes)
        throw (IOException);

    ConcurrentRFAccessCountList* registerFileAccessCounts(
        RegisterFileID registerFile) const
        throw (IOException);
    
    void addFunctionUnitOperationTriggerCount(
        FunctionUnitID functionUnit,
        OperationID operation,
        OperationTriggerCount count)
        throw (IOException);

    void addProcedureTransfer(
        ClockCycleCount cycle,
        InstructionAddress address,
        InstructionAddress sourceAddress,
        ProcedureEntryType type)
        throw (IOException);        

    FUOperationTriggerCountList* functionUnitOperationTriggerCounts(
        FunctionUnitID functionUnit) const
        throw (IOException);

    void addSocketWriteCount(SocketID socket, ClockCycleCount)
        throw (IOException);

    ClockCycleCount socketWriteCount(SocketID socket) const;

    void addBusWriteCount(BusID socket, ClockCycleCount count)
        throw (IOException);

    ClockCycleCount busWriteCount(BusID bus) const;
    
    void setSimulatedCycleCount(ClockCycleCount count)
        throw (IOException);

    ClockCycleCount simulatedCycleCount() const
        throw (IOException);
 
    InstructionExecution& instructionExecutions()
        throw (IOException);

    static ExecutionTrace* open(const std::string& fileName) 
        throw (IOException);    
    
    virtual ~ExecutionTrace();
    
protected:

    ExecutionTrace(const std::string& fileName, bool readOnly);
    void open() throw (RelationalDBException);

private:
    
    void initialize() throw (IOException);
    
    /// Filename of the trace database (sqlite file).
    const std::string& fileName_;
    /// The call trace file.
    std::fstream callTrace_;
    /// The instruction profile file.
    std::fstream instructionProfile_;
    /// Is the database access mode read-only?
    bool readOnly_;
    /// Handle to the sqlite trace database.
    RelationalDB* db_;
    /// Handle to the database connection;
    RelationalDBConnection* dbConnection_;
    /// Handle object for the queries of instruction executions.
    InstructionExecution* instructionExecution_;
    
};

#endif
