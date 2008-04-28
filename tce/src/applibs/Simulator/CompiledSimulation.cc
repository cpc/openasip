/**
 * @file CompiledSimulation.cc
 *
 * Definition of CompiledSimulation class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include <string>
#include "CompiledSimulation.hh"
#include "Machine.hh"
#include "Instruction.hh"
#include "SimulatorFrontend.hh"
#include "SimulationEventHandler.hh"
#include "SymbolGenerator.hh"
#include "DirectAccessMemory.hh"
#include "MemorySystem.hh"
#include "CompiledSimulationPimpl.hh"

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
    lastInstruction_(lastInstruction), pimpl_(new CompiledSimulationPimpl()) {
    pimpl_->memorySystem_ = &memorySystem;
    pimpl_->frontend_ = &frontend;
}

/**
 * The destructor
 */
CompiledSimulation::~CompiledSimulation() {
    delete pimpl_;
    pimpl_ = NULL;
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
        pimpl_->frontend_->eventHandler().handleEvent(
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
        pimpl_->frontend_->eventHandler().handleEvent(
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
        pimpl_->frontend_->eventHandler().handleEvent(
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
    const char* rfName, int registerIndex) {  
    RegisterFile& rf = *machine_.registerFileNavigator().item(rfName);
    std::string registerFile = SymbolGenerator::registerSymbol(rf, registerIndex);
    
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
CompiledSimulation::immediateUnitRegisterValue(
    const char* iuName, int index) {  
    ImmediateUnit& iu = *machine_.immediateUnitNavigator().item(iuName);
    std::string immediateUnit = SymbolGenerator::immediateRegisterSymbol(
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
CompiledSimulation::FUPortValue(
    const char* fuName, 
    const char* portName) {
    
    FunctionUnit& fu = *machine_.functionUnitNavigator().item(fuName);
    std::string fuPort = SymbolGenerator::portSymbol(*fu.port(portName));
    
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
 * @param msg The message string to be shown on the log stream
 */
void
CompiledSimulation::msg(const char* msg) const {
    Application::logStream() << msg << std::endl;
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
 * Gets a jump target for given address from the jump table
 * 
 * @return Jump target for given address from the jump table
 */
SimulateFunction 
CompiledSimulation::getJumpTargetFunction(InstructionAddress address) {
    return pimpl_->jumpTable_[address];
}

/**
 * Sets a jump target for given address at the jump table
 * 
 * @param address address to set a jump function for
 * @param fp function pointer to the address
 */
void 
CompiledSimulation::setJumpTargetFunction(
    InstructionAddress address,
    SimulateFunction fp) {
    pimpl_->jumpTable_[address] = fp;
}

SimValue* CompiledSimulation::getSymbolValue(const char* symbolName) {
    return pimpl_->symbols_[std::string(symbolName)];
}

void 
CompiledSimulation::addSymbol(const char* symbolName, SimValue& value) {
    pimpl_->symbols_[std::string(symbolName)] = &value;
}
