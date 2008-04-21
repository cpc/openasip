/**
 * @file CompiledSimulation.cc
 *
 * Definition of CompiledSimulation class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include "CompiledSimulation.hh"
#include "Machine.hh"
#include "Instruction.hh"
#include "SimulatorFrontend.hh"
#include "SimulationEventHandler.hh"
#include "SymbolGenerator.hh"
#include "DirectAccessMemory.hh"
#include "MemorySystem.hh"

using namespace TTAMachine;

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
    MemorySystem& memorySystem) :
    cycleCount_(0),
    basicBlockCount_(0),
    jumpTarget_(entryAddress),
    programCounter_(entryAddress),
    lastExecutedInstruction_(0),
    cyclesToSimulate_(MAX_CYCLES),                  
    stopRequested_(false),
    isFinished_(false),
    conflictDetected_(false),
    machine_(machine),
    entryAddress_(entryAddress),
    lastInstruction_(lastInstruction), 
    memorySystem_(&memorySystem), frontend_(frontend) {
}

/**
 * The destructor
 */
CompiledSimulation::~CompiledSimulation() {
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
        #ifndef DEBUG_SIMULATION
        frontend_.eventHandler().handleEvent(
            SimulationEventHandler::SE_CYCLE_END);
        #endif
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
        #ifndef DEBUG_SIMULATION
        frontend_.eventHandler().handleEvent(
            SimulationEventHandler::SE_CYCLE_END);
        #endif
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
        #ifndef DEBUG_SIMULATION
        frontend_.eventHandler().handleEvent(
            SimulationEventHandler::SE_CYCLE_END);
        #endif
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
CompiledSimulation::registerFileValue(
    const std::string& rfName, int registerIndex) {  
    RegisterFile& rf = *machine_.registerFileNavigator().item(rfName);
    std::string registerFile = SymbolGenerator::registerSymbol(rf, registerIndex);
    
    Symbols::iterator rfIterator = symbols_.find(registerFile);
    if (rfIterator != symbols_.end()) {
        return *(rfIterator->second);
    } else {
        throw InstanceNotFound(__FILE__, __LINE__, __func__, 
            "Register file " + rfName + " not found.");
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
CompiledSimulation::immediateUnitRegisterValue(
    const std::string& iuName, int index) {  
    ImmediateUnit& iu = *machine_.immediateUnitNavigator().item(iuName);
    std::string immediateUnit = SymbolGenerator::immediateRegisterSymbol(
        iu, index);
    
    Symbols::iterator iuIterator = symbols_.find(immediateUnit);
    if (iuIterator != symbols_.end()) {
        return *(iuIterator->second);
    } else {
        throw InstanceNotFound(__FILE__, __LINE__, __func__, 
            "Immediate unit " + iuName + " not found.");
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
CompiledSimulation::FUPortValue(
    const std::string& fuName, 
    const std::string& portName) {
    
    FunctionUnit& fu = *machine_.functionUnitNavigator().item(fuName);
    std::string fuPort = SymbolGenerator::portSymbol(*fu.port(portName));
    
    Symbols::iterator fuPortIterator = symbols_.find(fuPort);
    if (fuPortIterator != symbols_.end()) {
        return *(fuPortIterator->second);
    } else {
        throw InstanceNotFound(__FILE__, __LINE__, __func__, 
            "FU port " + fuPort + " not found.");
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
bool CompiledSimulation::stopRequested() const {
    return stopRequested_;
}

/** Returns true if the simulation is finished
 * 
 * @return true if the simulation is finished
 */
bool CompiledSimulation::isFinished() const {
    return isFinished_;
}

/**
 * Returns a function unit of the given name
 * 
 * @param name name of the function unit to return
 * @return function unit of given name
 * @exception InstanceNotFound If a function unit is not found
 */
TTAMachine::FunctionUnit& 
CompiledSimulation::functionUnit(const std::string& name) const
    throw (InstanceNotFound) {
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
CompiledSimulation::FUMemory(const std::string& FUName) const 
    throw (InstanceNotFound) {
    assert (machine_.functionUnitNavigator().item(FUName)->addressSpace() 
        != NULL);
    return dynamic_cast<DirectAccessMemory&>(memorySystem()->memory(
        *machine_.functionUnitNavigator().item(FUName)->addressSpace()));
}

/**
 * Returns a pointer to the memory system
 * 
 * @return a pointer to the memory system
 */
MemorySystem *
CompiledSimulation::memorySystem() const {
    assert (memorySystem_ != NULL);
    return memorySystem_;
}

/**
 * A short cut for printing debugging info from the compiled code.
 * 
 * @param msg The message string to be shown on the log stream
 */
void
CompiledSimulation::msg(const std::string& msg) const {
    Application::logStream() << msg << std::endl;
}
