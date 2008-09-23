/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file CompiledSimController.cc
 *
 * Definition of CompiledSimController class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <cstdlib>

#include "SimulatorFrontend.hh"
#include "CompiledSimController.hh"
#include "CompiledSimCodeGenerator.hh"
#include "CompiledSimCompiler.hh"
#include "FileSystem.hh"
#include "MemorySystem.hh"
#include "SimulatorToolbox.hh"
#include "CompiledSimulation.hh"
#include "POMValidator.hh"
#include "POMValidatorResults.hh"
#include "Instruction.hh"
#include "MathTools.hh"
#include "Program.hh"

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
    
    // Generate all simulation code at once
    CompiledSimCodeGenerator generator(sourceMachine_, program_, *this,
        frontend_.isSequentialSimulation(),
        frontend_.fuResourceConflictDetection(), frontend_.executionTracing(), 
        !frontend_.staticCompilation(), false, !frontend_.staticCompilation());
    CATCH_ANY(generator.generateToDirectory(compiledSimulationPath_));
    
    basicBlocks_ = generator.basicBlocks();
    procedureBBRelations_ = generator.procedureBBRelations();

    CompiledSimCompiler compiler;
    
    // Compile everything when using static compiled simulation
    if (frontend_.staticCompilation()) {
        if (compiler.compileDirectory(compiledSimulationPath_, "", false) 
            != 0) {
            Application::logStream() << "Compilation aborted." << endl;
            return;
        }
    } else { // Compile main engine file
        compiler.compileToSO(compiledSimulationPath_ 
            + FileSystem::DIRECTORY_SEPARATOR + "CompiledSimulationEngine.cc");
        
        // Precompile the simulation header
        compiler.compileFile(compiledSimulationPath_ 
            + FileSystem::DIRECTORY_SEPARATOR + "CompiledSimulationEngine.hh", 
            "-xc++-header", ".gch");
    }

    SimulationGetterFunction* simulationGetter = NULL;
    pluginTools_.addSearchPath(compiledSimulationPath_);

    // register individual simulation functions
    std::vector<std::string> sos;
    FileSystem::globPath(compiledSimulationPath_ + "/simulate_*.so", sos);
    for (std::size_t i = 0; i < sos.size(); ++i) {
        const std::string& so = FileSystem::fileOfPath(sos.at(i));
        pluginTools_.registerModule(so);
    }

    // register simulation getter function symbol
    pluginTools_.registerModule("CompiledSimulationEngine.so");
    pluginTools_.importSymbol("getSimulation", simulationGetter);
    simulation_.reset(
        simulationGetter(sourceMachine_, program_.entryAddress().location(),
            program_.lastInstruction().address().location(), 
            frontend_, *this, memorySystem(), !frontend_.staticCompilation(), 
            procedureBBRelations_));
    
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
 * @param address
 * @return instruction address of the basic block start
 */
InstructionAddress 
CompiledSimController::basicBlockStart(InstructionAddress address) const {
    return basicBlocks_.lower_bound(address)->second;
}

/**
 * Returns the program model
 * @return the program model
 */
const TTAProgram::Program&
CompiledSimController::program() const {
    return program_;
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
                registerFileValue(rfName.c_str(), registerIndex).intValue());
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
                registerFileValue(rfName.c_str(), i);
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
    const std::string& iuName, 
    int index) {
    return compiledSimulation()->immediateUnitRegisterValue(iuName.c_str(),
        index);
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
    return compiledSimulation()->FUPortValue(fuName.c_str(), portName.c_str());
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
