/**
 * @file CompiledSimMove.hh
 *
 * Declaration of CompiledSimMove class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
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
