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
 * @file CompiledSimulation.cc
 *
 * Definition of CompiledSimulation class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2009 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include "CompiledSimulation.hh"
#include "Machine.hh"
#include "Instruction.hh"
#include "SimulatorFrontend.hh"
#include "CompiledSimController.hh"
#include "SimulationEventHandler.hh"
#include "CompiledSimSymbolGenerator.hh"
#include "DirectAccessMemory.hh"
#include "CompiledSimulationPimpl.hh"
#include "ControlUnit.hh"
#include "CompiledSimCodeGenerator.hh"
#include "CompiledSimCompiler.hh"
#include "PluginTools.hh"
#include "FileSystem.hh"
#include "Program.hh"
#include "Move.hh"
#include "MemorySystem.hh"
#include "Conversion.hh"

using namespace TTAMachine;
using namespace TTAProgram;

static const ClockCycleCount MAX_CYCLES =
    std::numeric_limits<ClockCycleCount>::max();

/**
 * The constructor
 * 
 * Grabs all shared data from machine and program and saves them for
 * easy access for later usage.
 * 
 * @param machine The simulated machine
 * @param program The simulated program
 * @param frontend The simulation frontend
 * @param memorySystem The memory system
 * 
 */
CompiledSimulation::CompiledSimulation(
    const TTAMachine::Machine& machine, 
    InstructionAddress entryAddress,
    InstructionAddress lastInstruction,
    SimulatorFrontend& frontend,
    CompiledSimController& controller,
    MemorySystem& memorySystem,
    bool dynamicCompilation,
    ProcedureBBRelations& procedureBBRelations) :
    cycleCount_(0),
    basicBlockCount_(0),
    jumpTarget_(entryAddress),
    programCounter_(entryAddress),
    lastExecutedInstruction_(0),
    cyclesToSimulate_(MAX_CYCLES),                  
    stopRequested_(false),
    isFinished_(false),
    conflictDetected_(false),
    dynamicCompilation_(dynamicCompilation),
    procedureBBRelations_(procedureBBRelations),
    machine_(machine),
    entryAddress_(entryAddress),
    lastInstruction_(lastInstruction), pimpl_(new CompiledSimulationPimpl()) {
    pimpl_->memorySystem_ = &memorySystem;
    pimpl_->frontend_ = &frontend;
    pimpl_->controller_ = &controller;
    
    // Allocate memory for calculating move and basic block execution counts
    int moveCount = pimpl_->controller_->program().moveCount();
    moveExecCounts_ = new ClockCycleCount[moveCount];
    for (int i = 0; i < moveCount; ++i) {
        moveExecCounts_[i] = 0;
    }
    
    int bbCount = lastInstruction - entryAddress + 1;
    bbExecCounts_ = new ClockCycleCount[bbCount];
    for (int i = 0; i < bbCount; ++i) {
        bbExecCounts_[i] = 0;
    }
    
    // Find program exit points
    pimpl_->exitPoints_ = pimpl_->controller_->findProgramExitPoints(
        pimpl_->controller_->program(), machine_);
}

/**
 * The destructor. Frees private implementation
 */
CompiledSimulation::~CompiledSimulation() {
    delete[] bbExecCounts_;
    bbExecCounts_ = NULL;
    
    delete[] moveExecCounts_;
    moveExecCounts_ = NULL;
    
    delete pimpl_;
    pimpl_ = NULL;
    
}

/**
 * Lets the simulator frontend handle a single cycle end.
 * 
 * Used for example when generating traces.
 */
void 
CompiledSimulation::cycleEnd() {
    lastExecutedInstruction_ = programCounter_;
    programCounter_++;
    pimpl_->frontend_->eventHandler().handleEvent(
        SimulationEventHandler::SE_CYCLE_END);
}

/**
 * Advance the simulation by a given amout of cycles. 
 * 
 * @note Advances only at an accuracy of a one basic block!
 *
 * @param count The number of cycles the simulation should be advanced at least
 * @exception SimulationExecutionError If a runtime error occurs in
 *                                     the simulated program.
 */
void
CompiledSimulation::step(double count) {
    cyclesToSimulate_ = cycleCount_ + static_cast<ClockCycleCount>(count);
    stopRequested_ = false;
    
    while (!stopRequested_ && !isFinished_) {
        simulateCycle();
    }
}

