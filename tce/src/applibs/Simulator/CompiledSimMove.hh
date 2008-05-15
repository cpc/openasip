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

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <set>
#include <map>

#include "OperationPool.hh"
#include "SimulatorConstants.hh"
#include "ConflictDetectionCodeGenerator.hh"

namespace TTAMachine {
    class Guard;
    class Bus;
}

namespace TTAProgram {
    class Terminal;
    class Move;
}

/**
 * A class that handles copying values to/from a bus in the compiled simulation
 */
class CompiledSimMove {
public:   
    CompiledSimMove(
        const TTAProgram::Move& move,
        const std::string& guardSymbol);
    CompiledSimMove(const CompiledSimMove&);
    CompiledSimMove& operator=(const CompiledSimMove&);
    
    std::string copyToBusCode() const;
    std::string copyFromBusCode() const;
private:
    const TTAProgram::Move* move_;
    const TTAProgram::Terminal* source_;
    const TTAProgram::Terminal* destination_;
    const TTAMachine::Bus* bus_;
    std::string guardSymbol_;
};

#endif
