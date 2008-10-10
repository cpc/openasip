/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file SymbolGenerator.hh
 *
 * Declaration of CompiledSimSymbolGenerator class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
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
