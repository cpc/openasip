/**
 * @file CompiledSimMove.cc
 *
 * Definition of CompiledSimMove class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include "CompiledSimMove.hh"
#include "Move.hh"
#include "Machine.hh"
#include "Program.hh"
#include "CompiledSimSymbolGenerator.hh"
#include <string>

using std::string;
using std::endl;        

/**
 * The Constructor
 * 
 * @param move The original move
 * @param guardSymbol Symbolname of the guard or "" if no guard is used
 */
CompiledSimMove::CompiledSimMove(
    const TTAProgram::Move& move,
    const std::string& guardSymbol,
    const CompiledSimSymbolGenerator& symbolGenerator) :
    move_(&move),
    source_(&move.source()),
    destination_(&move.destination()),
    bus_(&move.bus()),
    guardSymbol_(guardSymbol),
    symbolGenerator_(symbolGenerator) {
}
    
/**
 * Copy constructor
 * 
 * @param move the move
 */
CompiledSimMove::CompiledSimMove(const CompiledSimMove& move) :
    source_(move.source_),
    destination_ (move.destination_),
    bus_(move.bus_),
    guardSymbol_(move.guardSymbol_),
    symbolGenerator_(move.symbolGenerator_) {
}

/**
 * Assignmnent operator
 * 
 * @param move the move
 */
CompiledSimMove& 
CompiledSimMove::operator=(const CompiledSimMove& move) {
    this->source_ = move.source_;
    this->destination_ = move.destination_;
    this->bus_ = move.bus_;
    this->guardSymbol_ = move.guardSymbol_;
    return *this;
}


/**
 * Generates C/C++ code that copies data from move source to a bus variable
 * 
 * @return The generated code
 */
std::string
CompiledSimMove::copyToBusCode() const {
    string source = symbolGenerator_.moveOperandSymbol(*source_, *move_);
    string bus = symbolGenerator_.busSymbol(*bus_);
    return bus + " = " + source + ";";
}

/**
 * Generates C/C++ code that copies data from bus variable to a move destination
 * 
 * @return The generated code
 */
std::string 
CompiledSimMove::copyFromBusCode() const {
    string destination = symbolGenerator_.moveOperandSymbol(*destination_,
        *move_);
    string bus = symbolGenerator_.busSymbol(*bus_);
    string code = destination + " = " + bus + ";";
    if (guardSymbol_ != "") {
        code = "if (" + guardSymbol_ + ") {" + code + " } ";
    }
    
    return code;
}
