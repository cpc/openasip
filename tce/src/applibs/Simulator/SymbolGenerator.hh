/**
 * @file SymbolGenerator.hh
 *
 * Declaration of SymbolGenerator class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef SYMBOL_GENERATOR_HH
#define SYMBOL_GENERATOR_HH

#include "BaseType.hh"

#include <string>

namespace TTAMachine {
    class Machine;
    class FunctionUnit;
    class FUPort;
    class Port;
    class Unit;
    class HWOperation;
    class Guard;
    class RegisterFile;
    class ImmediateUnit;
    class ControlUnit;
    class Bus;
}

namespace TTAProgram {
    class Instruction;
    class Terminal;
    class Move;
}


/**
 * A class that generates C/C++ symbol names for given objects in the simulation
 * 
 * Used for the compiled simulation
 * 
 */
class SymbolGenerator {
public:
    
    static std::string timeStamp();
    
    static std::string portSymbol(const TTAMachine::Port& port);
    
    static std::string registerSymbol(const TTAProgram::Terminal& terminal);
    static std::string registerSymbol(
        const TTAMachine::RegisterFile& rf, 
        int index);
    
    static std::string immediateRegisterSymbol(
        const TTAProgram::Terminal& terminal);
    static std::string immediateRegisterSymbol(
        const TTAMachine::ImmediateUnit& iu, 
        int index);
    
    static std::string busSymbol(const TTAMachine::Bus& bus);

    static std::string returnAddressSymbol(const TTAMachine::ControlUnit& gcu);
    
    static std::string moveOperandSymbol(
        const TTAProgram::Terminal& terminal,
        const TTAProgram::Move & move);
    
    static std::string operationContextSymbol(
        const TTAMachine::FunctionUnit& fu);
    
    static std::string conflictDetectorSymbol(
        const TTAMachine::FunctionUnit& fu);
     
    static std::string targetMemorySymbol(const TTAMachine::FunctionUnit& fu);
    
    static std::string DAMemorySymbol(const TTAMachine::FunctionUnit& fu);

    static std::string guardBoolSymbol();
    
    static std::string basicBlockSymbol(InstructionAddress startAddress);
    
    static std::string operationSymbol(
        const std::string& operationName,
        const TTAMachine::FunctionUnit& fu);
    
    static std::string FUResultSymbol(const TTAMachine::Port& port);
    
    static std::string generateTempVariable();
    
private:
    /// Copying not allowed.
    SymbolGenerator(const SymbolGenerator&);
    /// Assignment not allowed.
    SymbolGenerator& operator=(const SymbolGenerator&);        
};

#endif
