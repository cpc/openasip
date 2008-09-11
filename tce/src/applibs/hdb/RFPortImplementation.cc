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
