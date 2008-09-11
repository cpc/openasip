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
 * @file CompiledSimSymbolGenerator.hh
 *
 * Definition of CompiledSimSymbolGenerator class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "CompiledSimSymbolGenerator.hh"
#include "FunctionUnit.hh"
#include "RegisterFile.hh"
#include "ImmediateUnit.hh"
#include "ControlUnit.hh"
#include "Terminal.hh"
#include "SpecialRegisterPort.hh"
#include "Move.hh"
#include "MathTools.hh"

#include <string>
#include <ctime>

using std::string;

using namespace TTAMachine;
using namespace TTAProgram;


/**
 * Default constructor
 */
CompiledSimSymbolGenerator::CompiledSimSymbolGenerator() {
}

/**
 * Default destructor
 */
CompiledSimSymbolGenerator::~CompiledSimSymbolGenerator() {
}

/**
 * Sets a new prefix to be used for generated variable symbols
 * 
 * @param prefix new prefix to be set
 */
void 
CompiledSimSymbolGenerator::enablePrefix(const std::string& prefix) {
    prefix_ = prefix;
}

/**
 * Disables the usage of prefix.
 */
void 
CompiledSimSymbolGenerator::disablePrefix() {
    prefix_ = "";
}

/**
 * Generates a timestamp
 * 
 * @note Should not be printed to the simulation sources as this makes
 * ccache useless. There's always a cache miss due to the changing 
 * timestamp.
 *
 * @return a string containing the timestamp
 */
std::string
CompiledSimSymbolGenerator::timeStamp() const {
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
CompiledSimSymbolGenerator::portSymbol(const TTAMachine::Port& port) const {
    string symbolName;
    
    Unit* unit = port.parentUnit();    
    if (dynamic_cast<const FunctionUnit*>(unit) != NULL) {
        symbolName = "FU";
        if (dynamic_cast<const ControlUnit*>(unit) != NULL) {
            symbolName = "GCU";
        }
    } else if (dynamic_cast<const RegisterFile*>(unit) != NULL) {
        symbolName = "RF";
        if (dynamic_cast<const ImmediateUnit*>(unit) != NULL) {
            symbolName = "IU";
        }
    } else {
        symbolName = "unknown";
    }

    symbolName += "_" + unit->name() + "_" + port.name();
    return prefix_ + symbolName;
}

/**
 * Generates an unique symbol name for a RF register of the terminal
 * 
 * @param terminal Given terminal
 * @return A string containing the generated symbol name
 */
std::string
CompiledSimSymbolGenerator::registerSymbol(
    const TTAProgram::Terminal& terminal) const {
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
CompiledSimSymbolGenerator::registerSymbol(
    const TTAMachine::RegisterFile& rf, 
    int index) const {  
    return prefix_ + "RF_" + rf.name() + "_" + Conversion::toString(index);
}


/**
 * Generates an unique symbol name for a IU register of the terminal
 * 
 * @param terminal Given terminal
 * @return A string containing the generated symbol name
 */
std::string
CompiledSimSymbolGenerator::immediateRegisterSymbol(
    const TTAProgram::Terminal& terminal) const {
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
CompiledSimSymbolGenerator::immediateRegisterSymbol(
    const TTAMachine::ImmediateUnit& iu, 
    int index) const {  
    return prefix_ + "IU_" + iu.name() + "_" + Conversion::toString(index);
}

/**
 * Generates an unique symbol name for a TTA machine bus
 * 
 * @param bus The given bus
 * @return A string containing the generated symbol name
 */
std::string 
CompiledSimSymbolGenerator::busSymbol(const TTAMachine::Bus& bus) const {
    return prefix_ + "bus_" + bus.name();
}

/**
 * Generates a symbol name for the GCU's return address
 * 
 * @return A symbol name for the GCU's return address
 */
std::string
CompiledSimSymbolGenerator::returnAddressSymbol(
    const TTAMachine::ControlUnit& gcu) const {
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
CompiledSimSymbolGenerator::moveOperandSymbol(                                  
    const TTAProgram::Terminal& terminal, 
    const TTAProgram::Move & move) const {
            
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
CompiledSimSymbolGenerator::operationContextSymbol(
    const TTAMachine::FunctionUnit& fu) const {
    return prefix_ + "FU_" + fu.name() + "_context";
}

/**
 * Generates an unique symbol name for a given FU conflict detector.
 * 
 *@param fu The given function unit
 * @return std::string containing the generated symbol name
 */
std::string 
CompiledSimSymbolGenerator::conflictDetectorSymbol(
    const TTAMachine::FunctionUnit& fu) const {
    return prefix_ + "FU_" + fu.name() + "_conflict_detector";
} 
     
/**
 * Generates an unique symbol name for the given FU's memory space
 * 
 * @param fuName given function unit
 * @return A string containing the generated symbol name
 */
std::string 
CompiledSimSymbolGenerator::targetMemorySymbol(
    const TTAMachine::FunctionUnit& fu) const {
    return prefix_ + fu.name() + "_target_memory";
}

/**
 * Generates an unique symbol name for the given FU's DirectAccessMemory&
 * 
 * @param fuName Given function unit
 * @return A string containing the generated symbol name
 */
std::string
CompiledSimSymbolGenerator::DAMemorySymbol(const TTAMachine::FunctionUnit& fu) 
    const {
    return prefix_ + fu.name() + "_direct_access_memory";
}

/**
 * Generates an unique symbol name for a temporary guard variable
 *  
 * @return A string containing the generated symbol name
 */
std::string
CompiledSimSymbolGenerator::guardBoolSymbol() const {
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
CompiledSimSymbolGenerator::basicBlockSymbol(InstructionAddress startAddress) 
    const {
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
CompiledSimSymbolGenerator::operationSymbol(
    const std::string& operationName,
    const TTAMachine::FunctionUnit& fu) const {
    return prefix_ + "op_" + fu.name() + "_" + operationName;
}

/**
 * Generates an unique symbol name for FU's results.
 * 
 * @param port The FU port
 * @return A string containing the generated FU results symbol
 */
std::string 
CompiledSimSymbolGenerator::FUResultSymbol(const TTAMachine::Port& port) const {
    return portSymbol(port) + "_results";
}

/**
 * Generates name for temporary variables using incremental numbers
 * 
 * @return The generated temporary variable name
 */
std::string 
CompiledSimSymbolGenerator::generateTempVariable() const {
    static int counter = 0;
    return "_tmp_variable_" + Conversion::toString(counter++);
}

/**
 * Generates name for jump target setting function
 * 
 * @param address address of the simulate function to be set
 */
std::string 
CompiledSimSymbolGenerator::jumpTargetSetterSymbol(
    InstructionAddress address) const {
    return "setJumpTargetFor_" + Conversion::toString(address);
}