/**
 * Throws an exception since this feature is not supported yet!
 * 
 * @exception SimulationExecutionError always thrown
 */
void
CompiledSimulation::next(int count) {
    
    std::string msg(
        "Command nexti not yet supported in the compiled simulation!");
    throw SimulationExecutionError(__FILE__, __LINE__, __FUNCTION__, msg);
    
    if (count) {}
}

/**
 * Runs the simulation until it is finished or an exception occurs
 * 
 * @note Advances only at an accuracy of a one basic block!
 *
 * @param address An address the simulation is allowed to stop after
 * @exception SimulationExecutionError If a runtime error occurs in
 *                                     the simulated program.
 */
void
CompiledSimulation::run() {
    cyclesToSimulate_ = MAX_CYCLES;
    stopRequested_ = false;
    while (!isFinished_ && !stopRequested_) {
        simulateCycle();
    }
}

/**
 * Advance the simulation until a given address is reached
 * 
 * @note Advances only at an accuracy of a one basic block!
 *
 * @param address An address the simulation is allowed to stop after
 * @exception SimulationExecutionError If a runtime error occurs in
 *                                     the simulated program.
 */
void
CompiledSimulation::runUntil(UIntWord address) {
    cyclesToSimulate_ = MAX_CYCLES;
    stopRequested_ = false;
    while ((!stopRequested_ && !isFinished_ &&
        (jumpTarget_ != address || cycleCount_ == 0))) {
        simulateCycle();
    }
}

/**
 * Returns the value of the current PC
 *
 * @return the value of the current PC
 * 
 */
InstructionAddress 
CompiledSimulation::programCounter() const {
    return programCounter_;
}

/**
 * Returns address of the last executed instruction
 * 
 * @return Address of the last executed instruction
 */
InstructionAddress 
CompiledSimulation::lastExecutedInstruction() const {
    return lastExecutedInstruction_;    
}

/**
 * Returns the current cycle count
 *
 * @return the current cycle count
 */
ClockCycleCount CompiledSimulation::cycleCount() const {
    return cycleCount_;
}

/**
 * Returns the value of the selected register
 * 
 * @param rfName The name of the register file
 * @param registerIndex index of the register in the RF
 * @return A SimValue containing the present register value
 * @exception InstanceNotFound If the RF cannot be found
 */
SimValue 
CompiledSimulation::registerFileValue(const char* rfName, int registerIndex) {
      
    CompiledSimSymbolGenerator symbolGen;
    RegisterFile& rf = *machine_.registerFileNavigator().item(rfName);
    std::string registerFile = symbolGen.registerSymbol(rf, registerIndex);
    
    CompiledSimulationPimpl::Symbols::const_iterator rfIterator = 
        pimpl_->symbols_.find(registerFile);
    if (rfIterator != pimpl_->symbols_.end()) {
        return *(rfIterator->second);
    } else {
        throw InstanceNotFound(__FILE__, __LINE__, __func__, 
            "Register file " + std::string(rfName) + " not found.");
    }
}

/**
 * Returns the value of the selected immediate unit register
 * 
 * @param iuName The name of the immediate unit
 * @param index Index of the immediate register
 * @return A SimValue containing the present immediate register value
 * @exception InstanceNotFound If the immediate unit cannot be found
 */
SimValue 
CompiledSimulation::immediateUnitRegisterValue(const char* iuName, int index) {  
    
    CompiledSimSymbolGenerator symbolGen;
    ImmediateUnit& iu = *machine_.immediateUnitNavigator().item(iuName);
    std::string immediateUnit = symbolGen.immediateRegisterSymbol(
        iu, index);
    
    CompiledSimulationPimpl::Symbols::const_iterator iuIterator =
        pimpl_->symbols_.find(immediateUnit);
    if (iuIterator != pimpl_->symbols_.end()) {
        return *(iuIterator->second);
    } else {
        throw InstanceNotFound(__FILE__, __LINE__, __func__, 
            "Immediate unit " + std::string(iuName) + " not found.");
    }
}

/**
 * Returns the value of the given FU port
 * 
 * @param fuName Name of the FU
 * @param portIndex index of the port in the FU
 * @return A SimValue containing the port's present value
 * @exception InstanceNotFound If the FU port cannot be found
 */
