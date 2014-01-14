/**
 * @file TCEDBGController.hh
 *
 * Stand-in replacement for the FPGA controller in the HardwareDebugger branch on launchpad.
 * Minimum implmentation to get TCE to compile.
 */

#pragma once

#include "RemoteController.hh"

class TCEDBGController : public RemoteController {
public:
    typedef MinimumAddressableUnit MAU;
    TCEDBGController(
        SimulatorFrontend& frontend,
        const TTAMachine::Machine& machine, 
        const TTAProgram::Program& program);

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

};

/* vim: set ts=4 expandtab */
