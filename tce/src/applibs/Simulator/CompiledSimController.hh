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
 * @file CompiledSimController.hh
 *
 * Declaration of CompiledSimController class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIM_CONTROLLER_HH
#define COMPILED_SIM_CONTROLLER_HH

#include "TTASimulationController.hh"
#include "PluginTools.hh"
#include <boost/shared_ptr.hpp>
#include "CompiledSimCodeGenerator.hh"

class CompiledSimulation;
struct ProcedureBBRelations;

namespace TTAProgram {
    class Program;
}

/**
 * A Simulation controller that uses the compiled simulation techniques.
 */
class CompiledSimController : public TTASimulationController {
public:
    CompiledSimController(
        SimulatorFrontend& frontend,
        const TTAMachine::Machine& machine, 
        const TTAProgram::Program& program);

    virtual ~CompiledSimController();

    virtual void step(double count = 1)
        throw (SimulationExecutionError);

    virtual void next(int count = 1)
        throw (SimulationExecutionError);

    virtual void run()
        throw (SimulationExecutionError);

    virtual void runUntil(UIntWord address)
        throw (SimulationExecutionError);
    
    virtual void reset();
    
    virtual InstructionAddress programCounter() const;
    virtual InstructionAddress lastExecutedInstruction() const;
    virtual ClockCycleCount clockCount() const;    
        
    virtual boost::shared_ptr<CompiledSimulation> compiledSimulation();
    
    virtual void deleteGeneratedFiles();
    
    /// Function type for the getSimulation() function
    typedef CompiledSimulation* (SimulationGetterFunction)(
        const TTAMachine::Machine& machine, 
        InstructionAddress entryAddress,
        InstructionAddress lastInstruction,
        SimulatorFrontend& frontend,
        CompiledSimController& controller,
        MemorySystem& memorySystem,
        bool dynamicCompilation,
        ProcedureBBRelations& procedureBBRelations);
    
    virtual std::string registerFileValue(
        const std::string& rfName, 
        int registerIndex = -1);
    
    virtual SimValue immediateUnitRegisterValue(
    const std::string& iuName, int index = -1);
    
    virtual SimValue FUPortValue(
        const std::string& fuName, 
        const std::string& portName);
    
    virtual void prepareToStop(StopReason reason);
    
    InstructionAddress basicBlockStart(InstructionAddress address) const;
    const TTAProgram::Program& program() const;
        
private:
    /// Copying not allowed.
    CompiledSimController(const CompiledSimController&);
    /// Assignment not allowed.
    CompiledSimController& operator=(const CompiledSimController&);
      
    /// Used for loading the compiled simulation plugin
    PluginTools pluginTools_;    

    /// Pointer to the loaded simulation
    boost::shared_ptr<CompiledSimulation> simulation_;
    
    /// Path to the generated simulation files
    std::string compiledSimulationPath_;
    
    /// True, if the simulation should leave all the generated code files
    bool leaveDirty_;
    
    /// A map containing the basic blocks' start..end pairs
    CompiledSimCodeGenerator::AddressMap basicBlocks_;
    /// A struct for tracking basic blocks and their relation to their procedures
    ProcedureBBRelations procedureBBRelations_;
};

#endif
