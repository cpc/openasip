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
        const TTAProgram::Program& program,
        bool leaveDirty=false);

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
    /// The unique identifier for this simulation engine. Used for
    /// enabling multiple compiled engines in the same process while
    /// still having ccache hits (which would not happen when using
    /// the object address as the id).
    int instanceId_;
};

#endif