SimValue 
CompiledSimulation::FUPortValue(const char* fuName, const char* portName) {
    
    CompiledSimSymbolGenerator symbolGen;
    FunctionUnit* fu = NULL;
    try {
        fu = &functionUnit(fuName);
    } catch(InstanceNotFound& e) {
        fu = machine_.controlUnit();
    }
    
    std::string fuPort = symbolGen.portSymbol(*fu->port(portName));
    
    CompiledSimulationPimpl::Symbols::const_iterator fuPortIterator = 
        pimpl_->symbols_.find(fuPort);
    if (fuPortIterator != pimpl_->symbols_.end()) {
        return *(fuPortIterator->second);
    } else {
        throw InstanceNotFound(__FILE__, __LINE__, __func__, 
            "FU port " + std::string(fuPort) + " not found.");
    }
}

/**
 * Sets the simulation to be requested to stop
 */
void
CompiledSimulation::requestToStop() {
    stopRequested_ = true;
}

/** 
 * Returns true if the simulation is requested to stop
 * 
 * This can be either because the simulation has finished or the 
 * requested amount of cycles has been simulated.
 * 
 * @return true if the simulation should stop
 */
bool 
CompiledSimulation::stopRequested() const {
    return stopRequested_;
}

/** Returns true if the simulation is finished
 * 
 * @return true if the simulation is finished
 */
bool 
CompiledSimulation::isFinished() const {
    return isFinished_;
}

/**
 * Returns move execution count for move #moveNumber.
 *
 * @param moveNumber move number as in POM 
 * @return move execution count
 */
ClockCycleCount
CompiledSimulation::moveExecutionCount(
    int moveNumber,
    InstructionAddress address) const {
    const Program& program = pimpl_->controller_->program();
    InstructionAddress programStartAddress = program.startAddress().location();
    const Move& move = program.moveAt(moveNumber);
    
    if (move.isUnconditional() && pimpl_->exitPoints_.find(address) == 
        pimpl_->exitPoints_.end()) {
        // Grab the whole basic block execution count
        InstructionAddress bbStart = basicBlockStart(address - 
            programStartAddress);
        return bbExecCounts_[bbStart];
    } else { // guarded move or an exit point, grab single move execution count
        return moveExecCounts_[moveNumber];
    }
}

/**
 * Returns start of the basic block for given address of a basic block
 * @param address address of a basic block
 * @return start address of the basic block
 */
InstructionAddress 
CompiledSimulation::basicBlockStart(InstructionAddress address) const {
    return pimpl_->controller_->basicBlockStart(address);
}

/**
 * Returns a function unit of the given name
 * 
 * @param name name of the function unit to return
 * @return function unit of given name
 * @exception InstanceNotFound If a function unit is not found
 */
TTAMachine::FunctionUnit& 
CompiledSimulation::functionUnit(const char* name) const {
    return *machine_.functionUnitNavigator().item(name);
}

/**
 * Returns a memory object of the given function unit
 * 
 * @param FUName name of the function unit
 * @return memory object of the given function unit
 * @exception InstanceNotFound If an item is not found
 */
DirectAccessMemory& 
CompiledSimulation::FUMemory(const char* FUName) const {
    assert(
        machine_.functionUnitNavigator().item(FUName)->addressSpace() != NULL);
    return 
        dynamic_cast<DirectAccessMemory&>(
            *memorySystem()->memory(
                *machine_.functionUnitNavigator().item(
                    FUName)->addressSpace()).get());
}

/**
 * Returns a pointer to the memory system
 * 
 * @return a pointer to the memory system
 */
MemorySystem *
CompiledSimulation::memorySystem() const {
    assert (pimpl_->memorySystem_ != NULL);
    return pimpl_->memorySystem_;
}

/**
 * Returns a reference to the simulator frontend
 * 
 * @return a reference to the simulator frontend
 */
SimulatorFrontend& 
CompiledSimulation::frontend() const { 
    return *(pimpl_->frontend_); 
}

/**
 * A short cut for printing debugging info from the compiled code.
 * 
 * @param message The message string to be shown on the log stream
 */
void
CompiledSimulation::msg(const char* message) const {
    Application::logStream() << message << std::endl;
}

/**
 * Halts simulation by throwing an exception with a message attached to it
 * 
 * @param file file where the exception happened, i.e. __FILE__
 * @param line line where the exception happened, i.e. __LINE__
 * @param procedure function where the exception happened, i.e. __FUNCTION__
 * @param message message to attach
 * @exception SimulationExecutionError thrown always
 */
