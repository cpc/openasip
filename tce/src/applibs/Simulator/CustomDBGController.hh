/**
 * @file CustomDBGController.hh
 *
 * Stub file for user's custom debugger control
 */

#pragma once

#include "RemoteController.hh"
class CustomDBGController : public RemoteController {
public: 
    typedef MinimumAddressableUnit MAU;
    CustomDBGController(
        SimulatorFrontend& frontend,
        const TTAMachine::Machine& machine, 
        const TTAProgram::Program& program);
   
    // inherited virtual functions that must be implemented in this class
    virtual void writeMem(Word address, MAU data, const AddressSpace&);
    virtual MAU readMem(Word address, const AddressSpace&);
    virtual void writeIMem(const char *data, int size);

    virtual void step(double count = 1)
        throw (SimulationExecutionError);

    virtual void next(int count = 1)
        throw (SimulationExecutionError);

    virtual void run()
        throw (SimulationExecutionError);

    virtual void runUntil(UIntWord address)
        throw (SimulationExecutionError);

    virtual void reset();
    
    virtual std::string registerFileValue(
        const std::string& rfName, 
        int registerIndex = -1);
    
    virtual SimValue immediateUnitRegisterValue(
    const std::string& iuName, int index = -1);
    
    virtual SimValue FUPortValue(
        const std::string& fuName, 
        const std::string& portName);
    
    virtual InstructionAddress programCounter() const;
private:

};

/* vim: set ts=4 expandtab: */
