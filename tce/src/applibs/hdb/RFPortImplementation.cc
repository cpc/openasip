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
 * @file RFPortImplementation.cc
 *
 * Implementation of RFPortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "RFPortImplementation.hh"
#include "RFImplementation.hh"

namespace HDB {

/**
 * The constructor.
 *
 * Adds the port automatically to the given parent RF implementation.
 *
 * @param name Name of the port.
 * @param direction Direction of the port.
 * @param loadPort Name of the load enable port.
 * @param opcodePort Name of the opcode port.
 * @param parent The parent RF implementation.
 */
RFPortImplementation::RFPortImplementation(
    const std::string& name,
    Direction direction,
    const std::string& loadPort,
    const std::string& opcodePort,
    const std::string& opcodePortWidthFormula,
    RFImplementation& parent) :
    PortImplementation(name, loadPort), direction_(direction), 
    opcodePort_(opcodePort), 
    opcodePortWidthFormula_(opcodePortWidthFormula) {

    parent.addPort(this);
}


/**
 * The destructor.
 */
RFPortImplementation::~RFPortImplementation() {
}


/**
 * Sets the direction of the port.
 *
 * @param direction The new direction.
 */
void
RFPortImplementation::setDirection(Direction direction) {
    direction_ = direction;
}


/**
 * Returns the direction of the port.
 *
 * @return The direction of the port.
 */
Direction
RFPortImplementation::direction() const {
    return direction_;
}


/**
 * Sets the name of the opcode port.
 *
 * @param name The new name.
 */
void
RFPortImplementation::setOpcodePort(const std::string& name) {
    opcodePort_ = name;
}


/**
 * Returns the name of the opcode port.
 *
 * @return The name of the opcode port.
 */
std::string
RFPortImplementation::opcodePort() const {
    return opcodePort_;
}


/**
 * Sets the width formula of the opcode port.
 *
 * @param formula The new width formula.
 */
void
RFPortImplementation::setOpcodePortWidthFormula(const std::string& formula) {
    opcodePortWidthFormula_ = formula;
}


/**
 * Returns the formula of the width of the opcode port.
 *
 * @return The formula.
 */
std::string
RFPortImplementation::opcodePortWidthFormula() const {
    return opcodePortWidthFormula_;
}
}
