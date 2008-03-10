/**
 * @file CompiledSimFrontend.hh
 *
 * Declaration of CompiledSimFrontend class
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COMPILED_SIM_FRONTEND
#define TTA_COMPILED_SIM_FRONTEND

#include "SimulatorFrontend.hh"

class CompiledSimController;

/**
 * Frontend for the compiled simulation functionality.
 * 
 */
class CompiledSimFrontend : public SimulatorFrontend {
public:
    CompiledSimFrontend();
    virtual ~CompiledSimFrontend();

protected:
    /// Copying not allowed.
    CompiledSimFrontend(const CompiledSimFrontend&);
    /// Assignment not allowed.
    CompiledSimFrontend& operator=(const CompiledSimFrontend&);
    
    virtual void initializeSimulation();
    
    virtual bool isCompiledSimulation() const;
    
    virtual std::string registerFileValue(
        const std::string& rfName, 
        int registerIndex = -1);
    
    virtual SimValue immediateUnitRegisterValue(
    const std::string& iuName, int index = -1);
    
    virtual SimValue FUPortValue(
        const std::string& fuName, 
        const std::string& portName);
    
    virtual InstructionAddress lastExecutedInstruction() const;
    
private:
    CompiledSimController * compiledSimCon;
};

#endif
