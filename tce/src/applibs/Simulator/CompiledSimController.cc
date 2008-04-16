/**
 * @file CompiledSimController.cc
 *
 * Definition of CompiledSimController class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include <cstdlib>

#include "CompiledSimController.hh"
#include "CompiledSimCodeGenerator.hh"
#include "CompiledSimFrontend.hh"
#include "CompiledSimCompiler.hh"
#include "FileSystem.hh"
#include "MemorySystem.hh"
#include "SimulatorToolbox.hh"
#include "CompiledSimulation.hh"
#include "POMValidator.hh"
#include "POMValidatorResults.hh"
#include "Instruction.hh"

using std::endl;
using namespace TTAMachine;


/**
 * The constructor
 * 
 * @param frontend Simulator frontend
 * @param machine The machine model
 * @param program The program to be simulated
 */
CompiledSimController::CompiledSimController(
    SimulatorFrontend& frontend, const TTAMachine::Machine& machine, 
    const TTAProgram::Program& program) : 
    TTASimulationController(frontend, machine, program, false, true),
    pluginTools_(true), compiledSimulationPath_(""), leaveDirty_(false) {
    reset();
}

/**
 * The destructor
 */
CompiledSimController::~CompiledSimController() {
    deleteGeneratedFiles();
}

/**
 * Advances simulation by given amount of cycles
 * 
 * @Note The accuracy of this function is one basic block!
 */
void
CompiledSimController::step(double count)
    throw (SimulationExecutionError) {
    
    assert(state_ == STA_STOPPED || state_ == STA_INITIALIZED);
    stopReasons_.clear();
    state_ = STA_RUNNING;
    stopRequested_ = false;
    
    try {
        simulation_->step(count);
    }  catch (const Exception& e) {
        SimulatorToolbox::reportSimulatedProgramError(
            frontend_.eventHandler(),
            SimulatorToolbox::RES_FATAL,
            e.errorMessage());
        prepareToStop(SRE_RUNTIME_ERROR);
        return;
    } catch(...) {
        prepareToStop(SRE_RUNTIME_ERROR);
        return;
    }
    
    prepareToStop(SRE_AFTER_STEPPING);

    if (simulation_->isFinished()) { 
        state_ = STA_FINISHED;
    } else {
        state_ = STA_STOPPED;
    }

    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_SIMULATION_STOPPED);
}

/**
 * Advances simulation by a given amout of steps and skip procedure calls.,
 * 
 * @Note The accuracy of this function is one basic block!
 */
void 
CompiledSimController::next(int count)
    throw (SimulationExecutionError) {
    
    assert(state_ == STA_STOPPED || state_ == STA_INITIALIZED);
    stopReasons_.clear();
    state_ = STA_RUNNING;
    stopRequested_ = false;
    
    try {
        simulation_->next(count);
    } catch (const Exception& e) {
        SimulatorToolbox::reportSimulatedProgramError(
            frontend_.eventHandler(),
            SimulatorToolbox::RES_FATAL,
            e.errorMessage());
        prepareToStop(SRE_RUNTIME_ERROR);
        return;
    }  catch(...) {
        prepareToStop(SRE_RUNTIME_ERROR);
        return;
    }
    
    prepareToStop(SRE_AFTER_STEPPING);

    if (simulation_->isFinished()) { 
        state_ = STA_FINISHED;
    } else {
        state_ = STA_STOPPED;
    }

    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_SIMULATION_STOPPED);
}

/**
 * Advances the simulation until the program ends
 * 
 * @exception SimulationExecutionError If a runtime error occurs in 
 *                                     the simulated program.
 */
void
CompiledSimController::run() throw (SimulationExecutionError) {
    
    stopRequested_ = false;
    stopReasons_.clear();
    state_ = STA_RUNNING;
    
    // Run the program
    try {
        simulation_->run();
    } catch (const Exception& e) {
        SimulatorToolbox::reportSimulatedProgramError(
            frontend_.eventHandler(),
            SimulatorToolbox::RES_FATAL,
            e.errorMessage());
        prepareToStop(SRE_RUNTIME_ERROR);
        return;
    }  catch(...) {
        prepareToStop(SRE_RUNTIME_ERROR);
        return;
    }

    if (simulation_->isFinished()) { 
        state_ = STA_FINISHED;
    } else {
        state_ = STA_STOPPED;
    }

    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_SIMULATION_STOPPED);
}

