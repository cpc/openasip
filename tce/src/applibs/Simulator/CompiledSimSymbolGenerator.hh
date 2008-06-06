/**
 * @file SymbolGenerator.hh
 *
 * Declaration of CompiledSimSymbolGenerator class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIM_SYMBOL_GENERATOR_HH
#define COMPILED_SIM_SYMBOL_GENERATOR_HH

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
class CompiledSimSymbolGenerator {
public:
    
    CompiledSimSymbolGenerator();
    virtual ~CompiledSimSymbolGenerator();
    
    void enablePrefix(const std::string& prefix);
    void disablePrefix();
        
    
    std::string timeStamp() const;
    
    std::string portSymbol(const TTAMachine::Port& port) const;
    
    std::string registerSymbol(const TTAProgram::Terminal& terminal) const;
    std::string registerSymbol(
        const TTAMachine::RegisterFile& rf, 
        int index) const;
    
    std::string immediateRegisterSymbol(
        const TTAProgram::Terminal& terminal) const;
    std::string immediateRegisterSymbol(
        const TTAMachine::ImmediateUnit& iu, 
        int index) const;
    
    std::string busSymbol(const TTAMachine::Bus& bus) const;

    std::string returnAddressSymbol(const TTAMachine::ControlUnit& gcu) const;
    
    std::string moveOperandSymbol(
        const TTAProgram::Terminal& terminal,
        const TTAProgram::Move & move) const;
    
    std::string operationContextSymbol(
        const TTAMachine::FunctionUnit& fu) const;
    
    std::string conflictDetectorSymbol(
        const TTAMachine::FunctionUnit& fu) const;
     
    std::string targetMemorySymbol(const TTAMachine::FunctionUnit& fu) const;
    
    std::string DAMemorySymbol(const TTAMachine::FunctionUnit& fu) const;

    std::string guardBoolSymbol() const;
    
    std::string basicBlockSymbol(InstructionAddress startAddress) const;
    
    std::string operationSymbol(
        const std::string& operationName,
        const TTAMachine::FunctionUnit& fu) const;
    
    std::string FUResultSymbol(const TTAMachine::Port& port) const;
    
    std::string generateTempVariable() const;
    
    std::string jumpTargetSetterSymbol(InstructionAddress address) const;
    
private:
    /// Copying not allowed.
    CompiledSimSymbolGenerator(const CompiledSimSymbolGenerator&);
    /// Assignment not allowed.
    CompiledSimSymbolGenerator& operator=(const CompiledSimSymbolGenerator&);
    
    /// Prefix used for generated variable symbols
    std::string prefix_;
};

#endif
