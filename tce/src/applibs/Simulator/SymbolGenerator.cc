/**
 * @file SymbolGenerator.hh
 *
 * Definition of SymbolGenerator class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include "SymbolGenerator.hh"
#include "FunctionUnit.hh"
#include "RegisterFile.hh"
#include "ImmediateUnit.hh"
#include "ControlUnit.hh"
#include "Terminal.hh"
#include "SpecialRegisterPort.hh"
#include "Move.hh"

#include <string>
#include <ctime>

using std::string;

using namespace TTAMachine;
using namespace TTAProgram;


/**
 * Generates a timestamp
 * 
 * @note Should not be printed to the simulation sources as this makes
 * ccache useless. There's always a cache miss due to the changing 
 * timestamp.
 *
 * @return a string containing the timestamp
 */
std::string SymbolGenerator::timeStamp() {
    time_t rawTime;
    time(&rawTime);    
    return string(ctime(&rawTime));
}

/**
 * Generates an unique symbol name for the given port
 * 
 * Converts a TTA unit's port to a unique symbol name that can be later
 * referred as a SimValue variable in the generated C/C++ code.
 * 
 * @param port A port of the TTA unit
 * @return A string containing the generated symbol name
 */
std::string 
SymbolGenerator::portSymbol(const TTAMachine::Port& port) {
    string symbolName;
    
    Unit* unit = port.parentUnit();    
    if (dynamic_cast<const FunctionUnit*>(unit) != NULL) {
        symbolName = "FU";
    } else if (dynamic_cast<const RegisterFile*>(unit) != NULL) {
        symbolName = "RF";
        if (dynamic_cast<const ImmediateUnit*>(unit) != NULL) {
            symbolName = "IU";
        }
    }

    symbolName += "_" + unit->name() + "_" + port.name();
    return symbolName;
}

/**
 * Generates an unique symbol name for a RF register of the terminal
 * 
 * @param terminal Given terminal
 * @return A string containing the generated symbol name
 */
std::string
SymbolGenerator::registerSymbol(const TTAProgram::Terminal& terminal) {
    return registerSymbol(terminal.registerFile(), terminal.index());
}

/**
 * Generates an unique symbol name for a RF register
 * 
 * @param rf The given register file
 * @param index The index of the register
 * @return A string containing the generated symbol name
 */
std::string 
SymbolGenerator::registerSymbol(const TTAMachine::RegisterFile& rf, int index) {  
    return "RF_" + rf.name() + "_" + Conversion::toString(index);
}


/**
 * Generates an unique symbol name for a IU register of the terminal
 * 
 * @param terminal Given terminal
 * @return A string containing the generated symbol name
 */
std::string
SymbolGenerator::immediateRegisterSymbol(const TTAProgram::Terminal& terminal) {
    return immediateRegisterSymbol(terminal.immediateUnit(), terminal.index());
}

/**
 * Generates an unique symbol name for a IU register
 * 
 * @param iu The given IU
 * @param index Index of the immediate register
 * @return A string containing the generated symbol name
 */
std::string 
SymbolGenerator::immediateRegisterSymbol(
    const TTAMachine::ImmediateUnit& iu, 
    int index) {  
    return "IU_" + iu.name() + "_" + Conversion::toString(index);
}

/**
 * Generates an unique symbol name for a TTA machine bus
 * 
 * @param bus The given bus
 * @return A string containing the generated symbol name
 */
std::string 
SymbolGenerator::busSymbol(const TTAMachine::Bus& bus) {
    return "bus_" + bus.name();
}

/**
 * Generates a symbol name for the GCU's return address
 * 
 * @return A symbol name for the GCU's return address
 */
std::string
SymbolGenerator::returnAddressSymbol(const TTAMachine::ControlUnit& gcu) {
    return portSymbol(*gcu.returnAddressPort());
}

/**
 * Generates an unique symbol name for a given move operand (terminal)
 * 
 * @param terminal The operand terminal
 * @param move The move
 * @return a string containing the generated symbol name
 */
