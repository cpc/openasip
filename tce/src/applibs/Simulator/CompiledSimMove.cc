/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
