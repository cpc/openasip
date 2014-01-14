/**
 * @file CustomDBGController.cc
 *
 * Stub file for user's custom debugger control
 */


#include "CustomDBGController.hh"
#include "SimValue.hh"
#include "SimulatorFrontend.hh"
#include <cstdio>
    
#define DEBUG_REMOTE_DEBUGGER_CONTROLLER
#ifdef DEBUG_REMOTE_DEBUGGER_CONTROLLER
#define DPRINT(...) printf(__VA_ARGS__)
#define _WHERE() printf("%s:%d - %s\n", __FILE__, __LINE__, __func__)
#else
#define _WHERE()
#define DPRINT()
#endif

typedef MinimumAddressableUnit MAU;

CustomDBGController::CustomDBGController( 
        SimulatorFrontend& frontend,
        const TTAMachine::Machine& machine, 
        const TTAProgram::Program& program) : 
        RemoteController( frontend, machine, program )
{
    _WHERE();
    // Write instructions to TTA memory. This is not done by TCE in the simulator modes.
    // At some later point in time TCE fills the global data memory.
}

void CustomDBGController::writeMem(Word address, MAU data, const AddressSpace& space)
{
    _WHERE();
}

MAU CustomDBGController::readMem(Word address, const AddressSpace& space)
{
    MAU rv=0;
    return rv;
}

void CustomDBGController::writeIMem(const char *buff, int size)
{
    _WHERE();
}


void CustomDBGController::step(double /*count = 1*/)
        throw (SimulationExecutionError)
{
    _WHERE();
}

void CustomDBGController::next(int /*count = 1*/)
        throw (SimulationExecutionError)
{
    _WHERE();
}

void CustomDBGController::run()
        throw (SimulationExecutionError)
{
    _WHERE();
}

void CustomDBGController::runUntil(UIntWord address)
        throw (SimulationExecutionError)
{
    _WHERE();
}

void CustomDBGController::reset()
{
    _WHERE();
}
    
std::string CustomDBGController::registerFileValue(
        const std::string& rfName, 
        int registerIndex )
{
    std::string rv="hello";
    _WHERE();
    return rv;
}
    
SimValue CustomDBGController::immediateUnitRegisterValue(
    const std::string& iuName, int index)
{
    SimValue rv;
    return rv;
}
    
SimValue CustomDBGController::FUPortValue(
        const std::string& fuName, 
        const std::string& portName)
{
    SimValue rv;
    return rv;
}
    
InstructionAddress CustomDBGController::programCounter() const
{
    int pc;
    _WHERE();
    return pc;
}

/* vim: set ts=4 expandtab: */
