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
 * @file CompiledSimMove.hh
 *
 * Declaration of CompiledSimMove class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef COMPILED_SIM_MOVE_HH
#define COMPILED_SIM_MOVE_HH

#include <string>

namespace TTAMachine {
    class Guard;
    class Bus;
}

namespace TTAProgram {
    class Terminal;
    class Move;
}

class CompiledSimSymbolGenerator;

/**
 * A class that handles copying values to/from a bus in the compiled simulation
 */
class CompiledSimMove {
public:   
    CompiledSimMove(
        const TTAProgram::Move& move,
        const std::string& guardSymbol,
        const CompiledSimSymbolGenerator& symbolGenerator);
    CompiledSimMove(const CompiledSimMove&);
    CompiledSimMove& operator=(const CompiledSimMove&);
    
    std::string copyToBusCode() const;
    std::string copyFromBusCode() const;
    const TTAProgram::Terminal& destination() const { return *destination_; }
private:
    /// The move
    const TTAProgram::Move* move_;
    
    /// Source of move
    const TTAProgram::Terminal* source_;
    
    /// Destination of move
    const TTAProgram::Terminal* destination_;
    
    /// Bus 
    const TTAMachine::Bus* bus_;
    
    /// guard symbol used for move
    std::string guardSymbol_;
    
    /// the symbol generator
    const CompiledSimSymbolGenerator& symbolGenerator_;
};

#endif
