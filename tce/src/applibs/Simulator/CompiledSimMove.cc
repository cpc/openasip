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
 * @file CompiledSimMove.cc
 *
 * Definition of CompiledSimMove class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
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
