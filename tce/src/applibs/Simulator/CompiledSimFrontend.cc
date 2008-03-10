/**
 * @file CompiledSimFrontend.cc
 *
 * Definition of CompiledSimFrontend class.
 *
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include "CompiledSimFrontend.hh"
#include "CompiledSimController.hh"
#include "StopPointManager.hh"
#include "CompiledSimulation.hh"
#include "Machine.hh"

using namespace TTAMachine;
using namespace TTAProgram;
using namespace TPEF;

/**
 * The constructor
 */
CompiledSimFrontend::CompiledSimFrontend() :    
    SimulatorFrontend(),
    compiledSimCon(NULL) {
    setFUResourceConflictDetection(false); // disabled by default
}

/**
 * The destructor
 */
CompiledSimFrontend::~CompiledSimFrontend() {
}

/**
 * Initializes a new simulation.
 *
 * Creates a new compiled simulation controller. Assumes the simulation is not
 * running and the possible old simulation data is freed.
 */
void 
CompiledSimFrontend::initializeSimulation() {
    delete simCon_;
    simCon_ = NULL;

    compiledSimCon = new CompiledSimController(*this,
        *currentMachine_, *currentProgram_);
    
    simCon_ = compiledSimCon;

    machineState_ = &simCon_->machineState();
    delete stopPointManager_;
    stopPointManager_ = new StopPointManager(
        *simCon_, eventHandler());
}

/**
 * Returns true if the current simulation engine uses compiled simulation
 * 
 * @return true if the current simulation engine uses compiled simulation
 */
bool 
CompiledSimFrontend::isCompiledSimulation() const {
    return true;
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
CompiledSimFrontend::registerFileValue(
    const std::string& rfName, int registerIndex) {
    std::string stringValue("");
    
    assert(currentMachine_ != NULL);
    
    if (registerIndex >= 0) {
        stringValue += Conversion::toString(
            compiledSimCon->compiledSimulation()->
                registerFileValue(rfName, registerIndex).intValue());
    } else {
        Machine::RegisterFileNavigator navigator = 
            currentMachine_->registerFileNavigator();
        RegisterFile& rf = *navigator.item(rfName);
        
        bool firstReg = true;
        for (int i = 0; i < rf.numberOfRegisters(); ++i) {
            if (!firstReg) 
                stringValue += "\n";
            const std::string registerName = 
                rfName + "." + Conversion::toString(i);
            
            SimValue value = compiledSimCon->compiledSimulation()->
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
CompiledSimFrontend::immediateUnitRegisterValue(
const std::string& iuName, int index) {
    return compiledSimCon->compiledSimulation()->immediateUnitRegisterValue(
        iuName, index);
}

/**
 * Returns a FU port's value
 * 
 * @param fuName name of the function unit
 * @param portIndex Index of the port
 * @return A FU port's value
 */
SimValue 
CompiledSimFrontend::FUPortValue(
    const std::string& fuName, 
    const std::string& portName) {
    return compiledSimCon->compiledSimulation()->FUPortValue(fuName, portName);
}

/**
 * Returns address of the last executed instruction
 * 
 * @return Address of the last executed instruction
 */
InstructionAddress
CompiledSimFrontend::lastExecutedInstruction() const {
    return compiledSimCon->lastExecutedInstruction();
}

