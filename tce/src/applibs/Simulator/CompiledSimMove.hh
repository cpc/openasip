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
