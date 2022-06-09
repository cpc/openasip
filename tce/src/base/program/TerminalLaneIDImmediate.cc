/*
    Copyright (c) 2002-2013 Tampere University.

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
 * @file TerminalLaneIDImmediate.cc
 *
 * Implementation of TerminalLaneIDImmediate class.
 *
 * @author Pekka Jääskeläinen 2013
 * @note rating: red
 */

#include "TerminalLaneIDImmediate.hh"
#include "DisassemblyImmediate.hh"
#include "POMDisassembler.hh"

using namespace TTAMachine;

namespace TTAProgram {

/**
 * The constructor.
 *
 * @param value The value of the inline immediate.
 */
TerminalLaneIDImmediate::TerminalLaneIDImmediate(SimValue lane) :
    TerminalImmediate(lane) {
}

/**
 * The destructor.
 */
TerminalLaneIDImmediate::~TerminalLaneIDImmediate() {
}

/**
 * Returns the value of the inline immediate.
 *
 * @return The value of the inline immediate.
 * @exception WrongSubclass never.
 */
SimValue
TerminalLaneIDImmediate::value() const {
    return value_;
}

/**
 * Creates an exact copy of the terminal and returns it.
 *
 * @return A copy of the terminal.
 */
Terminal*
TerminalLaneIDImmediate::copy() const {
    return new TerminalLaneIDImmediate(value_);
}

/**
 * Checks if terminals are equal.
 *
 * @param other Terminal to compare.
 * @return true if terminals are equal.
 */
bool 
TerminalLaneIDImmediate::equals(const Terminal& other) const {

    if (dynamic_cast<const TerminalLaneIDImmediate*>(&other) == 
        NULL) {
        return false;
    }
    return value() == other.value();
}

TCEString
TerminalLaneIDImmediate::toString() const {
    DisassemblyImmediate* disasm = 
        POMDisassembler::createInlineImmediate(*this, false);
    TCEString dis = disasm->toString();
    delete disasm;
    return dis;
}


}
