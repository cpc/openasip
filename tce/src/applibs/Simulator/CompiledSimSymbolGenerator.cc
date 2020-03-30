/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
#include "DisassemblyRegister.hh"
#include "Conversion.hh"

#include <string>
#include <ctime>

using std::string;

using namespace TTAMachine;
using namespace TTAProgram;


/**
 * Default constructor.
 *
 * @param globalSymbolSuffix Can be used to produce unique
 * symbol names for the global BB simulation functions. 
 * This has to be used in case using multiple simulation 
 * engines in the same process.
 */
CompiledSimSymbolGenerator::CompiledSimSymbolGenerator(
    const TCEString& globalSymbolSuffix) : 
    globalSymbolSuffix_(globalSymbolSuffix) {
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
    // just speed opt. no need to reserve more at every append.
    symbolName = prefix_;
    symbolName.reserve(40);
    Unit* unit = port.parentUnit();    
    if (dynamic_cast<const FunctionUnit*>(unit) != NULL) {
        symbolName += "FU_";
        if (dynamic_cast<const ControlUnit*>(unit) != NULL) {
            symbolName += "GCU_";
        }
    } else if (dynamic_cast<const RegisterFile*>(unit) != NULL) {
        symbolName += "RF_";
        if (dynamic_cast<const ImmediateUnit*>(unit) != NULL) {
            symbolName += "IU_";
        }
    } else {
        symbolName += "unknown_";
    }

    symbolName += unit->name();
    symbolName += "_";
    symbolName += port.name();
    return symbolName;
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
    return prefix_ + "RF_" + 
        DisassemblyRegister::registerName(rf, index, '_');
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
    return prefix_ + "IU_" + DisassemblyRegister::registerName(iu, index, '_');
    //.name() + "_" + Conversion::toString(index);
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
        const Bus& bus = move.bus();
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
    return "simulate_" + globalSymbolSuffix_ + "_" + Conversion::toString(startAddress);
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