/**
 * Advances the simulation until a given address is reached.
 * 
 * @Note The accuracy of this function is one basic block!
 * 
 * @param address the address after the simulation execution should end
 * @exception SimulationExecutionError If a runtime error occurs in 
 *                                     the simulated program.
 */
void
CompiledSimController::runUntil(UIntWord address)
    throw (SimulationExecutionError) {
    
    stopRequested_ = false;
    stopReasons_.clear();
    state_ = STA_RUNNING;
    address = basicBlockStart(address); // find nearest bb start
    
    try {
        simulation_->runUntil(address);
    } catch (const Exception& e) {
        SimulatorToolbox::reportSimulatedProgramError(
            frontend_.eventHandler(),
            SimulatorToolbox::RES_FATAL,
            e.errorMessage());
        prepareToStop(SRE_RUNTIME_ERROR);
        return;
    }  catch(...) {
        prepareToStop(SRE_RUNTIME_ERROR);
        return;
    }
    
    prepareToStop(SRE_AFTER_UNTIL);

    if (simulation_->isFinished()) { 
        state_ = STA_FINISHED;
    } else {
        state_ = STA_STOPPED;
    }

    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_SIMULATION_STOPPED);
}

/**
 * Resets the simulation so it can be started from the beginning.
 *
 * Resets everything to initial values (program counter, clock cycles etc.)
 */
void 
CompiledSimController::reset() {
    
    state_ = STA_INITIALIZING;
    stopRequested_ = false;
    clockCount_ = 0;
    
    deleteGeneratedFiles();
        
    compiledSimulationPath_ = FileSystem::createTempDirectory();
    if (compiledSimulationPath_ == "") {
        Application::logStream() 
            << "Cannot create temporary path "
            << "for the generated simulation code!" << endl;
        return;
    }

    // Compile the program
    CompiledSimCodeGenerator generator(sourceMachine_, program_, *this,
        frontend_.isSequentialSimulation(),
        frontend_.fuResourceConflictDetection());
    CATCH_ANY(generator.generateToDirectory(compiledSimulationPath_));
    
    basicBlocks_ = generator.basicBlocks();

    std::string flags = "-O0";
    const char* fl = std::getenv("TTASIM_COMPILER_FLAGS");
    if (fl != NULL)
        flags = std::string(fl);

    CompiledSimCompiler compiler;
    if (compiler.compileDirectory(compiledSimulationPath_, flags) != 0) {
        Application::logStream() << "Failed to compile the program!" << endl;
        return;
    }
    
    SimulationGetterFunction* simulationGetter = NULL;
    pluginTools_.addSearchPath(compiledSimulationPath_);
    // register the individual simulation functions
    std::vector<std::string> sos;
    FileSystem::globPath(compiledSimulationPath_ + "/simulate_*.so", sos);
    for (std::size_t i = 0; i < sos.size(); ++i) {
        const std::string& so = FileSystem::fileOfPath(sos.at(i));
        //Application::logStream() << "registering " << so << std::endl;
        pluginTools_.registerModule(so);
    }

    pluginTools_.registerModule("engine.so");
    pluginTools_.importSymbol("getSimulation", simulationGetter);
    simulation_.reset(
        simulationGetter(sourceMachine_, program_.entryAddress().location(),
            program_.lastInstruction().address().location(),
            frontend_, memorySystem()));
    
    state_ = STA_INITIALIZED;
}

/**
 * Returns the program counter value.
 *
 * @return Program counter value.
 */
InstructionAddress
CompiledSimController::programCounter() const {
    return simulation_->programCounter();
}