std::string
SymbolGenerator::moveOperandSymbol(                                  
    const TTAProgram::Terminal& terminal, 
    const TTAProgram::Move & move) {
            
    if (terminal.isGPR()) {
        return registerSymbol(terminal);
    } else if (terminal.isImmediateRegister()) {
        return immediateRegisterSymbol(terminal);
    } else if (terminal.isImmediate()) {
        int value = terminal.value().unsignedValue();
        Bus& bus = move.bus();
        if (bus.signExtends()) {
            value = MathTools::signExtendTo(value, bus.immediateWidth());
        }
        else if (bus.zeroExtends()) {
            value = MathTools::zeroExtendTo(value, bus.immediateWidth());
        }        
        return "(int)" + Conversion::toString(value) + "u";
    } else {
        return portSymbol(terminal.port());
    }    
}
    
/**
 * Generates an unique symbol name for a given operation context
 * 
 * @param fu The given function unit
 * @return a string containing the generated symbol name
 */
std::string 
SymbolGenerator::operationContextSymbol(const TTAMachine::FunctionUnit& fu) {
    return "FU_" + fu.name() + "_context";
}
    
/**
 * Generates an unique symbol name for a given FU conflict detector.
 * 
 *@param fu The given function unit
 * @return std::string containing the generated symbol name
 */
std::string 
SymbolGenerator::conflictDetectorSymbol(const TTAMachine::FunctionUnit& fu) {
    return "FU_" + fu.name() + "_conflict_detector";
} 
     
/**
 * Generates an unique symbol name for the given FU's memory space
 * 
 * @param fuName given function unit
 * @return A string containing the generated symbol name
 */
std::string 
SymbolGenerator::targetMemorySymbol(const TTAMachine::FunctionUnit& fu) {
    return fu.name() + "_target_memory";
}

/**
 * Generates an unique symbol name for the given FU's DirectAccessMemory&
 * 
 * @param fuName Given function unit
 * @return A string containing the generated symbol name
 */
std::string
SymbolGenerator::DAMemorySymbol(const TTAMachine::FunctionUnit& fu) {
    return fu.name() + "_direct_access_memory";
}

/**
 * Generates an unique symbol name for a temporary guard variable
 *  
 * @return A string containing the generated symbol name
 */
std::string
SymbolGenerator::guardBoolSymbol() {
    static int guardNumber = 0;
    return "guard_" + Conversion::toString(guardNumber++);
}
    
/**
 * Generates an unique symbol name for a basic block function
 *
 * A new C/C++ function is created for each basic block in the program, so the
 * generated symbol will also be used as the generated function name.
 * 
 * @param startAddress The start address of the basic block
 * @return A string containing the generated symbol name
 */
std::string 
SymbolGenerator::basicBlockSymbol(InstructionAddress startAddress) {
    return "simulate_" + Conversion::toString(startAddress);
}

/**
 * Generates an unique symbol name for a FU's operation
 * 
 * @param operationName Name of the operation
 * @param fu The function unit containing the operation
 * @return The generated operation symbol string
 */
std::string 
SymbolGenerator::operationSymbol(
    const std::string& operationName,
    const TTAMachine::FunctionUnit& fu) {
    return "op_" + fu.name() + "_" + operationName;
}

/**
 * Generates an unique symbol name for FU's results.
 * 
 * @param port The FU port
 * @return A string containing the generated FU results symbol
 */
std::string 
SymbolGenerator::FUResultSymbol(const TTAMachine::Port& port) {
    return port.parentUnit()->name() + "_" + port.name() + "_results";
}

/**
 * Generates name for temporary variables using incremental numbers
 * 
 * @return The generated temporary variable name
 */
std::string 
SymbolGenerator::generateTempVariable() {
    static int counter = 0;
    return "_tmp_variable_" + Conversion::toString(counter++);
}

