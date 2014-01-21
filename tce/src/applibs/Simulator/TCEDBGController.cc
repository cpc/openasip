/**
 * @file TCEDBGController.cc
 *
 * Stand-in replacement for the FPGA controller in the HardwareDebugger branch on launchpad.
 * Minimum implmentation to get TCE to compile.
 */
#include "TCEDBGController.hh"
#include "SimValue.hh"

typedef MinimumAddressableUnit MAU;

   
TCEDBGController::TCEDBGController( 
    SimulatorFrontend& frontend,
    const TTAMachine::Machine& machine, 
    const TTAProgram::Program& program) : 
    RemoteController( frontend, machine, program )
{
}

void TCEDBGController::writeMem(
    Word /*address*/,
    MAU /*data*/,
    const AddressSpace& /*space*/)
{
}

MAU TCEDBGController::readMem(
    Word /*address*/,
    const AddressSpace& /*space*/)
{
    return 0;
}

void TCEDBGController::writeIMem(
    const char *,
    int /*size*/)
{
}


void TCEDBGController::step(double /*count = 1*/)
    throw (SimulationExecutionError)
{
}

void TCEDBGController::next(int /*count = 1*/)
    throw (SimulationExecutionError)
{
}

void TCEDBGController::run()
    throw (SimulationExecutionError)
{
}

void TCEDBGController::runUntil(UIntWord /*address*/)
    throw (SimulationExecutionError)
{
}

void TCEDBGController::reset()
{
}
    
std::string TCEDBGController::registerFileValue(
    const std::string& /*rfName*/, 
    int /*registerIndex = -1*/)
{
    std::string rv="hello";
    return rv;
}
    
SimValue TCEDBGController::immediateUnitRegisterValue(
    const std::string& /*iuName*/, int /*index = -1*/)
{
    SimValue rv;
    return rv;
}
    
SimValue TCEDBGController::FUPortValue(
    const std::string& /*fuName*/, 
    const std::string& /*portName*/)
{
    SimValue rv;
    return rv;
}
    
InstructionAddress TCEDBGController::programCounter() const
{
    return 0;
}

/* vim: set ts=4 expandtab: */