/**
 * Returns the address of the last executed instruction.
 * 
 * @return The address of the last executed instruction
 */
InstructionAddress
CompiledSimController::lastExecutedInstruction() const {
    return simulation_->lastExecutedInstruction();
}

/**
 * Returns the count of clock cycles simulated.
 *
 * @return Count of simulated clock cycles.
 */
ClockCycleCount
CompiledSimController::clockCount() const {
    if (simulation_) {
        return simulation_->cycleCount();
    } else {
        return 0;
    }
}

/**
 * Returns a pointer to the current CompiledSimulation object
 * 
 * @return A pointer to the current CompiledSimulation object
 */
boost::shared_ptr<CompiledSimulation> 
CompiledSimController::compiledSimulation() {
    return simulation_;
}

/**
 * Will delete all generated code files if leaveDirty_ is not set
 */
void 
CompiledSimController::deleteGeneratedFiles() {
    if (leaveDirty_) {
        return;
    }
    
    if (FileSystem::fileExists(compiledSimulationPath_) 
        && compiledSimulationPath_ != "") {
        FileSystem::removeFileOrDirectory(compiledSimulationPath_);
    }
}

/**
 * Returns the start of the basic block containing address
 * 
 * @return instruction address of the basic block start
 */
InstructionAddress 
CompiledSimController::basicBlockStart(InstructionAddress address) const {
    InstructionAddress start = 0;
    for (CompiledSimCodeGenerator::BasicBlocks::const_iterator it = 
        basicBlocks_.begin(); it != basicBlocks_.end(); ++it) {
        start = it->first;
        if (address>= it->first && address <= it->second) {
            break;
        }
    }
    
    return start;
}


/**
 * Returns a string containing the value(s) of the register file
 * 
 * @param rfName name of the register file to search for
 * @param registerIndex index of the register. if -1, all registers are listed
 * @return A string containing the value(s) of the register file
 * @exception InstanceNotFound If the register cannot be found.
 */
std::string 
CompiledSimController::registerFileValue(
    const std::string& rfName, int registerIndex) {
    std::string stringValue("");

    if (registerIndex >= 0) {
        stringValue += Conversion::toString(
            compiledSimulation()->
                registerFileValue(rfName, registerIndex).intValue());
    } else {
        Machine::RegisterFileNavigator navigator = 
            sourceMachine_.registerFileNavigator();
        RegisterFile& rf = *navigator.item(rfName);
        
        bool firstReg = true;
        for (int i = 0; i < rf.numberOfRegisters(); ++i) {
            if (!firstReg) 
                stringValue += "\n";
            const std::string registerName = 
                rfName + "." + Conversion::toString(i);
            
            SimValue value = compiledSimulation()->
                registerFileValue(rfName, i);
            stringValue += registerName + " " + Conversion::toHexString(
                static_cast<unsigned int>(MathTools::zeroExtendTo(
                    value.uIntWordValue(), value.width()))) + " " 
                        + Conversion::toString(
                            static_cast<int>(value.uIntWordValue()));
            firstReg = false;
        }
    }
    
    return stringValue;
}

/**
 * Returns the current value of a IU register
 * 
 * @param iuName name of the immediate unit
 * @param index index of the register
 * @return Current value of a IU register
 */
SimValue
CompiledSimController::immediateUnitRegisterValue(
const std::string& iuName, int index) {
    return compiledSimulation()->immediateUnitRegisterValue(iuName, index);
}

/**
 * Returns a FU port's value
 * 
 * @param fuName name of the function unit
 * @param portIndex Index of the port
 * @return A FU port's value
 */
SimValue 
CompiledSimController::FUPortValue(
    const std::string& fuName, 
    const std::string& portName) {
    return compiledSimulation()->FUPortValue(fuName, portName);
}

/**
 * Sends a stop request to the simulation controller and compiled simulation
 * 
 * @param reason The stop reason
 */
void
CompiledSimController::prepareToStop(StopReason reason) {
    TTASimulationController::prepareToStop(reason);
    compiledSimulation()->requestToStop();
}
