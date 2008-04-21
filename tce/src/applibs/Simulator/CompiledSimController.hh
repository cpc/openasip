/**
 * @file CompiledSimController.hh
 *
 * Declaration of CompiledSimController class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIM_CONTROLLER_HH
#define COMPILED_SIM_CONTROLLER_HH

#include "TTASimulationController.hh"
#include "PluginTools.hh"
#include <boost/shared_ptr.hpp>
#include "CompiledSimCodeGenerator.hh"

class CompiledSimulation;

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
        MemorySystem& memorySystem);
    
    virtual std::string registerFileValue(
        const std::string& rfName, 
        int registerIndex = -1);
    
    virtual SimValue immediateUnitRegisterValue(
    const std::string& iuName, int index = -1);
    
    virtual SimValue FUPortValue(
        const std::string& fuName, 
        const std::string& portName);
    
    virtual void prepareToStop(StopReason reason);
        
private:
    /// Copying not allowed.
    CompiledSimController(const CompiledSimController&);
    /// Assignment not allowed.
    CompiledSimController& operator=(const CompiledSimController&);
        
    InstructionAddress basicBlockStart(InstructionAddress address) const;
      
    /// Used for loading the compiled simulation plugin
    PluginTools pluginTools_;    

    /// Pointer to the loaded simulation
    boost::shared_ptr<CompiledSimulation> simulation_;
    
    /// Path to the generated simulation files
    std::string compiledSimulationPath_;
    
    /// True, if the simulation should leave all the generated code files
    bool leaveDirty_;
    
    /// A map containing the basic blocks' start..end pairs
    CompiledSimCodeGenerator::BasicBlocks basicBlocks_;
};

#endif
