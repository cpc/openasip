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
#include "TCEString.hh"

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
 * A class that generates C/C++ symbol names for given objects in the
 * simulation
 *
 * Used for the compiled simulation
 *
 */
class CompiledSimSymbolGenerator {
public:
    CompiledSimSymbolGenerator(const TCEString& globalSymbolSuffix);
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

    CompiledSimSymbolGenerator(const CompiledSimSymbolGenerator&) = delete;
    CompiledSimSymbolGenerator&
    operator=(const CompiledSimSymbolGenerator&) = delete;

private:
    /// Prefix used for generated variable symbols
    TCEString prefix_;

    /// Suffix used for the generated global function symbols.
    TCEString globalSymbolSuffix_;
};

#endif