void 
CompiledSimulation::haltSimulation(
    const char* file,
    int line,
    const char* procedure,
    const char* message) const {
    throw SimulationExecutionError(file, line, procedure, message);
}

/**
 * Resizes the jump table
 * 
 * @param newSize New size
 */
void
CompiledSimulation::resizeJumpTable(int newSize) {
    pimpl_->jumpTable_.resize(newSize, 0);
}

/**
 * Gets the simulate function of given address from the jump table.
 * 
 * If this is a dynamic compiled simulation, it'll first check if the simulate-
 * function is available. If not, it will compile the required files first and
 * then loads the simulate function symbols.
 * 
 * @param address address to get the simulate function for
 * @return Simulate Function of given address from the jump table
 * @exception SimulationExecutionError If the jump function couldn't be gotten
 */
SimulateFunction 
CompiledSimulation::getSimulateFunction(InstructionAddress address) {
    
    // Is there an already existing simulate function in the given address?
    SimulateFunction targetFunction = pimpl_->jumpTable_[address];
    if (targetFunction != 0) {
            return targetFunction;
    }
    
    if (dynamicCompilation_) {
        compileAndLoadFunction(address);
        targetFunction = pimpl_->jumpTable_[address];
        if (targetFunction != 0) {
            return targetFunction;
        }
    }
    
    throw SimulationExecutionError(__FILE__, __LINE__, __FUNCTION__,
        "Cannot simulate jump to address " + Conversion::toString(address) + 
        ". Please try with the interpretive simulation engine." );
}

/**
 * Sets a jump target function for given address at the jump table
 * 
 * @param address address to set a jump function for
 * @param fp function pointer to set at the address
 */
void 
CompiledSimulation::setJumpTargetFunction(
    InstructionAddress address,
    SimulateFunction fp) {
    pimpl_->jumpTable_[address] = fp;
}

/**
 * Compiles and loads all simulate functions belonging to a procedure 
 * containing the given address.
 * 
 * @param address (any) address of a procedure to compile
 */
void
CompiledSimulation::compileAndLoadFunction(InstructionAddress address) {
    
    InstructionAddress procedureStart =
        procedureBBRelations_.procedureStart[address];
    
    // Files compiled so far
    std::set<std::string> compiledFiles;
    
    CompiledSimSymbolGenerator symbolGen;
    
    // Get basic blocks of a procedure
    typedef ProcedureBBRelations::BasicBlockStarts::iterator BBIterator;
    std::pair<BBIterator, BBIterator> equalRange = 
        procedureBBRelations_.basicBlockStarts.equal_range(procedureStart);

    // Loop all basic blocks of a procedure, then compile all its files
    for (BBIterator it = equalRange.first; it != equalRange.second; ++it) {
        std::string file = procedureBBRelations_.basicBlockFiles[it->second];
        
        // Compile the file if it hasn't been already
        if (compiledFiles.find(file) == compiledFiles.end()) {
            pimpl_->compiler_.compileToSO(file);
            std::string soPath = FileSystem::directoryOfPath(file) 
                + FileSystem::DIRECTORY_SEPARATOR 
                + FileSystem::fileNameBody(file) + ".so";
            pimpl_->pluginTools_.registerModule(soPath);
            compiledFiles.insert(file);
        }

        // Load the generated simulate function
        SimulateFunction fn;
        pimpl_->pluginTools_.importSymbol(
            symbolGen.basicBlockSymbol(it->second), fn);        
        setJumpTargetFunction(it->second, fn);
    }
}

/**
 * Returns value of the given symbol (be it RF, FU, or IU)
 * 
 * @param symbolName Symbol name in the generated code
 * @return a pointer to the symbol's SimValue or 0 if the symbol wasn't found
 */
SimValue*
CompiledSimulation::getSymbolValue(const char* symbolName) {

    CompiledSimulationPimpl::Symbols::iterator it = pimpl_->symbols_.find(
        std::string(symbolName));
    
    if (it != pimpl_->symbols_.end()) {
        return it->second;
    } else {
        return 0;
    }
}

/**
 * Adds a new symbol name -> SimValue pair to the symbols map
 * @param symbolName the symbol name
 * @param value the SimValue
 */
void 
CompiledSimulation::addSymbol(const char* symbolName, SimValue& value) {
    pimpl_->symbols_[std::string(symbolName)] = &value;
}
